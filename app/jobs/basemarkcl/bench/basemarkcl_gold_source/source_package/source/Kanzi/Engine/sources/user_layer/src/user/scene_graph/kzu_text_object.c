/**
* \file
* Text object.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_text_object.h"

#include "kzu_object.h"
#include "kzu_object_private.h"
#include "bounding_volume/kzu_bounding_volume.h"

#include <user/scene_graph/kzu_transformed_object.h>
#include <user/renderer/kzu_renderer.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_query.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/renderer/kzu_renderer_util.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/font/kzc_text_layouter.h>
#include <core/util/font/kzc_text_layout.h>
#include <core/util/font/kzc_font.h>
#include <core/util/font/kzc_configured_font.h>
#include <core/util/math/kzc_volume.h>
#include <core/renderer/kzc_renderer.h>

#include <system/debug/kzs_log.h>


const kzFloat KZU_TEXT_OBJECT_SCALE = 1.0f / 36.0f; /* A font with size 36 will be rendered as one world unit height. */


/** Text object horizontal alignment. */
enum KzuTextObjectHorizontalAlignment
{
    KZU_BINARY_UI_TEXT_HORIZONTAL_ALIGNMENT_LEFT = 0, /**< Text is aligned left. */
    KZU_BINARY_UI_TEXT_HORIZONTAL_ALIGNMENT_CENTER = 1,  /**< Text is centered horizontally. */
    KZU_BINARY_UI_TEXT_HORIZONTAL_ALIGNMENT_RIGHT = 2 /**< Text is aligned right. */
};

/** Text object vertical alignment. */
enum KzuTextObjectVerticalAlignment
{
    KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_TOP = 0, /**< Text is aligned top. */
    KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_BASELINE = 1, /**< Text is aligned on baseline. */
    KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_CENTER = 2,  /**< Text is centered vertically. */
    KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_BOTTOM = 3  /**< Text is aligned bottom. */
};


/** Object node class for text object. */
extern const struct KzuObjectNodeClass KZU_TEXT_OBJECT_NODE_CLASS;

/* Unique object type for Text object is the memory address of the type. */
const KzuObjectType KZU_OBJECT_TYPE_TEXT_OBJECT = (void*)&KZU_TEXT_OBJECT_NODE_CLASS;


struct KzuTextObjectNode
{
    struct KzuObjectNode objectNode;
};

struct KzuTextObjectNodeData
{
    struct KzuObjectNodeData objectNodeData; /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuTextObjectNode::customObjectNode) Suppressed as this member is required but not referenced. */ 

    struct KzcConfiguredFont* font;
    struct KzcTextLayout* textLayout;
    struct KzuBoundingVolume* boundingVolume;
    void* lastLayoutProperties;
};


static kzsError kzuTextObjectCreateObjectNodeData_internal(const struct KzcMemoryManager* memoryManager,
                                                           struct KzuObjectNodeData** out_textObjectNodeData)
{
    kzsError result;
    struct KzuTextObjectNodeData* textObjectNodeData;

    result = kzcMemoryAllocVariable(memoryManager, textObjectNodeData, "Text object node data");
    kzsErrorForward(result);

    textObjectNodeData->font = KZ_NULL;
    textObjectNodeData->textLayout = KZ_NULL;

    result = kzuBoundingVolumeCreateEmpty(memoryManager, &textObjectNodeData->boundingVolume);
    kzsErrorForward(result);

    result = kzcMemoryAllocPointer(memoryManager, &textObjectNodeData->lastLayoutProperties, 1, "Text object layout properties");
    kzsErrorForward(result);

    *out_textObjectNodeData = &textObjectNodeData->objectNodeData;
    kzsSuccess();
}

kzsError kzuTextObjectNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                 kzString name, struct KzuTextObjectNode** out_textObjectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;
    struct KzuTextObjectNode* textObjectNode;

    result = kzuTextObjectCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_TEXT_OBJECT_NODE_CLASS, name, objectNodeData, &objectNode);
    kzsErrorForward(result);

    textObjectNode = (struct KzuTextObjectNode*)objectNode;

    *out_textObjectNode = textObjectNode;
    kzsSuccess();
}

