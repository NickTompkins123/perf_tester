/**
 * \file
 * Property accessors for KzuObjectNode.
 */
#ifndef KZU_OBJECT_PROPERTIES_H
#define KZU_OBJECT_PROPERTIES_H


#include "kzu_object.h"

#include <core/util/math/kzc_matrix4x4.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuObjectNode;


/** Gets the value of property Transformation in objectNode. */
struct KzcMatrix4x4 kzuObjectNodeGetTransformation(const struct KzuObjectNode* objectNode);
/** Gets the base value of property Transformation in objectNode not considering animations. */
struct KzcMatrix4x4 kzuObjectNodeGetBaseTransformation(const struct KzuObjectNode* objectNode);
/** Sets the value of property Transformation in objectNode. */
kzsError kzuObjectNodeSetTransformation(const struct KzuObjectNode* objectNode, const struct KzcMatrix4x4* value);


#endif
