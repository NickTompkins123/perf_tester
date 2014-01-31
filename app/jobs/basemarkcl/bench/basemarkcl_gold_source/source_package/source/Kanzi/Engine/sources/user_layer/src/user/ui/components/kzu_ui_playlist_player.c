/**
* \file
* User interface play list player.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_playlist_player.h"

#include <user/ui/kzu_ui_component_base.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_event.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/engine/kzu_engine_message.h>
#include <user/engine/kzu_engine_message_queue.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>

#include <system/debug/kzs_log.h>


#define KZU_UI_PLAYLIST_SEPARATOR_STRING ";"

/** Specifies how the playlist component behaves when next and previous buttons are pressed. */
enum KzuUiPlaylistPlayerPlayMode
{
    KZU_UI_PLAYLIST_PLAYER_PLAY_MODE_NORMAL = 0,  /**< Playback is in normal mode. Next and previous always move to next item. */
    KZU_UI_PLAYLIST_PLAYER_PLAY_MODE_TWO_WAY = 1, /**< Playback is in two way mode. Next and previous always move to next, but there during direction change the initial position is played twice. */
    KZU_UI_PLAYLIST_PLAYER_PLAY_MODE_SHUFFLE = 2  /**< Animation to be played is selected by random. */
};


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_PLAYLIST_PLAYER = (void*)&KZU_COMPONENT_TYPE_PLAYLIST_PLAYER;


/** Playing previous boolean. */
#define KZU_UI_ANIMATION_PLAYER_PLAY_NEXT KZ_FALSE
/** Playing next boolean. */
#define KZU_UI_ANIMATION_PLAYER_PLAY_PREVIOUS KZ_TRUE


/** Internal function for playing the animations from the list. */
static kzsError kzuUiPlaylistPlayerPlay_internal(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                          kzBool previous, struct KzcHashMap* parameters);
/** Adds play animation command to application event list. */
static kzsError kzuUiPlaylistPlayerPlayAction_internal(const struct KzuUiComponentNode* componentNode, struct KzuStringProperty* targetProperty, void* userData, 
                                                       kzBool playBackwards, struct KzcHashMap* parameters);
/** Gets UI list player path. */
kzsError kzuUiPlaylistPlayerGetPath_internal(const struct KzuUiComponentNode* componentNode, kzBool previous, kzMutableString* out_path);

/** 
* Internal helper for selecting animation to be played. 
* \param mode, playback mode
* \param previous, playing diretion KZ_TRUE plays previous, KZ_FALSE plays next
* \param loop, looping enabled
* \param previousIndex previously set index, used on some animations
* \param count number of animations in playlist player
* \param out_runAnimation should animation be played, (when no looping is used some cases of next/previous can result in state where no animation should be played)
* \param out_indexValue value to be set to index property
* \param out_playIndex index of player entry to be played
* \param out_playBackwards play entry backwards
*/
static kzsError kzuUiPlayListPlayerGetIndexToPlay_internal(enum KzuUiPlaylistPlayerPlayMode mode, kzBool previous, kzBool loop, kzInt previousIndex, kzUint count,
                                                           kzBool* out_runAnimation, kzInt* out_indexValue, kzInt* out_playIndex, kzBool* out_playBackwards)
{ 
    kzInt index = previousIndex;
    kzInt playIndex;
    kzBool runAnimation = KZ_TRUE;
    kzBool playBackwards = KZ_FALSE;

    switch(mode)
    {
        case KZU_UI_PLAYLIST_PLAYER_PLAY_MODE_NORMAL:
        {
            index += previous ? -1 : 1;
            if(index < 0)
            {
                if(loop)
                {
                    index = (kzInt)count - 1;
                }
                else
                {
                    runAnimation = KZ_FALSE;
                    index = 0;
                }
            }
            if(index >= (kzInt)count)
            {
                if(loop)
                {
                    index = 0;
                }
                else
                {
                    runAnimation = KZ_FALSE;
                    index = (kzInt)count - 1;
                }
            }
            playIndex = index;

            break;
        }
        case KZU_UI_PLAYLIST_PLAYER_PLAY_MODE_TWO_WAY:
        {
            kzInt propertyValue = previousIndex;
            kzInt oldDirection = (kzUint)propertyValue >> 16;
            kzInt currentValue = (kzUint)propertyValue & 0xFF;

            playIndex = currentValue;

            if(oldDirection == (previous ? 0 : 1))
            {
                playIndex = currentValue + (previous ? -1 : 1);

                if(loop)
                {
                    if(playIndex < 0)
                    {
                        playIndex = (kzInt)count - 1;
                    }
                    else if(playIndex >= (kzInt)count)
                    {
                        playIndex = 0;
                    }
                }
                else
                {
                    playIndex = kzsClampi(playIndex, 0, (kzInt)count - 1);
                }

                /* If we have single item list, we still want to replay the animation even if it did not change. */                    
                if(currentValue == playIndex && !loop)
                {
                    runAnimation = KZ_FALSE;
                }
            }

            currentValue = playIndex;
            oldDirection = (previous ? 0 : 1);

            index = (kzInt)( ((kzUint)oldDirection << 16) | (kzUint)currentValue );

            playBackwards = previous;

            break;
        }
        case KZU_UI_PLAYLIST_PLAYER_PLAY_MODE_SHUFFLE:
        {
            kzInt propertyValue = previousIndex;
            kzInt seed = (kzUint)propertyValue >> 16;
            kzInt maximum = (kzInt)count;
            seed = 1664525L * seed + 1013904223L;
            index = (kzInt)((kzU32)seed >> 16) * (maximum) / 0x10000;                
            playIndex = index;
            index = (kzInt)( ((kzUint)seed << 16) | (kzUint)index );

            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown player play mode.");
        }
    }

    if(out_playIndex != KZ_NULL) *out_playIndex = playIndex;
    if(out_runAnimation != KZ_NULL) *out_runAnimation = runAnimation;
    if(out_indexValue != KZ_NULL) *out_indexValue = index;
    if(out_playBackwards != KZ_NULL) *out_playBackwards = playBackwards;
    kzsSuccess();
}

