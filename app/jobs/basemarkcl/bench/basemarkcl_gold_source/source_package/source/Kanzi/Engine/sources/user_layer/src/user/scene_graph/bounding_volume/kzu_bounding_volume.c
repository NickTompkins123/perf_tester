/**
* \file
* Bounding volume structure, that is, volume that bounds some area, typically geometry.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_bounding_volume.h"

#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_plane.h>
#include <core/util/math/kzc_volume.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_byte_buffer.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


struct KzuBoundingVolumeAxisAlignedBox
{
    struct KzcVector3 minimumCorner; /**< Minimum corner vector in local coordinates. */
    struct KzcVector3 maximumCorner; /**< Maximum corner vector in local coordinates. */
    struct KzcVector3 cornerPoints[8]; /**< Corner points. */
};

struct KzuBoundingVolumeSphere
{
    struct KzcVector3 center; /**< Local center location. */
    kzFloat radius; /**< Sphere radius. */
};

struct KzuBoundingVolume
{
    struct KzuBoundingVolumeAxisAlignedBox aabb; /**< Axis-aligned bounding box. */
    struct KzuBoundingVolumeSphere sphere; /**< Bounding sphere. */
};


kzsError kzuBoundingVolumeCreateEmpty(const struct KzcMemoryManager* memoryManager, struct KzuBoundingVolume** out_volume)
{
    kzsError result;
    struct KzuBoundingVolume* boundingVolume;

    result = kzuBoundingVolumeCreate(memoryManager, KZ_NULL, &boundingVolume);
    kzsErrorForward(result);
    *out_volume = boundingVolume;
    kzsSuccess();
}

