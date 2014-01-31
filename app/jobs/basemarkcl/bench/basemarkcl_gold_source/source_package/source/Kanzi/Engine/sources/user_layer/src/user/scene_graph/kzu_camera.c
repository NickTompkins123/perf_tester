/**
* \file
* Camera node and data
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_camera.h"

#include "kzu_object.h"
#include "kzu_object_private.h"

#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_plane.h>

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_math.h>


struct KzuCameraNode
{
    struct KzuObjectNode objectNode;
};

struct KzuCameraNodeData
{
    struct KzuObjectNodeData objectNodeData; /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuCameraNodeData::objectNodeData) Suppressed as this member is required but not referenced. */ 
    kzFloat inheritedAspectRatio;
};


/** Object node class for camera. */
extern const struct KzuObjectNodeClass KZU_CAMERA_NODE_CLASS;

/** Object type identifier for Camera objects. */
const KzuObjectType KZU_OBJECT_TYPE_CAMERA = (void*)&KZU_CAMERA_NODE_CLASS;


static struct KzuCameraNodeData* kzuCameraNodeGetData_internal(const struct KzuCameraNode* cameraNode)
{
    kzsAssert(kzuObjectNodeIsValid(&cameraNode->objectNode));

    return (struct KzuCameraNodeData*)cameraNode->objectNode.data;
}

static kzsError kzuCameraCreateObjectNodeData_internal(const struct KzcMemoryManager* memoryManager,
                                                       struct KzuObjectNodeData** out_cameraNodeData)
{
    kzsError result;
    struct KzuCameraNodeData* cameraNodeData;

    result = kzcMemoryAllocVariable(memoryManager, cameraNodeData, "Camera node data");
    kzsErrorForward(result);

    cameraNodeData->inheritedAspectRatio = 1.6666f;

    *out_cameraNodeData = &cameraNodeData->objectNodeData;
    kzsSuccess();
}

kzsError kzuCameraNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                             kzString name, struct KzuCameraNode** out_cameraNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;
    struct KzuCameraNode* cameraNode;

    result = kzuCameraCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_CAMERA_NODE_CLASS, name, objectNodeData, &objectNode);
    kzsErrorForward(result);
    
    cameraNode = (struct KzuCameraNode*)objectNode;

    *out_cameraNode = cameraNode;
    kzsSuccess();
}