static kzsError kzuUiPlaylistPlayerPlayAction_internal(const struct KzuUiComponentNode* componentNode, struct KzuStringProperty* targetProperty, void* userData,
                                                       kzBool playBackwards, struct KzcHashMap* parameters)
{
    kzsError result;
    struct KzuIntProperty* property = KZ_NULL;

    result = kzcHashMapPut(parameters, "AnimationItemName", targetProperty);
    kzsErrorForward(result);

    if(playBackwards) 
    {
        result = kzuIntPropertyCreate(kzcMemoryGetManager(componentNode), kzuIntPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_ANIMATION_PLAYBACK_MODE), 1, &property);
        kzsErrorForward(result);
        result = kzcHashMapPut(parameters, "AnimationPlaybackMode", property);
        kzsErrorForward(result);
    }

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_START_ANIMATION, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuUiPlaylistPlayerPlay_internal(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                          kzBool previous, struct KzcHashMap* parameters)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    kzBool runAnimation = KZ_TRUE;

    kzsAssert(kzcIsValidPointer(node));

    {
        kzMutableString* strings;
        kzUint count;
        kzInt index;
        kzInt playIndex;        
        kzInt mode = kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_PLAY_TYPE);
        kzBool playBackwards = KZ_FALSE;
        kzBool loop;

        index = kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_INDEX);

        result = kzcStringSplit(kzcMemoryGetManager(componentNode), kzuObjectNodeGetStringPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_ITEMS), KZU_UI_PLAYLIST_SEPARATOR_STRING, &count, &strings);
        kzsErrorForward(result);

        loop = kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_LOOPING);

        result = kzuUiPlayListPlayerGetIndexToPlay_internal(/*lint -e{930}*/(enum KzuUiPlaylistPlayerPlayMode)mode, previous, loop, index, count, &runAnimation, &index, &playIndex, &playBackwards);
        kzsErrorForward(result);

        result = kzuObjectNodeSetIntProperty(node, KZU_PROPERTY_TYPE_PLAYLIST_INDEX, index);
        kzsErrorForward(result);

        if(runAnimation)
        {
            struct KzuStringProperty* stringProperty;
            kzString string = strings[playIndex];

            result = kzuObjectNodeSetStringProperty(node, KZU_PROPERTY_TYPE_PLAYLIST_ANIMATION_ITEM_NAME, string);
            kzsErrorForward(result);

            result = kzuStringPropertyCreate(kzcMemoryGetManager(componentNode), kzuStringPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_PLAYLIST_ANIMATION_ITEM_NAME), string, &stringProperty);
            kzsErrorForward(result);

            result = kzuUiPlaylistPlayerPlayAction_internal(componentNode, stringProperty, userData, playBackwards, parameters);
            kzsErrorForward(result);

            {
                result = kzcLog(kzcMemoryGetManager(componentNode), KZS_LOG_LEVEL_INFO, "%d: %s", playIndex, strings[playIndex]);
                kzsErrorForward(result);
            }

            {
                struct KzuEngineMessageQueue* queue;
                struct KzuEngineMessageQueueHelper helper;
                helper = *(struct KzuEngineMessageQueueHelper*)userData;
                queue = helper.queue;
                result = kzuUiComponentNodeHandleEvents(componentNode, event, "PlaylistAnimationStarted", queue);
                kzsErrorForward(result);
            }
        }
        else
        {
            /* Clean up event data. */
            {
                struct KzcHashMapIterator it;
                it = kzcHashMapGetIterator(parameters);
                while(kzcHashMapIterate(it))
                {
                    struct KzuProperty* property = (struct KzuProperty*)kzcHashMapIteratorGetValue(it);
                    kzsAssert(kzcIsValidPointer(property));

                    if(!kzuPropertyGetFromKzb(property))
                    {
                        result = kzuPropertyDelete(property);
                        kzsErrorForward(result);
                    }
                }
            }
        }

        {
            kzUint i; 
            for (i = 0; i < kzcArrayLength(strings); ++i)
            {
                result = kzcStringDelete(strings[i]);
                kzsErrorForward(result);
            }
        }

        result = kzcMemoryFreeArray(strings);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackPlaylistPlayerNext(const struct KzuUiComponentNode* componentNode, void* userData,
                                                     struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiPlaylistPlayerPlay_internal(componentNode, userData, event, KZU_UI_ANIMATION_PLAYER_PLAY_NEXT, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackPlaylistPlayerPrevious(const struct KzuUiComponentNode* componentNode, void* userData,
                                                         struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiPlaylistPlayerPlay_internal(componentNode, userData, event, KZU_UI_ANIMATION_PLAYER_PLAY_PREVIOUS, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiPlaylistPlayerInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_PLAYLIST_PLAYER);
    kzsSuccess();
}

kzUint kzuUiPlaylistPlayerGetAnimationCount(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    
    return kzcStringGetSubstringCount(kzuObjectNodeGetStringPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_ITEMS),
                                      KZU_UI_PLAYLIST_SEPARATOR_STRING);
}

kzUint kzuUiPlaylistPlayerGetLastAnimationId(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    kzInt propertyValue = kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_INDEX);

    propertyValue &= 0xFF;

    return (kzUint)propertyValue;
}

