/**
 * \file
 * Application framework events.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kza_application_events.h"

#include <application/kza_application.h>
#include <application/transition/kza_transition.h>

#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/engine/kzu_engine.h>
#include <user/engine/kzu_engine_message.h>
#include <user/engine/kzu_engine_message_queue.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/renderer/kzu_composer.h>
#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader.h>
#include <user/project/kzu_project_patcher.h>
#include <user/project/kzu_project_loader_transition.h>
#include <user/project/kzu_project_loader_animation.h>
#include <user/project/kzu_project_loader_camera.h>
#include <user/project/kzu_project_loader_property_group.h>
#include <user/project/kzu_project_loader_texture.h>
#include <user/animation/kzu_time_line_entry.h>
#include <user/animation/kzu_animation.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_group.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_matrix3x3_property.h>
#include <user/properties/kzu_matrix2x2_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/material/kzu_material.h>
#include <user/ui/kzu_ui_screen.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/components/kzu_ui_slider.h>
#include <user/ui/components/kzu_ui_dropdown.h>
#include <user/ui/components/kzu_ui_toggle_button.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_light.h>
#include <user/kzu_error_codes.h>
#include <user/scripting/lua/kzu_lua.h>
#include <user/animation/kzu_animation_player.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_system.h>
#include <core/memory/kzc_memory_pooled.h>
#include <core/util/string/kzc_string.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/renderer/kzc_renderer.h>
#include <core/debug/kzc_log.h>

#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h> 


kzsError kzaApplicationHandleEventQueue(struct KzaApplication* application, kzUint deltaTime)
{
    kzsError result;
    struct KzuEngine* engine = kzaApplicationGetEngine(application);
    struct KzuProject* project = kzaApplicationGetProject(application);
    
    /* Handle events. */
    {
        struct KzuEngineMessageQueue* queue = kzuEngineGetMessageQueue(engine);
        struct KzcDynamicArrayMutableIterator it = kzuEngineMessageQueueGetMutableIterator(queue);
        while(kzcDynamicArrayMutableIterate(it))
        {
            enum KzuEngineMessageType messageType;
            struct KzuEngineMessage* message = kzcDynamicArrayMutableIteratorGetValue(it);
            kzBool launchAction = kzuEngineMessageUpdate(message, deltaTime);

            if(launchAction)
            {
                messageType = kzuEngineMessageGetType(message);

                switch(messageType)
                {
                    case KZU_ENGINE_MESSAGE_TYPE_CHANGE_SCENE:
                    {
                        struct KzuVoidProperty* property = kzuEngineMessageGetArgument(message, "SceneName");
                        struct KzuScene* scene = (struct KzuScene*)kzuVoidPropertyGetValue(property);

                        result = kzaApplicationSetScene(application, scene);
                        kzsErrorForward(result); 

                        break;
                    }

                    case KZU_ENGINE_MESSAGE_START_ANIMATION:
                    {
                        /* Name is only required argument. Others can be KZ_NULL. */
                        struct KzuStringProperty* nameProperty = kzuEngineMessageGetArgument(message, "AnimationItemName");
                        struct KzuIntProperty* modeProperty = kzuEngineMessageGetArgument(message, "AnimationPlaybackMode");
                        struct KzuFloatProperty* durationProperty = kzuEngineMessageGetArgument(message, "AnimationDurationScale");
                        struct KzuIntProperty* repeatProperty = kzuEngineMessageGetArgument(message, "AnimationRepeatCount");

                        kzString name = kzuStringPropertyGetValue(nameProperty);
                        kzFloat durationScale = (durationProperty != KZ_NULL ? kzuFloatPropertyGetValue(durationProperty) : 1.0f);
                        kzInt repeatCount = (repeatProperty != KZ_NULL ? kzuIntPropertyGetValue(repeatProperty) : 1);
                        enum KzuTimeLineEntryPlaybackMode mode = (modeProperty != KZ_NULL ?
                                                                  (enum KzuTimeLineEntryPlaybackMode)kzuIntPropertyGetValue(modeProperty) : /*lint !e930 */
                                                                  KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_NORMAL);

                        struct KzuAnimationClip* clip = KZ_NULL;
                        struct KzuTimeLineSequence* sequence;

                        if(name != KZ_NULL && kzcStringLength(name) > 0)
                        {
                            kzUint namePieceCount;
                            kzMutableString* namePieces;
                            kzsException exception;

                            result = kzcStringSplit(kzcMemoryGetManager(nameProperty), name, ";", &namePieceCount, &namePieces);
                            kzsErrorForward(result);

                            /* TODO: Use exceptions properly with kzsException* macros */
                            exception = kzuProjectLoaderLoadTimeLineSequence(project, namePieces[0], &sequence);
                            kzsErrorIf(exception)
                            {
                                sequence = KZ_NULL;
                            }
                            if (sequence == KZ_NULL)
                            {
                                exception = kzuProjectLoaderLoadAnimationClip(project, namePieces[0], &clip); 
                                kzsErrorIf(exception)
                                {
                                    kzsLog(KZS_LOG_LEVEL_INFO, namePieces[0]);
                                    kzsErrorThrow(KZU_ERROR_PROJECT_LOADING_FAILED, "Loading animation sequence failed.");
                                }
                            }

                            if(sequence != KZ_NULL || clip != KZ_NULL)
                            {
                                struct KzuAnimationPlayer* player = kzuSceneGetAnimationPlayer(kzaApplicationGetScene(application));
                                struct KzuTimeLineEntry* entry;

                                if(sequence != KZ_NULL)
                                {
                                    result = kzuTimeLineEntryCreateSequence(kzcMemoryGetManager(player), kzuAnimationPlayerGetTime(player), sequence, &entry);
                                    kzsErrorForward(result);
                                }
                                else
                                {
                                    result = kzuTimeLineEntryCreateClip(kzcMemoryGetManager(player), kzuAnimationPlayerGetTime(player), clip, &entry);
                                    kzsErrorForward(result);
                                }

                                kzuTimeLineEntrySetDurationScale(entry, durationScale);
                                kzuTimeLineEntrySetRepeatCount(entry, repeatCount);
                                kzuTimeLineEntrySetPlaybackMode(entry, mode);
                                kzuTimeLineEntrySetDeleteAfterPlayback(entry, KZ_TRUE);

                                /* If contains at least on override, add to entry. */
                                if(namePieceCount > 7) 
                                {
                                    kzUint i;
                                    kzUint end = namePieceCount - 6;
                                    for(i = 1; i < end; i += 6)
                                    {
                                        kzString sourcePath = namePieces[i];
                                        kzString sourcePropertyName = namePieces[i + 1];
                                        kzString targetPath = namePieces[i + 3];
                                        kzString targetPropertyName = namePieces[i + 4];

                                        struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);

                                        struct KzuScene* sourceScene = (struct KzuScene*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_SCENE, sourcePath);
                                        struct KzuMaterial* sourceMaterial = (struct KzuMaterial*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, sourcePath);
                                        struct KzuObjectNode* sourceNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, sourcePath);
                                        struct KzuPropertyGroup* sourceGroup = (struct KzuPropertyGroup*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP, sourcePath);

                                        struct KzuScene* targetScene = (struct KzuScene*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_SCENE, targetPath);
                                        struct KzuMaterial* targetMaterial = (struct KzuMaterial*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, targetPath);
                                        struct KzuObjectNode* targetNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, targetPath);
                                        struct KzuPropertyGroup* targetGroup = (struct KzuPropertyGroup*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP, targetPath);
                                        
                                        struct KzuAnimationOverride sourceOverride;
                                        struct KzuAnimationOverride targetOverride;

                                        sourceOverride.object = KZ_NULL;
                                        sourceOverride.propertyType = kzuPropertyManagerFindPropertyType(propertyManager, sourcePropertyName);

                                        targetOverride.object = KZ_NULL;
                                        targetOverride.propertyType = kzuPropertyManagerFindPropertyType(propertyManager, targetPropertyName);

                                        if(sourceScene != KZ_NULL)
                                        {
                                            sourceOverride.object = sourceScene;
                                        }
                                        else if(sourceMaterial != KZ_NULL)
                                        {
                                            sourceOverride.object = sourceMaterial;
                                        }
                                        else if(sourceNode != KZ_NULL)
                                        {
                                            sourceOverride.object = sourceNode;
                                        }
                                        else if(sourceGroup != KZ_NULL)
                                        {
                                            sourceOverride.object = sourceGroup;
                                        }
                                        else
                                        {
                                            kzsLog(KZS_LOG_LEVEL_WARNING, "Play animation override target: No source object found.");
                                        }

                                        if(targetScene != KZ_NULL)
                                        {
                                            targetOverride.object = targetScene;
                                        }
                                        else if(targetMaterial != KZ_NULL)
                                        {
                                            targetOverride.object = targetMaterial;
                                        }
                                        else if(targetNode != KZ_NULL)
                                        {
                                            targetOverride.object = targetNode;
                                        }
                                        else if(targetGroup != KZ_NULL)
                                        {
                                            targetOverride.object = targetGroup;
                                        }
                                        else
                                        {
                                            kzsLog(KZS_LOG_LEVEL_WARNING, "Play animation override target: No target object found.");
                                        }

                                        if (sourceOverride.object != KZ_NULL && targetOverride.object != KZ_NULL &&
                                            sourceOverride.propertyType != KZ_NULL && targetOverride.propertyType != KZ_NULL)
                                        {
                                            result = kzuTimeLineEntryAddOverride(entry, sourceOverride, targetOverride);
                                            kzsErrorForward(result);
                                        }
                                    }
                                }

                                result = kzuAnimationPlayerAddTimeLineEntry(player, entry);
                                kzsErrorForward(result);
                            }
                            else
                            {
                                result = kzcLog(kzcMemoryGetManager(engine), KZS_LOG_LEVEL_WARNING, "No TimelineSequence or AnimationClip with path '%s' found from project.", name);
                                kzsErrorForward(result);
                            }

                            {
                                kzUint i; 
                                for (i = 0; i < kzcArrayLength(namePieces); ++i)
                                {
                                    result = kzcStringDelete(namePieces[i]);
                                    kzsErrorForward(result);
                                }
                            }
                            result = kzcMemoryFreeArray(namePieces);
                            kzsErrorForward(result);
                        }

                        break;
                    }

                    case KZU_ENGINE_MESSAGE_SET_PROPERTY_VALUE:
                    case KZU_ENGINE_MESSAGE_SWITCH_BOOLEAN_PROPERTY:
                    case KZU_ENGINE_MESSAGE_SET_BOOLEAN_PROPERTY:
                    {
                        struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
                        struct KzuVoidProperty* targetPropertyName = kzuEngineMessageGetArgument(message, "SetPropertyTargetPropertyName");
                        const struct KzuPropertyType* targetProperty = (const struct KzuPropertyType*)kzuVoidPropertyGetValue(targetPropertyName);

                        if (messageType == KZU_ENGINE_MESSAGE_SET_PROPERTY_VALUE)
                        {
                            struct KzuVoidProperty* targetItem = kzuEngineMessageGetArgument(message, "SetPropertyTargetItem");
                            const void* targetObject = kzuVoidPropertyGetValue(targetItem);
                            struct KzuIntProperty* attributeProperty = kzuEngineMessageGetArgument(message, "SetPropertyTargetPropertyAttribute");
                            struct KzuFloatProperty* floatProperty = kzuEngineMessageGetArgument(message, "FloatValue");
                            kzInt propertyAttribute = kzuIntPropertyGetValue(attributeProperty);
                            kzFloat value = kzuFloatPropertyGetValue(floatProperty);

                            enum KzuAnimationTargetAttribute targetAttribute;
                            result = kzuProjectLoaderLoadAnimationConvertAttribute(targetProperty, (enum KzuBinaryAnimationTargetAttribute)propertyAttribute, &targetAttribute); /*lint !e930 */
                            kzsErrorForward(result);
                            result = kzuAnimationAnimatePropertyFloat(propertyManager, targetProperty, targetAttribute, targetObject, KZ_TRUE, value);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            struct KzuVoidProperty* targetItem = kzuEngineMessageGetArgument(message, "SetPropertyTargetItem");
                            const void* targetObject = kzuVoidPropertyGetValue(targetItem);
                            kzBool newValue;

                            if (messageType == KZU_ENGINE_MESSAGE_SWITCH_BOOLEAN_PROPERTY)
                            {
                                newValue = !kzuPropertyManagerGetBoolDefault(propertyManager, targetObject, targetProperty);
                            }
                            else
                            {
                                struct KzuBoolProperty* valueProperty  = kzuEngineMessageGetArgument(message, "SetPropertyBooleanValue");
                                newValue = kzuBoolPropertyGetValue(valueProperty);
                            }

                            result = kzuPropertyManagerSetBool(propertyManager, targetObject, targetProperty, newValue);
                            kzsErrorForward(result);
                        }
                        
                        break;
                    }

                    case KZU_ENGINE_MESSAGE_START_TRANSITION:
                    {
                        struct KzuVoidProperty* targetSceneProperty = kzuEngineMessageGetArgument(message, "StartTransitionTargetScene");
                        struct KzuVoidProperty* transitionProperty = kzuEngineMessageGetArgument(message, "StartTransitionReference");
                        struct KzuVoidProperty* targetCameraProperty = kzuEngineMessageGetArgument(message, "StartTransitionTargetCamera");
                        struct KzuFloatProperty* durationProperty = kzuEngineMessageGetArgument(message, "StartTransitionDuration");

                        struct KzuScene* targetScene = (struct KzuScene*)kzuVoidPropertyGetValue(targetSceneProperty);
                        struct KzuTransition* transition = (struct KzuTransition*)kzuVoidPropertyGetValue(transitionProperty);
                        struct KzuCameraNode* cameraNode = (struct KzuCameraNode*)kzuVoidPropertyGetValue(targetCameraProperty);
                        kzUint duration = (kzUint)(kzuFloatPropertyGetValue(durationProperty) * 1000.0f);

                        if (targetScene != KZ_NULL && transition != KZ_NULL)
                        {
                            struct KzuScene* sourceScene;

                            sourceScene = kzaApplicationGetScene(application);

                            if (cameraNode == KZ_NULL)
                            {
                                cameraNode = kzuSceneGetViewCamera(targetScene);
                            }

                            result = kzaApplicationStartTransition(application, transition, duration, sourceScene, targetScene, cameraNode);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Start Transition, transition, source scene or target scene not set.");
                        }

                        break;
                    }
                    case KZU_ENGINE_MESSAGE_SET_SLIDER_VALUE:
                    {
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuFloatProperty* valueProperty = kzuEngineMessageGetArgument(message, "FloatValue");
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));
                        kzFloat value = (valueProperty != KZ_NULL ? kzuFloatPropertyGetValue(valueProperty) : KZ_NULL);

                        if (node != KZ_NULL)
                        {
                            result = kzuUiSliderSetValueAction(kzuUiComponentNodeFromObjectNode(node), value, kzuEngineMessageGetUserData(message), event, queue);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Set slider target item not set.");
                        }

                        break;
                    }
                    case KZU_ENGINE_MESSAGE_INCREASE_SLIDER_VALUE:
                    {
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuFloatProperty* valueProperty = kzuEngineMessageGetArgument(message, "FloatValue");
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));
                        kzFloat value = (valueProperty != KZ_NULL ? kzuFloatPropertyGetValue(valueProperty) : KZ_NULL);

                        if (node != KZ_NULL)
                        {
                            kzFloat newValue;

                            newValue = kzuUiSliderGetValue(kzuUiComponentNodeFromObjectNode(node));
                            newValue += value;

                            result = kzuUiSliderSetValueAction(kzuUiComponentNodeFromObjectNode(node), newValue, kzuEngineMessageGetUserData(message), event, queue);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Set slider target item not set.");
                        }

                        break;
                    }
                    case KZU_ENGINE_MESSAGE_DECREASE_SLIDER_VALUE:
                    {
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuFloatProperty* valueProperty = kzuEngineMessageGetArgument(message, "FloatValue");
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));
                        kzFloat value = (valueProperty != KZ_NULL ? kzuFloatPropertyGetValue(valueProperty) : KZ_NULL);

                        if (node != KZ_NULL)
                        {
                            kzFloat newValue;

                            newValue = kzuUiSliderGetValue(kzuUiComponentNodeFromObjectNode(node));
                            newValue -= value;

                            result = kzuUiSliderSetValueAction(kzuUiComponentNodeFromObjectNode(node), newValue , kzuEngineMessageGetUserData(message), event, queue);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Set slider target item not set.");
                        }

                        break;
                    }
                    case KZU_ENGINE_MESSAGE_CHANGE_RENDER_PASS_CAMERA:
                    {
                        struct KzuVoidProperty* cameraProperty = kzuEngineMessageGetArgument(message, "ChangeRenderPassCameraCamera");
                        struct KzuVoidProperty* renderPassProperty = kzuEngineMessageGetArgument(message, "ChangeRenderPassCameraRenderPass");
                        struct KzuCameraNode* cameraNode = (struct KzuCameraNode*)kzuVoidPropertyGetValue(cameraProperty);
                        struct KzuRenderPass* renderPass = (struct KzuRenderPass*)kzuVoidPropertyGetValue(renderPassProperty);
                        
                        if (renderPass != KZ_NULL && cameraNode != KZ_NULL)
                        {
                            kzuRenderPassSetCamera(renderPass, cameraNode);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Change render pass camera render pass or camera not found.");
                        }

                        break;
                    }
                    case KZU_ENGINE_MESSAGE_CHANGE_2D_PICKING_CAMERA:
                    {
                        struct KzuVoidProperty* cameraProperty = kzuEngineMessageGetArgument(message, "Change2DPickingCameraCamera");
                        struct KzuVoidProperty* sceneProperty = kzuEngineMessageGetArgument(message, "Change2DPickingCameraScene");
                        struct KzuCameraNode* cameraNode = (struct KzuCameraNode*)kzuVoidPropertyGetValue(cameraProperty);
                        struct KzuScene* scene = (struct KzuScene*)kzuVoidPropertyGetValue(sceneProperty);

                        if (scene != KZ_NULL)
                        {
                            struct KzuUiScreen* screen = kzuSceneGetUiScreen(scene);
                            kzuUiScreenSet2DPickingCamera(screen, cameraNode);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Change 2D picking camera target scene was not found.");
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_SET_TRANSFORMATION_TO_IDENTITY:
                    {
                        struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
                        struct KzuVoidProperty* targetPropertyName = kzuEngineMessageGetArgument(message, "SetPropertyTargetPropertyName");
                        struct KzuVoidProperty* targetItem = kzuEngineMessageGetArgument(message, "SetPropertyTargetItem");

                        void* targetObject = kzuVoidPropertyGetValue(targetItem);
                        const struct KzuPropertyType* targetProperty = (const struct KzuPropertyType*)kzuVoidPropertyGetValue(targetPropertyName);

                        if (targetProperty != KZ_NULL)
                        {
                            enum KzuPropertyDataType dataType = kzuPropertyTypeGetDataType(targetProperty);

                            switch(dataType)
                            {
                                case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                                {
                                    result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetProperty, &KZC_MATRIX4X4_IDENTITY);
                                    kzsErrorForward(result);
                                    break;
                                }
                                case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                                {
                                    result = kzuPropertyManagerSetMatrix3x3(propertyManager, targetObject, targetProperty, &KZC_MATRIX3X3_IDENTITY);
                                    kzsErrorForward(result);
                                    break;
                                }
                                case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                                {
                                    result = kzuPropertyManagerSetMatrix2x2(propertyManager, targetObject, targetProperty, &KZC_MATRIX2X2_IDENTITY);
                                    kzsErrorForward(result);
                                    break;
                                }
                                case KZU_PROPERTY_DATA_TYPE_COLOR:
                                case KZU_PROPERTY_DATA_TYPE_FLOAT:
                                case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                                case KZU_PROPERTY_DATA_TYPE_INT:
                                case KZU_PROPERTY_DATA_TYPE_LIGHT:
                                case KZU_PROPERTY_DATA_TYPE_STRING:
                                case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                                case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                                case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                                case KZU_PROPERTY_DATA_TYPE_BOOL:
                                case KZU_PROPERTY_DATA_TYPE_VOID:
                                default: 
                                {
                                    kzsLog(KZS_LOG_LEVEL_WARNING, "MatrixProperty value change: Target was not matrix property.");
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_EXECUTE_SCRIPT:
                    {
#undef ENABLE_LUA
#ifdef ENABLE_LUA
                        struct KzuStringProperty* functionNameProperty = kzuEngineMessageGetArgument(message, "ScriptFunctionName");

                        if(functionNameProperty != KZ_NULL)
                        {
                            kzInt value; /* TODO: Why is this value not used? */
                            kzString functionName = kzuStringPropertyGetValue(functionNameProperty);

                            result = kzuLuaCallFunction(kzaApplicationGetLua(application), functionName, "pPpP>i", 
                                                        "KzuProject", kzaApplicationGetProject(application), 
                                                        "KzuScene", kzaApplicationGetScene(application), &value);
                            kzsErrorForward(result);
                        }
#endif
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_CHANGE_SCENE_CAMERA:
                    {
                        struct KzuVoidProperty* cameraProperty = kzuEngineMessageGetArgument(message, "ChangeSceneCameraCamera");
                        struct KzuVoidProperty* sceneProperty = kzuEngineMessageGetArgument(message, "ChangeSceneCameraScene");
                        struct KzuCameraNode* cameraNode = (struct KzuCameraNode*)kzuVoidPropertyGetValue(cameraProperty);
                        struct KzuScene* scene = (struct KzuScene*)kzuVoidPropertyGetValue(sceneProperty);

                        if (scene != KZ_NULL)
                        {
                            kzuSceneSetViewCamera(scene, cameraNode);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Change scene camera, target scene was not found.");
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_SWITCH_TOGGLE_BUTTON_STATE:
                    {
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));

                        if (node != KZ_NULL)
                        {
                            kzBool oldValue;

                            oldValue = kzuUiToggleButtonGetState(kzuUiComponentNodeFromObjectNode(node));
                            
                            result = kzuUiToggleButtonSetStateAction(kzuUiComponentNodeFromObjectNode(node), !oldValue, kzuEngineMessageGetUserData(message), event, queue);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Set toggle button state target item not set.");
                        }

                        break;
                    }
                    case KZU_ENGINE_MESSAGE_DROPDOWN_OPEN:
                    {
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));

                        if (node != KZ_NULL)
                        {
                            result = kzuUiDropdownSetOpenAction(kzuUiComponentNodeFromObjectNode(node), kzuEngineMessageGetUserData(message), event, queue);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Dropdown open target not set.");
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_DROPDOWN_CLOSE:
                    {
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));

                        if (node != KZ_NULL)
                        {
                            result = kzuUiDropdownSetClosedAction(kzuUiComponentNodeFromObjectNode(node), kzuEngineMessageGetUserData(message), event, queue);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Dropdown close target not set.");
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_DROPDOWN_SET_SELECTED_ITEM:
                    {
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));

                        if (node != KZ_NULL)
                        {
                            kzInt newValue;
                            struct KzuIntProperty* targetItemProperty = kzuEngineMessageGetArgument(message, "DropdownNewValue");

                            newValue = kzuIntPropertyGetValue(targetItemProperty);
                            
                            result = kzuUiDropdownSetSelectedItemAction(kzuUiComponentNodeFromObjectNode(node), newValue,
                                kzuEngineMessageGetUserData(message), event, queue);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Dropdown close target not set.");
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_DROPDOWN_SWITCH_OPEN_STATE:
                    { 
                        struct KzuVoidProperty* targetProperty = kzuEngineMessageGetArgument(message, "TargetComponentLocation");
                        struct KzuObjectNode* node = kzuVoidPropertyGetValue(targetProperty);
                        struct KzuUiEvent* event = kzuUiScreenGetEvent(kzuSceneGetUiScreen(kzaApplicationGetScene(application)));

                        if (node != KZ_NULL)
                        {
                            struct KzuUiComponentNode* componentNode;
                            kzBool oldOpenState;

                            componentNode = kzuUiComponentNodeFromObjectNode(node);

                            oldOpenState = kzuUiDropdownIsOpen(componentNode);

                            if(oldOpenState)
                            {
                                result = kzuUiDropdownSetClosedAction(componentNode, kzuEngineMessageGetUserData(message), event, queue);
                                kzsErrorForward(result);                                
                            }
                            else
                            {
                                result = kzuUiDropdownSetOpenAction(componentNode, kzuEngineMessageGetUserData(message), event, queue);
                                kzsErrorForward(result);
                            }
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Dropdown switch state no target component set.");
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_RENDER_PASS_SET_ENABLED_STATE:
                    case KZU_ENGINE_MESSAGE_RENDER_PASS_RENDER_FOR_ONE_FRAME_AND_DISABLE:
                    case KZU_ENGINE_MESSAGE_RENDER_PASS_TOGGLE_ENABLED_STATE:
                    { 
                        struct KzuVoidProperty* locationProperty = kzuEngineMessageGetArgument(message, "RenderPassActionRenderPass");
                        struct KzuComposer* composer = (struct KzuComposer*)kzuVoidPropertyGetValue(locationProperty); 

                        if (composer != KZ_NULL && kzuComposerGetType(composer) == KZU_COMPOSER_TYPE_RENDER_PASS)
                        {
                            struct KzuRenderPass* renderPass;

                            renderPass = kzuRenderPassFromComposer(composer);

                            if(messageType == KZU_ENGINE_MESSAGE_RENDER_PASS_RENDER_FOR_ONE_FRAME_AND_DISABLE)
                            {
                                kzuRenderPassSetEnabled(renderPass, KZ_TRUE);
                                kzuRenderPassSetRenderOnce(renderPass);         
                            }
                            else 
                            {
                                kzBool newState;
                                if(messageType == KZU_ENGINE_MESSAGE_RENDER_PASS_SET_ENABLED_STATE)
                                {
                                    struct KzuBoolProperty* enabledProperty = kzuEngineMessageGetArgument(message, "RenderPassActionEnabled");
                                    newState = kzuBoolPropertyGetValue(enabledProperty);
                                }
                                else 
                                {
                                    newState = !kzuRenderPassIsEnabled(renderPass);
                                }
                                kzuRenderPassSetEnabled(renderPass, newState);
                            }
                        }
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_CHANGE_SCENE_COMPOSER:
                    {
                        struct KzuVoidProperty* composerProperty = kzuEngineMessageGetArgument(message, "ChangeComposerComposer");
                        struct KzuVoidProperty* sceneProperty = kzuEngineMessageGetArgument(message, "ChangeComposerScene");
                        struct KzuComposer* composer = (struct KzuComposer*)kzuVoidPropertyGetValue(composerProperty);
                        struct KzuScene* scene = (struct KzuScene*)kzuVoidPropertyGetValue(sceneProperty);

                        kzuSceneSetComposer(scene, composer);
                        break;
                    }
                    case KZU_ENGINE_MESSAGE_SET_TEXTURE_PROPERTY:
                    {
                        struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
                        struct KzuVoidProperty* targetPropertyName = kzuEngineMessageGetArgument(message, "SetPropertyTargetPropertyName");
                        struct KzuVoidProperty* targetItem = kzuEngineMessageGetArgument(message, "SetTextureTargetItem");
                        struct KzuVoidProperty* textureProperty = kzuEngineMessageGetArgument(message, "SetPropertyTextureValue");

                        struct KzuMaterial* material = (struct KzuMaterial*)kzuVoidPropertyGetValue(targetItem);

                        const struct KzuPropertyType* targetProperty = (struct KzuPropertyType*)kzuVoidPropertyGetValue(targetPropertyName);

                        if (material != KZ_NULL && targetProperty != KZ_NULL)
                        {
                            struct KzcTexture* texture = (struct KzcTexture*)kzuVoidPropertyGetValue(textureProperty);
                            
                            result = kzuPropertyManagerSetVoid(propertyManager, material, targetProperty, texture);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Texture property set value: No target object found.");
                        }

                        break;
                    }   
                }

                result = kzuEngineMessageDelete(message);
                kzsErrorForward(result);
                result = kzcDynamicArrayMutableIteratorRemove(it);
                kzsErrorForward(result);
            }
        }
    }

    kzsSuccess();
}
 
kzsError kzaApplicationStartTransition(struct KzaApplication* application, struct KzuTransition* transition, 
                                       kzUint duration, struct KzuScene* sourceScene, struct KzuScene* targetScene,
                                       struct KzuCameraNode* targetCameraNode)
{
    kzsError result;
    
    kzsAssert(kzcIsValidPointer(application));

    result = kzaTransitionStart(kzaApplicationGetTransition(application), transition, application, duration, sourceScene, targetScene, targetCameraNode);
    kzsErrorForward(result);

    kzsSuccess();
}