kzsError kzuCameraNodeDelete(struct KzuCameraNode* cameraNode)
{
    kzsError result;

    result = kzuObjectNodeDeleteBase_private(&cameraNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuCameraNode* kzuCameraNodeFromObjectNode(const struct KzuObjectNode* objectNode)
{
    struct KzuCameraNode* cameraNode;

    kzsAssert(kzcIsValidPointer(objectNode));
    kzsAssert(objectNode->data->objectNodeClass == &KZU_CAMERA_NODE_CLASS);

    cameraNode = (struct KzuCameraNode*)objectNode;

    return cameraNode;
}

struct KzuObjectNode* kzuCameraNodeToObjectNode(const struct KzuCameraNode* cameraNode)
{
    return (struct KzuObjectNode*)cameraNode;
}

kzsError kzuCameraNodeSetOrthogonalProjection(const struct KzuCameraNode* cameraNode, enum KzuOrthogonalCoordinateSystem coordinateSystem)
{
    kzsError result;
    struct KzuObjectNode* cameraObjectNode = kzuCameraNodeToObjectNode(cameraNode);

    result = kzuObjectNodeSetIntProperty(cameraObjectNode, KZU_PROPERTY_TYPE_CAMERA_PROJECTION_TYPE, /*lint -e{930}*/(kzInt)KZU_PROJECTION_ORTHOGRAPHIC);
    kzsErrorForward(result);
    result = kzuObjectNodeSetIntProperty(cameraObjectNode, KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE, /*lint -e{930}*/(kzInt)coordinateSystem);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuCameraNodeSetPerspectiveProjection(const struct KzuCameraNode* cameraNode)
{
    kzsError result;
    struct KzuObjectNode* cameraObjectNode = kzuCameraNodeToObjectNode(cameraNode);

    result = kzuObjectNodeSetIntProperty(cameraObjectNode, KZU_PROPERTY_TYPE_CAMERA_PROJECTION_TYPE, /*lint -e{930}*/(kzInt)KZU_PROJECTION_PERSPECTIVE);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuCameraNodeSetAspectRatio(const struct KzuCameraNode* cameraNode, kzFloat aspectRatio)
{
    kzsError result;
    struct KzuObjectNode* cameraObjectNode = kzuCameraNodeToObjectNode(cameraNode);

    result = kzuObjectNodeSetFloatProperty(cameraObjectNode, KZU_PROPERTY_TYPE_CAMERA_ASPECT_RATIO, aspectRatio);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat kzuCameraNodeGetAspectRatio(const struct KzuCameraNode *cameraNode)
{
    struct KzuObjectNode* cameraObjectNode = kzuCameraNodeToObjectNode(cameraNode);
    struct KzuCameraNodeData* cameraNodeData = kzuCameraNodeGetData_internal(cameraNode);
    kzFloat aspectRatio = 0.0f;

    kzsAssert(kzcIsValidPointer(cameraNodeData));

    if (kzuCameraNodeGetDisableAspectRatio(cameraNode))
    {
        aspectRatio = cameraNodeData->inheritedAspectRatio;
    }
    else
    {
        aspectRatio = kzuObjectNodeGetFloatPropertyDefault(cameraObjectNode, KZU_PROPERTY_TYPE_CAMERA_ASPECT_RATIO);
    }
    
    return aspectRatio;
}

void kzuCameraNodeSetInheritedAspectRatio(const struct KzuCameraNode* cameraNode, kzFloat inheritedAspectRatio)
{
    struct KzuCameraNodeData* cameraNodeData = kzuCameraNodeGetData_internal(cameraNode);

    kzsAssert(kzcIsValidPointer(cameraNodeData));

    cameraNodeData->inheritedAspectRatio = inheritedAspectRatio;
}

kzsError kzuCameraNodeLookAt(const struct KzuCameraNode* cameraNode, struct KzcVector3 position, 
                             struct KzcVector3 lookAtPoint, struct KzcVector3 upVector)
{
    kzsError result;
    struct KzcVector3 forward;
    struct KzcVector3 right;
    struct KzcVector3 up;
    struct KzcMatrix4x4 cameraMatrix;

    if(kzcVector3IsEqual(&position, &lookAtPoint))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Camera position and look-at points should not be equal!");
    }
    if(kzcVector3IsEqual(&upVector, &KZC_VECTOR3_ZERO))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Camera up vector should not be zero!");
    }

    /* Calculate the forward vector Z */
    kzcVector3Subtract(&lookAtPoint, &position, &forward);
    kzcVector3Normalize(&forward);
    kzcVector3Normalize(&upVector);

    /* Right vector X = Y x Z. */
    kzcVector3Cross(&forward, &upVector, &right);
    kzcVector3Normalize(&right);
    
    /* Up vector Y = Z x X */
    kzcVector3Cross(&right, &forward, &up);
    kzcVector3Normalize(&up);

    kzcMatrix4x4SetIdentity(&cameraMatrix);
    
    cameraMatrix.data[0] = kzcVector3GetX(&right);    cameraMatrix.data[4] = kzcVector3GetY(&right);    cameraMatrix.data[8] = kzcVector3GetZ(&right);
    cameraMatrix.data[1] = kzcVector3GetX(&up);       cameraMatrix.data[5] = kzcVector3GetY(&up);       cameraMatrix.data[9] = kzcVector3GetZ(&up);
    cameraMatrix.data[2] = -kzcVector3GetX(&forward); cameraMatrix.data[6] = -kzcVector3GetY(&forward); cameraMatrix.data[10] = -kzcVector3GetZ(&forward);

    {
        struct KzcMatrix4x4 cameraMatrixInverse;

        kzcMatrix4x4Inverse(&cameraMatrix, &cameraMatrixInverse);
        cameraMatrixInverse.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = kzcVector3GetX(&position);
        cameraMatrixInverse.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = kzcVector3GetY(&position);
        cameraMatrixInverse.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = kzcVector3GetZ(&position);

        result = kzuObjectNodeSetTransformation((struct KzuObjectNode*)cameraNode, &cameraMatrixInverse);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void kzuCameraNodeCalculateFrustumPlanes(const struct KzuCameraNode* cameraNode, const struct KzcMatrix4x4* cameraViewMatrix,
                                         struct KzcPlane* out_frustumPlanes)
{
    struct KzcVector3 viewPosition;
    struct KzcVector3 viewDirection;
    struct KzcVector3 viewRight;
    struct KzcVector3 viewUp;
    struct KzcMatrix4x4 viewMatrixInverse;
    struct KzcVector3 nearPlanePoint;
    kzFloat nearPlaneWidth = 1.0f;
    kzFloat nearPlaneHeight = 1.0f;
    struct KzcPlane frustumPlanes[6];
    enum KzuProjectionType projectionType = kzuCameraNodeGetProjectionType(cameraNode);
    
    viewDirection = kzcVector3(-cameraViewMatrix->data[2], -cameraViewMatrix->data[6], -cameraViewMatrix->data[10]);
    viewUp = kzcVector3(cameraViewMatrix->data[1], cameraViewMatrix->data[5], cameraViewMatrix->data[9]);
    viewRight = kzcVector3(cameraViewMatrix->data[0], cameraViewMatrix->data[4], cameraViewMatrix->data[8]);
    kzcMatrix4x4Inverse(cameraViewMatrix, &viewMatrixInverse);
    viewPosition = kzcVector3(viewMatrixInverse.data[12], viewMatrixInverse.data[13], viewMatrixInverse.data[14]);

    kzcVector3Normalize(&viewDirection);
    kzcVector3Normalize(&viewUp);
    kzcVector3Normalize(&viewRight);

    /* Calculate near plane width & height, horizontal FOV. */
    {
        kzFloat aspectRatio = kzuCameraNodeGetAspectRatio(cameraNode);
        enum KzuFieldOfViewType fovType = kzuCameraNodeGetFovType(cameraNode);
        kzFloat value;

        if (projectionType == KZU_PROJECTION_PERSPECTIVE)
        {
            value = kzsTanf(kzsDegreesToRadians(kzuCameraNodeGetFov(cameraNode)) / 2.0f) * kzuCameraNodeGetZNear(cameraNode);
        }
        else if (projectionType == KZU_PROJECTION_ORTHOGRAPHIC)
        {
            if (fovType == KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL)
            {
                value = kzuCameraNodeGetOrthogonalPlaneHeight(cameraNode);
            }
            else
            {
                value = kzuCameraNodeGetZNear(cameraNode);
            }
        }
        else
        {
            kzsAssert(KZ_FALSE);
            value = 1.0f;
        }
        
        if (fovType == KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL)
        {
            nearPlaneHeight = value;
            nearPlaneWidth = aspectRatio * nearPlaneHeight;
        }
        else
        {
            nearPlaneWidth = value;
            nearPlaneHeight = 1.0f / aspectRatio * nearPlaneWidth;
        }
    }

    if (projectionType == KZU_PROJECTION_ORTHOGRAPHIC)
    {
        if (kzuCameraNodeGetOrthogonalCoordinateSystemType(cameraNode) == KZU_ORTHOGONAL_COORDINATE_SYSTEM_ABSOLUTE)
        {
            nearPlaneWidth = KZ_FLOAT_MAXIMUM / 2.0f;
            nearPlaneHeight = KZ_FLOAT_MAXIMUM / 2.0f;
        }
    }

    /* Extract planes. */
    {
        struct KzcVector3 directionScaled;
        struct KzcVector3 viewDirectionInverse;
        struct KzcVector3 planeNormal;

        /* Create near plane. */
        {
            kzcVector3Scale(&viewDirection, kzuCameraNodeGetZNear(cameraNode), &directionScaled);
            kzcVector3Add(&viewPosition, &directionScaled, &nearPlanePoint);
            kzcPlaneCreateFromPointAndNormal(&frustumPlanes[KZU_FRUSTUM_PLANE_NEAR], &nearPlanePoint, &viewDirection);
        }
        /* Create far plane. */
        {
            struct KzcVector3 farPlanePoint;
            kzcVector3Scale(&viewDirection, kzuCameraNodeGetZFar(cameraNode), &directionScaled);
            kzcVector3Add(&viewPosition, &directionScaled, &farPlanePoint);
            kzcVector3Subtract(&KZC_VECTOR3_ZERO, &viewDirection, &viewDirectionInverse);
            kzcPlaneCreateFromPointAndNormal(&frustumPlanes[KZU_FRUSTUM_PLANE_FAR], &farPlanePoint, &viewDirectionInverse);
        }
        /* Create right plane. */
        {
            struct KzcVector3 rightPlaneDirection;
            struct KzcVector3 pointOnRightPlane = nearPlanePoint;
            struct KzcVector3 viewRightScaled;

            kzcVector3Scale(&viewRight, nearPlaneWidth, &viewRightScaled);
            kzcVector3Add(&pointOnRightPlane, &viewRightScaled, &pointOnRightPlane);

            /* Calculate right plane's direction. */
            if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_PERSPECTIVE)
            {
                kzcVector3Subtract(&pointOnRightPlane, &viewPosition, &rightPlaneDirection);
                
            }
            else if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_ORTHOGRAPHIC)
            {
                rightPlaneDirection = viewRightScaled;
            }
            
            kzcVector3Normalize(&rightPlaneDirection);
            /* Calculate right plane's normal up x dir. */
            kzcVector3Cross(&viewUp, &rightPlaneDirection, &planeNormal);
            kzcVector3Normalize(&planeNormal);

            kzcPlaneCreateFromPointAndNormal(&frustumPlanes[KZU_FRUSTUM_PLANE_RIGHT], &pointOnRightPlane, &planeNormal);
        }
        /* Create left plane. */
        {
            struct KzcVector3 leftPlaneDirection;
            struct KzcVector3 pointOnLeftPlane = nearPlanePoint;
            struct KzcVector3 viewLeftScaled;
            
            /* Calculate left plane's direction. */
            kzcVector3Scale(&viewRight, -nearPlaneWidth, &viewLeftScaled);
            kzcVector3Add(&pointOnLeftPlane, &viewLeftScaled, &pointOnLeftPlane);

            if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_PERSPECTIVE)
            {
                kzcVector3Subtract(&pointOnLeftPlane, &viewPosition, &leftPlaneDirection);
            }
            else if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_ORTHOGRAPHIC)
            {
                leftPlaneDirection = viewLeftScaled;
            }

            kzcVector3Normalize(&leftPlaneDirection);
            /* Calculate left plane's normal dir x up. */
            kzcVector3Cross(&leftPlaneDirection, &viewUp, &planeNormal);
            kzcVector3Normalize(&planeNormal);

            kzcPlaneCreateFromPointAndNormal(&frustumPlanes[KZU_FRUSTUM_PLANE_LEFT], &pointOnLeftPlane, &planeNormal);
        }
        /* Create top plane. */
        {
            struct KzcVector3 topPlaneDirection;
            struct KzcVector3 pointOnTopPlane = nearPlanePoint;
            struct KzcVector3 viewTopScaled;
            kzcVector3Scale(&viewUp, nearPlaneHeight, &viewTopScaled);
            kzcVector3Add(&pointOnTopPlane, &viewTopScaled, &pointOnTopPlane);

            if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_PERSPECTIVE)
            {
                kzcVector3Subtract(&pointOnTopPlane, &viewPosition, &topPlaneDirection);
            }
            else if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_ORTHOGRAPHIC)
            {
                topPlaneDirection = viewTopScaled;
            }

            kzcVector3Normalize(&topPlaneDirection);
            /* Calculate top plane's normal dir x right. */
            kzcVector3Cross(&topPlaneDirection, &viewRight, &planeNormal);
            kzcVector3Normalize(&planeNormal);

            kzcPlaneCreateFromPointAndNormal(&frustumPlanes[KZU_FRUSTUM_PLANE_TOP], &pointOnTopPlane, &planeNormal);
        }
        /* Create bottom plane. */
        {
            struct KzcVector3 bottomPlaneDirection;
            struct KzcVector3 pointOnBottomPlane = nearPlanePoint;
            struct KzcVector3 viewBottomScaled;
            kzcVector3Scale(&viewUp, -nearPlaneHeight, &viewBottomScaled);
            kzcVector3Add(&pointOnBottomPlane, &viewBottomScaled, &pointOnBottomPlane);
            if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_PERSPECTIVE)
            {
                kzcVector3Subtract(&pointOnBottomPlane, &viewPosition, &bottomPlaneDirection);
            }
            else if(kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_ORTHOGRAPHIC)
            {
                bottomPlaneDirection = viewBottomScaled;
            }

            kzcVector3Normalize(&bottomPlaneDirection);

            /* Calculate bottom plane's normal right x dir. */
            kzcVector3Cross(&viewRight, &bottomPlaneDirection, &planeNormal);
            kzcVector3Normalize(&planeNormal);

            kzcPlaneCreateFromPointAndNormal(&frustumPlanes[KZU_FRUSTUM_PLANE_BOTTOM], &pointOnBottomPlane, &planeNormal);
        }
    }
    out_frustumPlanes[0] = frustumPlanes[0];
    out_frustumPlanes[1] = frustumPlanes[1];
    out_frustumPlanes[2] = frustumPlanes[2];
    out_frustumPlanes[3] = frustumPlanes[3];
    out_frustumPlanes[4] = frustumPlanes[4];
    out_frustumPlanes[5] = frustumPlanes[5];
}

