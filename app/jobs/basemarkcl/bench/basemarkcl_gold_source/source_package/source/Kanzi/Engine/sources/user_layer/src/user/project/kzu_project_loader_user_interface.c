/**
* \file
* Project loader for user interface files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_user_interface.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_object_node_private.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_texture.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/binary/kzu_binary_base.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_event_listener.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/ui/components/kzu_ui_trajectory_layout.h>
#include <user/ui/components/kzu_ui_grid_layout.h>
#include <user/ui/components/kzu_ui_label.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_text_object.h>
#include <user/kzu_error_codes.h>
#include <user/material/kzu_material.h>
#include <user/project/kzu_project_loader_property.h>
#include <user/project/kzu_project_loader_font.h>
#include <user/project/kzu_project_loader_trajectory.h>
#include <user/project/kzu_project_loader_material.h>

#include <core/memory/kzc_byte_buffer.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/curve/kzc_trajectory.h>
#include <core/util/font/kzc_font.h>
#include <core/util/font/kzc_configured_font.h>
#include <core/util/font/kzc_text_layouter.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>

#include <system/debug/kzs_log.h>


/** Location of the user interface action. */
enum KzuBinaryUiActionLocation
{
    KZU_BINARY_UI_ACTION_LOCATION_ENGINE = 0,
    KZU_BINARY_UI_ACTION_LOCATION_APPLICATION = 1,
    KZU_BINARY_UI_ACTION_LOCATION_SCRIPT = 2,
    KZU_BINARY_UI_ACTION_LOCATION_COMPONENT = 3
};

/** Location of the user interface component callback. */
enum KzuBinaryUiCallbackLocation
{
    KZU_BINARY_UI_CALLBACK_ENGINE = 0,
    KZU_BINARY_UI_CALLBACK_APPLICATION = 1,
    KZU_BINARY_UI_CALLBACK_SCRIPT = 2
};

/** Argument value type. */
enum KzuBinaryUiArgumentValueType
{
    KZU_BINARY_UI_ARGUMENT_VALUE_FIXED = 0,
    KZU_BINARY_UI_ARGUMENT_VALUE_FORWARD = 1,
    KZU_BINARY_UI_ARGUMENT_VALUE_FORWARD_PROPERTY = 2
};

/** Component callback function type. */
enum KzuBinaryUiCallbackType
{
    KZU_BINARY_UI_CALLBACK_TYPE_INITIALIZE = 0,
    KZU_BINARY_UI_CALLBACK_TYPE_VISUAL_UPDATE = 1, /* TODO: Remove */
    KZU_BINARY_UI_CALLBACK_TYPE_LOGIC_UPDATE = 2,
    KZU_BINARY_UI_CALLBACK_TYPE_UNINITIALIZE = 3,
    KZU_BINARY_UI_CALLBACK_TYPE_HANDLE_EVENTS = 4
};