kzsError kzuBoundingVolumeCreate(const struct KzcMemoryManager* memoryManager, const struct KzcVertexList* vertexList, struct KzuBoundingVolume** out_volume)
{
    kzsError result;
    struct KzuBoundingVolume* boundingVolume;
    struct KzcVolume boundingVolumeCalculated = KZC_VOLUME_EMPTY;
    kzBool volumeAssigned = KZ_FALSE;
    struct KzcVector3 boundingBoxMiddle = KZC_VECTOR3_ZERO;
    kzUint dataElementCount = 1;

    if(vertexList != KZ_NULL)
    {
        kzU8* data = kzcVertexListGetData(vertexList);

        kzsAssert(kzcIsValidPointer(vertexList));

        dataElementCount = kzcVertexListGetNumDataElements(vertexList);

        if(kzcVertexListGetFormat(vertexList) == KZC_VERTEX_LIST_FORMAT_LIST_OF_ARRAYS)
        {
            struct KzcDynamicArrayIterator it;
            struct KzcByteReadBuffer byteBuffer;
            kzcByteBufferInitializeRead(&byteBuffer, data, kzcVertexListGetMemorySize(vertexList));

            it = kzcDynamicArrayGetIterator(kzcVertexListGetVertexAttributes(vertexList));
            while(kzcDynamicArrayIterate(it))
            {
                struct KzcVertexAttribute* attribute = (struct KzcVertexAttribute*)kzcDynamicArrayIteratorGetValue(it);

                /* Calculate bounding volume from positions. */
                if(kzcVertexAttributeGetSemantic(attribute) == KZC_VERTEX_ATTRIBUTE_POSITION)
                {
                    kzUint i;

                    for(i = 0; i < dataElementCount; ++i)
                    {
                        struct KzcVector3 vertex;
                        struct KzcVolume newVolume;
                        struct KzcVolume vertexVolume;

                        kzcByteBufferRead(&byteBuffer, vertex);

                        vertexVolume = kzcVolume(vertex.data[0], vertex.data[1], vertex.data[2], 0.0f, 0.0f, 0.0f);
                        if(!volumeAssigned)
                        {
                            boundingVolumeCalculated = vertexVolume;
                            volumeAssigned = KZ_TRUE;
                        }
                        kzcVolumeUnion(&vertexVolume, &boundingVolumeCalculated, &newVolume);
                        boundingVolumeCalculated = newVolume;

                        kzcVector3Add(&boundingBoxMiddle, &vertex, &boundingBoxMiddle);
                    }
                    break;
                }
                else
                {
                    kzcByteBufferSkip(&byteBuffer, kzcVertexAttributeGetMemorySize(attribute));
                }
            }
        }
        else
        {
            struct KzcDynamicArrayIterator it;
            struct KzcByteReadBuffer byteBuffer;
            kzUint vertexAttributesTotalSize = kzcVertexListGetVertexAttributesTotalSize(vertexList);
            kzByte* currentData = (kzByte*)data;
            kzcByteBufferInitializeRead(&byteBuffer, data, kzcVertexListGetMemorySize(vertexList));

            it = kzcDynamicArrayGetIterator(kzcVertexListGetVertexAttributes(vertexList));
            while(kzcDynamicArrayIterate(it))
            {
                struct KzcVertexAttribute* attribute = (struct KzcVertexAttribute*)kzcDynamicArrayIteratorGetValue(it);

                if(kzcVertexAttributeGetSemantic(attribute) == KZC_VERTEX_ATTRIBUTE_POSITION)
                {
                    kzUint i;
                    /*kzUint positionAttributeSize = kzcVertexAttributeGetMemorySize(attribute);*/
                    enum KzcVertexAttributeDataType dataType = kzcVertexAttributeGetDataType(attribute);
                    for(i = 0; i < dataElementCount; ++i)
                    {
                        struct KzcVector3 vertex = KZC_VECTOR3_ZERO;
                        struct KzcVolume newVolume;
                        struct KzcVolume vertexVolume;

                        kzsAssert(currentData != KZ_NULL);
                        if(dataType == KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32)
                        {
                            vertex.data[0] = /*lint -e{826}*/*(kzFloat*)&currentData[0];
                            vertex.data[1] = /*lint -e{826}*/*(kzFloat*)&currentData[4];
                            vertex.data[2] = /*lint -e{826}*/*(kzFloat*)&currentData[8];
                        }
                        else if(dataType == KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16)
                        {
                            kzU16 halfFloatData[3];
                            halfFloatData[0] = /*lint -e{826}*/*(kzU16*)&currentData[0];
                            halfFloatData[1] = /*lint -e{826}*/*(kzU16*)&currentData[2];
                            halfFloatData[2] = /*lint -e{826}*/*(kzU16*)&currentData[4];
                            vertex.data[0] = kzcVertexHalfFloatToFloat(halfFloatData[0]);
                            vertex.data[1] = kzcVertexHalfFloatToFloat(halfFloatData[1]);
                            vertex.data[2] = kzcVertexHalfFloatToFloat(halfFloatData[2]);
                        }
                        else
                        {
                            vertex = KZC_VECTOR3_ZERO;
                            kzsAssertText(KZ_FALSE, "Unsupported vertex format given for bounding volume calculation");
                        }

                        vertexVolume = kzcVolume(vertex.data[0], vertex.data[1], vertex.data[2], 0.0f, 0.0f, 0.0f);
                        if(!volumeAssigned)
                        {
                            boundingVolumeCalculated = vertexVolume;
                            volumeAssigned = KZ_TRUE;
                        }
                        kzcVolumeUnion(&vertexVolume, &boundingVolumeCalculated, &newVolume);
                        boundingVolumeCalculated = newVolume;

                        kzcVector3Add(&boundingBoxMiddle, &vertex, &boundingBoxMiddle);
                        currentData += vertexAttributesTotalSize;
                    }
                    break;
                }
                else
                {
                    kzcByteBufferSkip(&byteBuffer, kzcVertexAttributeGetMemorySize(attribute));
                }
            }
        }
    }

    /* Allocate memory. */
    result = kzcMemoryAllocVariable(memoryManager, boundingVolume, "BoundingVolume");
    kzsErrorForward(result);

    kzuBoundingVolumeSetVolume(boundingVolume, &boundingVolumeCalculated);

    *out_volume = boundingVolume;
    kzsSuccess();
}

