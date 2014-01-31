/**
* \file
* Ray picking utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_raypick.h"

#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/filter/kzu_object_source.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/renderer/kzu_renderer.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_ray.h>
#include <core/debug/kzc_log.h>

#include <system/display/kzs_window.h>


kzsError kzuRayPick(const struct KzuRenderer* renderer, const struct KzsWindow* window, const struct KzuObjectSource* pickingSource,
                    const struct KzuTransformedScene* transformedScene, const struct KzuCameraNode* cameraNode,
                    kzInt positionX, kzInt positionY, kzBool pick2D, kzFloat* out_distance, struct KzuObjectNode** out_node,
                    struct KzuTransformedObjectNode** out_transformedNode, struct KzcVector3* out_offsetFromOrigin,
                    struct KzcRay* out_ray)
{
    kzsError result;
    struct KzcVector3 objectOrigin = KZC_VECTOR3_ZERO;
    struct KzcVector3 offsetFromOrigin = KZC_VECTOR3_ZERO;
    struct KzuObjectNode* resultNode = KZ_NULL;
    struct KzuTransformedObjectNode* resultTransformedNode = KZ_NULL;
    kzFloat resultDistance = KZ_FLOAT_MAXIMUM;
    struct KzcRay ray; /* Picking ray */
    struct KzcDynamicArray* allTransformedObjects = kzuTransformedSceneGetObjects(transformedScene);
    struct KzuTransformedObjectNode* transformedCameraNode = kzuTransformedObjectListFindObject(allTransformedObjects,
        kzuCameraNodeToObjectNode(cameraNode));

    kzcRaySet(KZC_VECTOR3_ZERO, KZC_VECTOR3_ZERO, &ray);

    if(transformedCameraNode != KZ_NULL && kzuTransformedObjectNodeGetObjectNode(transformedCameraNode) != KZ_NULL)
    {
        kzFloat windowX;
        kzFloat windowY;
        kzFloat windowWidth;
        kzFloat windowHeight;

        result = kzuRayPickCalculateRay(positionX, positionY, renderer, window, transformedCameraNode, &ray);
        kzsErrorForward(result);

        kzuRendererGetAdjustedViewport(renderer, window, &windowX, &windowY, &windowWidth, &windowHeight);

        /* Find the object the picking ray hits */
        {
            kzBool isHit;
            kzFloat distance;
            struct KzcDynamicArray* pickableTransformedObjects; 
            struct KzcDynamicArrayIterator it;

            result = kzuObjectSourceFetchGraph(pickingSource, kzuTransformedSceneGetObjectSourceRuntimeData(transformedScene),
                                               transformedCameraNode, &pickableTransformedObjects);
            kzsErrorForward(result);

            kzsAssert(kzcIsValidPointer(pickableTransformedObjects)); /* Assert; Ray picking should be done after kzuEngineExtractScene() call */

            /* Loop through all objects */
            it = kzcDynamicArrayGetIterator(pickableTransformedObjects);
            while(kzcDynamicArrayIterate(it))
            {
                struct KzuTransformedObjectNode* transformedNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
                struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);
                
                kzBool pickable2D = kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_PICKABLE_2D);
                kzBool pickable = kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_PICKABLE);

                if (pick2D ? pickable2D : pickable)
                {
                    struct KzcVector3 position;
                    struct KzcMatrix4x4 transformedNodeMatrix = kzuTransformedObjectNodeGetMatrix(transformedNode);
                    struct KzuBoundingVolume* boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);
                    kzcMatrix4x4GetPosition(&transformedNodeMatrix, &position);
                    isHit = KZ_FALSE;
                    distance = 0.0f;

                    if (boundingVolume != KZ_NULL)
                    {
                        struct KzuBoundingVolumeAxisAlignedBox* boundingBox = kzuBoundingVolumeGetAxisAlignedBox(boundingVolume);
                        struct KzcVector3 minimumCorner = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(boundingBox);
                        struct KzcVector3 maximumCorner = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(boundingBox);

                        isHit = kzcRayBoundingBoxIntersection(&ray, &minimumCorner, &maximumCorner, &transformedNodeMatrix, &distance);
                    }

                    if (isHit && distance < resultDistance && distance > 0.0f)
                    {
                        objectOrigin = position;
                        resultNode = objectNode;
                        resultTransformedNode = transformedNode;
                        resultDistance = distance;
                    }
                }
            }
        }

        /* Calculate the vector objects origin to hit position. */
        {
            struct KzcVector3 hitPosition = kzcVector3(kzcVector3GetX(&ray.origin) + kzcVector3GetX(&ray.direction) * resultDistance, 
                kzcVector3GetY(&ray.origin) + kzcVector3GetY(&ray.direction) * resultDistance, 
                kzcVector3GetZ(&ray.origin) + kzcVector3GetZ(&ray.direction) * resultDistance);
            kzcVector3Subtract(&hitPosition, &objectOrigin, &offsetFromOrigin);
        }
    }

    *out_offsetFromOrigin = offsetFromOrigin;
    *out_distance = resultDistance;
    *out_node = resultNode;
    if (out_transformedNode != KZ_NULL)
    {
        *out_transformedNode = resultTransformedNode;
    }
    *out_ray = ray;
    kzsSuccess();
}