/** Converts enum from KzuBinaryUiCallbackType to KzuUiComponentCallback. */
static kzsError kzuProjectLoaderUiComponentTypeGetComponent_internal(enum KzuBinaryUiCallbackType type, enum KzuUiComponentCallback* out_callbackType)
{
    enum KzuUiComponentCallback callback;
    switch(type)
    {
        case KZU_BINARY_UI_CALLBACK_TYPE_INITIALIZE: callback = KZU_UI_COMPONENT_CALLBACK_INITIALIZE; break;
        case KZU_BINARY_UI_CALLBACK_TYPE_UNINITIALIZE: callback = KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE; break;
        case KZU_BINARY_UI_CALLBACK_TYPE_LOGIC_UPDATE: callback = KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE; break;
        case KZU_BINARY_UI_CALLBACK_TYPE_HANDLE_EVENTS: callback = KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS; break;
        case KZU_BINARY_UI_CALLBACK_TYPE_VISUAL_UPDATE:
        default: 
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Uknown component callback type.");
        }
    }
    *out_callbackType = callback;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadUiComponentType(struct KzuProject* project, kzString path,
                                                 struct KzuUiComponentType** out_componentType)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuUiComponentType* componentType;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load user interface component type with null path");

    componentType = (struct KzuUiComponentType*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE, path);

    if (!kzuUiComponentTypeIsValid(componentType))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_UI_COMPONENT_TYPE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load ui component file.");

        result = kzuUiComponentTypeCreate(memoryManager, &componentType);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE, componentType, kzuBinaryFileInfoGetPath(file), (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&componentType);
        kzsErrorForward(result);

        /* Load data */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                /* Property types */
                {
                    kzUint numPropertyTypes;
                    kzUint i;

                    result = kzcInputStreamReadU32Int(inputStream, &numPropertyTypes);
                    kzsErrorForward(result);

                    for (i = 0; i < numPropertyTypes; ++i)
                    {
                        kzString propertyTypePath;
                        struct KzuPropertyType* propertyType;

                        result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
                        kzsErrorForward(result);

                        result = kzuProjectLoaderLoadPropertyType(project, propertyTypePath, &propertyType);
                        kzsErrorForward(result);

                        if (propertyType == KZ_NULL)
                        {
                            /* Unknown property type is not an error due to flexible support for future binary formats as well. */
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Unable to load property due to unknown property type");
                        }
                        else
                        {
                            result = kzuUiComponentTypeAddPropertyType(componentType, propertyType);
                            kzsErrorForward(result);
                        }
                    }
                }

                /* Callbacks */
                {
                    kzUint i;
                    kzUint callbackCount;

                    result = kzcInputStreamReadU32Int(inputStream, &callbackCount);
                    kzsErrorForward(result);

                    for(i = 0; i < callbackCount; ++i)
                    {
                        enum KzuBinaryUiCallbackType function;
                        enum KzuBinaryUiCallbackLocation location;
                        kzMutableString name;

                        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&function);
                        kzsErrorForward(result);
                        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&location);
                        kzsErrorForward(result);
                        result = kzuBinaryReadString(memoryManager, inputStream, &name);
                        kzsErrorForward(result);

                        switch (location)
                        {
                            case KZU_BINARY_UI_CALLBACK_SCRIPT:
                            {
                                kzString scriptPath;
                                enum KzuUiComponentCallback callbackType;

                                result = kzuBinaryDirectoryReadReference(inputStream, file, &scriptPath);
                                kzsErrorForward(result);

                                result = kzuProjectLoaderUiComponentTypeGetComponent_internal(function, &callbackType);
                                kzsErrorForward(result);

                                result = kzuUiComponentTypeSetCallbackScript(memoryManager, componentType, callbackType, name);
                                kzsErrorForward(result);
                                break;
                            }

                            case KZU_BINARY_UI_CALLBACK_APPLICATION:
                            case KZU_BINARY_UI_CALLBACK_ENGINE:
                            {
                                struct KzuUiManager* uiManager = kzuProjectGetUiProject(project);
                                switch (function)
                                {
                                    case KZU_BINARY_UI_CALLBACK_TYPE_INITIALIZE:
                                    {
                                        kzuUiComponentTypeSetInitializeCallback(componentType, kzuUiManagerGetInitializeCallback(uiManager, name));
                                        break;
                                    }
                                    case KZU_BINARY_UI_CALLBACK_TYPE_LOGIC_UPDATE:
                                    {
                                        kzuUiComponentTypeSetLogicUpdateCallback(componentType, kzuUiManagerGetUpdateCallback(uiManager, name));
                                        break;
                                    }
                                    case KZU_BINARY_UI_CALLBACK_TYPE_UNINITIALIZE:
                                    {
                                        kzuUiComponentTypeSetUninitializeCallback(componentType, kzuUiManagerGetUninitializeCallback(uiManager, name));
                                        break;
                                    }
                                    case KZU_BINARY_UI_CALLBACK_TYPE_HANDLE_EVENTS:
                                    {
                                        kzuUiComponentTypeSetHandleEventsCallback(componentType, kzuUiManagerGetHandleEventsCallback(uiManager, name));
                                        break;
                                    }
                                    case KZU_BINARY_UI_CALLBACK_TYPE_VISUAL_UPDATE:
                                    {
                                        /* TODO: Remove */
                                        break;
                                    }
                                    default:
                                    {
                                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid ui component callback type.");
                                    }
                                }

                                break;
                            }

                            default:
                            {
                                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid ui component callback location.");
                            }
                        }
                        

                        result = kzcStringDelete(name);
                        kzsErrorForward(result);
                    }
                }

                /* Actions */
                {
                    kzUint i;
                    kzUint actionCount;

                    result = kzcInputStreamReadU32Int(inputStream, &actionCount);
                    kzsErrorForward(result);

                    for(i = 0; i < actionCount; ++i)
                    {
                        kzMutableString name; /* TODO: Name is not used anywhere */
                        result = kzuBinaryReadString(memoryManager, inputStream, &name);
                        kzsErrorForward(result);

                        /* Event data */
                        {
                            kzUint j;
                            kzUint propertyCount;

                            result = kzcInputStreamReadU32Int(inputStream, &propertyCount);
                            kzsErrorForward(result);

                            for (j = 0; j < propertyCount; ++j)
                            {
                                kzString propertyTypePath; /* TODO: Property type path is not used anywhere */
                                result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
                                kzsErrorForward(result);                                            
                            }
                        }

                        result = kzcStringDelete(name);
                        kzsErrorForward(result);
                    }
                }

                /* Events */
                {
                    kzUint i;
                    kzUint eventCount;

                    result = kzcInputStreamReadU32Int(inputStream, &eventCount);
                    kzsErrorForward(result);

                    for(i = 0; i < eventCount; ++i)
                    {
                        kzMutableString name;
                        
                        result = kzuBinaryReadString(memoryManager, inputStream, &name);
                        kzsErrorForward(result);
                        
                        {
                            kzUint n;
                            kzUint propertyCount;

                            result = kzcInputStreamReadU32Int(inputStream, &propertyCount);
                            kzsErrorForward(result);

                            for (n = 0; n < propertyCount; ++n)
                            {

                                kzString propertyTypePath;
                                result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
                                kzsErrorForward(result);
                            }
                        }

                        result = kzcStringDelete(name);
                        kzsErrorForward(result);
                    }
                }                
                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }
        }
    }

    *out_componentType = componentType;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadUiComponentNode(struct KzuProject* project, kzString path,
                                                 struct KzuUiComponentNode** out_uiComponentNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuUiComponentNode* uiComponentNode;
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load UI component node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (kzuObjectNodeIsValid(objectNode))
    {
        uiComponentNode = kzuUiComponentNodeFromObjectNode(objectNode);
    }
    else
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_UI_COMPONENT_NODE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load UI component node file.");

        /* Create uiComponent node. */
        result = kzuUiComponentNodeCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project),
                                          kzuBinaryFileInfoGetName(file), KZ_NULL, &uiComponentNode);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, kzuUiComponentNodeToObjectNode(uiComponentNode), kzuBinaryFileInfoGetPath(file), (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectNode);
        kzsErrorForward(result);

        uiComponentNode = kzuUiComponentNodeFromObjectNode(objectNode);

        {
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            /* Load object node base. */
            result = kzuProjectLoaderLoadObjectNodeBase_private(project, inputStream, file, objectNode);
            kzsErrorForward(result);

            /* Load uiComponent node specific data. */
            {
                kzUint eventCount;
                kzString componentTypePath;
                struct KzuUiComponentType* componentType;
                
                result = kzuBinaryDirectoryReadReference(inputStream, file, &componentTypePath);
                kzsErrorForward(result);

                /* Component type */
                kzsErrorTest(componentTypePath != KZ_NULL, KZU_ERROR_INVALID_BINARY_DATA, "Component type reference in component must not be NULL.");

                result = kzuProjectLoaderLoadUiComponentType(project, componentTypePath, &componentType);
                kzsErrorForward(result);

                kzuUiComponentNodeSetComponentType(uiComponentNode, componentType);

                /* Event listeners */
                {
                    kzUint i;

                    result = kzcInputStreamReadU32Int(inputStream, &eventCount);
                    kzsErrorForward(result);

                    for (i = 0; i < eventCount; ++i)
                    {
                        kzUint n;
                        kzMutableString eventName;
                        kzUint actionListenerCount;

                        result = kzuBinaryReadString(memoryManager, inputStream, &eventName);
                        kzsErrorForward(result);
                        result = kzcInputStreamReadU32Int(inputStream, &actionListenerCount);
                        kzsErrorForward(result);

                        /* Action references. */
                        for (n = 0; n < actionListenerCount; ++n)
                        {
                            kzUint j;
                            kzMutableString actionName;
                            kzUint actionLocation;
                            kzUint argumentCount;
                            kzUint delay;
                            struct KzuUiEventListener* eventListener;

                            result = kzuUiEventListenerCreate(memoryManager, eventName, &eventListener);
                            kzsErrorForward(result);
                            
                            result = kzuBinaryReadString(memoryManager, inputStream, &actionName);
                            kzsErrorForward(result);

                            result = kzcInputStreamReadU32Int(inputStream, &delay);
                            kzsErrorForward(result);

                            kzuUiEventListenerSetDelay(eventListener, delay);

                            /* Get the action from project and bind to listener. */
                            {
                                struct KzuUiManager* uiManager = kzuProjectGetUiProject(project);
                                struct KzuUiAction* baseAction = kzuUiManagerGetAction(uiManager, actionName);

                                if (baseAction != KZ_NULL)
                                {
                                    struct KzuUiAction* action;

                                    result = kzuUiActionCopy(memoryManager, baseAction, &action);
                                    kzsErrorForward(result);

                                    kzuUiEventListenerSetAction(eventListener, action);
                                }
                                else
                                {
                                    kzuUiEventListenerSetAction(eventListener, KZ_NULL);
                                }
                            }

                            result = kzcStringDelete(actionName);
                            kzsErrorForward(result);
                            result = kzcInputStreamReadU32Int(inputStream, &actionLocation);
                            kzsErrorForward(result);

                            switch (actionLocation)
                            {
                                case KZU_BINARY_UI_ACTION_LOCATION_SCRIPT:
                                {
                                    KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Scripting is not supported yet.");
                                }
                                case KZU_BINARY_UI_ACTION_LOCATION_COMPONENT:
                                {
                                    kzString targetComponentLocation;

                                    result = kzuBinaryDirectoryReadReference(inputStream, file, &targetComponentLocation);
                                    kzsErrorForward(result);

                                    if(targetComponentLocation == KZ_NULL)
                                    {
                                        kzuUiEventListenerSetTargetComponent(eventListener, KZ_NULL);
                                    }
                                    else
                                    {
                                        result = kzuUiEventListenerSetTargetComponentPath(eventListener, targetComponentLocation);
                                        kzsErrorForward(result);
                                    }

                                    break;
                                }
                                case KZU_BINARY_UI_ACTION_LOCATION_ENGINE:
                                case KZU_BINARY_UI_ACTION_LOCATION_APPLICATION:
                                {
                                    /* Nothing to load here. */
                                    break;
                                }
                                default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid action location.");
                            }

                            result = kzcInputStreamReadU32Int(inputStream, &argumentCount);
                            kzsErrorForward(result);

                            /* Arguments. */
                            for (j = 0; j < argumentCount; ++j)
                            {
                                struct KzuUiEventListenerArgument* argument;
                                kzMutableString argumentName;
                                kzUint argumentValueType;

                                result = kzuBinaryReadString(memoryManager, inputStream, &argumentName);
                                kzsErrorForward(result);

                                result = kzuUiEventListenerArgumentCreate(memoryManager, argumentName, &argument);
                                kzsErrorForward(result);

                                result = kzcInputStreamReadU32Int(inputStream, &argumentValueType);
                                kzsErrorForward(result);

                                switch (argumentValueType)
                                {
                                    case KZU_BINARY_UI_ARGUMENT_VALUE_FIXED:
                                    {
                                        struct KzuProperty* property;
                                        result = kzuProjectLoaderReadProperty_old(project, inputStream, file, &property);
                                        kzsErrorForward(result);

                                        kzuUiEventListenerArgumentSetFixedValue(argument, property);
                                        break;
                                    }
                                    case KZU_BINARY_UI_ARGUMENT_VALUE_FORWARD:
                                    {
                                        kzMutableString forwardArgumentName;
                                        result = kzuBinaryReadString(memoryManager, inputStream, &forwardArgumentName);
                                        kzsErrorForward(result);

                                        result = kzuUiEventListenerArgumentSetForwardEventValue(argument, (kzString)forwardArgumentName);
                                        kzsErrorForward(result);

                                        result = kzcStringDelete(forwardArgumentName);
                                        kzsErrorForward(result);
                                        break;
                                    }
                                    case KZU_BINARY_UI_ARGUMENT_VALUE_FORWARD_PROPERTY:
                                    {
                                        kzMutableString forwardArgumentName;
                                        result = kzuBinaryReadString(memoryManager, inputStream, &forwardArgumentName);
                                        kzsErrorForward(result);

                                        result = kzuUiEventListenerArgumentSetForwardValue(argument, (kzString)forwardArgumentName);
                                        kzsErrorForward(result);

                                        result = kzcStringDelete(forwardArgumentName);
                                        kzsErrorForward(result);

                                        break;
                                    }
                                    default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid ui component action argument value type.");
                                }

                                result = kzuUiEventListenerAddArgument(eventListener, argument);
                                kzsErrorForward(result);

                                result = kzcStringDelete(argumentName);
                                kzsErrorForward(result);
                            }

                            result = kzuUiComponentNodeAddEventListener(uiComponentNode, eventName, eventListener);
                            kzsErrorForward(result);
                            
                        }
                        result = kzcStringDelete(eventName);
                        kzsErrorForward(result);
                    }
                }

                result = kzuUiComponentNodeInitialize(uiComponentNode, memoryManager);
                kzsErrorForward(result);

                result = kzuProjectLoaderLoadUiComponentEventListenerTargets(project, uiComponentNode);
                kzsErrorForward(result);
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }
    }

    *out_uiComponentNode = uiComponentNode;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadUiComponentEventListenerTargets(struct KzuProject* project, const struct KzuUiComponentNode* componentNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    struct KzcDynamicArray* eventListeners;
     
    result = kzuUiComponentNodeGetEventListeners(kzcMemoryGetManager(componentNode), componentNode, &eventListeners);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(eventListeners);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzuUiEventListener* eventListener = (struct KzuUiEventListener*)kzcDynamicArrayIteratorGetValue(it);
        enum KzuUiActionLocation actionLocation = kzuUiEventListenerGetTargetLocation(eventListener);
        kzString targetComponentLocation = kzuUiEventListenerGetTargetComponentPath(eventListener);

        if(actionLocation == KZU_UI_ACTION_LOCATION_COMPONENT && targetComponentLocation != KZ_NULL)
        {
            struct KzuObjectNode* targetObject;
            struct KzuUiComponentNode* targetComponentNode;
            result = kzuProjectLoaderLoadObjectNode(project, targetComponentLocation, &targetObject);
            kzsErrorForward(result);
            result = kzuUiEventListenerSetTargetComponentPath(eventListener, KZ_NULL);
            kzsErrorForward(result);
            targetComponentNode = kzuUiComponentNodeFromObjectNode(targetObject);
            kzuUiEventListenerSetTargetComponent(eventListener, targetComponentNode);
        }
    }

    result = kzcDynamicArrayDelete(eventListeners);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadUiComponentType(struct KzuProject* project, const struct KzuUiComponentType* componentType)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, componentType);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
