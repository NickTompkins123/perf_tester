/**
* \file
* User renderer utilities, e.g. wireframe grid, axes, debug.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_renderer_util.h"

#include "kzu_renderer.h"

#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/material/kzu_material.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/components/kzu_ui_grid_layout.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>


/** Number of lines in a circle in layout visualizations. */
#define KZU_LAYOUT_VISUALIZATION_DETAIL 13

/** Size of the circles in layout visualizations. */ 
#define KZU_LAYOUT_VISUALIZATION_SCALE 0.025f

/** Amount the layout visualization circles poke forward. */
#define KZU_LAYOUT_VISUALIZATION_POKE_OUT 0.01f


kzsError kzuRendererSetSolidColor(const struct KzuRenderer* renderer, const struct KzcColorRGBA* color)
{
    kzsError result;
    struct KzuMaterial* solidColorMaterial = kzuRendererGetSolidColorMaterial(renderer);

    result = kzuPropertyManagerSetColor(kzuMaterialGetPropertyManager(solidColorMaterial), solidColorMaterial, KZU_PROPERTY_TYPE_EMISSIVE, *color);
    kzsErrorForward(result);

    kzsSuccess();
}

static void kzuRendererAddXYZToArray_internal(kzFloat x, kzFloat y, kzFloat z, kzFloat** destinationArray)
{
    kzFloat* array = (*destinationArray);

    *(array++) = x;
    *(array++) = y;
    *(array++) = z;

    *destinationArray = array;
}

static void kzuRendererAddVector3ToArray_internal(const struct KzcVector3* vector, kzFloat** destinationArray)
{
    kzFloat* array = (*destinationArray);

    *(array++) = kzcVector3GetX(vector);
    *(array++) = kzcVector3GetY(vector);
    *(array++) = kzcVector3GetZ(vector);

    *destinationArray = array;
}

static void kzuRendererAddVector3ToArray2_internal(struct KzcVector3 stackVector, kzFloat** destinationArray)
{
    kzuRendererAddVector3ToArray_internal(&stackVector, destinationArray);
}

kzsError kzuRendererDrawWireframeGrid(struct KzuRenderer* renderer, kzFloat spacing)
{
    kzsError result;
    kzInt i;
    kzFloat* vertices;
    kzFloat* verticesCenter;
    kzFloat* originalData;
    kzFloat* axes;
    kzFloat* floatValueArray = kzuRendererGetFloatBuffer(renderer);

    kzsAssert(kzcIsValidPointer(floatValueArray));

    if(spacing <= 0.0f)
    {
        spacing = 1.0f;
    }

    vertices = (kzFloat*)&floatValueArray[0];
    verticesCenter = (kzFloat*)&floatValueArray[2 * 3 * (20 + 20)];
    axes = (kzFloat*)&floatValueArray[2 * 3 * (20 + 20) + 2 * 3 * 2];

    originalData = axes;
    kzuRendererAddVector3ToArray_internal(&KZC_VECTOR3_ZERO, &axes);
    kzuRendererAddVector3ToArray_internal(&KZC_VECTOR3_X_AXIS, &axes);
    kzuRendererAddVector3ToArray_internal(&KZC_VECTOR3_ZERO, &axes);
    kzuRendererAddVector3ToArray_internal(&KZC_VECTOR3_Y_AXIS, &axes);
    kzuRendererAddVector3ToArray_internal(&KZC_VECTOR3_ZERO, &axes);
    kzuRendererAddVector3ToArray_internal(&KZC_VECTOR3_Z_AXIS, &axes);
    axes = originalData;

    originalData = vertices;
    for(i = -10; i < 11; ++i)
    {
        if(i != 0)
        {
            struct KzcVector3 lineHorizontal1 = kzcVector3(-10.0f * spacing, 0.0f, (kzFloat)i * spacing);
            struct KzcVector3 lineHorizontal2 = kzcVector3(10.0f * spacing, 0.0f, (kzFloat)i * spacing);
            struct KzcVector3 lineVertical1 = kzcVector3((kzFloat)i * spacing, 0.0f, -10.0f * spacing);
            struct KzcVector3 lineVertical2 = kzcVector3((kzFloat)i * spacing, 0.0f, 10.0f * spacing);

            kzuRendererAddVector3ToArray_internal(&lineHorizontal1, &vertices);
            kzuRendererAddVector3ToArray_internal(&lineHorizontal2, &vertices);
            kzuRendererAddVector3ToArray_internal(&lineVertical1, &vertices);
            kzuRendererAddVector3ToArray_internal(&lineVertical2, &vertices);
        }
    }
    vertices = originalData;
    originalData = verticesCenter;
    {
        struct KzcVector3 lineHorizontal1 = kzcVector3(-10.0f * spacing, 0.0f, 0.0f);
        struct KzcVector3 lineHorizontal2 = kzcVector3(10.0f * spacing, 0.0f, 0.0f);
        struct KzcVector3 lineVertical1 = kzcVector3(0.0f, 0.0f, -10.0f * spacing);
        struct KzcVector3 lineVertical2 = kzcVector3(0.0f, 0.0f, 10.0f * spacing);

        kzuRendererAddVector3ToArray_internal(&lineHorizontal1, &verticesCenter);
        kzuRendererAddVector3ToArray_internal(&lineHorizontal2, &verticesCenter);
        kzuRendererAddVector3ToArray_internal(&lineVertical1, &verticesCenter);
        kzuRendererAddVector3ToArray_internal(&lineVertical2, &verticesCenter);
    }
    verticesCenter = originalData;

    kzcRendererSetState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);
    kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_WRITE);
    kzcRendererSetLineWidth(kzuRendererGetCoreRenderer(renderer), 1.0f);

    /* Draw grid. */
    {
        struct KzcColorRGBA gridColor = kzcColorRGBA(0.2f, 0.2f, 0.2f, 1.0f);

        result = kzuRendererDrawLineList(renderer, vertices, 20 + 20, &gridColor, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);
    }
    /* Center lines. */
    {
        result = kzuRendererDrawLineList(renderer, verticesCenter, 2, &KZC_COLOR_BLACK, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);
    }
    
    kzcRendererSetLineWidth(kzuRendererGetCoreRenderer(renderer), 2.0f);
    /* Axes. */
    {
        result = kzuRendererDrawLineList(renderer, &axes[0], 1, &KZC_COLOR_RED, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);

        result = kzuRendererDrawLineList(renderer, &axes[6], 1, &KZC_COLOR_GREEN, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);

        result = kzuRendererDrawLineList(renderer, 
            &axes[12], 1, 
            &KZC_COLOR_BLUE,
            &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);
    }

    kzcRendererSetLineWidth(kzuRendererGetCoreRenderer(renderer), 1.0f);

    kzsSuccess();
}