void kzuCameraNodeCalculateOrthogonalProjectionFromValues(kzFloat left, kzFloat right, kzFloat top, kzFloat bottom, 
                                                          enum KzuOrthogonalCoordinateSystem coordinateSystem, 
                                                          enum KzuFieldOfViewType fovType, kzFloat orthoHeight, kzFloat aspectRatio,
                                                          struct KzcMatrix4x4* out_projectionMatrix)
{
    kzFloat scale;

    kzsAssert(orthoHeight != 0.0f);

    kzcMatrix4x4CreateOrthogonalProjection(out_projectionMatrix, left, right, bottom, top, -1.0f, 1.0f);

    if(coordinateSystem == KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE)
    {
        scale = 1.0f / orthoHeight;

        if(fovType == KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL)
        {
            out_projectionMatrix->data[0] *= 1.0f / aspectRatio;
            out_projectionMatrix->data[5] *= 1.0f;
        }
        else if(fovType == KZU_CAMERA_FIELD_OF_VIEW_VERTICAL)
        {
            out_projectionMatrix->data[0] *= 1.0f;
            out_projectionMatrix->data[5] *= 1.0f * aspectRatio;
        }

        out_projectionMatrix->data[0] *= scale;
        out_projectionMatrix->data[5] *= scale;
        out_projectionMatrix->data[10] *= scale * 0.01f;
    }
}