kzsError kzuBoundingVolumeCopy(const struct KzcMemoryManager* memoryManager, const struct KzuBoundingVolume* sourceBoundingVolume, struct KzuBoundingVolume** out_targetBoundingVolume)
{
    kzsError result;
    struct KzuBoundingVolume* boundingVolume;

    result = kzuBoundingVolumeCreateEmpty(memoryManager, &boundingVolume);
    kzsErrorForward(result);

    boundingVolume->aabb = sourceBoundingVolume->aabb;
    boundingVolume->sphere = sourceBoundingVolume->sphere;

    *out_targetBoundingVolume = boundingVolume;
    kzsSuccess();
}

kzsError kzuBoundingVolumeDelete(struct KzuBoundingVolume* boundingVolume)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(boundingVolume));

    result = kzcMemoryFreeVariable(boundingVolume);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuBoundingVolumeCalculateTransformedBoundingVolume(struct KzuBoundingVolume* boundingVolume, const struct KzcMatrix4x4* transformation,
                                                         struct KzuTransformedBoundingVolume* out_transformedBoundingVolume)
{
    struct KzuTransformedBoundingVolume transformedBoundingVolume;
    struct KzuBoundingVolumeAxisAlignedBox* box = &boundingVolume->aabb;

    kzsAssert(kzcIsValidPointer(boundingVolume));

    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[0], &transformedBoundingVolume.cornerPoints[0]);
    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[1], &transformedBoundingVolume.cornerPoints[1]);
    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[2], &transformedBoundingVolume.cornerPoints[2]);
    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[3], &transformedBoundingVolume.cornerPoints[3]);
    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[4], &transformedBoundingVolume.cornerPoints[4]);
    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[5], &transformedBoundingVolume.cornerPoints[5]);
    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[6], &transformedBoundingVolume.cornerPoints[6]);
    kzcMatrix4x4MultiplyVector3(transformation, &box->cornerPoints[7], &transformedBoundingVolume.cornerPoints[7]);

    /* TODO: Remove. */
    /*
    }
    else if(boundingVolume->type == KZU_BOUNDING_VOLUME_TYPE_SPHERE)
    {
        struct KzuBoundingVolumeSphere* sphere = &boundingVolume->sphere;

        kzcMatrix4x4MultiplyVector3(transformation, &sphere->center, &transformedBoundingVolume.transformedCenter);
    }*/
    
    *out_transformedBoundingVolume = transformedBoundingVolume;
}

void kzuAxisAlignedBoundingBoxFromTransformedAABB(const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax, const struct KzcMatrix4x4* transform,
                                                  struct KzcVector3* out_aabbMin, struct KzcVector3* out_aabbMax)
{
    kzUint i;
    struct KzuTransformedBoundingVolume transformedVolume;
    struct KzcVector3 cornerPoints[8];
    struct KzcVector3 minCorner, maxCorner;
    cornerPoints[0] = kzcVector3(aabbMin->data[0], aabbMin->data[1], aabbMin->data[2]);
    cornerPoints[1] = kzcVector3(aabbMin->data[0], aabbMax->data[1], aabbMin->data[2]);
    cornerPoints[2] = kzcVector3(aabbMax->data[0], aabbMax->data[1], aabbMin->data[2]);
    cornerPoints[3] = kzcVector3(aabbMax->data[0], aabbMin->data[1], aabbMin->data[2]);
    cornerPoints[4] = kzcVector3(aabbMin->data[0], aabbMin->data[1], aabbMax->data[2]);
    cornerPoints[5] = kzcVector3(aabbMin->data[0], aabbMax->data[1], aabbMax->data[2]);
    cornerPoints[6] = kzcVector3(aabbMax->data[0], aabbMax->data[1], aabbMax->data[2]);
    cornerPoints[7] = kzcVector3(aabbMax->data[0], aabbMin->data[1], aabbMax->data[2]);

    for (i = 0; i < 8; ++i)
    {
        kzcMatrix4x4MultiplyVector3(transform, &cornerPoints[i], &transformedVolume.cornerPoints[i]);
    }

    kzuTransformedBoundingVolumeGetAABB(&transformedVolume, &minCorner, &maxCorner);