kzsError kzuRendererDrawBoundingVolume(struct KzuRenderer* renderer, struct KzuBoundingVolume* boundingVolume,
                                       struct KzcMatrix4x4 worldTransformation)
{
    kzsError result;
    struct KzuTransformedBoundingVolume transformedBoundingVolume;
    
    if(boundingVolume != KZ_NULL)
    {
        kzuBoundingVolumeCalculateTransformedBoundingVolume(boundingVolume, &worldTransformation, &transformedBoundingVolume);

        result = kzuRendererDrawTransformedBoundingVolume(renderer, &transformedBoundingVolume, &KZC_COLOR_RED);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuRendererDrawTransformedBoundingVolume(struct KzuRenderer* renderer, const struct KzuTransformedBoundingVolume* volume, const struct KzcColorRGBA* color)
{
    kzsError result;
    kzUint idx = 0;
    kzUint verticeCount;
    kzFloat* vertices = kzuRendererGetFloatBuffer(renderer);

    struct KzcVector3 backBottomLeft = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_LEFT);
    struct KzcVector3 backBottomRight = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_RIGHT);
    struct KzcVector3 backTopRight = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_BACK_TOP_RIGHT);
    struct KzcVector3 backTopLeft = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_BACK_TOP_LEFT);
    struct KzcVector3 frontBottomLeft = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_LEFT);
    struct KzcVector3 frontBottomRight = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_RIGHT);
    struct KzcVector3 frontTopRight = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_FRONT_TOP_RIGHT);
    struct KzcVector3 frontTopLeft = kzuTransformedBoundingVolumeBoxGetCornerPoint(volume, KZU_BOUNDING_BOX_CORNER_FRONT_TOP_LEFT);

    verticeCount = 12;
    kzsAssert(kzcIsValidPointer(vertices));

    /* Left */
    vertices[idx++] = backBottomLeft.data[0]; vertices[idx++] = backBottomLeft.data[1]; vertices[idx++] = backBottomLeft.data[2];
    vertices[idx++] = frontBottomLeft.data[0]; vertices[idx++] = frontBottomLeft.data[1]; vertices[idx++] = frontBottomLeft.data[2];
    vertices[idx++] = backTopLeft.data[0]; vertices[idx++] = backTopLeft.data[1]; vertices[idx++] = backTopLeft.data[2];
    vertices[idx++] = frontTopLeft.data[0]; vertices[idx++] = frontTopLeft.data[1]; vertices[idx++] = frontTopLeft.data[2];
    vertices[idx++] = backTopLeft.data[0]; vertices[idx++] = backTopLeft.data[1]; vertices[idx++] = backTopLeft.data[2];
    vertices[idx++] = backBottomLeft.data[0]; vertices[idx++] = backBottomLeft.data[1]; vertices[idx++] = backBottomLeft.data[2];
    vertices[idx++] = frontTopLeft.data[0]; vertices[idx++] = frontTopLeft.data[1]; vertices[idx++] = frontTopLeft.data[2];
    vertices[idx++] = frontBottomLeft.data[0]; vertices[idx++] = frontBottomLeft.data[1]; vertices[idx++] = frontBottomLeft.data[2];

    /* Right */
    vertices[idx++] = backBottomRight.data[0]; vertices[idx++] = backBottomRight.data[1]; vertices[idx++] = backBottomRight.data[2];
    vertices[idx++] = frontBottomRight.data[0]; vertices[idx++] = frontBottomRight.data[1]; vertices[idx++] = frontBottomRight.data[2];
    vertices[idx++] = backTopRight.data[0]; vertices[idx++] = backTopRight.data[1]; vertices[idx++] = backTopRight.data[2];
    vertices[idx++] = frontTopRight.data[0]; vertices[idx++] = frontTopRight.data[1]; vertices[idx++] = frontTopRight.data[2];
    vertices[idx++] = backTopRight.data[0]; vertices[idx++] = backTopRight.data[1]; vertices[idx++] = backTopRight.data[2];
    vertices[idx++] = backBottomRight.data[0]; vertices[idx++] = backBottomRight.data[1]; vertices[idx++] = backBottomRight.data[2];
    vertices[idx++] = frontTopRight.data[0]; vertices[idx++] = frontTopRight.data[1]; vertices[idx++] = frontTopRight.data[2];
    vertices[idx++] = frontBottomRight.data[0]; vertices[idx++] = frontBottomRight.data[1]; vertices[idx++] = frontBottomRight.data[2];

    vertices[idx++] = backBottomLeft.data[0]; vertices[idx++] = backBottomLeft.data[1]; vertices[idx++] = backBottomLeft.data[2];
    vertices[idx++] = backBottomRight.data[0]; vertices[idx++] = backBottomRight.data[1]; vertices[idx++] = backBottomRight.data[2];
    vertices[idx++] = backTopLeft.data[0]; vertices[idx++] = backTopLeft.data[1]; vertices[idx++] = backTopLeft.data[2];
    vertices[idx++] = backTopRight.data[0]; vertices[idx++] = backTopRight.data[1]; vertices[idx++] = backTopRight.data[2];
    vertices[idx++] = frontBottomLeft.data[0]; vertices[idx++] = frontBottomLeft.data[1]; vertices[idx++] = frontBottomLeft.data[2];
    vertices[idx++] = frontBottomRight.data[0]; vertices[idx++] = frontBottomRight.data[1]; vertices[idx++] = frontBottomRight.data[2];
    vertices[idx++] = frontTopLeft.data[0]; vertices[idx++] = frontTopLeft.data[1]; vertices[idx++] = frontTopLeft.data[2];
    vertices[idx++] = frontTopRight.data[0]; vertices[idx++] = frontTopRight.data[1]; vertices[idx++] = frontTopRight.data[2];
    
    result = kzuRendererDrawLineList(renderer, vertices, verticeCount, color, &KZC_MATRIX4X4_IDENTITY);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRendererDrawTransformedBoundingVolumeAB(struct KzuRenderer* renderer, const struct KzcVector3* backBottomLeft, const struct KzcVector3 *frontTopRight, const struct KzcMatrix4x4 *transform, const struct KzcColorRGBA* color)
{
    kzsError result;
    struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
    kzUint idx = 0;
    kzFloat* vertices = kzuRendererGetFloatBuffer(renderer);

    struct KzcVector3 backBottomRight;
    struct KzcVector3 backTopRight;
    struct KzcVector3 backTopLeft;
    struct KzcVector3 frontBottomLeft;
    struct KzcVector3 frontBottomRight;
    struct KzcVector3 frontTopLeft;

    backBottomRight.data[0] = frontTopRight->data[0];
    backBottomRight.data[1] = backBottomLeft->data[1];
    backBottomRight.data[2] = backBottomLeft->data[2];

    backTopLeft.data[0] = backBottomLeft->data[0];
    backTopLeft.data[1] = frontTopRight->data[1];
    backTopLeft.data[2] = backBottomLeft->data[2];

    frontBottomLeft.data[0] = backBottomLeft->data[0];
    frontBottomLeft.data[1] = backBottomLeft->data[1];
    frontBottomLeft.data[2] = frontTopRight->data[2];

    frontTopLeft.data[0] = backBottomLeft->data[0];
    frontTopLeft.data[1] = frontTopRight->data[1];
    frontTopLeft.data[2] = frontTopRight->data[2];

    frontBottomRight.data[0] = frontTopRight->data[0];
    frontBottomRight.data[1] = backBottomLeft->data[1];
    frontBottomRight.data[2] = frontTopRight->data[2];

    backTopRight.data[0] = frontTopRight->data[0];
    backTopRight.data[1] = frontTopRight->data[1];
    backTopRight.data[2] = backBottomLeft->data[2];

    kzsAssert(kzcIsValidPointer(vertices));
    kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);

    /* Left */
    vertices[idx++] = backBottomLeft->data[0]; vertices[idx++] = backBottomLeft->data[1]; vertices[idx++] = backBottomLeft->data[2];
    vertices[idx++] = frontBottomLeft.data[0]; vertices[idx++] = frontBottomLeft.data[1]; vertices[idx++] = frontBottomLeft.data[2];
    vertices[idx++] = backTopLeft.data[0]; vertices[idx++] = backTopLeft.data[1]; vertices[idx++] = backTopLeft.data[2];
    vertices[idx++] = frontTopLeft.data[0]; vertices[idx++] = frontTopLeft.data[1]; vertices[idx++] = frontTopLeft.data[2];
    vertices[idx++] = backTopLeft.data[0]; vertices[idx++] = backTopLeft.data[1]; vertices[idx++] = backTopLeft.data[2];
    vertices[idx++] = backBottomLeft->data[0]; vertices[idx++] = backBottomLeft->data[1]; vertices[idx++] = backBottomLeft->data[2];
    vertices[idx++] = frontTopLeft.data[0]; vertices[idx++] = frontTopLeft.data[1]; vertices[idx++] = frontTopLeft.data[2];
    vertices[idx++] = frontBottomLeft.data[0]; vertices[idx++] = frontBottomLeft.data[1]; vertices[idx++] = frontBottomLeft.data[2];

    /* Right */
    vertices[idx++] = backBottomRight.data[0]; vertices[idx++] = backBottomRight.data[1]; vertices[idx++] = backBottomRight.data[2];
    vertices[idx++] = frontBottomRight.data[0]; vertices[idx++] = frontBottomRight.data[1]; vertices[idx++] = frontBottomRight.data[2];
    vertices[idx++] = backTopRight.data[0]; vertices[idx++] = backTopRight.data[1]; vertices[idx++] = backTopRight.data[2];
    vertices[idx++] = frontTopRight->data[0]; vertices[idx++] = frontTopRight->data[1]; vertices[idx++] = frontTopRight->data[2];
    vertices[idx++] = backTopRight.data[0]; vertices[idx++] = backTopRight.data[1]; vertices[idx++] = backTopRight.data[2];
    vertices[idx++] = backBottomRight.data[0]; vertices[idx++] = backBottomRight.data[1]; vertices[idx++] = backBottomRight.data[2];
    vertices[idx++] = frontTopRight->data[0]; vertices[idx++] = frontTopRight->data[1]; vertices[idx++] = frontTopRight->data[2];
    vertices[idx++] = frontBottomRight.data[0]; vertices[idx++] = frontBottomRight.data[1]; vertices[idx++] = frontBottomRight.data[2];

    vertices[idx++] = backBottomLeft->data[0]; vertices[idx++] = backBottomLeft->data[1]; vertices[idx++] = backBottomLeft->data[2];
    vertices[idx++] = backBottomRight.data[0]; vertices[idx++] = backBottomRight.data[1]; vertices[idx++] = backBottomRight.data[2];
    vertices[idx++] = backTopLeft.data[0]; vertices[idx++] = backTopLeft.data[1]; vertices[idx++] = backTopLeft.data[2];
    vertices[idx++] = backTopRight.data[0]; vertices[idx++] = backTopRight.data[1]; vertices[idx++] = backTopRight.data[2];
    vertices[idx++] = frontBottomLeft.data[0]; vertices[idx++] = frontBottomLeft.data[1]; vertices[idx++] = frontBottomLeft.data[2];
    vertices[idx++] = frontBottomRight.data[0]; vertices[idx++] = frontBottomRight.data[1]; vertices[idx++] = frontBottomRight.data[2];
    vertices[idx++] = frontTopLeft.data[0]; vertices[idx++] = frontTopLeft.data[1]; vertices[idx++] = frontTopLeft.data[2];
    vertices[idx++] = frontTopRight->data[0]; vertices[idx++] = frontTopRight->data[1]; vertices[idx++] = frontTopRight->data[2];

    result = kzuRendererDrawLineList(renderer, vertices, idx / 6, color, transform);
    kzsErrorForward(result);

    kzcRendererSetState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);
    kzsSuccess();
}

