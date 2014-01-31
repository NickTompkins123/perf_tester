/**
* \file
* Transform gizmo utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_transform_gizmo.h"

#include <user/filter/kzu_object_source.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/properties/kzu_property_manager.h>
#include <user/material/kzu_material.h>
#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader.h>
#include <user/binary/kzu_binary_directory.h>
#include <user/engine/kzu_engine.h>
#include <user/util/kzu_pick.h>
#include <user/util/kzu_raypick.h>

#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_ray.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>


/** Axis on which the transform gizmo is operating on. For convenience, this is a bit flag. */
typedef kzU32 KzuTransformGizmoAxis;

#define KZU_TRANSFORM_GIZMO_AXIS_NONE 0 /**< No axis. */
#define KZU_TRANSFORM_GIZMO_AXIS_X    1 /**< X axis. */
#define KZU_TRANSFORM_GIZMO_AXIS_Y    2 /**< Y axis. */
#define KZU_TRANSFORM_GIZMO_AXIS_Z    4 /**< Z axis. */
#define KZU_TRANSFORM_GIZMO_AXIS_XY   (8 + KZU_TRANSFORM_GIZMO_AXIS_X + KZU_TRANSFORM_GIZMO_AXIS_Y) /**< XY axises, value has to be bigger than single axises. */
#define KZU_TRANSFORM_GIZMO_AXIS_YZ   (8 + KZU_TRANSFORM_GIZMO_AXIS_Y + KZU_TRANSFORM_GIZMO_AXIS_Z) /**< YZ axises. */
#define KZU_TRANSFORM_GIZMO_AXIS_XZ   (8 + KZU_TRANSFORM_GIZMO_AXIS_X + KZU_TRANSFORM_GIZMO_AXIS_Z) /**< XZ axises. */


/** Data to keep the snap info in. */
struct KzuSnapData
{
    kzFloat shortestDistance;
    kzFloat snapDistance;
    kzUint snapAxis;
};

struct KzuTransformGizmo
{
    enum KzuTransformGizmoMode mode; /** Current mode. */
    enum KzuTransformGizmoReferenceMode referenceMode; /** Current reference mode. */
    kzMutableString targetObjectPath; /** Current target object path. */
    
    KzuTransformGizmoCallback callback;
    void* callbackUserData;

    kzBool dragMode;    /** Drag mode flag. */
    kzBool gridSnapEnabled; /** Grid snap on/off */
    kzInt dragX, dragY; /** Drag mode coordinates. */
    kzFloat gridSize;   /** Grid snap size. */
    enum KzuTransformGizmoGridSnapMode gridSnapMode; /**< Grid snap mode. */
    KzuTransformGizmoAxis axis; /** Drag mode axis. */
    struct KzcVector3 startOffset;
    struct KzcMatrix4x4 startObjectTransform, startGizmoTransform;
    kzFloat rotateOffset;

    struct KzuProject* project; /** Gizmo project. */
    struct KzuScene* scene; /** Gizmo scene. */
    struct KzuTransformedScene* transformedScene; /** Gizmo transformed scene. */
    struct KzuObjectNode* rootNode; /** Gizmo root node. */
    struct KzuObjectNode* translateNode; /** Gizmo translate node. */
    struct KzuObjectNode* rotateNode; /** Gizmo rotate node. */
    struct KzuObjectNode* scaleNode; /** Gizmo scale node. */
    struct KzuObjectNode *translateAxisX, *translateAxisY, *translateAxisZ;
    struct KzuObjectNode *translateAxisXY, *translateAxisXZ, *translateAxisYZ;
    struct KzuObjectNode *rotateAxisX, *rotateAxisY, *rotateAxisZ;
    struct KzuObjectNode *scaleAxisX, *scaleAxisY, *scaleAxisZ;
    struct KzuObjectNode *scaleAxisXY, *scaleAxisXZ, *scaleAxisYZ;

    struct KzuMaterial* materialAxisX;
    struct KzuMaterial* materialAxisY;
    struct KzuMaterial* materialAxisZ;
    struct KzuMaterial* materialAxisXY;
    struct KzuMaterial* materialAxisYZ;
    struct KzuMaterial* materialAxisXZ;    
};


#define KZU_TRANSFORM_GIZMO_SCENE_PATH "Scenes/SRTHandles.dae/Root Node/Scene_Root"


/** Round all axises of a vector into a grid size. */
static void kzuRoundVector3ToGrid_internal(struct KzcVector3* source, kzFloat grid, KzuTransformGizmoAxis axis,
                                           struct KzcVector3* out_vector)
{
    kzFloat* in;
    kzFloat* out;

    kzsAssert(source != NULL);
    kzsAssert(out_vector != NULL);

    in = source->data;
    out =  out_vector->data;

    if((axis & KZU_TRANSFORM_GIZMO_AXIS_X) != 0)
    {
        out[0] = kzsFloorf(in[0] / grid + 0.5f) * grid;
    }
    if((axis & KZU_TRANSFORM_GIZMO_AXIS_Y) != 0)
    {
        out[1] = kzsFloorf(in[1] / grid + 0.5f) * grid;
    }
    if((axis & KZU_TRANSFORM_GIZMO_AXIS_Z) != 0)
    {
        out[2] = kzsFloorf(in[2] / grid + 0.5f) * grid;
    }
}

/* Find the shortest snapping distance between two bounding boxes.
*
* Returns true if snapped, false otherwise.
*/
static kzBool kzuTransformGizmoSnapToBoundingBox_internal(const struct KzcVector3* aaMin, const struct KzcVector3* aaMax,
                                                          const struct KzcVector3* bbMin, const struct KzcVector3* bbMax,
                                                          kzFloat gridSize, kzUint index, struct KzuSnapData* out_snapData)
{
    kzFloat oppositeMax = bbMin->data[index] - aaMax->data[index];
    kzFloat oppositeMin = bbMax->data[index] - aaMin->data[index];
    kzFloat nearMax = bbMax->data[index] - aaMax->data[index];
    kzFloat nearMin = bbMin->data[index] - aaMin->data[index];
    kzFloat oppositeMaxAbs = kzsFabsf(oppositeMax);
    kzFloat oppositeMinAbs = kzsFabsf(oppositeMin);
    kzFloat nearMaxAbs = kzsFabsf(nearMax);
    kzFloat nearMinAbs = kzsFabsf(nearMin);
    kzFloat distance = out_snapData->shortestDistance;
    kzBool returnValue = KZ_FALSE;

    if((oppositeMaxAbs < gridSize) && (oppositeMaxAbs < distance))
    {
        distance = oppositeMaxAbs;
        out_snapData->shortestDistance = oppositeMaxAbs;
        out_snapData->snapDistance = oppositeMax;
        out_snapData->snapAxis = index;
        returnValue = KZ_TRUE;
    }

    if((oppositeMinAbs < gridSize) && (oppositeMinAbs < distance))
    {
        distance = oppositeMinAbs;
        out_snapData->shortestDistance = oppositeMinAbs;
        out_snapData->snapDistance = oppositeMin;
        out_snapData->snapAxis = index;
        returnValue = KZ_TRUE;
    }

    if((nearMaxAbs < gridSize) && (nearMaxAbs < distance))
    {
        distance = nearMaxAbs;
        out_snapData->shortestDistance = nearMaxAbs;
        out_snapData->snapDistance = nearMax;
        out_snapData->snapAxis = index;
        returnValue = KZ_TRUE;
    }

    if((nearMinAbs < gridSize) && (nearMinAbs < distance))
    {
        out_snapData->shortestDistance = nearMinAbs; /* No need to set distance anymore. */
        out_snapData->snapDistance = nearMin;
        out_snapData->snapAxis = index;
        returnValue = KZ_TRUE;
    }

    return returnValue;
}