    *out_aabbMin = minCorner;
    *out_aabbMax = maxCorner;
}

void kzuTransformedBoundingVolumeCreateFromAABB(const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax, const struct KzcMatrix4x4* worldTransformation,
                                                struct KzuTransformedBoundingVolume* out_transformedBoundingVolume)
{
    kzUint i;
    out_transformedBoundingVolume->cornerPoints[0] = kzcVector3(aabbMin->data[0], aabbMin->data[1], aabbMin->data[2]);
    out_transformedBoundingVolume->cornerPoints[1] = kzcVector3(aabbMin->data[0], aabbMax->data[1], aabbMin->data[2]);
    out_transformedBoundingVolume->cornerPoints[2] = kzcVector3(aabbMax->data[0], aabbMax->data[1], aabbMin->data[2]);
    out_transformedBoundingVolume->cornerPoints[3] = kzcVector3(aabbMax->data[0], aabbMin->data[1], aabbMin->data[2]);
    out_transformedBoundingVolume->cornerPoints[4] = kzcVector3(aabbMin->data[0], aabbMin->data[1], aabbMax->data[2]);
    out_transformedBoundingVolume->cornerPoints[5] = kzcVector3(aabbMin->data[0], aabbMax->data[1], aabbMax->data[2]);
    out_transformedBoundingVolume->cornerPoints[6] = kzcVector3(aabbMax->data[0], aabbMax->data[1], aabbMax->data[2]);
    out_transformedBoundingVolume->cornerPoints[7] = kzcVector3(aabbMax->data[0], aabbMin->data[1], aabbMax->data[2]);

    for (i = 0; i < 8; ++i)
    {
        kzcMatrix4x4MultiplyVector3(worldTransformation, &out_transformedBoundingVolume->cornerPoints[i], &out_transformedBoundingVolume->cornerPoints[i]);
    }
}

void kzuTransformedBoundingVolumeGetAABB(const struct KzuTransformedBoundingVolume* transformedBoundingVolume, struct KzcVector3* out_minimum,
                                         struct KzcVector3* out_maximum)
{
    struct KzcVolume volume = KZC_VOLUME_EMPTY;
    kzBool volumeAssigned = KZ_FALSE;
    kzUint i;

    for(i = 0; i < 8; ++i)
    {
        struct KzcVolume newVolume;
        struct KzcVolume vertexVolume = kzcVolume(transformedBoundingVolume->cornerPoints[i].data[0],
            transformedBoundingVolume->cornerPoints[i].data[1], transformedBoundingVolume->cornerPoints[i].data[2], 0.0f, 0.0f, 0.0f);
        if(!volumeAssigned)
        {
            volumeAssigned = KZ_TRUE;
            volume = vertexVolume;
        }
        kzcVolumeUnion(&vertexVolume, &volume, &newVolume);
        volume = newVolume;
    }

    *out_minimum = kzcVector3(volume.x, volume.y, volume.z);
    *out_maximum = kzcVector3(volume.x + volume.width, volume.y + volume.height, volume.z + volume.depth);
}

kzBool kzuTransformedBoundingVolumeIsAbovePlane(const struct KzuTransformedBoundingVolume* transformedBoundingVolume, const struct KzcPlane* plane)
{
    kzBool inside = KZ_FALSE;
    kzUint i;

    for(i = 0; i < 8; ++i)
    {
        /* If any of the points is inside the plane, break. */
        if(kzcPlaneDistanceToPoint(plane, &transformedBoundingVolume->cornerPoints[i]) > 0.0f)
        {
            inside = KZ_TRUE;
            break;
        }
    }
    /*
    }
    else if(boundingVolume->type == KZU_BOUNDING_VOLUME_TYPE_SPHERE)
    {
        struct KzuBoundingVolumeSphere* sphere = &boundingVolume->sphere;

        if(kzcPlaneDistanceToPoint(plane, &transformedBoundingVolume->transformedCenter) > -sphere->radius)
        {
            inside = KZ_TRUE;
        }
    }*/
    return inside;
}