kzsError kzuRayPickCalculateRay(kzInt pointerX, kzInt pointerY, const struct KzuRenderer* renderer,
                                const struct KzsWindow* window, 
                                const struct KzuTransformedObjectNode* transformedCameraNode, struct KzcRay* out_ray)
{
    struct KzcRay ray;
    struct KzuObjectNode* cameraObjectNode = kzuTransformedObjectNodeGetObjectNode(transformedCameraNode);
    kzcRaySet(KZC_VECTOR3_ZERO, KZC_VECTOR3_ZERO, &ray);

    if (cameraObjectNode != KZ_NULL)
    {
        struct KzuCameraNode* cameraNode;
        /* Window */
        kzFloat windowX;
        kzFloat windowY;
        kzFloat windowWidth;
        kzFloat windowHeight;

        enum KzuFieldOfViewType fovType;
        kzFloat farHeight;
        kzFloat nearHeight;
        /* Position in relative coordinates [-1,1] */
        kzInt relativeX;
        kzInt relativeY;
        kzFloat relativeXf;
        kzFloat relativeYf;

        /* Perspective settings */
        kzFloat aspectRatioX;
        kzFloat aspectRatioY;
        kzFloat nearClipPlane;
        kzFloat farClipPlane;
        kzFloat fieldOfView;
        /* View transformation matrix */
        struct KzcMatrix4x4 cameraMatrix;
        /* Ray coordinates in camera space */
        struct KzcVector3 rayStart;
        struct KzcVector3 rayEnd;
        /* Ray coordinates in world coordinates */
        struct KzcVector3 rayEndTransformed, rayStartTransformed;

        cameraNode = kzuCameraNodeFromObjectNode(cameraObjectNode);
        nearClipPlane = kzuCameraNodeGetZNear(cameraNode);
        farClipPlane = kzuCameraNodeGetZFar(cameraNode);
        fieldOfView = kzsDegreesToRadians(kzuCameraNodeGetFov(cameraNode));
        fovType = kzuCameraNodeGetFovType(cameraNode);
        cameraMatrix = kzuTransformedObjectNodeGetMatrix(transformedCameraNode);

        if (kzuCameraNodeGetDisableAspectRatio(cameraNode))
        {
            kzFloat dummy;
            kzFloat width;
            kzFloat height;
            kzuRendererGetAdjustedViewport(renderer, window, &dummy, &dummy, &width, &height);
            kzuCameraNodeSetInheritedAspectRatio(cameraNode, width / height);
        }

        aspectRatioX = kzuCameraNodeGetAspectRatio(cameraNode);
        aspectRatioY = 1.0f;

        /* Convert fov. */
        if(fovType == KZU_CAMERA_FIELD_OF_VIEW_VERTICAL)
        {
            fieldOfView = 2.0f * kzsAtanf(kzsTanf(fieldOfView * 0.5f) / aspectRatioX);
        }

        if (kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_PERSPECTIVE)
        {
            kzFloat fovTangent = kzsTanf(fieldOfView * 0.5f);
            farHeight = farClipPlane * fovTangent;
            nearHeight = nearClipPlane * fovTangent;
        }
        else
        {
            farHeight = kzuCameraNodeGetOrthogonalPlaneHeight(cameraNode);
            nearHeight = kzuCameraNodeGetOrthogonalPlaneHeight(cameraNode);
            if(kzuCameraNodeGetFovType(cameraNode) == KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL)
            {
                aspectRatioX = 1.0f * kzuCameraNodeGetAspectRatio(cameraNode);
                aspectRatioY = 1.0f;
            }
            else
            {
                aspectRatioX = 1.0f;
                aspectRatioY = 1.0f / kzuCameraNodeGetAspectRatio(cameraNode);
            }
        }

        kzuRendererGetAdjustedViewport(renderer, window, &windowX, &windowY, &windowWidth, &windowHeight);

        /* Adjust settings for fixed viewport. */
        {
            kzFloat fixedW, fixedH;
            kzBool relative;
            kzuRendererGetViewport(renderer, KZ_NULL, KZ_NULL, &fixedW, &fixedH, KZ_NULL, &relative);
    
            if(!relative)
            {
                windowX = 0.0f;
                windowY = kzsWindowGetHeight(window) - fixedH;
                windowWidth = fixedW;
                windowHeight = fixedH;
            }
        }

        /* Move pointer position to scaled window coordinates. Remove black borders because of aspect ratio. */
        pointerX -= (kzInt)windowX;
        pointerY -= (kzInt)windowY;

        relativeX = pointerX - ((kzInt)windowWidth / 2);
        relativeY = ((kzInt)windowHeight - pointerY) - ((kzInt)windowHeight / 2);
        relativeXf = relativeX / ((kzFloat)windowWidth  * 0.5f);
        relativeYf = relativeY / ((kzFloat)windowHeight * 0.5f);

        rayStart = kzcVector3(relativeXf * nearHeight * aspectRatioX, relativeYf * nearHeight * aspectRatioY, -nearClipPlane);
        rayEnd = kzcVector3(relativeXf * farHeight * aspectRatioX, relativeYf * farHeight * aspectRatioY, -farClipPlane);

        /* Transform ray coordinates from camera space to world space */
        kzcMatrix4x4MultiplyVector3(&cameraMatrix, &rayEnd, &rayEndTransformed);
        kzcMatrix4x4MultiplyVector3(&cameraMatrix, &rayStart, &rayStartTransformed);

        /* Calculate picking ray */
        {
            struct KzcVector3 direction;
            kzcVector3Subtract(&rayEndTransformed, &rayStartTransformed, &direction);
            kzcRaySet(rayStartTransformed, direction, &ray);
        }
    }

    *out_ray = ray;
    kzsSuccess();
}