/** Find potential shorter distance to the bounding box of an object. Write translation info if found. */
static kzBool kzuTransformGizmoSnapToObject_internal(const struct KzcVector3* aaMin, const struct KzcVector3* aaMax,
                                                     kzFloat gridSize, KzuTransformGizmoAxis axis,
                                                     const struct KzuTransformedObjectNode* node,
                                                     struct KzuSnapData* out_snapData)
{
    struct KzcMatrix4x4 matrix = kzuTransformedObjectNodeGetMatrix(node);
    struct KzuObjectNode* plainObjectNode = kzuTransformedObjectNodeGetObjectNode(node);
    struct KzuBoundingVolume* volume = kzuObjectNodeGetBoundingVolume(plainObjectNode);
    kzBool returnValue = KZ_FALSE;

    if(volume != KZ_NULL)
    {
        struct KzuBoundingVolumeAxisAlignedBox* box = kzuBoundingVolumeGetAxisAlignedBox(volume);
        struct KzcVector3 bbMinTemp = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(box);
        struct KzcVector3 bbMaxTemp = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(box);
        struct KzcVector3 bbMin;
        struct KzcVector3 bbMax;

        kzuAxisAlignedBoundingBoxFromTransformedAABB(&bbMinTemp, &bbMaxTemp, &matrix, &bbMin, &bbMax);
        
        if((axis & KZU_TRANSFORM_GIZMO_AXIS_X) != 0)
        {
            if(kzuTransformGizmoSnapToBoundingBox_internal(aaMin, aaMax, &bbMin, &bbMax, gridSize, 0, out_snapData))
            {
                returnValue = KZ_TRUE;
            }
        }
        if((axis & KZU_TRANSFORM_GIZMO_AXIS_Y) != 0)
        {
            if(kzuTransformGizmoSnapToBoundingBox_internal(aaMin, aaMax, &bbMin, &bbMax, gridSize, 1, out_snapData))
            {
                returnValue = KZ_TRUE;
            }
        }
        if((axis & KZU_TRANSFORM_GIZMO_AXIS_Z) != 0)
        {
            if(kzuTransformGizmoSnapToBoundingBox_internal(aaMin, aaMax, &bbMin, &bbMax, gridSize, 2, out_snapData))
            {
                returnValue = KZ_TRUE;
            }
        }
    }

    return returnValue;
}

/** Snap to any object in an object list that is not a children of specified node.
*
* Snapping starts at distance gridSize. */
static void kzuTransformGizmoSnapToObjectList_internal(const struct KzcVector3* position, kzFloat gridSize,
                                                       KzuTransformGizmoAxis axis, const struct KzuTransformedObjectNode* node,
                                                       const struct KzcDynamicArray* objectList,
                                                       struct KzcVector3* out_position)
{
    struct KzcMatrix4x4 matrix = kzuTransformedObjectNodeGetMatrix(node);
    struct KzuObjectNode* plainObjectNode = kzuTransformedObjectNodeGetObjectNode(node);
    struct KzuBoundingVolume* volume = kzuObjectNodeGetBoundingVolume(plainObjectNode);
    struct KzcDynamicArrayIterator it;
    struct KzcVector3 aaMin;
    struct KzcVector3 aaMax;
    struct KzcVector3 formerPosition;
    struct KzuSnapData snapData;
    kzBool snapFound = KZ_FALSE;

    {
        struct KzuBoundingVolumeAxisAlignedBox* box = kzuBoundingVolumeGetAxisAlignedBox(volume);
        struct KzcVector3 aaMinTemp = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(box);
        struct KzcVector3 aaMaxTemp = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(box);

        kzuAxisAlignedBoundingBoxFromTransformedAABB(&aaMinTemp, &aaMaxTemp, &matrix, &aaMin, &aaMax);
    }

    kzuTransformedObjectNodeGetPosition(node, &formerPosition);

    kzcVector3Subtract(position, &formerPosition, &formerPosition);
    kzcVector3Add(&formerPosition, &aaMin, &aaMin);
    kzcVector3Add(&formerPosition, &aaMax, &aaMax);

    snapData.snapAxis = 0;
    snapData.snapDistance = 0.0f;
    snapData.shortestDistance = FLT_MAX;
    it =  kzcDynamicArrayGetIterator(objectList);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* currentNode = kzcDynamicArrayIteratorGetValue(it);
        struct KzuObjectNode* plainChildNode = kzuTransformedObjectNodeGetObjectNode(currentNode);


        if(!kzuObjectnodeIsParentOf(plainObjectNode, plainChildNode))
        {
            if(kzuTransformGizmoSnapToObject_internal(&aaMin, &aaMax, gridSize, axis, currentNode, &snapData))
            {
                snapFound = KZ_TRUE;
            }
        }
    }

    if(snapFound)
    {
        *out_position = *position;
        out_position->data[snapData.snapAxis] += snapData.snapDistance;
    }
}

/* TODO: Comment */
static kzsError kzuTransformGizmoPickAxis_internal(const struct KzuTransformGizmo* gizmo, const struct KzuEngine* engine,
                                                   struct KzuPick* pick, kzUint x, kzUint y, struct KzuObjectNode** out_axisNode)
{
    kzsError result;
    struct KzuObjectNode* axisNode = KZ_NULL;    

