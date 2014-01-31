/**
 * \file
 * Property accessors for KzuLightNode.
 */
#ifndef KZU_LIGHT_PROPERTIES_H
#define KZU_LIGHT_PROPERTIES_H


#include "kzu_light.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuLightNode;


/** Gets the value of property Enabled in lightNode. */
kzBool kzuLightNodeGetEnabled(const struct KzuLightNode* lightNode);
/** Sets the value of property Enabled in lightNode. */
kzsError kzuLightNodeSetEnabled(const struct KzuLightNode* lightNode, kzBool value);


#endif