kzsError kzuTextObjectNodeDelete(struct KzuTextObjectNode* textObjectNode)
{
    kzsError result;
    const struct KzuObjectNode* objectNode = kzuTextObjectNodeToObjectNode((struct KzuTextObjectNode*)textObjectNode);
    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(objectNode);
    struct KzuTextObjectNodeData* textObjectNodeData;

    kzsAssert(kzcIsValidPointer(textObjectNode));

    textObjectNodeData = (struct KzuTextObjectNodeData*)textObjectNode->objectNode.data;

    result = kzuPropertyManagerRemoveProperties(propertyManager, textObjectNodeData->lastLayoutProperties);
    kzsErrorForward(result);

    result = kzcMemoryFreePointer(textObjectNodeData->lastLayoutProperties);
    kzsErrorForward(result);

    if (textObjectNodeData->textLayout != KZ_NULL)
    {
        result = kzcTextLayoutDelete(textObjectNodeData->textLayout);
        kzsErrorForward(result);
    }

    if (textObjectNodeData->font != KZ_NULL)
    {
        result = kzcConfiguredFontDelete(textObjectNodeData->font);
        kzsErrorForward(result);
    }

    result = kzuBoundingVolumeDelete(textObjectNodeData->boundingVolume);
    kzsErrorForward(result);

    result = kzuObjectNodeDeleteBase_private(&textObjectNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuObjectNode* kzuTextObjectNodeToObjectNode(struct KzuTextObjectNode* textObjectNode)
{
    kzsAssert(kzcIsValidPointer(textObjectNode));

    return (struct KzuObjectNode*)&textObjectNode->objectNode;
}

struct KzuTextObjectNode* kzuTextObjectNodeFromObjectNode(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzcIsValidPointer(objectNode));

    return (struct KzuTextObjectNode*)objectNode;
}

static kzBool kzuTextObjectArePropertiesEqual_internal(const struct KzuPropertyManager* propertyManager, const struct KzuObjectNode* objectNode,
                                                       const void* layoutProperties, kzUint propertyCount, const struct KzuPropertyType* const* allPropertyTypes)
{
    kzBool result = KZ_TRUE;
    kzUint i;
    
    for (i = 0; i < propertyCount; ++i)
    {
        const struct KzuPropertyType* propertyType = allPropertyTypes[i];
        struct KzuObjectNode* objectWithProperty = kzuObjectNodeFindHighestPriorityPropertySource(objectNode, propertyType);
        kzBool objectHasProperty = objectWithProperty != KZ_NULL;
        kzBool cacheHasProperty = kzuPropertyManagerHasProperty(propertyManager, layoutProperties, propertyType);
        if (objectHasProperty != cacheHasProperty ||
            !kzuPropertyManagerIsPropertyEqual(propertyManager, objectWithProperty, layoutProperties, propertyType))
        {
            result = KZ_FALSE;
            break;
        }
    }

    return result;
}

static kzsError kzuTextObjectCopyProperties_internal(const struct KzuPropertyManager* propertyManager, const struct KzuObjectNode* objectNode,
                                                     const void* layoutProperties, kzUint propertyCount, const struct KzuPropertyType* const* allPropertyTypes)
{
    kzsError result;
    kzUint i;
    
    for (i = 0; i < propertyCount; ++i)
    {
        const struct KzuPropertyType* propertyType = allPropertyTypes[i];
        struct KzuObjectNode* objectWithProperty = kzuObjectNodeFindHighestPriorityPropertySource(objectNode, propertyType);

        if (objectWithProperty != KZ_NULL)
        {
            kzsAssert(propertyManager == kzuObjectNodeGetPropertyManager(objectWithProperty));
            /* TODO: Add copy property function for object node, which asserts that the property managers are equal */

            result = kzuPropertyManagerCopyProperty(propertyManager, objectWithProperty, layoutProperties, propertyType);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuTextObjectNodeValidate(const struct KzuTextObjectNode* textObjectNode, const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsError result;
    const struct KzuObjectNode* objectNode = kzuTextObjectNodeToObjectNode((struct KzuTextObjectNode*)textObjectNode);
    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(objectNode);

    {
        struct KzcMemoryManager* quickMemoryManager = kzcMemoryGetManager(transformedObjectNode);
        struct KzuTextObjectNodeData* textObjectNodeData = (struct KzuTextObjectNodeData*)textObjectNode->objectNode.data;
        void* layoutProperties = textObjectNodeData->lastLayoutProperties;

        const struct KzuPropertyType* allPropertyTypes[12];
        allPropertyTypes[0] = KZU_PROPERTY_TYPE_LABEL_TEXT;
        allPropertyTypes[1] = KZU_PROPERTY_TYPE_LABEL_FONT_SIZE;
        allPropertyTypes[2] = KZU_PROPERTY_TYPE_LABEL_HORIZONTAL_ALIGNMENT;
        allPropertyTypes[3] = KZU_PROPERTY_TYPE_LABEL_VERTICAL_ALIGNMENT;
        allPropertyTypes[4] = KZU_PROPERTY_TYPE_LABEL_LINE_SPACING;
        allPropertyTypes[5] = KZU_PROPERTY_TYPE_LABEL_COLOR;
        allPropertyTypes[6] = KZU_PROPERTY_TYPE_LABEL_MAXIMUM_WIDTH;
        allPropertyTypes[7] = KZU_PROPERTY_TYPE_LABEL_MAXIMUM_CHARACTERS_PER_LINE;
        allPropertyTypes[8] = KZU_PROPERTY_TYPE_LABEL_MAXIMUM_HEIGHT;
        allPropertyTypes[9] = KZU_PROPERTY_TYPE_LABEL_MAXIMUM_LINES;
        allPropertyTypes[10] = KZU_PROPERTY_TYPE_LABEL_TERMINATOR;
        allPropertyTypes[11] = KZU_PROPERTY_TYPE_LABEL_FONT;

        kzsAssert(kzcIsValidPointer(textObjectNode));

        textObjectNodeData = (struct KzuTextObjectNodeData*)textObjectNode->objectNode.data;

        /* TODO: Remove when not needed */
        {
            kzUint i;

            for (i = 0; i < 12; ++i)
            {
                kzuPropertyTypeSetInherited(allPropertyTypes[i], KZ_TRUE);
            }
        }

        if (!kzuTextObjectArePropertiesEqual_internal(propertyManager, objectNode, textObjectNodeData->lastLayoutProperties, 12, allPropertyTypes))
        {
            struct KzcFont* font;

            result = kzuPropertyManagerRemoveProperties(propertyManager, textObjectNodeData->lastLayoutProperties);
            kzsErrorForward(result);

            if (textObjectNodeData->textLayout != KZ_NULL)
            {
                result = kzcTextLayoutDelete(textObjectNodeData->textLayout);
                kzsErrorForward(result);
            }

            if (textObjectNodeData->font != KZ_NULL)
            {
                result = kzcConfiguredFontDelete(textObjectNodeData->font);
                kzsErrorForward(result);
            }

            result = kzuTextObjectCopyProperties_internal(propertyManager, objectNode, layoutProperties, 12, allPropertyTypes);
            kzsErrorForward(result);

            font = (struct KzcFont*)kzuPropertyManagerGetVoidDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_FONT);

            if (font != KZ_NULL)
            {
                /* TODO: Change width and height properties to float type */
                struct KzcConfiguredFont* configuredFont;
                struct KzcTextLayouter* textLayouter;
                struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(textObjectNode);
                kzString text = kzuPropertyManagerGetStringDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_TEXT);
                kzFloat fontSize = kzuPropertyManagerGetFloatDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_FONT_SIZE);
                struct KzcColorRGBA color = kzuPropertyManagerGetColorDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_COLOR);

                result = kzcConfiguredFontCreate(memoryManager, font, fontSize, color, &configuredFont);
                kzsErrorForward(result);

                textObjectNodeData->font = configuredFont;

                font = kzcConfiguredFontToFont(configuredFont);

                result = kzcTextLayouterCreate(quickMemoryManager, font, &textLayouter);
                kzsErrorForward(result);

                /* Parse horizontal alignment. */
                {
                    enum KzcTextHorizontalAlignment horizontalAlignment;
                    switch (kzuPropertyManagerGetIntDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_HORIZONTAL_ALIGNMENT))
                    {
                        case KZU_BINARY_UI_TEXT_HORIZONTAL_ALIGNMENT_LEFT:   horizontalAlignment = KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT;   break;
                        case KZU_BINARY_UI_TEXT_HORIZONTAL_ALIGNMENT_CENTER: horizontalAlignment = KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER; break;
                        case KZU_BINARY_UI_TEXT_HORIZONTAL_ALIGNMENT_RIGHT:  horizontalAlignment = KZC_TEXT_HORIZONTAL_ALIGNMENT_RIGHT;  break;
                        default:
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid horizontal alignment for text label");
                            horizontalAlignment = KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT;
                            break;
                        }
                    }
                    kzcTextLayouterSetHorizontalAlignment(textLayouter, horizontalAlignment);
                }

                /* Parse vertical alignment. */
                {
                    enum KzcTextVerticalAlignment verticalAlignment;
                    switch (kzuPropertyManagerGetIntDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_VERTICAL_ALIGNMENT))
                    {
                        case KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_TOP:      verticalAlignment = KZC_TEXT_VERTICAL_ALIGNMENT_TOP;      break;
                        case KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_BASELINE: verticalAlignment = KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE; break;
                        case KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_CENTER:   verticalAlignment = KZC_TEXT_VERTICAL_ALIGNMENT_CENTER;   break;
                        case KZU_BINARY_UI_TEXT_VERTICAL_ALIGNMENT_BOTTOM:   verticalAlignment = KZC_TEXT_VERTICAL_ALIGNMENT_BOTTOM;   break;
                        default:
                        {
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid vertical alignment for text label");
                            verticalAlignment = KZC_TEXT_VERTICAL_ALIGNMENT_TOP;
                            break;
                        }
                    }
                    kzcTextLayouterSetVerticalAlignment(textLayouter, verticalAlignment);
                }

                kzcTextLayouterSetMaximumWidth(textLayouter, (kzFloat)kzuPropertyManagerGetIntDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_MAXIMUM_WIDTH));
                kzcTextLayouterSetMaximumCharactersPerLine(textLayouter, (kzUint)kzuPropertyManagerGetIntDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_MAXIMUM_CHARACTERS_PER_LINE));
                kzcTextLayouterSetMaximumHeight(textLayouter, (kzFloat)kzuPropertyManagerGetIntDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_MAXIMUM_HEIGHT));
                kzcTextLayouterSetMaximumLines(textLayouter, (kzUint)kzuPropertyManagerGetIntDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_MAXIMUM_LINES));
                result = kzcTextLayouterSetTerminator(textLayouter, kzuPropertyManagerGetStringDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_TERMINATOR));
                kzsErrorForward(result);
                kzcTextLayouterSetLineSpacing(textLayouter, kzuPropertyManagerGetFloatDefault(propertyManager, layoutProperties, KZU_PROPERTY_TYPE_LABEL_LINE_SPACING));

                result = kzcTextLayouterCreateLayout(textLayouter, memoryManager, text, &textObjectNodeData->textLayout);
                kzsErrorForward(result);

                result = kzcTextLayouterDelete(textLayouter);
                kzsErrorForward(result);

                /* Bounding volume */
                {
                    struct KzcVolume volume = kzcVolume(textObjectNodeData->textLayout->left * KZU_TEXT_OBJECT_SCALE, textObjectNodeData->textLayout->bottom * KZU_TEXT_OBJECT_SCALE, 0.0f,
                                                        textObjectNodeData->textLayout->width * KZU_TEXT_OBJECT_SCALE, textObjectNodeData->textLayout->height * KZU_TEXT_OBJECT_SCALE, 0.0f);
                    kzuBoundingVolumeSetVolume(textObjectNodeData->boundingVolume, &volume);
                }
            }
        }
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuTextObjectNodeDeleteImplementation_internal(struct KzuObjectNode* objectNode)
{
    kzsError result = kzuTextObjectNodeDelete(kzuTextObjectNodeFromObjectNode(objectNode));
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuTextObjectNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                         const struct KzuObjectNode* sourceObjectNode,
                                                                         struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;

    result = kzuTextObjectCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    *out_objectNode = objectNode;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuTextObjectNodeRender_internal(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);
    struct KzuTextObjectNodeData* textObjectNodeData;
    struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
    struct KzcMatrix4x4 matrix;
    struct KzcMatrix4x4 oldMatrix;
    
    kzsAssert(kzcIsValidPointer(objectNode));

    textObjectNodeData = (struct KzuTextObjectNodeData*)objectNode->data;

    oldMatrix = kzcRendererGetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD);
    matrix = oldMatrix;
    kzcMatrix4x4ScaleUniform(&matrix, KZU_TEXT_OBJECT_SCALE);
    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, &matrix);

    /* Apply override callback for text objects, too. */
    if (kzuRendererGetOverrideMaterialCallback(renderer) != KZ_NULL)
    {
        struct KzuMaterial* overrideMaterial;
        KzuRendererOverrideMaterialCallback materialCallback = kzuRendererGetOverrideMaterialCallback(renderer);
        void* materialCallbackUserData = kzuRendererGetOverrideMaterialCallbackUserData(renderer);
        struct KzuTransformedObjectNode* parentTransformedObject;

        parentTransformedObject = kzuTransformedObjectNodeGetParent(transformedObjectNode);

        kzsAssert(materialCallback != KZ_NULL);

        result = materialCallback(parentTransformedObject, KZ_NULL, 0, materialCallbackUserData, &overrideMaterial);
        kzsErrorForward(result);

        result = kzuRendererApplyMaterial(renderer, overrideMaterial);
        kzsErrorForward(result);
        result = kzuRendererDrawBoundingVolumeSolid(renderer, textObjectNodeData->boundingVolume, &oldMatrix);
        kzsErrorForward(result);
    }
    else
    {
        /* TODO: Remove this after 2.0 and fix possible places where text layout could be non-null. */
        if (textObjectNodeData->textLayout != KZ_NULL)
        {
            /* Text color */
            {
                struct KzuPropertyQuery* propertyQuery = kzuRendererGetPropertyQuery(renderer);
                struct KzcColorRGBA color = kzuPropertyQueryGetColor(propertyQuery, KZU_PROPERTY_TYPE_LABEL_COLOR);

                result = kzcConfiguredFontSetColor(textObjectNodeData->font, color);
                kzsErrorForward(result);
            }

            result = kzcFontDrawTextLayout3D(textObjectNodeData->textLayout, kzuRendererGetCoreRenderer(renderer));
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

KZ_CALLBACK static struct KzuBoundingVolume* kzuTextObjectGetBoundingVolume_internal(const struct KzuObjectNode* objectNode)
{
    struct KzuTextObjectNodeData* textObjectNodeData;
    
    kzsAssert(kzcIsValidPointer(objectNode));

    textObjectNodeData = (struct KzuTextObjectNodeData*)objectNode->data;

    return textObjectNodeData->boundingVolume;
}

const struct KzuObjectNodeClass KZU_TEXT_OBJECT_NODE_CLASS =
{
    kzuTextObjectNodeDeleteImplementation_internal,
    kzuTextObjectNodeCopyImplementation_internal,
    kzuTextObjectNodeRender_internal,
    kzuTextObjectGetBoundingVolume_internal
};