    /* Hit test the gizmo. */
    kzBool oldPickMode = kzuPickIsDoubleSided(pick);
    kzuPickEnableDoubleSided(pick, KZ_TRUE);

    result = kzuPick(pick, engine, kzuProjectGetRootSource(gizmo->project), gizmo->transformedScene,
                     kzuSceneGetViewCamera(gizmo->scene), x, y, &axisNode, KZ_NULL);
    kzsErrorForward(result);

    kzuPickEnableDoubleSided(pick, oldPickMode);

    *out_axisNode = axisNode;
    kzsSuccess();
}

/* TODO: Comment */
static kzsError kzuTransformGizmoSyncWithScene_internal(const struct KzuTransformGizmo* gizmo, const struct KzuProject* project,
                                                        const struct KzuTransformedScene* transformedScene, struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, gizmo->targetObjectPath);

    if (kzuObjectNodeGetParent(objectNode) == KZ_NULL)
    {
        objectNode = KZ_NULL;
    }
    else
    {
        struct KzuTransformedObjectNode* objectInWorldCoordinates;
        struct KzuTransformedObjectNode* cameraObjectInWorldCoordinates;
        struct KzuScene* scene = kzuTransformedSceneGetScene(transformedScene);
        struct KzuCameraNode* cameraNode = kzuSceneGetViewCamera(scene);
        struct KzuObjectNode* cameraObjectNode = kzuCameraNodeToObjectNode(cameraNode);
        struct KzcDynamicArray* objectList = kzuTransformedSceneGetObjects(transformedScene);

        objectInWorldCoordinates = kzuTransformedObjectListFindObject(objectList, objectNode);
        cameraObjectInWorldCoordinates = kzuTransformedObjectListFindObject(objectList, cameraObjectNode);

        if (objectInWorldCoordinates == KZ_NULL || cameraObjectInWorldCoordinates == KZ_NULL)
        {
            objectNode = KZ_NULL;
        }
        else
        {
            struct KzcMatrix4x4 objectMatrix = kzuTransformedObjectNodeGetMatrix(objectInWorldCoordinates);
            struct KzcMatrix4x4 cameraMatrix = kzuTransformedObjectNodeGetMatrix(cameraObjectInWorldCoordinates);
            struct KzuCameraNode* gizmoCameraNode = kzuSceneGetViewCamera(gizmo->scene);
            struct KzuObjectNode* gizmoCameraObjectNode = kzuCameraNodeToObjectNode(gizmoCameraNode);

            struct KzcMatrix4x4 newTransform, scaleTransform, normalizedTransform;
            kzFloat distance = 0.15f;

            if (kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_PERSPECTIVE)
            {
                struct KzcVector3 cameraPosition;
                struct KzcVector3 objectPosition;
                struct KzcVector3 distanceVector;

                kzcMatrix4x4GetPosition(&cameraMatrix, &cameraPosition);
                kzcMatrix4x4GetPosition(&objectMatrix, &objectPosition);

                kzcVector3Subtract(&objectPosition, &cameraPosition, &distanceVector);
                distance *= kzcVector3GetLength(&distanceVector);
            }
            else if (kzuCameraNodeGetProjectionType(cameraNode) == KZU_PROJECTION_ORTHOGRAPHIC)
            {
                distance *= kzuCameraNodeGetOrthogonalPlaneHeight(cameraNode) * 2.0f;
            }

            scaleTransform = KZC_MATRIX4X4_IDENTITY;
            kzcMatrix4x4ScaleUniform(&scaleTransform, distance);
            kzcMatrix4x4Normalize(&objectMatrix, &normalizedTransform);
            kzcMatrix4x4Multiply(&scaleTransform, &normalizedTransform, &newTransform);

            result = kzuCameraNodeCloneData(cameraNode, gizmoCameraNode);
            kzsErrorForward(result);
            result = kzuObjectNodeSetTransformation(gizmoCameraObjectNode, &cameraMatrix);
            kzsErrorForward(result);
            result = kzuObjectNodeSetTransformation(gizmo->rootNode, &newTransform);
            kzsErrorForward(result);
        }
    }

    result = kzuTransformedSceneExtract(gizmo->transformedScene, gizmo->scene);
    kzsErrorForward(result);

    *out_objectNode = objectNode;
    kzsSuccess();
}