struct KzuBoundingVolumeAxisAlignedBox* kzuBoundingVolumeGetAxisAlignedBox(struct KzuBoundingVolume* boundingVolume)
{
    return &boundingVolume->aabb;
}

struct KzuBoundingVolumeSphere* kzuBoundingVolumeGetSphere(struct KzuBoundingVolume* boundingVolume)
{
    kzsAssert(kzcIsValidPointer(boundingVolume));

    return &boundingVolume->sphere;
}

struct KzcVector3 kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(const struct KzuBoundingVolumeAxisAlignedBox* boundingVolume)
{
    kzsAssert(kzcIsValidPointer(boundingVolume));
    return boundingVolume->minimumCorner;
}

struct KzcVector3 kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(const struct KzuBoundingVolumeAxisAlignedBox* boundingVolume)
{
    kzsAssert(kzcIsValidPointer(boundingVolume));
    return boundingVolume->maximumCorner;
}

kzFloat kzuBoundingVolumeSphereGetRadius(const struct KzuBoundingVolumeSphere* boundingVolume)
{
    kzsAssert(kzcIsValidPointer(boundingVolume));
    return boundingVolume->radius;
}

struct KzcVector3 kzuTransformedBoundingVolumeBoxGetCornerPoint(const struct KzuTransformedBoundingVolume* boundingVolume, kzUint cornerIndex)
{
    kzsAssert(boundingVolume != KZ_NULL);
    return boundingVolume->cornerPoints[cornerIndex];
}

void kzuBoundingVolumeSetVolume(struct KzuBoundingVolume* boundingVolume, const struct KzcVolume* volume)
{
    kzsAssert(kzcIsValidPointer(boundingVolume));

    /* Box */
    {
        struct KzuBoundingVolumeAxisAlignedBox* boundingBox = &boundingVolume->aabb;
        boundingBox->maximumCorner = kzcVector3(volume->x + volume->width,
                                                volume->y + volume->height,
                                                volume->z + volume->depth);
        boundingBox->minimumCorner = kzcVector3(volume->x, volume->y, volume->z);

        boundingBox->cornerPoints[0] = kzcVector3(boundingBox->minimumCorner.data[0], boundingBox->minimumCorner.data[1], boundingBox->minimumCorner.data[2]);
        boundingBox->cornerPoints[1] = kzcVector3(boundingBox->minimumCorner.data[0], boundingBox->maximumCorner.data[1], boundingBox->minimumCorner.data[2]);
        boundingBox->cornerPoints[2] = kzcVector3(boundingBox->maximumCorner.data[0], boundingBox->maximumCorner.data[1], boundingBox->minimumCorner.data[2]);
        boundingBox->cornerPoints[3] = kzcVector3(boundingBox->maximumCorner.data[0], boundingBox->minimumCorner.data[1], boundingBox->minimumCorner.data[2]);
        boundingBox->cornerPoints[4] = kzcVector3(boundingBox->minimumCorner.data[0], boundingBox->minimumCorner.data[1], boundingBox->maximumCorner.data[2]);
        boundingBox->cornerPoints[5] = kzcVector3(boundingBox->minimumCorner.data[0], boundingBox->maximumCorner.data[1], boundingBox->maximumCorner.data[2]);
        boundingBox->cornerPoints[6] = kzcVector3(boundingBox->maximumCorner.data[0], boundingBox->maximumCorner.data[1], boundingBox->maximumCorner.data[2]);
        boundingBox->cornerPoints[7] = kzcVector3(boundingBox->maximumCorner.data[0], boundingBox->minimumCorner.data[1], boundingBox->maximumCorner.data[2]);
    }

    /* Sphere */
    {
        struct KzuBoundingVolumeSphere* boundingSphere = &boundingVolume->sphere;
        struct KzcVector3 sphereRadius = kzcVector3(volume->width / 2.0f, volume->height / 2.0f, volume->depth / 2.0f);

        boundingSphere->center = kzcVector3(volume->x + volume->width / 2.0f,
                                            volume->y + volume->height / 2.0f,
                                            volume->z + volume->depth / 2.0f);
        boundingSphere->radius = kzcVector3GetLength(&sphereRadius);
    }
}
