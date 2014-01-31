/**
* \file
* Specifies a shadow map rendering utility, that is used for rendering shadows based on depth pass rendered from light.
*/
#ifndef KZU_SHADOW_MAP_H
#define KZU_SHADOW_MAP_H


#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_opengl_base.h>

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)

extern const kzString KZU_PROPERTY_SHADOW_PROJECTION_MATRIX; /**< Shadow map property. */


/* Forward declarations. */
struct KzuRenderer;
struct KzcMemoryManager;


/**
 * \struct KzuShadowMap
 * Rendering utility for shadow map; Preparation and applying for material.
 */
struct KzuShadowMap;


/** Creates shadow map. */
kzsError kzuShadowMapCreate(const struct KzcMemoryManager* memoryManager, struct KzuShadowMap** out_shadowMap);
/** Deletes shadow map. */
kzsError kzuShadowMapDelete(struct KzuShadowMap* shadowMap);

/** Prepares shadow map from current renderer state. */
kzsError kzuShadowMapPrepare(const struct KzuShadowMap* shadowMap, const struct KzuRenderer* renderer);
/** Applies shadow map for material. */
kzsError kzuShadowMapApply(const struct KzuShadowMap* shadowMap, const struct KzuRenderer* renderer);

/** Sets shadow map depth pass enabled / disabled. */
void kzuShadowMapSetDepthPass(struct KzuShadowMap* shadowMap, kzBool enabled);
/** Returns if this is shadow pass or not. */
kzBool kzuShadowMapIsDepthPass(const struct KzuShadowMap* shadowMap);

#endif

#endif