static kzsError kzuTransformPickOnAxis_internal(const struct KzuTransformGizmo* gizmo, const struct KzuEngine* engine,
                                                kzUint x, kzUint y, struct KzcVector3* out_vector, kzBool* out_picked, kzFloat* out_distance)
{
    kzsError result;
    struct KzcVector3 resultVector = KZC_VECTOR3_ZERO;
    struct KzcMatrix4x4 worldTransform;
    struct KzcRay ray;
    struct KzcVector3 position;
    struct KzcPlane xPlane, yPlane, zPlane;
    struct KzcPlane pickPlane = kzcPlane(KZS_NAN, KZS_NAN, KZS_NAN, KZS_NAN);
    struct KzcVector3 transformedUp, transformedRight, transformedFront;
    kzBool picked = KZ_FALSE;    
    kzFloat lengthX, lengthY, lengthZ;
    struct KzcVector3 axisVector = KZC_VECTOR3_ZERO;
    kzFloat distance = 0.0f;

    if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_ROTATE || gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_SCALE)
    {
        /* Original transform is used for rotation picking. */
        worldTransform = gizmo->startGizmoTransform;
    }
    else
    {
        worldTransform = kzuObjectNodeGetTransformation(gizmo->rootNode);
    }

    {
        struct KzcDynamicArray* allTransformedObjects = kzuTransformedSceneGetObjects(gizmo->transformedScene);
        struct KzuTransformedObjectNode* transformedCameraNode = kzuTransformedObjectListFindObject(allTransformedObjects, kzuCameraNodeToObjectNode(kzuSceneGetViewCamera(gizmo->scene)));
        result = kzuRayPickCalculateRay((kzInt)x, (kzInt)y, kzuEngineGetRenderer(engine), kzuEngineGetActiveWindow(engine), transformedCameraNode, &ray);
        kzsErrorForward(result);
    }
    
    kzcMatrix4x4GetPosition(&worldTransform, &position);

    {
        struct KzcVector3 projectedX, projectedY, projectedZ;       

        /* Get axis vectors in world space. */
        kzcMatrix4x4MultiplyVector3By3x3(&worldTransform, &KZC_VECTOR3_X_AXIS, &transformedRight);
        kzcMatrix4x4MultiplyVector3By3x3(&worldTransform, &KZC_VECTOR3_Y_AXIS, &transformedUp);
        kzcMatrix4x4MultiplyVector3By3x3(&worldTransform, &KZC_VECTOR3_Z_AXIS, &transformedFront);
        kzcVector3Normalize(&transformedRight);
        kzcVector3Normalize(&transformedUp);
        kzcVector3Normalize(&transformedFront);        

        kzcPlaneCreateFromPointAndNormal(&xPlane, &position, &transformedRight);
        kzcPlaneCreateFromPointAndNormal(&yPlane, &position, &transformedUp);
        kzcPlaneCreateFromPointAndNormal(&zPlane, &position, &transformedFront);

        /* Find most orthogonal plane. */
        kzcPlaneProjectVector3(&xPlane, &ray.direction, &projectedX);
        kzcPlaneProjectVector3(&yPlane, &ray.direction, &projectedY);
        kzcPlaneProjectVector3(&zPlane, &ray.direction, &projectedZ);

        lengthX = kzcVector3GetLength(&projectedX);
        lengthY = kzcVector3GetLength(&projectedY);
        lengthZ = kzcVector3GetLength(&projectedZ);

        if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_ROTATE)
        {
            if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_X)
            {
                pickPlane = yPlane;
                axisVector = transformedFront;
                if (lengthZ < lengthY)
                {
                    pickPlane = zPlane;
                    axisVector = transformedUp;
                }
            }
            else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Y)
            {
                pickPlane = xPlane;
                axisVector = transformedFront;
                if (lengthZ < lengthX)
                {
                    pickPlane = zPlane;
                    axisVector = transformedRight;
                }
            }
            else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Z)
            {
                pickPlane = xPlane;
                axisVector = transformedUp;
                if (lengthY < lengthX)
                {
                    pickPlane = yPlane;
                    axisVector = transformedRight;
                }
            }
        }
        else
        {
            if (gizmo->axis >= KZU_TRANSFORM_GIZMO_AXIS_XY)
            {
                if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_XY)
                {
                    pickPlane = zPlane;
                }
                else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_XZ)
                {
                    pickPlane = yPlane;
                }
                else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_YZ)
                {
                    pickPlane = xPlane;
                }
            }
            else
            {
                if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_X)
                {
                    pickPlane = (lengthY < lengthZ) ? yPlane : zPlane;
                }
                else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Y)
                {
                    pickPlane = (lengthZ < lengthX) ? zPlane : xPlane;
                }
                else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Z)
                {
                    pickPlane = (lengthX < lengthY) ? xPlane : yPlane;
                }
            }
        }        
    }

    {
        struct KzcVector3 intersectionPoint;
        kzFloat intersectionDistance;
        if (kzcPlaneRayIntersection(&pickPlane, &ray, 0.05f, &intersectionDistance, &intersectionPoint))
        {
            struct KzcVector3 differenceVector;
            kzcVector3Subtract(&intersectionPoint, &position, &differenceVector);

            picked = KZ_TRUE;

            if (gizmo->axis >= KZU_TRANSFORM_GIZMO_AXIS_XY)
            {
                resultVector = differenceVector;
            }
            else
            {
                /* Project onto axis vector. */
                if (gizmo->mode != KZU_TRANSFORM_GIZMO_MODE_ROTATE)
                {
                    if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_X)
                    {
                        axisVector = transformedRight;
                    }
                    else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Y)
                    {
                        axisVector = transformedUp;
                    }
                    else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Z)
                    {
                        axisVector = transformedFront;
                    }
                }

                distance = kzcVector3Dot(&axisVector, &differenceVector);
                kzcVector3Scale(&axisVector, distance, &resultVector);
            }
        }
    }

    *out_vector = resultVector;
    *out_picked = picked;
    *out_distance = distance;
    kzsSuccess();
}

