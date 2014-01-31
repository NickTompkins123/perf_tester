/**
* \file
* User interface grid layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_grid_layout.h"
#include "kzu_ui_component_common.h"

#include <user/ui/kzu_ui_component_base.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>


/* TODO: Document all structs and members. */
enum KzuUiGridLayoutSizeMode
{
    KZU_UI_GRID_LAYOUT_SIZE_USER    = 0,
    KZU_UI_GRID_LAYOUT_SIZE_AUTO    = 1,
    KZU_UI_GRID_LAYOUT_SIZE_WEIGHED = 2
};

struct KzuUiGridLayoutDefinition
{
    enum KzuUiGridLayoutSizeMode mode;
    kzFloat userSize;
    kzFloat maxSize;
    kzFloat offset;
};

struct KzuUiGridLayoutData
{
    struct KzcDynamicArray* rows; /**< Row definitions <KzuUiGridLayoutDefinition>. */
    struct KzcDynamicArray* columns; /**< Column definitions <KzuUiGridLayoutDefinition>. */
};


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_GRID_LAYOUT = (void*)&KZU_COMPONENT_TYPE_GRID_LAYOUT;


/** Copy an array of grid layout definitions. */
static kzsError kzuUiGridLayoutDefinitionArrayCopy_internal(const struct KzcMemoryManager* memoryManager,
                                                            const struct KzcDynamicArray* sourceArray,
                                                            struct KzcDynamicArray** out_targetArray)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    struct KzcDynamicArray* targetArray;

    result = kzcDynamicArrayCreate(memoryManager, &targetArray);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(sourceArray);
    while(kzcDynamicArrayIterate(it))
    {
        const struct KzuUiGridLayoutDefinition* sourceDefinition;
        struct KzuUiGridLayoutDefinition* targetDefinition;

        sourceDefinition = (const struct KzuUiGridLayoutDefinition*)kzcDynamicArrayIteratorGetValue(it);

        kzsAssert(kzcIsValidPointer(sourceDefinition));
        
        result = kzcMemoryAllocVariable(memoryManager, targetDefinition, "GridLayoutData");
        kzsErrorForward(result);

        *targetDefinition = *sourceDefinition;
        
        result = kzcDynamicArrayAdd(targetArray, targetDefinition);
        kzsErrorForward(result);
    }

    *out_targetArray = targetArray;
    kzsSuccess();
}