kzsError kzuRendererDrawGridLayoutCells(struct KzuRenderer* renderer, const struct KzcVector3* backBottomLeft, const struct KzcVector3 *frontTopRight,
                                        const struct KzcMatrix4x4 *transform, const struct KzcMatrix4x4* camera,
                                        const struct KzuUiComponentNode* gridLayout, const struct KzcColorRGBA* color)
{
    kzsError result;
    struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
    kzFloat* vertices = kzuRendererGetFloatBuffer(renderer);
    kzFloat angleDelta = KZS_PI * 2.0f / (kzFloat)KZU_LAYOUT_VISUALIZATION_DETAIL;
    kzFloat scale;
    kzUint columnCount = kzuUiGridLayoutGetColumnCount(gridLayout);
    kzUint rowCount = kzuUiGridLayoutGetRowCount(gridLayout);
    kzUint idx = 0;

    struct KzcMatrix4x4 world;
    struct KzcVector3 backTopLeft;
    struct KzcVector3 backTopRight;
    struct KzcVector3 frontTopLeft;
    struct KzcVector3 frontBottomLeft;
    struct KzcVector3 right;
    struct KzcVector3 down;
    struct KzcVector3 front;
    struct KzcVector3 cameraRight;
    struct KzcVector3 cameraUp;

    kzcMatrix4x4Multiply(transform, camera, &world);
    cameraRight.data[0] = world.data[0];
    cameraRight.data[1] = world.data[4];
    cameraRight.data[2] = world.data[8];
    cameraUp.data[0] = world.data[1];
    cameraUp.data[1] = world.data[5];
    cameraUp.data[2] = world.data[9];

    backTopLeft.data[0] = backBottomLeft->data[0];
    backTopLeft.data[1] = frontTopRight->data[1];
    backTopLeft.data[2] = backBottomLeft->data[2];

    backTopRight.data[0] = frontTopRight->data[0];
    backTopRight.data[1] = frontTopRight->data[1];
    backTopRight.data[2] = backBottomLeft->data[2];

    frontTopLeft.data[0] = backBottomLeft->data[0];
    frontTopLeft.data[1] = frontTopRight->data[1];
    frontTopLeft.data[2] = frontTopRight->data[2];

    frontBottomLeft.data[0] = backBottomLeft->data[0];
    frontBottomLeft.data[1] = backBottomLeft->data[1];
    frontBottomLeft.data[2] = frontTopRight->data[2];

    kzcVector3Subtract(&backTopRight, &backTopLeft, &right);
    kzcVector3Subtract(backBottomLeft, &backTopLeft, &down);
    {
        kzFloat lenX = kzcVector3GetLength(&right);
        kzFloat lenY = kzcVector3GetLength(&down);
        scale = kzsMinf(lenX, lenY) * KZU_LAYOUT_VISUALIZATION_SCALE;
        kzcVector3Scale(&right, 1.0f / lenX, &right);
        kzcVector3Scale(&down, 1.0f / lenY, &down);
    }
    kzcVector3Subtract(&frontTopLeft, &backTopLeft, &front);

    kzsAssert(kzcIsValidPointer(vertices));
    kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);

    /* Do the front-back spikes first. */
    if((rowCount > 0) && (columnCount > 0))
    {
        kzFloat accumRight = 0.0f;
        kzUint i = 0;

        do 
        {
            kzFloat accumDown = 0.0f;
            kzFloat cellWidth = 0.0f;
            kzFloat rightComponent = 0.0f; /* For lint only. */
            kzUint j = 0;

            if(i < columnCount)
            {
                vertices[idx++] = backTopLeft.data[0] + right.data[0] * accumRight;
                vertices[idx++] = backTopLeft.data[1] + right.data[1] * accumRight;
                vertices[idx++] = backTopLeft.data[2] + right.data[2] * accumRight;
                vertices[idx++] = backBottomLeft->data[0] + right.data[0] * accumRight;
                vertices[idx++] = backBottomLeft->data[1] + right.data[1] * accumRight;
                vertices[idx++] = backBottomLeft->data[2] + right.data[2] * accumRight;
                vertices[idx++] = frontTopLeft.data[0] + right.data[0] * accumRight;
                vertices[idx++] = frontTopLeft.data[1] + right.data[1] * accumRight;
                vertices[idx++] = frontTopLeft.data[2] + right.data[2] * accumRight;
                vertices[idx++] = frontBottomLeft.data[0] + right.data[0] * accumRight;
                vertices[idx++] = frontBottomLeft.data[1] + right.data[1] * accumRight;
                vertices[idx++] = frontBottomLeft.data[2] + right.data[2] * accumRight;

                cellWidth = kzuUiGridLayoutGetActualColumnSize(gridLayout, i);
                rightComponent = accumRight + 0.5f * cellWidth;
            }

            do 
            {
                kzFloat cellHeight = 0.0f;
                kzFloat downComponent;
                kzUint indexX = idx++;
                kzUint indexY = idx++;
                kzUint indexZ = idx++;

                vertices[indexX] = backTopLeft.data[0] + right.data[0] * accumRight + down.data[0] * accumDown;
                vertices[indexY] = backTopLeft.data[1] + right.data[1] * accumRight + down.data[1] * accumDown;
                vertices[indexZ] = backTopLeft.data[2] + right.data[2] * accumRight + down.data[2] * accumDown;
                vertices[idx++] = vertices[indexX] + front.data[0];
                vertices[idx++] = vertices[indexY] + front.data[1];
                vertices[idx++] = vertices[indexZ] + front.data[2];

                if(j < rowCount)
                {
                    cellHeight = kzuUiGridLayoutGetActualRowSize(gridLayout, j);
                    downComponent = accumDown + 0.5f * cellHeight;

                    if(i < columnCount)
                    {
                        struct KzcVector3 center;
                        kzFloat currentAngle = angleDelta;
                        kzFloat cosAnglePrevious = 1.0f * scale;
                        kzFloat sinAnglePrevious = 0.0f;
                        kzUint k;

                        center.data[0] = backTopLeft.data[0] + right.data[0] * rightComponent + down.data[0] * downComponent + front.data[0] * 0.5f;
                        center.data[1] = backTopLeft.data[1] + right.data[1] * rightComponent + down.data[1] * downComponent + front.data[1] * 0.5f;
                        center.data[2] = backTopLeft.data[2] + right.data[2] * rightComponent + down.data[2] * downComponent + front.data[2] * 0.5f;

                        for(k = 1; (k <= KZU_LAYOUT_VISUALIZATION_DETAIL); ++k)
                        {
                            kzFloat cosAngle = kzsCosf(currentAngle) * scale;
                            kzFloat sinAngle = kzsSinf(currentAngle) * scale;

                            vertices[idx++] = center.data[0] + cameraRight.data[0] * cosAnglePrevious + cameraUp.data[0] * sinAnglePrevious;
                            vertices[idx++] = center.data[1] + cameraRight.data[1] * cosAnglePrevious + cameraUp.data[1] * sinAnglePrevious;
                            vertices[idx++] = center.data[2] + cameraRight.data[2] * cosAnglePrevious + cameraUp.data[2] * sinAnglePrevious;

                            vertices[idx++] = center.data[0] + cameraRight.data[0] * cosAngle + cameraUp.data[0] * sinAngle;
                            vertices[idx++] = center.data[1] + cameraRight.data[1] * cosAngle + cameraUp.data[1] * sinAngle;
                            vertices[idx++] = center.data[2] + cameraRight.data[2] * cosAngle + cameraUp.data[2] * sinAngle;

                            cosAnglePrevious = cosAngle;
                            sinAnglePrevious = sinAngle;
                            currentAngle += angleDelta;
                        }
                    }

                    vertices[idx++] = backTopLeft.data[0] + down.data[0] * accumDown;
                    vertices[idx++] = backTopLeft.data[1] + down.data[1] * accumDown;
                    vertices[idx++] = backTopLeft.data[2] + down.data[2] * accumDown;
                    vertices[idx++] = backTopRight.data[0] + down.data[0] * accumDown;
                    vertices[idx++] = backTopRight.data[1] + down.data[1] * accumDown;
                    vertices[idx++] = backTopRight.data[2] + down.data[2] * accumDown;
                    vertices[idx++] = frontTopLeft.data[0] + down.data[0] * accumDown;
                    vertices[idx++] = frontTopLeft.data[1] + down.data[1] * accumDown;
                    vertices[idx++] = frontTopLeft.data[2] + down.data[2] * accumDown;
                    vertices[idx++] = frontTopRight->data[0] + down.data[0] * accumDown;
                    vertices[idx++] = frontTopRight->data[1] + down.data[1] * accumDown;
                    vertices[idx++] = frontTopRight->data[2] + down.data[2] * accumDown;                
                }

                accumDown += cellHeight;
                ++j;
            } while (j <= rowCount);

            accumRight += cellWidth;
            ++i;
        } while (i <= columnCount);
    }

    result = kzuRendererDrawLineList(renderer, vertices, idx / 6, color, transform);
    kzsErrorForward(result);

    kzcRendererSetState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);
    kzsSuccess();
}