kzsError kzuTransformGizmoCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                 kzString gizmoResourcePath, struct KzuTransformGizmo** out_gizmo)
{
    kzsError result;
    struct KzuTransformGizmo* gizmo;

    kzsAssert(gizmoResourcePath != KZ_NULL);

    result = kzcMemoryAllocVariable(memoryManager, gizmo, "TransformGizmo");
    kzsErrorForward(result);

    result = kzuTransformedSceneCreate(memoryManager, &gizmo->transformedScene);
    kzsErrorForward(result);

    gizmo->callback = KZ_NULL;
    gizmo->callbackUserData = KZ_NULL;

    gizmo->mode = KZU_TRANSFORM_GIZMO_MODE_NONE;
    gizmo->referenceMode = KZU_TRANSFORM_GIZMO_REFERENCE_LOCAL;
    gizmo->targetObjectPath = KZ_NULL;
    gizmo->dragMode = KZ_FALSE;
    gizmo->gridSnapEnabled = KZ_FALSE;
    gizmo->gridSize = 1.0f;
    gizmo->gridSnapMode = KZU_TRANSFORM_GIZMO_GRID_SNAP_RELATIVE; /* NONE only when transform gizmo is not available. */
    gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_NONE;
    gizmo->startOffset = KZC_VECTOR3_ZERO;
    gizmo->rotateOffset = 0.0f;

    /* Load the gizmo scene. */
    {
        struct KzuBinaryDirectory* directory;

        /* Create initial binary directory. */
        result = kzuBinaryDirectoryCreateFromFileResource(memoryManager, gizmoResourcePath, &directory);
        kzsErrorForward(result);

        result = kzuProjectCreate(memoryManager, propertyManager, messageDispatcher, directory, &gizmo->project);
        kzsErrorForward(result);

        /* Load the scene from the file. */
        result = kzuProjectLoaderLoadScene(gizmo->project, "Scenes/SRTHandles.dae", &gizmo->scene);
        kzsErrorForward(result);
    }

    /* Get the nodes for manipulation later. */
    {
        gizmo->rootNode = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH);

        gizmo->translateNode = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/TranslateHandle");
        gizmo->rotateNode = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/RotateHandle");
        gizmo->scaleNode = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/ScaleHandle");

        gizmo->scaleAxisX = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/ScaleHandle/ScaleHandle_ScaleX");
        gizmo->scaleAxisY = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/ScaleHandle/ScaleHandle_ScaleY");
        gizmo->scaleAxisZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/ScaleHandle/ScaleHandle_ScaleZ");
        gizmo->scaleAxisXY = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/ScaleHandle/ScaleHandle_ScaleXY");
        gizmo->scaleAxisXZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/ScaleHandle/ScaleHandle_ScaleXZ");
        gizmo->scaleAxisYZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/ScaleHandle/ScaleHandle_ScaleYZ");
        
        gizmo->rotateAxisX = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/RotateHandle/RotateHandle_RotateX");
        gizmo->rotateAxisY = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/RotateHandle/RotateHandle_RotateY");
        gizmo->rotateAxisZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/RotateHandle/RotateHandle_RotateZ");
        
        gizmo->translateAxisX = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/TranslateHandle/TranslateHandle_TranslateX");
        gizmo->translateAxisY = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/TranslateHandle/TranslateHandle_TranslateY");
        gizmo->translateAxisZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/TranslateHandle/TranslateHandle_TranslateZ");
        gizmo->translateAxisXY = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/TranslateHandle/TranslateHandle_TranslateXY");
        gizmo->translateAxisXZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/TranslateHandle/TranslateHandle_TranslateXZ");
        gizmo->translateAxisYZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, KZU_TRANSFORM_GIZMO_SCENE_PATH "/TranslateHandle/TranslateHandle_TranslateYZ");

        kzsAssert(gizmo->rootNode != KZ_NULL);
        kzsAssert(gizmo->translateNode != KZ_NULL);
        kzsAssert(gizmo->rotateNode != KZ_NULL);
        kzsAssert(gizmo->scaleNode != KZ_NULL);

        kzsAssert(gizmo->scaleAxisX != KZ_NULL);
        kzsAssert(gizmo->scaleAxisY != KZ_NULL);
        kzsAssert(gizmo->scaleAxisZ != KZ_NULL);
        kzsAssert(gizmo->scaleAxisXY != KZ_NULL);
        kzsAssert(gizmo->scaleAxisXZ != KZ_NULL);
        kzsAssert(gizmo->scaleAxisYZ != KZ_NULL);
        
        kzsAssert(gizmo->rotateAxisX != KZ_NULL);
        kzsAssert(gizmo->rotateAxisY != KZ_NULL);
        kzsAssert(gizmo->rotateAxisZ != KZ_NULL);
        
        kzsAssert(gizmo->translateAxisX != KZ_NULL);
        kzsAssert(gizmo->translateAxisY != KZ_NULL);
        kzsAssert(gizmo->translateAxisZ != KZ_NULL);
        kzsAssert(gizmo->translateAxisXY != KZ_NULL);
        kzsAssert(gizmo->translateAxisXZ != KZ_NULL);
        kzsAssert(gizmo->translateAxisYZ != KZ_NULL);

        result = kzuObjectNodeRemoveChild(gizmo->rootNode, gizmo->translateNode);
        kzsErrorForward(result);
        result = kzuObjectNodeRemoveChild(gizmo->rootNode, gizmo->rotateNode);
        kzsErrorForward(result);
        result = kzuObjectNodeRemoveChild(gizmo->rootNode, gizmo->scaleNode);
        kzsErrorForward(result);
    }

    /* Get materials. */
    {
        gizmo->materialAxisX = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, "Materials/Imported Materials/X");
        gizmo->materialAxisY = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, "Materials/Imported Materials/Y");
        gizmo->materialAxisZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, "Materials/Imported Materials/Z");
        gizmo->materialAxisXY = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, "Materials/Imported Materials/XY");
        gizmo->materialAxisYZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, "Materials/Imported Materials/YZ");
        gizmo->materialAxisXZ = kzuProjectGetObject(gizmo->project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, "Materials/Imported Materials/XZ");

        kzsAssert(gizmo->materialAxisX != KZ_NULL);
        kzsAssert(gizmo->materialAxisY != KZ_NULL);
        kzsAssert(gizmo->materialAxisZ != KZ_NULL);
        kzsAssert(gizmo->materialAxisXY != KZ_NULL);
        kzsAssert(gizmo->materialAxisYZ != KZ_NULL);
        kzsAssert(gizmo->materialAxisXZ != KZ_NULL);
    }

    *out_gizmo = gizmo;
    kzsSuccess();
}

