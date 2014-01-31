/**
* \file
* Ray picking utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_RAYPICK_H
#define KZU_RAYPICK_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzuRenderer;
struct KzuCameraNode;
struct KzuObjectNode;
struct KzuObjectSource;
struct KzuTransformedScene;
struct KzuTransformedObjectNode;
struct KzcVector3;
struct KzcRay;
struct KzsWindow;


/**
* Finds ObjectNode positioned in 3d space on a ray shot from point on near clip plane at viewport x,y to the
* direction of viewport x,y on far clip plane.
*
* After the call:
*  If object was hit:
*   - out_distance will contain the distance from camera to object
*   - out_node will contain the node hit
*   - out_vector_to_origin will contain the vector from object's origin to hit position
*
*  If object was not hit
*   - out_distance will be KZ_FLOAT_MAXIMUM
*   - out_node will be KZ_NULL 
*   - out_vector_to_origin will will be a vector with all components set to KZ_FLOAT_MAXIMUM
*
* \param source Object source used for picking. It can be used to invalidate parts of scene from picking.
* \param out_transformedNode Transformed node of the hit object. Can be KZ_NULL
*/
kzsError kzuRayPick(const struct KzuRenderer* renderer,
                    const struct KzsWindow* window,
                    const struct KzuObjectSource* pickingSource,
                    const struct KzuTransformedScene* transformedScene,
                    const struct KzuCameraNode* cameraNode, 
                    kzInt positionX,
                    kzInt positionY,
                    kzBool pick2D,
                    kzFloat* out_distance, 
                    struct KzuObjectNode** out_node, 
                    struct KzuTransformedObjectNode** out_transformedNode,
                    struct KzcVector3* out_offsetFromOrigin,
                    struct KzcRay* out_ray);

/** Calculates a ray in world space from active window and given pointer coordinates. */
kzsError kzuRayPickCalculateRay(kzInt pointerX, kzInt pointerY, const struct KzuRenderer* renderer,
                                const struct KzsWindow* window,
                                const struct KzuTransformedObjectNode* transformedCameraNode, struct KzcRay* out_ray);


#endif
