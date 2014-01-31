/**
 * \file
 * Property accessors for KzuObjectNode.
 */
#include "kzu_object.h"
#include "kzu_object_properties.h"

#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>


struct KzcMatrix4x4 kzuObjectNodeGetTransformation(const struct KzuObjectNode* objectNode)
{
    return kzuObjectNodeGetMatrix4x4PropertyDefault(objectNode, KZU_PROPERTY_TYPE_TRANSFORMATION);
}

struct KzcMatrix4x4 kzuObjectNodeGetBaseTransformation(const struct KzuObjectNode* objectNode)
{
    struct KzcMatrix4x4 value;
    kzBool found;
    
    found = kzuObjectNodeGetBaseMatrix4x4Property(objectNode, KZU_PROPERTY_TYPE_TRANSFORMATION, &value);
    kzsAssert(found);
    
    return value;
}

kzsError kzuObjectNodeSetTransformation(const struct KzuObjectNode* objectNode, const struct KzcMatrix4x4* value)
{
    kzsError result;
    
    result = kzuObjectNodeSetMatrix4x4Property(objectNode, KZU_PROPERTY_TYPE_TRANSFORMATION, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}