kzsError kzuRendererDrawGridLayoutElementPositions(struct KzuRenderer* renderer, const struct KzcVector3* backBottomLeft,
                                                   const struct KzcVector3* frontTopRight, const struct KzcMatrix4x4 *transform,
                                                   const struct KzcMatrix4x4* camera, const struct KzuTransformedObjectNode* layout,
                                                   const struct KzcColorRGBA* color)
{
    kzsError result;
    struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
    kzFloat* vertices = kzuRendererGetFloatBuffer(renderer);
    kzFloat angleDelta = KZS_PI * 2.0f / (kzFloat)KZU_LAYOUT_VISUALIZATION_DETAIL;
    kzFloat scale;
    struct KzcDynamicArrayIterator it;

    struct KzcMatrix4x4 world;
    struct KzcVector3 backTopLeft;
    struct KzcVector3 backBottomRight;
    struct KzcVector3 frontTopLeft;
    struct KzcVector3 right;
    struct KzcVector3 down;
    struct KzcVector3 front;
    struct KzcVector3 cameraRight;
    struct KzcVector3 cameraUp;

    kzcMatrix4x4Multiply(transform, camera, &world);
    cameraRight.data[0] = world.data[0];
    cameraRight.data[1] = world.data[4];
    cameraRight.data[2] = world.data[8];
    cameraUp.data[0] = world.data[1];
    cameraUp.data[1] = world.data[5];
    cameraUp.data[2] = world.data[9];

    backBottomRight.data[0] = frontTopRight->data[0];
    backBottomRight.data[1] = backBottomLeft->data[1];
    backBottomRight.data[2] = backBottomLeft->data[2];

    backTopLeft.data[0] = backBottomLeft->data[0];
    backTopLeft.data[1] = frontTopRight->data[1];
    backTopLeft.data[2] = backBottomLeft->data[2];

    frontTopLeft.data[0] = backBottomLeft->data[0];
    frontTopLeft.data[1] = frontTopRight->data[1];
    frontTopLeft.data[2] = frontTopRight->data[2];

    kzcVector3Subtract(&backBottomRight, backBottomLeft, &right);
    kzcVector3Subtract(backBottomLeft, &backTopLeft, &down);
    {
        kzFloat lenX = kzcVector3GetLength(&right);
        kzFloat lenY = kzcVector3GetLength(&down);
        scale = kzsMinf(lenX, lenY) * KZU_LAYOUT_VISUALIZATION_SCALE;
        kzcVector3Scale(&right, 1.0f / lenX, &right);
        kzcVector3Scale(&down, 1.0f / lenY, &down);
    }
    kzcVector3Subtract(&frontTopLeft, &backTopLeft, &front);
    kzcVector3Scale(&front, 0.5f + KZU_LAYOUT_VISUALIZATION_POKE_OUT, &front);

    kzsAssert(kzcIsValidPointer(vertices));
    kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);

    it = kzuTransformedObjectNodeGetChildren(layout);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* child = kzcDynamicArrayIteratorGetValue(it);
        struct KzcVector3 center;
        kzFloat currentAngle = angleDelta;
        kzUint idx = 0;
        kzUint i;

        kzuTransformedObjectNodeGetPosition(child, &center);

        vertices[idx++] = center.data[0];
        vertices[idx++] = center.data[1];
        vertices[idx++] = center.data[2];

        for(i = 0; (i <= KZU_LAYOUT_VISUALIZATION_DETAIL); ++i)
        {
            kzFloat cosAngle = kzsCosf(currentAngle) * scale;
            kzFloat sinAngle = kzsSinf(currentAngle) * scale;

            vertices[idx++] = center.data[0] + cameraRight.data[0] * cosAngle + cameraUp.data[0] * sinAngle;
            vertices[idx++] = center.data[1] + cameraRight.data[1] * cosAngle + cameraUp.data[1] * sinAngle;
            vertices[idx++] = center.data[2] + cameraRight.data[2] * cosAngle + cameraUp.data[2] * sinAngle;

            currentAngle += angleDelta;
        }

        result = kzuRendererDrawTriangleFan(renderer, vertices, KZU_LAYOUT_VISUALIZATION_DETAIL, color, transform);
        kzsErrorForward(result);
    }

    kzcRendererSetState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);
    kzsSuccess();
}