/** Delete a dynamic array consisting of grid layout definitions. */
static kzsError kzuUiGridLayoutDefinitionArrayDelete_internal(struct KzcDynamicArray* definitionArray)
{
    kzsError result;
    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(definitionArray);

    while(kzcDynamicArrayIterate(it))
    {
        struct KzuUiGridLayoutDefinition* definition = kzcDynamicArrayIteratorGetValue(it);

        result = kzcMemoryFreeVariable(definition);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(definitionArray);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzUint kzuUiGridLayoutGetAddressProperty_internal(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzUint minValue)
{
    kzUint address = minValue;
    kzInt value;

    kzsAssert(kzuObjectNodeIsValid(objectNode));

    if (kzuObjectNodeGetIntProperty(objectNode, propertyType, &value))
    {
        if (value >= 0 && (kzUint)value >= minValue)
        {
            address = (kzUint)value;
        }
    }

    return address;
}

static struct KzuUiGridLayoutData* kzuUiGridLayoutGetData_internal(const struct KzuObjectNode* objectNode)
{
    return kzuObjectNodeGetVoidPropertyDefault(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_DATA);
}

static kzsError kzuUiGridLayoutParseDefinitions_internal(const struct KzcMemoryManager* memoryManager, kzString definitionCollection, struct KzcDynamicArray* array)
{
    kzsError result;
    kzUint i;
    kzUint count;
    kzMutableString* splitDefinitions;

    kzcDynamicArrayClear(array);

    result = kzcStringSplit(memoryManager, definitionCollection, ";", &count, &splitDefinitions);
    kzsErrorForward(result);

    kzsAssert(count > 0);

    for (i = 0; i < count; ++i)
    {
        kzMutableString definitionString = splitDefinitions[i];

        if (i < count - 1)
        {
            struct KzuUiGridLayoutDefinition* definitionEntry;

            result = kzcMemoryAllocVariable(memoryManager, definitionEntry, "GridLayoutDefinition");
            kzsErrorForward(result);

            definitionEntry->userSize = 0.0f;
            definitionEntry->maxSize = 0.0f;
            definitionEntry->offset = 0.0f;

            result = kzcDynamicArrayAdd(array, definitionEntry);
            kzsErrorForward(result);

            if (kzcStringLength(definitionString) == 0)
            {
                definitionEntry->mode = KZU_UI_GRID_LAYOUT_SIZE_AUTO;
            }
            else
            {
                if (definitionString[0] == '*')
                {
                    definitionEntry->mode = KZU_UI_GRID_LAYOUT_SIZE_WEIGHED;
                    definitionEntry->userSize = kzcStringToFloat(definitionString + 1);
                }
                else
                {
                    definitionEntry->mode = KZU_UI_GRID_LAYOUT_SIZE_USER;
                    definitionEntry->userSize = kzcStringToFloat(definitionString);
                }
            }
        }
        
        result = kzcStringDelete(definitionString);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeArray(splitDefinitions);
    kzsErrorForward(result);

    kzsSuccess();
}

static void kzuUiGridLayoutResetDefinitions_internal(const struct KzcDynamicArray* array)
{
    kzUint i;
    kzUint count = kzcDynamicArrayGetSize(array);
    for (i = 0; i != count; ++i)
    {
        struct KzuUiGridLayoutDefinition* definition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(array, i);
        kzsAssert(definition != KZ_NULL);

        definition->maxSize = 0.0f;
        definition->offset = 0.0f;

        if (definition->mode == KZU_UI_GRID_LAYOUT_SIZE_USER)
        {
            definition->maxSize = definition->userSize;
        }
    }
}

static kzFloat kzuUiGridLayoutFindMaximumAccumulated_internal(const struct KzcDynamicArray* array)
{
    kzUint i;
    kzFloat accumulated = 0.0f;
    kzUint count = kzcDynamicArrayGetSize(array);
    for (i = 0; i != count; ++i)
    {
        struct KzuUiGridLayoutDefinition* definition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(array, i);
        kzsAssert(definition != KZ_NULL);

        accumulated += definition->maxSize;
    }

    return accumulated;
}

static void kzuUiGridLayoutCalculateOffsets_internal(const struct KzcDynamicArray* array)
{
    kzUint i;
    kzFloat accumulated = 0.0f;
    kzUint count = kzcDynamicArrayGetSize(array);
    for (i = 0; i != count; ++i)
    {
        struct KzuUiGridLayoutDefinition* definition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(array, i);
        kzsAssert(definition != KZ_NULL);

        definition->offset = accumulated;
        accumulated += definition->maxSize;
    }
}

KZ_CALLBACK static kzsError kzuUiGridLayoutMeasure(struct KzuTransformedObjectNode* transformedNode)
{
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);
    struct KzuUiGridLayoutData* data = kzuUiGridLayoutGetData_internal(objectNode);
    struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);
    kzUint rowCount;
    kzUint columnCount;
    kzFloat maxZ = 0.0f;

    kzsAssert(data != KZ_NULL);

    rowCount = kzcDynamicArrayGetSize(data->rows);
    columnCount = kzcDynamicArrayGetSize(data->columns);
    
    kzuUiGridLayoutResetDefinitions_internal(data->rows);
    kzuUiGridLayoutResetDefinitions_internal(data->columns);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        struct KzuObjectNode* childNode = kzuTransformedObjectNodeGetObjectNode(transformedChildNode);
        const kzUint row = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW, 0);
        const kzUint column = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN, 0);        

        if (row < rowCount && column < columnCount)
        {
            kzUint i, end;
            kzUint rowSpan = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_SPAN, 1);
            kzUint columnSpan = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_SPAN, 1);

            struct KzcVector3 childMin, childMax;
            struct KzcVector3 childSize;
            kzuTransformedObjectNodeGetDesiredSize(transformedChildNode, &childMin, &childMax);
            kzcVector3Subtract(&childMax, &childMin, &childSize);

            /* Limit the spans to the grid boundaries. */
            if (row + rowSpan > rowCount)
            {
                rowSpan = rowCount - row;
            }
            if (column + columnSpan > columnCount)
            {
                columnSpan = columnCount - column;
            }

            /* Child size is even part between all spanned rows or columns. */
            if (rowSpan > 1)
            {
                kzFloat evenPart = kzcVector3GetY(&childSize) / (kzFloat)rowSpan;
                kzcVector3SetY(&childSize, evenPart);
            }
            if (columnSpan > 1)
            {
                kzFloat evenPart = kzcVector3GetX(&childSize) / (kzFloat)columnSpan;
                kzcVector3SetX(&childSize, evenPart);
            }

            /* Update all rows. */
            for (i = row, end = row + rowSpan; i != end; ++i)
            {
                struct KzuUiGridLayoutDefinition* rowDefinition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(data->rows, i);
                kzsAssert(rowDefinition != KZ_NULL);

                /* For rows the maximum size is Y axis. */
                if (rowDefinition->mode == KZU_UI_GRID_LAYOUT_SIZE_AUTO)
                {
                    rowDefinition->maxSize = kzsMaxf(rowDefinition->maxSize, kzcVector3GetY(&childSize));
                }
            }

            /* Update all columns. */
            for (i = column, end = column + columnSpan; i != end; ++i)
            {
                struct KzuUiGridLayoutDefinition* columnDefinition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(data->columns, i);
                kzsAssert(columnDefinition != KZ_NULL);

                /* For columns the maximum size is X axis. */
                if (columnDefinition->mode == KZU_UI_GRID_LAYOUT_SIZE_AUTO)
                {
                    columnDefinition->maxSize = kzsMaxf(columnDefinition->maxSize, kzcVector3GetX(&childSize));
                }
            }

            maxZ = kzsMaxf(maxZ, kzcVector3GetZ(&childSize));
        }
    }

    /* Calculate bounding boxes. */
    {
        struct KzcVector3 halfSize;
        struct KzcVector3 aabbMin, aabbMax;
        struct KzcVector3 contentMin, contentMax;
        struct KzcVector3 desiredMin, desiredMax;
        struct KzcVector3 contentSize = kzcVector3(kzuUiGridLayoutFindMaximumAccumulated_internal(data->columns),
                                                   kzuUiGridLayoutFindMaximumAccumulated_internal(data->rows),
                                                   maxZ);

        kzcVector3Scale(&contentSize, 0.5f, &halfSize);
        kzcVector3Add(&KZC_VECTOR3_ZERO, &halfSize, &aabbMax);
        kzcVector3Subtract(&KZC_VECTOR3_ZERO, &halfSize, &aabbMin);

        /* Final content size is content size restricted by user defined dimensions. */
        kzuObjectNodeCalculateUserRestrictedSize(objectNode, &aabbMin, &aabbMax, &contentMin, &contentMax);
        kzuTransformedObjectNodeSetContentSize(transformedNode, &contentMin, &contentMax);

        /* Calculate desired size. */
        kzuTransformedObjectNodeCalculateDesiredSize(transformedNode, &contentMin, &contentMax, &desiredMin, &desiredMax);
        kzuTransformedObjectNodeSetDesiredSize(transformedNode, &desiredMin, &desiredMax);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiGridLayoutArrange(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode,
                                                   struct KzuTransformedObjectNode* transformedParentNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);
    struct KzuUiGridLayoutData* data = kzuUiGridLayoutGetData_internal(objectNode);
    kzUint rowCount;
    kzUint columnCount;
    struct KzcVector3 contentMin, contentMax;
    struct KzcDynamicArrayIterator it;

    kzsAssert(data != KZ_NULL);

    rowCount = kzcDynamicArrayGetSize(data->rows);
    columnCount = kzcDynamicArrayGetSize(data->columns);

    kzuTransformedObjectNodeDefaultArrange(transformedNode);

    kzuTransformedObjectNodeGetContentSize(transformedNode, &contentMin, &contentMax);

    /* TODO: Calculate weighed rows and columns. */
    kzuUiGridLayoutCalculateOffsets_internal(data->rows);
    kzuUiGridLayoutCalculateOffsets_internal(data->columns);

    /* Position objects */
    it = kzuTransformedObjectNodeGetChildren(transformedNode);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        struct KzuObjectNode* childNode = kzuTransformedObjectNodeGetObjectNode(transformedChildNode);
        const kzUint row = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW, 0);
        const kzUint column = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN, 0);
        struct KzcVector3 childMin = KZC_VECTOR3_ZERO;
        struct KzcVector3 childMax = KZC_VECTOR3_ZERO;

        if (row < rowCount && column < columnCount)
        {
            kzUint i, end;
            kzUint rowSpan = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_SPAN, 1);
            kzUint columnSpan = kzuUiGridLayoutGetAddressProperty_internal(childNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_SPAN, 1);
            struct KzuUiGridLayoutDefinition* firstRowDefinition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(data->rows, row);
            struct KzuUiGridLayoutDefinition* firstColumnDefinition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(data->columns, column);

            kzsAssert(firstRowDefinition != KZ_NULL);
            kzsAssert(firstColumnDefinition != KZ_NULL);

            /* Limit the spans to the grid boundaries. */
            if (row + rowSpan > rowCount)
            {
                rowSpan = rowCount - row;
            }
            if (column + columnSpan > columnCount)
            {
                columnSpan = columnCount - column;
            }

            kzcVector3SetX(&childMin, kzcVector3GetX(&contentMin) + firstColumnDefinition->offset);
            kzcVector3SetY(&childMax, kzcVector3GetY(&contentMax) - firstRowDefinition->offset);

            kzcVector3SetX(&childMax, kzcVector3GetX(&childMin));
            kzcVector3SetY(&childMin, kzcVector3GetY(&childMax));

            for (i = row, end = row + rowSpan; i != end; ++i)
            {
                struct KzuUiGridLayoutDefinition* rowDefinition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(data->rows, i);
                kzsAssert(rowDefinition != KZ_NULL);

                kzcVector3SetY(&childMin, kzcVector3GetY(&childMin) - rowDefinition->maxSize);
            }

            for (i = column, end = column + columnSpan; i != end; ++i)
            {
                struct KzuUiGridLayoutDefinition* columnDefinition = (struct KzuUiGridLayoutDefinition*)kzcDynamicArrayGet(data->columns, i);
                kzsAssert(columnDefinition != KZ_NULL);

                kzcVector3SetX(&childMax, kzcVector3GetX(&childMax) + columnDefinition->maxSize);
            }

            kzcVector3SetZ(&childMin, kzcVector3GetZ(&contentMin));
            kzcVector3SetZ(&childMax, kzcVector3GetZ(&contentMax));
        }

        kzuTransformedObjectNodeSetLayoutSize(transformedChildNode, &childMin, &childMax);

        result = kzuSceneArrangeNode(scene, transformedChildNode, transformedNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiGridLayoutSpecificCopy(const struct KzuObjectNode* sourceNode, struct KzuObjectNode* targetNode)
{
    kzsError result;
    const struct KzuUiGridLayoutData* sourceData;
    struct KzuUiGridLayoutData* targetData;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(targetNode);

    sourceData = kzuUiGridLayoutGetData_internal(sourceNode);
    kzsAssert(kzcIsValidPointer(sourceData));

    result = kzcMemoryAllocVariable(memoryManager, targetData, "GridLayoutData");
    kzsErrorForward(result);

    result = kzuUiGridLayoutDefinitionArrayCopy_internal(memoryManager, sourceData->columns, &targetData->columns);
    kzsErrorForward(result);

    result = kzuUiGridLayoutDefinitionArrayCopy_internal(memoryManager, sourceData->rows, &targetData->rows);
    kzsErrorForward(result);

    result = kzuObjectNodeSetVoidProperty(targetNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_DATA, targetData);
    kzsErrorForward(result);
    
    kzsSuccess();
}                                                   

static kzsError kzuUiGridLayoutInitialize_internal(const struct KzcMemoryManager* memoryManager, const struct KzuUiComponentNode* componentNode)
{
    kzsError result;
    struct KzuUiGridLayoutData* data;
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
    kzString rowDefinitions, columnDefinitions;

    result = kzcMemoryAllocVariable(memoryManager, data, "GridLayoutData");
    kzsErrorForward(result);
    
    result = kzuObjectNodeSetVoidProperty(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_DATA, data);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &data->rows);
    kzsErrorForward(result);
    result = kzcDynamicArrayCreate(memoryManager, &data->columns);
    kzsErrorForward(result);

    rowDefinitions = kzuObjectNodeGetStringPropertyDefault(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_DEFINITIONS);
    columnDefinitions = kzuObjectNodeGetStringPropertyDefault(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_DEFINITIONS);

    result = kzuUiGridLayoutParseDefinitions_internal(memoryManager, rowDefinitions, data->rows);
    kzsErrorForward(result);
    result = kzuUiGridLayoutParseDefinitions_internal(memoryManager, columnDefinitions, data->columns);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiGridLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzsError result;

    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_GRID_LAYOUT);
    kzuUiComponentNodeSetMeasureFunction(componentNode, kzuUiGridLayoutMeasure);
    kzuUiComponentNodeSetArrangeFunction(componentNode, kzuUiGridLayoutArrange);
    kzuUiComponentNodeSetSpecificCopyFunction(componentNode, kzuUiGridLayoutSpecificCopy);

    result = kzuUiGridLayoutInitialize_internal(memoryManager, componentNode);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiGridLayoutUninitialize(struct KzuUiComponentNode* componentNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode;
    struct KzuUiGridLayoutData* data;

    objectNode = kzuUiComponentNodeToObjectNode(componentNode);
    data = kzuUiGridLayoutGetData_internal(objectNode);

    kzsAssert(data != KZ_NULL);

    result = kzuUiGridLayoutDefinitionArrayDelete_internal(data->columns);
    kzsErrorForward(result);

    result = kzuUiGridLayoutDefinitionArrayDelete_internal(data->rows);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(data);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzuUiGridLayoutGetRowCount(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
    struct KzuUiGridLayoutData* data = kzuUiGridLayoutGetData_internal(objectNode);
    
    kzsAssert(kzcIsValidPointer(data));

    return kzcDynamicArrayGetSize(data->rows);
}

kzUint kzuUiGridLayoutGetColumnCount(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
    struct KzuUiGridLayoutData* data = kzuUiGridLayoutGetData_internal(objectNode);

    kzsAssert(kzcIsValidPointer(data));

    return kzcDynamicArrayGetSize(data->columns);
}

kzFloat kzuUiGridLayoutGetActualRowSize(const struct KzuUiComponentNode* componentNode, kzUint rowIndex)
{
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
    struct KzuUiGridLayoutData* data = kzuUiGridLayoutGetData_internal(objectNode);
    struct KzuUiGridLayoutDefinition* rowDefinition;

    kzsAssert(kzcIsValidPointer(data));

    rowDefinition = kzcDynamicArrayGet(data->rows, rowIndex);
    kzsAssert(kzcIsValidPointer(rowDefinition));

    return rowDefinition->maxSize;
}

kzFloat kzuUiGridLayoutGetActualColumnSize(const struct KzuUiComponentNode* componentNode, kzUint columnIndex)
{
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
    struct KzuUiGridLayoutData* data = kzuUiGridLayoutGetData_internal(objectNode);
    struct KzuUiGridLayoutDefinition* columnDefinition;

    kzsAssert(kzcIsValidPointer(data));

    columnDefinition = kzcDynamicArrayGet(data->columns, columnIndex);
    kzsAssert(kzcIsValidPointer(columnDefinition));

    return columnDefinition->maxSize;
}

kzsError kzuUiGridLayoutSetObjectRow(const struct KzuObjectNode* objectNode, kzUint row)
{
    kzsError result;

    result = kzuObjectNodeSetIntProperty(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW, (kzInt)row);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzuUiGridLayoutGetObjectRow(const struct KzuObjectNode* objectNode)
{
    return (kzUint)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW);
}

kzsError kzuUiGridLayoutSetObjectRowSpan(const struct KzuObjectNode* objectNode, kzUint rowSpan)
{
    kzsError result;

    result = kzuObjectNodeSetIntProperty(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_SPAN, (kzInt)rowSpan);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzuUiGridLayoutGetObjectRowSpan(const struct KzuObjectNode* objectNode)
{
    return (kzUint)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_SPAN);
}

kzsError kzuUiGridLayoutSetObjectColumn(const struct KzuObjectNode* objectNode, kzUint column)
{
    kzsError result;

    result = kzuObjectNodeSetIntProperty(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN, (kzInt)column);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzuUiGridLayoutGetObjectColumn(const struct KzuObjectNode* objectNode)
{
    return (kzUint)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN);
}

kzsError kzuUiGridLayoutSetObjectColumnSpan(const struct KzuObjectNode* objectNode, kzUint columnSpan)
{
    kzsError result;

    result = kzuObjectNodeSetIntProperty(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_SPAN, (kzInt)columnSpan);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzuUiGridLayoutGetObjectColumnSpan(const struct KzuObjectNode* objectNode)
{
    return (kzUint)kzuObjectNodeGetIntPropertyDefault(objectNode, KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_SPAN);
}