void kzuCameraNodeCalculateOrthogonalProjection(const struct KzuCameraNode* cameraNode, kzFloat left, kzFloat right, kzFloat top, kzFloat bottom, 
                                                struct KzcMatrix4x4* out_projectionMatrix)
{
    kzuCameraNodeCalculateOrthogonalProjectionFromValues(left, right, top, bottom, kzuCameraNodeGetOrthogonalCoordinateSystemType(cameraNode), 
                                                         kzuCameraNodeGetFovType(cameraNode), kzuCameraNodeGetOrthogonalPlaneHeight(cameraNode),
                                                         kzuCameraNodeGetAspectRatio(cameraNode), out_projectionMatrix);

}

void kzuCameraNodeCalculateProjectionMatrix(const struct KzuCameraNode* cameraNode,
                                            struct KzcMatrix4x4* out_projectionMatrix)
{
    /* Cotangent definition: cot(x) = 1 / tan(x) */
    kzFloat f;
    kzFloat nearClip = kzuCameraNodeGetZNear(cameraNode);
    kzFloat farClip = kzuCameraNodeGetZFar(cameraNode);
    enum KzuFieldOfViewType fovType = kzuCameraNodeGetFovType(cameraNode);
    kzFloat aspectRatio = kzuCameraNodeGetAspectRatio(cameraNode);

