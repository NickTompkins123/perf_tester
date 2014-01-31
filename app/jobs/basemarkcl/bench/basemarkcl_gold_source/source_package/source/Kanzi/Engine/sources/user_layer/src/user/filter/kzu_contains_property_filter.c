/**
* \file
* Specifies contains property filter.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_contains_property_filter.h"

#include "kzu_object_source_private.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>


/** Structure for property filter. */
struct KzuContainsPropertyFilter
{
    struct KzuFilterObjectSource filterObjectSource; /**< Inheritance. */
    const struct KzuPropertyType* propertyType; /**< Property type is used as condition for this filter. */
};


/** Call-back function for applying object set filter. */
static kzBool kzuContainsPropertyFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                      const struct KzuObjectSourceRuntimeData* runtimeData,
                                                      const struct KzuTransformedObjectNode* camera,
                                                      const struct KzuTransformedObjectNode* transformedObject);


/** Filter configuration for frustum cull filters. */
static const struct KzuFilterConfiguration KZU_CONTAINS_PROPERTY_FILTER_CONFIGURATION =
{
    KZ_NULL, /* Start function */
    KZ_NULL, /* Delete function */
    kzuContainsPropertyFilterApply_internal /* Apply function */
};


kzsError kzuContainsPropertyFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                         const struct KzuPropertyType* propertyType, kzBool isExclusive,
                                         struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuContainsPropertyFilter* propertyFilter;

    result = kzcMemoryAllocVariable(memoryManager, propertyFilter, "Property filter");
    kzsErrorForward(result);

    result = kzuFilterObjectSourceInitialize_private(memoryManager, &propertyFilter->filterObjectSource, input,
                                                     isExclusive, &KZU_CONTAINS_PROPERTY_FILTER_CONFIGURATION, &objectSource);
    kzsErrorForward(result);

    propertyFilter->propertyType = propertyType;
   
    *out_objectSource = objectSource;
    kzsSuccess();
}

KZ_CALLBACK static kzBool kzuContainsPropertyFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                                  const struct KzuObjectSourceRuntimeData* runtimeData,
                                                                  const struct KzuTransformedObjectNode* camera,
                                                                  const struct KzuTransformedObjectNode* transformedObject)
{
    kzBool result;
    const struct KzuContainsPropertyFilter* propertyFilter = (const struct KzuContainsPropertyFilter*)filterObjectSource;

    kzsAssert(kzcIsValidPointer(propertyFilter));

    result = kzuObjectNodeHasProperty(kzuTransformedObjectNodeGetObjectNode(transformedObject), propertyFilter->propertyType);
    if (filterObjectSource->isExclusive)
    {
        result = !result;
    }

    return result;
}
