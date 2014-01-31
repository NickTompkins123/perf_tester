/**
* \file
* Property query.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_QUERY_H
#define KZU_PROPERTY_QUERY_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzuPropertyType;
struct KzuPropertyManager;
struct KzuObjectNode;
struct KzcVector2;
struct KzcVector3;
struct KzcVector4;
struct KzcMatrix2x2;
struct KzcMatrix3x3;
struct KzcMatrix4x4;


/**
 * \struct KzuPropertyQuery
 * Query object for querying property values with temporary master values. When querying property values trough
 * a property query object, all properties from pushed objects are considered as well if the actual object doesn't
 * have the property associated.
 */
struct KzuPropertyQuery;


/** Create a property query object. */
kzsError kzuPropertyQueryCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                struct KzuPropertyQuery** out_propertyQuery);

/** Delete a property query object. */
kzsError kzuPropertyQueryDelete(struct KzuPropertyQuery* propertyQuery);

/** Gets the property manager of the property query */
struct KzuPropertyManager* kzuPropertyQueryGetPropertyManager(const struct KzuPropertyQuery* propertyQuery);

/** Push an object to the property query. */
kzsError kzuPropertyQueryPushObject(const struct KzuPropertyQuery* propertyQuery, const void* object);

/** Push an object node to the property query. */
kzsError kzuPropertyQueryPushObjectNode(const struct KzuPropertyQuery* propertyQuery, const struct KzuObjectNode* objectNode);

/** Pop and object from the property query. */
kzsError kzuPropertyQueryPopObject(const struct KzuPropertyQuery* propertyQuery);

/**
 * Get the value of a property using the property query.
 * The return value is calculated based on the following steps:
 * 1. kzuPropertyManagerGetFloat is used for getting the property value from all pushed objects.
 * 2. If the property is found from multiple objects, the one with the highest priority wins.
 *    If the highest priority is on multiple objects, the last pushed object of those wins.
 * 3. If the property is not found from any of the pushed objects, the default value from the property type is returned.
 */
kzFloat kzuPropertyQueryGetFloat(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);

/* TODO: Comment. */
kzInt kzuPropertyQueryGetInt(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
kzBool kzuPropertyQueryGetBool(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
struct KzcColorRGBA kzuPropertyQueryGetColor(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
struct KzcVector2 kzuPropertyQueryGetVector2(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
struct KzcVector3 kzuPropertyQueryGetVector3(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
struct KzcVector4 kzuPropertyQueryGetVector4(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
struct KzcMatrix2x2 kzuPropertyQueryGetMatrix2x2(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
struct KzcMatrix3x3 kzuPropertyQueryGetMatrix3x3(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
struct KzcMatrix4x4 kzuPropertyQueryGetMatrix4x4(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);
void* kzuPropertyQueryGetVoid(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType);


#endif
