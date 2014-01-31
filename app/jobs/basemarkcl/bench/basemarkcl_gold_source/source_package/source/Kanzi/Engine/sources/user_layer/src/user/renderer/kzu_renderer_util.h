/**
* \file
* User renderer utilities, e.g. wireframe grid, axes, debug.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_RENDERER_UTIL_H
#define KZU_RENDERER_UTIL_H


#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>

#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuRenderer;
struct KzuSkeleton;
struct KzuLightNode;
struct KzuCameraNode;
struct KzuMaterial;
struct KzuBoundingVolume;
struct KzuTransformedBoundingVolume;
struct KzuTransformedObjectNode;
struct KzuUiComponentNode;
struct KzcColorRGBA;
struct KzcDynamicArray;
struct KzsWindow;


/** Sets solid color for rendering. Has to be called after materal has been applied. */
kzsError kzuRendererSetSolidColor(const struct KzuRenderer* renderer, const struct KzcColorRGBA* color);
/** Draws wireframe bounding volume. */
kzsError kzuRendererDrawBoundingVolume(struct KzuRenderer* renderer, struct KzuBoundingVolume* boundingVolume,
                                       struct KzcMatrix4x4 worldTransformation);
/** Draws a transformed bounding volume. */
kzsError kzuRendererDrawTransformedBoundingVolume(struct KzuRenderer* renderer, const struct KzuTransformedBoundingVolume* volume, const struct KzcColorRGBA* color);
/** Draws a transformed bounding volume from corner points.
*
* The corner points are assumed axis-aligned, transformation is done with a matrix.
*/
kzsError kzuRendererDrawTransformedBoundingVolumeAB(struct KzuRenderer* renderer, const struct KzcVector3* backBottomLeft, const struct KzcVector3 *frontTopRight, const struct KzcMatrix4x4 *transform, const struct KzcColorRGBA* color);
/** Draws solid bounding volume using currently bound material. */
kzsError kzuRendererDrawBoundingVolumeSolid(const struct KzuRenderer* renderer, struct KzuBoundingVolume* boundingVolume,
                                            const struct KzcMatrix4x4* worldTransformation);
/** Visualize all the cells in a grid layout. */
kzsError kzuRendererDrawGridLayoutCells(struct KzuRenderer* renderer, const struct KzcVector3* backBottomLeft, const struct KzcVector3 *frontTopRight,
                                        const struct KzcMatrix4x4 *transform, const struct KzcMatrix4x4* camera,
                                        const struct KzuUiComponentNode* gridLayout, const struct KzcColorRGBA* color);
kzsError kzuRendererDrawGridLayoutElementPositions(struct KzuRenderer* renderer, const struct KzcVector3* backBottomLeft,
                                                   const struct KzcVector3* frontTopRight, const struct KzcMatrix4x4 *transform,
                                                   const struct KzcMatrix4x4* camera, const struct KzuTransformedObjectNode* layout,
                                                   const struct KzcColorRGBA* color);

/** Draws line list. */
kzsError kzuRendererDrawLineList(struct KzuRenderer* renderer, const kzFloat* lineList, kzUint lineCount, const struct KzcColorRGBA* color,
                                 const struct KzcMatrix4x4* worldTransformation);
/** Draws a triangle fan. */
kzsError kzuRendererDrawTriangleFan(struct KzuRenderer* renderer, const kzFloat* triangleList, kzUint triangleCount, const struct KzcColorRGBA* color,
                                    const struct KzcMatrix4x4* worldTransformation);
/** Draws quad using solid color. */
kzsError kzuRendererDrawQuad(struct KzuRenderer* renderer, const struct KzcVector3* p1, const struct KzcVector3* p2, const struct KzcVector3* p3,
                             const struct KzcVector3* p4, const struct KzcColorRGBA* color, const struct KzcMatrix4x4* worldTransformation);
/** Draws quad using specified material. */
kzsError kzuRendererDrawQuadWithMaterial(struct KzuRenderer* renderer, const struct KzcVector3* p1, const struct KzcVector3* p2, const struct KzcVector3* p3,
                                         const struct KzcVector3* p4, struct KzuMaterial* material, const struct KzcMatrix4x4* worldTransformation);

/** Sets default orthogonal projection and viewport for renderer. */
void kzuRendererSetDefaultOrthoProjectionAndViewport(const struct KzuRenderer* renderer);

/** Draws wireframe grid. */
kzsError kzuRendererDrawWireframeGrid(struct KzuRenderer* renderer, kzFloat spacing);
/** Function for drawing wire frame box. Useful for debugging. */
kzsError kzuRendererDrawWireframeBox(struct KzuRenderer* renderer, const struct KzcVector3* boundingBoxMin,
                                     const struct KzcVector3* boundingBoxMax);

/** Draws wireframe light. */
kzsError kzuRendererDrawLight(struct KzuRenderer* renderer, const struct KzcMatrix4x4* worldTransformation,
                              const struct KzuLightNode* lightNode);
/** Draws wireframe camera. */
kzsError kzuRendererDrawCamera(struct KzuRenderer* renderer, const struct KzcMatrix4x4* worldTransformation,
                               const struct KzuCameraNode* cameraNode);
/** Draws empty with wireframe lines. */
kzsError kzuRendererDrawEmptyObject(struct KzuRenderer* renderer, const struct KzcMatrix4x4* worldTransformation);

/** Draws skeleton with wireframe lines. */
kzsError kzuRendererDrawSkeleton(struct KzuRenderer* renderer, const struct KzcDynamicArray* transformedObjectList,
                                 const struct KzuSkeleton* skeleton);

/** Draws solid renderer borders. */
kzsError kzuRendererDrawBorders(const struct KzuRenderer* renderer, const struct KzsWindow* window);

/** Gets adjusted viewport rectangle according to input parameters. */
void kzuWindowGetAdjustedViewportRectangle(kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat aspectRatio, kzFloat* out_newX,
                                           kzFloat* out_newY, kzFloat* out_newWidth, kzFloat* out_newHeight);


#endif