static kzsError kzuRendererDrawPrimitives_internal(struct KzuRenderer* renderer, const kzFloat* vertexList, kzUint vertexCount,
                                                   enum KzcPrimitiveType type, const struct KzcColorRGBA* color,
                                                   const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;
    struct KzcRenderer* coreRenderer;

    kzsAssert(kzcIsValidPointer(renderer));

    coreRenderer = kzuRendererGetCoreRenderer(renderer);

    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, worldTransformation);

    result = kzuRendererSetSolidColor(renderer, color);
    kzsErrorForward(result);
    result = kzuRendererApplyMaterial(renderer, kzuRendererGetSolidColorMaterial(renderer));
    kzsErrorForward(result);

    kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION);
    kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, vertexList);
    kzcRendererEndVertexArray(coreRenderer, type, vertexCount);

    kzsSuccess();
}

kzsError kzuRendererDrawLineList(struct KzuRenderer* renderer, const kzFloat* lineList, kzUint lineCount, const struct KzcColorRGBA* color,
                                 const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;
    
    result = kzuRendererDrawPrimitives_internal(renderer, lineList, lineCount * 2, KZC_PRIMITIVE_TYPE_LINES, color, worldTransformation);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRendererDrawTriangleFan(struct KzuRenderer* renderer, const kzFloat* triangleList, kzUint triangleCount, const struct KzcColorRGBA* color,
                                    const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;

    result = kzuRendererDrawPrimitives_internal(renderer, triangleList, triangleCount + 2, KZC_PRIMITIVE_TYPE_TRIANGLE_FAN, color, worldTransformation);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuRendererSetDefaultOrthoProjectionAndViewport(const struct KzuRenderer* renderer)
{
    struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
    struct KzcMatrix4x4 projectionMatrix;
    struct KzcMatrix4x4 cameraMatrix;

    kzuCameraNodeCalculateOrthogonalProjectionFromValues(-1.0f, 1.0f, 1.0f, -1.0f, KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE, KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL,
        1.0f, 1.0f, &projectionMatrix);

    cameraMatrix = KZC_MATRIX4X4_IDENTITY;
    cameraMatrix.data[14] = 0.5f;

    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION, &projectionMatrix);
    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_CAMERA, &cameraMatrix);
}

