/**
 * \file
 * Property accessors for KzuLightNode.
 */
#include "kzu_light.h"
#include "kzu_light_properties.h"

#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>


kzBool kzuLightNodeGetEnabled(const struct KzuLightNode* lightNode)
{
    return kzuObjectNodeGetBoolPropertyDefault(kzuLightNodeToObjectNode(lightNode), KZU_PROPERTY_TYPE_LIGHT_ENABLED);
}

kzsError kzuLightNodeSetEnabled(const struct KzuLightNode* lightNode, kzBool value)
{
    kzsError result;
    
    result = kzuObjectNodeSetBoolProperty(kzuLightNodeToObjectNode(lightNode), KZU_PROPERTY_TYPE_LIGHT_ENABLED, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}