kzsError kzuUiPlaylistPlayerSetAnimationId(const struct KzuUiComponentNode* componentNode, kzUint index)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    kzUint count = kzuUiPlaylistPlayerGetAnimationCount(componentNode);
    kzUint newIndex = kzsClampui(index, 0, count);    

    {
        kzInt seed;
        kzInt newValue;
        kzInt propertyValue = kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_INDEX);
        seed = (kzUint)propertyValue >> 16;
        newValue = (kzInt)(((kzUint)seed << 16) | (kzUint)newIndex);
        result = kzuObjectNodeSetIntProperty(node, KZU_PROPERTY_TYPE_PLAYLIST_INDEX, newValue);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuUiPlaylistPlayerGetPath_internal(const struct KzuUiComponentNode* componentNode, kzBool previous, kzMutableString* out_path)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    kzBool runAnimation = KZ_TRUE;
    kzMutableString* strings;
    kzMutableString path;
    kzUint count;
    kzInt index;
    kzInt playIndex;        
    kzBool playBackwards;
    kzBool loop;

    kzsAssert(kzcIsValidPointer(node));

    {
        kzInt mode = kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_PLAY_TYPE);
        index = kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_INDEX);

        result = kzcStringSplit(kzcMemoryGetManager(componentNode), kzuObjectNodeGetStringPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_ITEMS), ";", &count, &strings);
        kzsErrorForward(result);

        loop = kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_PLAYLIST_LOOPING);

        result = kzuUiPlayListPlayerGetIndexToPlay_internal(/*lint -e{930}*/(enum KzuUiPlaylistPlayerPlayMode)mode, previous, loop, index, count, 
            &runAnimation, KZ_NULL, &playIndex, &playBackwards);
        kzsErrorForward(result);

        result = kzcStringCopy(kzcMemoryGetManager(componentNode), strings[playIndex], &path);
        kzsErrorForward(result);
    }

    {
        kzUint i; 
        for (i = 0; i < kzcArrayLength(strings); ++i)
        {
            result = kzcStringDelete(strings[i]);
            kzsErrorForward(result);
        }
    }

    result = kzcMemoryFreeArray(strings);
    kzsErrorForward(result);

     *out_path = path;
    kzsSuccess();
}

kzsError kzuUiPlaylistPlayerGetNext(const struct KzuUiComponentNode* componentNode, kzMutableString* out_path)
{
    kzsError result;
    
    result = kzuUiPlaylistPlayerGetPath_internal(componentNode, KZ_FALSE, out_path);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiPlaylistPlayerGetPrevious(const struct KzuUiComponentNode* componentNode, kzMutableString* out_path)
{
    kzsError result;

    result = kzuUiPlaylistPlayerGetPath_internal(componentNode, KZ_TRUE, out_path);
    kzsErrorForward(result);

    kzsSuccess();
}