    /* From OpenGL specification (gluPerspective):
        Depth buffer precision is affected by the values specified for zNear and zFar. 
        The greater the ratio of zFar to zNear is, the less effective the depth buffer will be
        at distinguishing between surfaces that are near each other. If
            r = zFar/zNear
        roughly log2(r) bits of depth buffer precision are lost. 
        Because r approaches infinity as zNear approaches 0, zNear must never be set to 0. 
    */
    
    kzcMatrix4x4SetIdentity(out_projectionMatrix);

    f = 1.0f / kzsTanf(kzsDegreesToRadians(kzuCameraNodeGetFov(cameraNode)) / 2.0f);

    if (fovType == KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL)
    {
        out_projectionMatrix->data[0] = f / aspectRatio;
        out_projectionMatrix->data[5] = f;
    }
    else if (fovType == KZU_CAMERA_FIELD_OF_VIEW_VERTICAL)
    {
        out_projectionMatrix->data[0] = f;
        out_projectionMatrix->data[5] = f * aspectRatio;
    }

    /* TODO: Do not read output parameter */

    out_projectionMatrix->data[10] = (nearClip + farClip) / (nearClip - farClip);
    out_projectionMatrix->data[11] = -1.0f;
    out_projectionMatrix->data[14] = (2.0f * farClip * nearClip) / (nearClip - farClip);
    out_projectionMatrix->data[15] = 0.0f;
}