static kzsError kzuRendererDrawQuad_internal(struct KzuRenderer* renderer, const struct KzcVector3* p1, const struct KzcVector3* p2, const struct KzcVector3* p3,
                                             const struct KzcVector3* p4, struct KzuMaterial* material, const struct KzcColorRGBA* color, const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;
    struct KzcRenderer* coreRenderer;
    kzFloat* floatBuffer = kzuRendererGetFloatBuffer(renderer);
    kzUint offset = 0;

    kzsAssert(floatBuffer != KZ_NULL);

    floatBuffer[offset++] = p1->data[0];
    floatBuffer[offset++] = p1->data[1];
    floatBuffer[offset++] = p1->data[2];
    floatBuffer[offset++] = p2->data[0];
    floatBuffer[offset++] = p2->data[1];
    floatBuffer[offset++] = p2->data[2];
    floatBuffer[offset++] = p3->data[0];
    floatBuffer[offset++] = p3->data[1];
    floatBuffer[offset++] = p3->data[2];

    floatBuffer[offset++] = p3->data[0];
    floatBuffer[offset++] = p3->data[1];
    floatBuffer[offset++] = p3->data[2];
    floatBuffer[offset++] = p4->data[0];
    floatBuffer[offset++] = p4->data[1];
    floatBuffer[offset++] = p4->data[2];
    floatBuffer[offset++] = p1->data[0];
    floatBuffer[offset++] = p1->data[1];
    floatBuffer[offset++] = p1->data[2];

    floatBuffer[offset++] = 0.0f;
    floatBuffer[offset++] = 1.0f;
    floatBuffer[offset++] = 1.0f;
    floatBuffer[offset++] = 1.0f;
    floatBuffer[offset++] = 1.0f;
    floatBuffer[offset++] = 0.0f;

    floatBuffer[offset++] = 1.0f;
    floatBuffer[offset++] = 0.0f;
    floatBuffer[offset++] = 0.0f;
    floatBuffer[offset++] = 0.0f;
    floatBuffer[offset++] = 0.0f;
    floatBuffer[offset++] = 1.0f;

    kzsAssert(kzcIsValidPointer(renderer));

    coreRenderer = kzuRendererGetCoreRenderer(renderer);

    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, worldTransformation);

    if(material != KZ_NULL)
    {
        result = kzuRendererSetSolidColor(renderer, color);
        kzsErrorForward(result);
        result = kzuRendererApplyMaterial(renderer, material);
        kzsErrorForward(result);
    }

    kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
    kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, floatBuffer);
    kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, &floatBuffer[18]);
    kzcRendererEndVertexArray(coreRenderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6);

    kzsSuccess();
}

kzsError kzuRendererDrawQuad(struct KzuRenderer* renderer, const struct KzcVector3* p1, const struct KzcVector3* p2, const struct KzcVector3* p3,
                             const struct KzcVector3* p4, const struct KzcColorRGBA* color, const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;
 
    result = kzuRendererDrawQuad_internal(renderer, p1, p2, p3, p4, kzuRendererGetSolidColorMaterial(renderer), color, worldTransformation);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRendererDrawQuadWithMaterial(struct KzuRenderer* renderer, const struct KzcVector3* p1, const struct KzcVector3* p2, const struct KzcVector3* p3,
                                       const struct KzcVector3* p4, struct KzuMaterial* material, const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;

    result = kzuRendererDrawQuad_internal(renderer, p1, p2, p3, p4, material, &KZC_COLOR_BLACK, worldTransformation);
    kzsErrorForward(result);

    kzsSuccess();}

kzsError kzuRendererDrawBoundingVolumeSolid(const struct KzuRenderer* renderer, struct KzuBoundingVolume* boundingVolume,
                                            const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;
    struct KzcRenderer* coreRenderer;
    kzFloat* floatBuffer = kzuRendererGetFloatBuffer(renderer);
    struct KzcVector3 minCorner = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(kzuBoundingVolumeGetAxisAlignedBox(boundingVolume));
    struct KzcVector3 maxCorner = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(kzuBoundingVolumeGetAxisAlignedBox(boundingVolume));
    kzFloat* originalFloatBuffer = floatBuffer;

    kzsAssert(floatBuffer != KZ_NULL);

    /* Left. */
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);

    /* Front. */
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);

    /* Right. */
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);
    /* Back. */
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    /* Top. */
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], maxCorner.data[1], maxCorner.data[2], &floatBuffer);
    /* Bottom. */
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], minCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(maxCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);
    kzuRendererAddXYZToArray_internal(minCorner.data[0], minCorner.data[1], maxCorner.data[2], &floatBuffer);

    kzsAssert(kzcIsValidPointer(renderer));

    floatBuffer = originalFloatBuffer;

    coreRenderer = kzuRendererGetCoreRenderer(renderer);

    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, worldTransformation);

    result = kzcRendererApplyTransformation(coreRenderer);
    kzsErrorForward(result);

    kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION);
    kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, floatBuffer);
    kzcRendererEndVertexArray(coreRenderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6 * 6);

    kzsSuccess();
}

kzsError kzuRendererDrawWireframeBox(struct KzuRenderer* renderer, const struct KzcVector3* boxMinimum, const struct KzcVector3* boxMaximum)
{
    kzsError result;
    kzFloat* vertices;
    kzUint idx = 0;
    struct KzcRenderer* coreRenderer;

    coreRenderer = kzuRendererGetCoreRenderer(renderer);

    vertices = kzuRendererGetFloatBuffer(renderer);
    kzsAssert(kzcIsValidPointer(vertices));

    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMinimum->data[2];
    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMinimum->data[2];

    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMinimum->data[2];
    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMinimum->data[2];

    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMinimum->data[2];
    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMaximum->data[2];

    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMinimum->data[2];
    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMinimum->data[2];

    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMinimum->data[2];
    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMaximum->data[2];

    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMaximum->data[2];
    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMaximum->data[2];

    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMaximum->data[2];
    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMaximum->data[2];

    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMaximum->data[2];
    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMaximum->data[2];

    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMaximum->data[2];
    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMinimum->data[2];

    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMaximum->data[1]; vertices[idx++] = boxMinimum->data[2];
    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMinimum->data[2];

    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMaximum->data[2];
    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMinimum->data[2];

    vertices[idx++] = boxMaximum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMaximum->data[2];
    vertices[idx++] = boxMinimum->data[0]; vertices[idx++] = boxMinimum->data[1]; vertices[idx++] = boxMaximum->data[2];

    kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION);
    kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, vertices);

    result = kzuRendererSetSolidColor(renderer, &KZC_COLOR_WHITE);
    kzsErrorForward(result);

    result = kzuRendererApplyMaterial(renderer, kzuRendererGetSolidColorMaterial(renderer));
    kzsErrorForward(result);
    result = kzcRendererApplyTransformation(coreRenderer);
    kzsErrorForward(result);

    kzcRendererEndVertexArray(coreRenderer, KZC_PRIMITIVE_TYPE_LINES, 24);

    kzsSuccess();
}