kzsError kzuTransformGizmoDelete(struct KzuTransformGizmo* gizmo)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(gizmo));

    if (gizmo->targetObjectPath != KZ_NULL)
    {
        result = kzcStringDelete(gizmo->targetObjectPath);
        kzsErrorForward(result);
    }

    result = kzuBinaryDirectoryDelete(kzuProjectGetBinaryDirectory(gizmo->project));
    kzsErrorForward(result);
    result = kzuProjectDelete(gizmo->project);
    kzsErrorForward(result);
    result = kzuTransformedSceneDelete(gizmo->transformedScene);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(gizmo);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuTransformGizmoSetMode(struct KzuTransformGizmo* gizmo, enum KzuTransformGizmoMode mode)
{
    kzsError result;
    enum KzuTransformGizmoMode oldMode;
    struct KzuObjectNode* oldNode;
    struct KzuObjectNode* newNode;
    
    kzsAssert(kzcIsValidPointer(gizmo));

    oldMode = gizmo->mode;
    gizmo->mode = mode;

    /* TODO: Use object node visibility flag after it is implemented in Engine */
    switch (oldMode)
    {
        case KZU_TRANSFORM_GIZMO_MODE_NONE: oldNode = KZ_NULL; break;
        case KZU_TRANSFORM_GIZMO_MODE_TRANSLATE: oldNode = gizmo->translateNode; break;
        case KZU_TRANSFORM_GIZMO_MODE_ROTATE: oldNode = gizmo->rotateNode; break;
        case KZU_TRANSFORM_GIZMO_MODE_SCALE: oldNode = gizmo->scaleNode; break;
        default: oldNode = KZ_NULL; break;
    }
    if (oldNode != KZ_NULL)
    {
        result = kzuObjectNodeRemoveChild(gizmo->rootNode, oldNode);
        kzsErrorForward(result);
    }

    switch (mode)
    {
        case KZU_TRANSFORM_GIZMO_MODE_NONE: newNode = KZ_NULL; break;
        case KZU_TRANSFORM_GIZMO_MODE_TRANSLATE: newNode = gizmo->translateNode; break;
        case KZU_TRANSFORM_GIZMO_MODE_ROTATE: newNode = gizmo->rotateNode; break;
        case KZU_TRANSFORM_GIZMO_MODE_SCALE: newNode = gizmo->scaleNode; break;
        default: newNode = KZ_NULL; break;
    }
    if (newNode != KZ_NULL)
    {
        result = kzuObjectNodeAddChild(gizmo->rootNode, newNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

enum KzuTransformGizmoMode kzuTransformGizmoGetMode(const struct KzuTransformGizmo* gizmo)
{
    kzsAssert(kzcIsValidPointer(gizmo));

    return gizmo->mode;
}

kzsError kzuTransformGizmoSetTarget(struct KzuTransformGizmo* gizmo, kzString objectPath)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(gizmo));

    /* Clean up old string if any. */
    if (gizmo->targetObjectPath != KZ_NULL)
    {
        result = kzcStringDelete(gizmo->targetObjectPath);
        kzsErrorForward(result);
    }

    if (objectPath == KZ_NULL)
    {
        gizmo->targetObjectPath = KZ_NULL;
    }
    else
    {
        result = kzcStringCopy(kzcMemoryGetManager(gizmo), objectPath, &gizmo->targetObjectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzString kzuTransformGizmoGetTarget(const struct KzuTransformGizmo* gizmo)
{
    kzsAssert(kzcIsValidPointer(gizmo));

    return gizmo->targetObjectPath;
}

kzBool kzuTransformGizmoIsGridSnapEnabled(const struct KzuTransformGizmo* gizmo)
{
    kzsAssert(gizmo != NULL);

    return gizmo->gridSnapEnabled;
}

void kzuTransformGizmoSetGridSnapEnabled(struct KzuTransformGizmo* gizmo, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(gizmo));

    gizmo->gridSnapEnabled = enabled;
}

kzFloat kzuTransformGizmoGetGridSize(const struct KzuTransformGizmo* gizmo)
{
    kzsAssert(gizmo != NULL);

    return gizmo->gridSize;
}

void kzuTransformGizmoSetGridSize(struct KzuTransformGizmo* gizmo, kzFloat size)
{
    kzsAssert(gizmo != NULL);

    if(size > 0.0f)
    {
        gizmo->gridSize = size;
    }
}

enum KzuTransformGizmoGridSnapMode kzuTransformGizmoGetGridSnapMode(const struct KzuTransformGizmo* gizmo)
{
    kzsAssert(gizmo != NULL);

    return gizmo->gridSnapMode;
}

void kzuTransformGizmoSetGridSnapMode(struct KzuTransformGizmo* gizmo, enum KzuTransformGizmoGridSnapMode mode)
{
    kzsAssert(gizmo != NULL);

    gizmo->gridSnapMode = mode;
}

kzsError kzuTransformGizmoRender(const struct KzuTransformGizmo* gizmo, const struct KzuEngine* engine,
                                 const struct KzuProject* project, const struct KzuTransformedScene* transformedScene)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(gizmo));
    kzsAssert(kzcIsValidPointer(engine));
    kzsAssert(kzcIsValidPointer(project));

    if (gizmo->mode != KZU_TRANSFORM_GIZMO_MODE_NONE &&
        gizmo->targetObjectPath != KZ_NULL)
    {
        struct KzuObjectNode* objectNode;

        result = kzuTransformGizmoSyncWithScene_internal(gizmo, project, transformedScene, &objectNode);
        kzsErrorForward(result);

        if (objectNode != KZ_NULL)
        {
            result = kzuEngineRender(engine, gizmo->transformedScene);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuTransformGizmoProcessInput(struct KzuTransformGizmo* gizmo, const struct KzuEngine* engine, const struct KzuProject* project,
                                       struct KzuTransformedScene* transformedScene,
                                       struct KzuPick* pick, kzInt x, kzInt y, enum KzsInputPointingDeviceState state, kzBool* out_handled)
{
    kzsError result;
    kzBool handled = KZ_FALSE;
    kzBool transformModified = KZ_FALSE;
    struct KzuScene* scene;

    kzsAssert(kzcIsValidPointer(gizmo));
    kzsAssert(kzcIsValidPointer(engine));
    kzsAssert(kzcIsValidPointer(project));
    kzsAssert(kzcIsValidPointer(pick));

    scene = kzuTransformedSceneGetScene(transformedScene);

    kzsAssert(scene != KZ_NULL);

    if (gizmo->mode != KZU_TRANSFORM_GIZMO_MODE_NONE &&
        gizmo->targetObjectPath != KZ_NULL)
    {
        struct KzuObjectNode* objectNode;

        result = kzuTransformedSceneExtract(transformedScene, scene);
        kzsErrorForward(result);

        result = kzuTransformGizmoSyncWithScene_internal(gizmo, project, transformedScene, &objectNode);
        kzsErrorForward(result);

        if (state == KZS_POINTING_DEVICE_STATE_DOWN)
        {
            if (objectNode != KZ_NULL)
            {
                /* Hit test the gizmo. */
                struct KzuObjectNode* axisNode = KZ_NULL;
                result = kzuTransformGizmoPickAxis_internal(gizmo, engine, pick, (kzUint)x, (kzUint)y, &axisNode);
                kzsErrorForward(result);

                if (axisNode != KZ_NULL)
                {
                    /* Enter drag mode. */
                    kzBool picked;
                    handled = KZ_TRUE;
                    gizmo->dragMode = KZ_TRUE;
                    gizmo->dragX = x;
                    gizmo->dragY = y;
                    gizmo->startGizmoTransform = kzuObjectNodeGetTransformation(gizmo->rootNode);
                    gizmo->startObjectTransform = kzuObjectNodeGetTransformation(objectNode);

                    if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_SCALE)
                    {
                        if (axisNode == gizmo->scaleAxisX)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_X;
                        }
                        else if (axisNode == gizmo->scaleAxisY)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_Y;
                        }
                        else if (axisNode == gizmo->scaleAxisZ)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_Z;
                        }
                        else if (axisNode == gizmo->scaleAxisXY)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_XY;
                        }
                        else if (axisNode == gizmo->scaleAxisXZ)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_XZ;
                        }
                        else if (axisNode == gizmo->scaleAxisYZ)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_YZ;
                        }
                    }
                    else if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_ROTATE)
                    {
                        if (axisNode == gizmo->rotateAxisX)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_X;
                        }
                        else if (axisNode == gizmo->rotateAxisY)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_Y;
                        }
                        else if (axisNode == gizmo->rotateAxisZ)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_Z;
                        }
                    }
                    else if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_TRANSLATE)
                    {
                        if (axisNode == gizmo->translateAxisX)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_X;
                        }
                        else if (axisNode == gizmo->translateAxisY)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_Y;
                        }
                        else if (axisNode == gizmo->translateAxisZ)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_Z;
                        }
                        else if (axisNode == gizmo->translateAxisXY)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_XY;
                        }
                        else if (axisNode == gizmo->translateAxisXZ)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_XZ;
                        }
                        else if (axisNode == gizmo->translateAxisYZ)
                        {
                            gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_YZ;
                        }
                    }

                    /* Save initial offset. */
                    result = kzuTransformPickOnAxis_internal(gizmo, engine, (kzUint)x, (kzUint)y, &gizmo->startOffset, &picked, &gizmo->rotateOffset);
                    kzsErrorForward(result);
                }
            }
        }
        else if (state == KZS_POINTING_DEVICE_STATE_UP)
        {
            if (gizmo->dragMode)
            {
                handled = KZ_TRUE;

                /* Cancel drag mode. */
                gizmo->dragMode = KZ_FALSE;
                gizmo->axis = KZU_TRANSFORM_GIZMO_AXIS_NONE;
                gizmo->startOffset = KZC_VECTOR3_ZERO;
                gizmo->startObjectTransform = KZC_MATRIX4X4_IDENTITY;
                gizmo->startGizmoTransform = KZC_MATRIX4X4_IDENTITY;
            }
        }
        else if (state == KZS_POINTING_DEVICE_STATE_DRAG && gizmo->dragMode)
        {
            const kzInt yDrag = y - gizmo->dragY;
            const kzInt xDrag = x - gizmo->dragX;

            handled = KZ_TRUE;

            /* Ensure that at least some dragging happened. */
            if (!(xDrag == 0 && yDrag == 0))
            {
                /* Update drag mode. */
                if (objectNode != KZ_NULL)
                {
                    kzBool picked;
                    kzFloat distance;
                    struct KzcVector3 pickPosition;
                    result = kzuTransformPickOnAxis_internal(gizmo, engine, (kzUint)x, (kzUint)y, &pickPosition, &picked, &distance);
                    kzsErrorForward(result);

                    if (picked)
                    {
                        struct KzcVector3 offsetDifference;
                        struct KzcMatrix4x4 matrix = kzuObjectNodeGetTransformation(objectNode);
                        struct KzcMatrix4x4 inverseWorld, inverseParent, inverseNormalizedWorld, inverseNormalizedParent;
                        struct KzcMatrix4x4 newMatrix;
                        struct KzcDynamicArray* objectList = kzuTransformedSceneGetObjects(transformedScene);
                        struct KzuTransformedObjectNode* objectInWorldCoordinates = kzuTransformedObjectListFindObject(objectList, objectNode);
                        struct KzuTransformedObjectNode* parent = kzuTransformedObjectNodeGetParent(objectInWorldCoordinates);

                        kzcMatrix4x4SetIdentity(&newMatrix);

                        kzcVector3Subtract(&pickPosition, &gizmo->startOffset, &offsetDifference);

                        {
                            struct KzcMatrix4x4 world = kzuTransformedObjectNodeGetMatrix(objectInWorldCoordinates);
                            struct KzcMatrix4x4 parentTransform = kzuTransformedObjectNodeGetMatrix(parent);
                            struct KzcMatrix4x4 normalizedWorld, normalizedParent;
                            kzcMatrix4x4Inverse(&world, &inverseWorld);
                            kzcMatrix4x4Inverse(&parentTransform, &inverseParent);
                            kzcMatrix4x4Normalize(&world, &normalizedWorld);
                            kzcMatrix4x4Inverse(&normalizedWorld, &inverseNormalizedWorld);
                            kzcMatrix4x4Normalize(&parentTransform, &normalizedParent);
                            kzcMatrix4x4Inverse(&normalizedParent, &inverseNormalizedParent);
                        }
                        
                        if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_TRANSLATE)
                        {
                            struct KzcVector3 newPosition;
                            kzBool gridSnapEnabled = gizmo->gridSnapEnabled;
                            enum KzuTransformGizmoGridSnapMode gridSnapMode = gizmo->gridSnapMode;

                            if(gridSnapEnabled && (gridSnapMode == KZU_TRANSFORM_GIZMO_GRID_SNAP_RELATIVE))
                            {
                                kzuRoundVector3ToGrid_internal(&offsetDifference, gizmo->gridSize, gizmo->axis,
                                                               &offsetDifference);
                            }

                            if (parent != KZ_NULL)
                            {
                                kzcMatrix4x4MultiplyVector3By3x3(&inverseParent, &offsetDifference, &offsetDifference);
                            }
              
                            newMatrix = matrix;
                            kzcMatrix4x4GetPosition(&newMatrix, &newPosition);
                            kzcVector3Add(&newPosition, &offsetDifference, &newPosition);

                            if(gridSnapEnabled)
                            {
                                if(gridSnapMode == KZU_TRANSFORM_GIZMO_GRID_SNAP_ABSOLUTE)
                                {
                                    kzuRoundVector3ToGrid_internal(&newPosition, gizmo->gridSize, gizmo->axis,
                                                                   &newPosition);
                                }
                                else if(gridSnapMode == KZU_TRANSFORM_GIZMO_GRID_SNAP_OBJECT)
                                {
                                    kzuTransformGizmoSnapToObjectList_internal(&newPosition, gizmo->gridSize, gizmo->axis,
                                                                 objectInWorldCoordinates, objectList, &newPosition);
                                }
                            }

                            kzcMatrix4x4SetPositionVector3(&newMatrix, &newPosition);

                        }
                        else if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_ROTATE)
                        {
                            struct KzcMatrix4x4 rotateMatrix;
                            const kzFloat rotateSpeed = 0.5f;
                            kzFloat rotate;
                            kzFloat axisX, axisY, axisZ;                            

                            kzcVector3Subtract(&pickPosition, &gizmo->startOffset, &offsetDifference);
                            rotate = rotateSpeed * (distance - gizmo->rotateOffset);

                            axisX = axisY = axisZ = 0.0f;

                            if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_X)
                            {
                                axisX = 1.0f;
                                axisY = axisZ = 0.0f;
                            }
                            else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Y)
                            {
                                axisY = 1.0f;
                                axisX = axisZ = 0.0f;
                            }
                            else if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Z)
                            {
                                axisZ = 1.0f;
                                axisX = axisY = 0.0f;
                            }

                            rotate = kzsFmod(rotate, 2.0f * KZS_PI);

                            kzcMatrix4x4SetIdentity(&rotateMatrix);
                            {
                                struct KzcMatrix4x4 normalizedStartMatrix;
                                struct KzcMatrix4x4 scaleMatrix;
                                struct KzcMatrix4x4 multipliedMatrix;
                                kzFloat scale[3];
                                scale[0] = kzcMatrix4x4GetScaleX(&gizmo->startObjectTransform);
                                scale[1] = kzcMatrix4x4GetScaleY(&gizmo->startObjectTransform);
                                scale[2] = kzcMatrix4x4GetScaleZ(&gizmo->startObjectTransform);

                                kzcMatrix4x4Normalize(&gizmo->startObjectTransform, &normalizedStartMatrix);
                                kzcMatrix4x4Rotate(&rotateMatrix, rotate, axisX, axisY, axisZ);
                                kzcMatrix4x4SetIdentity(&scaleMatrix);
                                kzcMatrix4x4Scale(&scaleMatrix, scale[0], scale[1], scale[2]);
                                kzcMatrix4x4Multiply(&scaleMatrix, &rotateMatrix, &multipliedMatrix);

                                kzcMatrix4x4Multiply(&multipliedMatrix, &normalizedStartMatrix, &newMatrix);
                            }
                        }
                        else if (gizmo->mode == KZU_TRANSFORM_GIZMO_MODE_SCALE)
                        {
                            struct KzcVector3 scaleVector;
                            kzFloat initialScaleX = kzcMatrix4x4GetScaleX(&gizmo->startObjectTransform);
                            kzFloat initialScaleY = kzcMatrix4x4GetScaleY(&gizmo->startObjectTransform);
                            kzFloat initialScaleZ = kzcMatrix4x4GetScaleZ(&gizmo->startObjectTransform);
                            struct KzcMatrix4x4 normalizedMatrix;
                            struct KzcMatrix4x4 scaleMatrix = KZC_MATRIX4X4_IDENTITY;

                            kzcVector3Subtract(&pickPosition, &gizmo->startOffset, &scaleVector);
                            kzcMatrix4x4Normalize(&gizmo->startObjectTransform, &normalizedMatrix);

                            if (parent != KZ_NULL)
                            {
                                struct KzcMatrix4x4 inverseNormalizedObject;
                                kzcMatrix4x4MultiplyVector3By3x3(&inverseParent, &scaleVector, &scaleVector);
                                kzcMatrix4x4Inverse(&normalizedMatrix, &inverseNormalizedObject);
                                kzcMatrix4x4MultiplyVector3By3x3(&inverseNormalizedObject, &scaleVector, &scaleVector);
                            }

                            if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_X ||
                                gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_XY ||
                                gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_XZ)
                            {
                                initialScaleX += scaleVector.data[0];
                            }

                            if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Y ||
                                gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_XY ||
                                gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_YZ)
                            {
                                initialScaleY += scaleVector.data[1];
                            }

                            if (gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_Z ||
                                gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_XZ ||
                                gizmo->axis == KZU_TRANSFORM_GIZMO_AXIS_YZ)
                            {
                                initialScaleZ += scaleVector.data[2];
                            }
                            
                            kzcMatrix4x4Scale(&scaleMatrix, initialScaleX, initialScaleY, initialScaleZ);
                            kzcMatrix4x4Multiply(&scaleMatrix, &normalizedMatrix, &newMatrix);
                        }

                        result = kzuObjectNodeSetTransformation(objectNode, &newMatrix);
                        kzsErrorForward(result);
                        transformModified = KZ_TRUE;

                        /* Save new reference coordinates. */
                        gizmo->dragX = x;
                        gizmo->dragY = y;
                    }
                }
            }
        }
        else if (state == KZS_POINTING_DEVICE_STATE_MOVE)
        {
            struct KzuMaterial* material = KZ_NULL;
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(gizmo->materialAxisX);
            const struct KzuPropertyType* colorType = kzuPropertyManagerFindPropertyType(propertyManager, "kzColor0");

            /* Hit test the gizmo. */
            struct KzuObjectNode* axisNode = KZ_NULL;
            result = kzuTransformGizmoPickAxis_internal(gizmo, engine, pick, (kzUint)x, (kzUint)y, &axisNode);
            kzsErrorForward(result);

            if (axisNode == gizmo->translateAxisX || axisNode == gizmo->scaleAxisX || axisNode == gizmo->rotateAxisX)
            {
                material = gizmo->materialAxisX;
            }
            else if (axisNode == gizmo->translateAxisY || axisNode == gizmo->scaleAxisY || axisNode == gizmo->rotateAxisY)
            {
                material = gizmo->materialAxisY;
            }
            else if (axisNode == gizmo->translateAxisZ || axisNode == gizmo->scaleAxisZ || axisNode == gizmo->rotateAxisZ)
            {
                material = gizmo->materialAxisZ;
            }
            else if (axisNode == gizmo->translateAxisXY || axisNode == gizmo->scaleAxisXY)
            {
                material = gizmo->materialAxisXY;
            }
            else if (axisNode == gizmo->translateAxisXZ || axisNode == gizmo->scaleAxisXZ)
            {
                material = gizmo->materialAxisXZ;
            }
            else if (axisNode == gizmo->translateAxisYZ || axisNode == gizmo->scaleAxisYZ)
            {
                material = gizmo->materialAxisYZ;
            }

            result = kzuPropertyManagerSetColor(propertyManager, gizmo->materialAxisX, colorType, KZC_COLOR_RED);
            kzsErrorForward(result);

            result = kzuPropertyManagerSetColor(propertyManager, gizmo->materialAxisY, colorType, KZC_COLOR_GREEN);
            kzsErrorForward(result);

            result = kzuPropertyManagerSetColor(propertyManager, gizmo->materialAxisZ, colorType, KZC_COLOR_BLUE);
            kzsErrorForward(result);

            result = kzuPropertyManagerSetColor(propertyManager, gizmo->materialAxisXY, colorType, KZC_COLOR_GRAY);
            kzsErrorForward(result);

            result = kzuPropertyManagerSetColor(propertyManager, gizmo->materialAxisYZ, colorType, KZC_COLOR_GRAY);
            kzsErrorForward(result);

            result = kzuPropertyManagerSetColor(propertyManager, gizmo->materialAxisXZ, colorType, KZC_COLOR_GRAY);
            kzsErrorForward(result);

            if (material != KZ_NULL)
            {
                result = kzuPropertyManagerSetColor(propertyManager, material, colorType, KZC_COLOR_YELLOW);
                kzsErrorForward(result);
            }
        }

        if (transformModified)
        {
            if (gizmo->callback != KZ_NULL)
            {
                result = gizmo->callback(gizmo->targetObjectPath, gizmo->callbackUserData);
                kzsErrorForward(result);
            }
        }
    }

    *out_handled = handled;
    kzsSuccess();
}

void kzuTransformGizmoSetCallback(struct KzuTransformGizmo* gizmo, KzuTransformGizmoCallback callback, void* userData)
{
    kzsAssert(kzcIsValidPointer(gizmo));
    
    gizmo->callback = callback;
    gizmo->callbackUserData = userData;
}