kzsError kzuCameraNodeCloneData(const struct KzuCameraNode* sourceCameraNode, const struct KzuCameraNode* destinationCameraNode)
{
    kzsError result;
    struct KzuCameraNodeData* sourceData = kzuCameraNodeGetData_internal(sourceCameraNode);
    struct KzuCameraNodeData* destinationData = kzuCameraNodeGetData_internal(destinationCameraNode);
    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(kzuCameraNodeToObjectNode(sourceCameraNode));

    kzsAssert(kzcIsValidPointer(sourceData));
    kzsAssert(kzcIsValidPointer(destinationData));

    result = kzuPropertyManagerRemoveProperties(propertyManager, destinationCameraNode);
    kzsErrorForward(result);

    result = kzuPropertyManagerCopyProperties(propertyManager, sourceCameraNode, destinationCameraNode);
    kzsErrorForward(result);

    destinationData->inheritedAspectRatio = sourceData->inheritedAspectRatio;

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuCameraNodeDeleteImplementation_internal(struct KzuObjectNode* objectNode)
{
    kzsError result = kzuCameraNodeDelete(kzuCameraNodeFromObjectNode(objectNode));
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuCameraNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                     const struct KzuObjectNode* sourceObjectNode,
                                                                     struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;

    result = kzuCameraCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    {
        struct KzuCameraNodeData* sourceCameraNodeData = ((struct KzuCameraNodeData*)sourceObjectNode->data);
        struct KzuCameraNodeData* cameraNodeData = (struct KzuCameraNodeData*)objectNodeData;

        cameraNodeData->inheritedAspectRatio = sourceCameraNodeData->inheritedAspectRatio;
    }

    *out_objectNode = objectNode;
    kzsSuccess();
}

const struct KzuObjectNodeClass KZU_CAMERA_NODE_CLASS =
{
    kzuCameraNodeDeleteImplementation_internal,
    kzuCameraNodeCopyImplementation_internal,
    KZ_NULL,
    KZ_NULL
};