kzsError kzuRendererDrawSkeleton(struct KzuRenderer* renderer, const struct KzcDynamicArray* transformedObjectList,
                                 const struct KzuSkeleton* skeleton)
{
    kzsError result;
    kzUint i;
    kzFloat* data = kzuRendererGetFloatBuffer(renderer);
    kzFloat* originalData;
    kzUint lineCount = 0;
    
    kzsAssert(kzcIsValidPointer(renderer));
    
    originalData = data;

    for(i = 0; i < kzcArrayLength(skeleton->bones); ++i)
    {
        struct KzuMeshBone bone = skeleton->bones[i];
        struct KzuTransformedObjectNode* startObjectNode;
        struct KzcVector3 startPoint;

        startObjectNode = kzuTransformedObjectListFindObject(transformedObjectList, bone.bone);
        kzuTransformedObjectNodeGetPosition(startObjectNode, &startPoint);
        
        if(bone.childJoint != KZ_NULL)
        {
            struct KzuTransformedObjectNode* endObjectNode;
            struct KzcVector3 endPoint;
            
            endObjectNode = kzuTransformedObjectListFindObject(transformedObjectList, bone.childJoint);
            
            kzuTransformedObjectNodeGetPosition(endObjectNode, &endPoint);

            kzuRendererAddVector3ToArray2_internal(startPoint, &data);
            kzuRendererAddVector3ToArray2_internal(endPoint, &data);
            ++lineCount;

        }
        
        {
            struct KzcVector3 minCoordinate;
            struct KzcVector3 maxCoordinate;
            struct KzcVector3 jointVector = kzcVector3(0.1f, 0.1f, 0.1f);
            /* struct KzcMatrix4x4 rotationPart; */
            kzcVector3Subtract(&startPoint, &jointVector, &minCoordinate);
            kzcVector3Add(&startPoint, &jointVector, &maxCoordinate);
            /*rotationPart = kzuTransformedObjectNodeGetMatrix(startObjectNode);
            rotationPart.data[12] = 0.0f;
            rotationPart.data[13] = 0.0f;
            rotationPart.data[14] = 0.0f;
            rotationPart.data[15] = 1.0f;*/
            
            kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);
            
            result = kzuRendererDrawWireframeBox(renderer, &minCoordinate, &maxCoordinate);
            kzsErrorForward(result);
        }
    }
    
    data = originalData;

    /* Skeleton visualize. */
    if(data != KZ_NULL)
    {
        kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);
        result = kzuRendererDrawLineList(renderer, &data[0], lineCount, &KZC_COLOR_RED, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuRendererDrawLight(struct KzuRenderer* renderer, const struct KzcMatrix4x4* worldTransformation,
                              const struct KzuLightNode* lightNode)
{
    kzsError result;
    kzFloat* floatValueArray = kzuRendererGetFloatBuffer(renderer);
    kzFloat* data = floatValueArray;
    kzFloat* originalData;
    kzUint lineCount = 0;
    struct KzuLightPropertyType* lightPropertyType = kzuLightNodeGetLightPropertyType(lightNode);
    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(kzuLightNodeToObjectNode(lightNode));

    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(kzcIsValidPointer(lightNode));
    
    originalData = data;
    /* Point light visualization. */
    if (lightPropertyType == kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_POINT_LIGHT))
    {
        kzFloat radius = 0.5f;
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.1f, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(radius, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(-0.1f, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.1f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, radius, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, -0.1f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, -radius, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, 0.1f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, radius), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -0.1f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -radius), &data);
        lineCount = 6;
    }
    /* Directional light visualization. */
    else if (lightPropertyType == kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT))
    {
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -1.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -1.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.2f, 0.0f, -0.8f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -1.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(-0.2f, 0.0f, -0.8f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -1.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.2f, -0.8f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -1.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, -0.2f, -0.8f), &data);
        lineCount = 5;
    }
    /* Spot light visualization. */
    else if (lightPropertyType == kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_SPOT_LIGHT))
    {
        kzUint i;
        kzFloat angle = kzuPropertyManagerGetFloatDefault(propertyManager, lightNode, KZU_PROPERTY_TYPE_SPOT_LIGHT_CUTOFF_ANGLE);
        kzFloat z = kzsCosf(angle * 0.5f * KZS_PI / 180.0f);

        for(i = 0; i < 8; ++i)
        {
            kzFloat x, y;
            kzFloat radius = kzsSqrtf(1.0f - z * z);
            x = kzsSinf((kzFloat)i * KZS_PI * 2.0f / 8.0f) * radius;
            y = -kzsCosf((kzFloat)i * KZS_PI * 2.0f / 8.0f) * radius;

            kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3(x, y, -z), &data);
        }
        lineCount = 8;
    }

    data = originalData;

    /* Light visualize. */
    if(data != KZ_NULL)
    {
        result = kzuRendererDrawLineList(renderer, &data[0], lineCount, &KZC_COLOR_YELLOW, worldTransformation);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuRendererDrawEmptyObject(struct KzuRenderer* renderer, const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;
    kzFloat* floatValueArray = kzuRendererGetFloatBuffer(renderer);
    kzFloat* data = floatValueArray;
    kzFloat* originalData;
    kzUint lineCount = 6;

    kzsAssert(kzcIsValidPointer(renderer));

    originalData = data;
    {
        kzFloat radius = 0.2f;
        kzuRendererAddVector3ToArray2_internal(kzcVector3(radius * 0.5f, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(radius * 2.0f, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius * 0.5f, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius * 2.0f, 0.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, radius * 0.5f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, radius * 2.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, -radius * 0.5f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, -radius * 2.0f, 0.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, radius * 0.5f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, radius * 2.0f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -radius * 0.5f), &data);
        kzuRendererAddVector3ToArray2_internal(kzcVector3(0.0f, 0.0f, -radius * 2.0f), &data);
    }
    

    data = originalData;

    /* Empty visualize. */
    if(data != KZ_NULL)
    {
        result = kzuRendererDrawLineList(renderer, &data[0], lineCount, &KZC_COLOR_DARK_GRAY, worldTransformation);
        kzsErrorForward(result);
    }

    kzsSuccess();
}


kzsError kzuRendererDrawCamera(struct KzuRenderer* renderer, const struct KzcMatrix4x4* worldTransformation,
                               const struct KzuCameraNode* cameraNode)
{
    kzsError result;
    kzFloat* floatValueArray = kzuRendererGetFloatBuffer(renderer);
    kzFloat* data = floatValueArray;
    kzFloat* originalData;
    kzUint lineCount;
    const kzFloat radius = 1.0f;

    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(kzcIsValidPointer(cameraNode));

    {
        originalData = data;
        /* Box. */
        {
            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius, radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, -radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, -radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( -radius, -radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( -radius, -radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius, radius, 0.0f), &data);

            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius, radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius, radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius,-radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius,-radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius,-radius, 0.0f), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius,-radius, radius), &data);

            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius, radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, -radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( radius, -radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( -radius, -radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3( -radius, -radius, radius), &data);
            kzuRendererAddVector3ToArray2_internal(kzcVector3(-radius, radius, radius), &data);
        }

        lineCount = 12;

        /* Look cone. */
        {
            kzUint i;
            kzFloat angle = kzsDegreesToRadians(kzuCameraNodeGetFov(cameraNode));
            kzFloat z = kzsCosf(angle * 0.5f);

            for(i = 0; i < 8; ++i)
            {
                kzFloat x;
                kzFloat y;
                kzFloat x2;
                kzFloat y2;

                kzFloat viewRadius = kzsSqrtf(1.0f - z * z);
                kzFloat innerRadius = 0.5f;
                if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_ORTHOGRAPHIC)
                {
                    innerRadius = 1.0f;
                }
                x = kzsSinf((kzFloat)i * KZS_PI * 2.0f / 8.0f) * viewRadius;
                y = -kzsCosf((kzFloat)i * KZS_PI * 2.0f / 8.0f) * viewRadius;
                x2 = kzsSinf((kzFloat)(i + 1) * KZS_PI * 2.0f / 8.0f) * viewRadius;
                y2 = -kzsCosf((kzFloat)(i + 1) * KZS_PI * 2.0f / 8.0f) * viewRadius;

                kzuRendererAddVector3ToArray2_internal(kzcVector3(x * innerRadius, y * innerRadius, 0.0f), &data);
                kzuRendererAddVector3ToArray2_internal(kzcVector3(x, y, -z), &data);

                kzuRendererAddVector3ToArray2_internal(kzcVector3(x * innerRadius, y * innerRadius, 0.0f), &data);
                kzuRendererAddVector3ToArray2_internal(kzcVector3(x2 * innerRadius, y2 * innerRadius, 0.0f), &data);

                kzuRendererAddVector3ToArray2_internal(kzcVector3(x, y, -z), &data);
                kzuRendererAddVector3ToArray2_internal(kzcVector3(x2, y2, -z), &data);
            }
        }
        lineCount += 24;
        data = originalData;

        /* Camera NULL. */
        if(data != KZ_NULL)
        {
            result = kzuRendererDrawLineList(renderer, &data[0], lineCount, &KZC_COLOR_PURPLE, worldTransformation);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuRendererDrawBorders(const struct KzuRenderer* renderer, const struct KzsWindow* window)
{
    kzsError result;
    struct KzcRenderer* coreRenderer;
    kzFloat x, y, width, height;
    
    kzsAssert(kzcIsValidPointer(renderer));

    coreRenderer = kzuRendererGetCoreRenderer(renderer);

    kzuRendererGetAdjustedViewport(renderer, window, &x, &y, &width, &height);

    kzcRendererSetViewport(coreRenderer, 0, 0, kzsWindowGetWidth(window), kzsWindowGetHeight(window));
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzcRendererSetBlendIntensity(coreRenderer, 1.0f);
#endif
    kzcRendererSetBlendMode(coreRenderer, KZC_RENDERER_BLEND_MODE_OPAQUE);

    if(width + 0.5f < (kzFloat)kzsWindowGetWidth(window))
    {
        /* Left border. */
        result = kzcRendererDrawSolidQuad2DAbsolute(coreRenderer, 0.0f, 0.0f, x, (kzFloat)kzsWindowGetHeight(window), KZC_COLOR_BLACK);
        kzsErrorForward(result);
        
        /* Right border. */
        result = kzcRendererDrawSolidQuad2DAbsolute(coreRenderer, (kzFloat)(kzInt)(x + width - 0.5f), 0.0f, (kzFloat)kzsWindowGetWidth(window),
            (kzFloat)kzsWindowGetHeight(window), KZC_COLOR_BLACK);

        kzsErrorForward(result);
    }

    if(height + 0.5f < (kzFloat)kzsWindowGetHeight(window))
    {
        /* Bottom border. */
        result = kzcRendererDrawSolidQuad2DAbsolute(coreRenderer, 0.0f, 0.0f, (kzFloat)kzsWindowGetWidth(window), y, KZC_COLOR_BLACK);
        kzsErrorForward(result);

        /* Top border. */
        result = kzcRendererDrawSolidQuad2DAbsolute(coreRenderer, 0.0f, (kzFloat)(kzInt)(y + height - 0.5f), (kzFloat)kzsWindowGetWidth(window), (kzFloat)kzsWindowGetHeight(window), KZC_COLOR_BLACK);
        kzsErrorForward(result);
    }


    kzsSuccess();
}

void kzuWindowGetAdjustedViewportRectangle(kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat aspectRatio,
                                           kzFloat* out_newX, kzFloat* out_newY,
                                           kzFloat* out_newWidth, kzFloat* out_newHeight)
{
    kzFloat windowRatio;
    kzFloat xMultiply;
    kzFloat yMultiply;
    kzFloat xStart;
    kzFloat yStart;

    if (height > 0.0f)
    {
        windowRatio = width / height;
    }
    else
    {
        windowRatio = aspectRatio;
    }

    if(windowRatio > aspectRatio)
    {
        xMultiply = aspectRatio / windowRatio;
        yMultiply = 1.0f;
    }
    else
    {
        xMultiply = 1.0f;
        yMultiply = windowRatio / aspectRatio;
    }

    xStart = (1.0f - xMultiply) / 2.0f;
    yStart = (1.0f - yMultiply) / 2.0f;

    *out_newX = x + xStart * width;
    *out_newY = y + yStart * height;
    *out_newWidth = xMultiply * width;
    *out_newHeight = yMultiply * height;
}
