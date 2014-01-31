/**
* \file
* Project loader for properties and property type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_PROPERTY_H
#define KZU_PROJECT_LOADER_PROPERTY_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuPropertyType;
struct KzuProperty;
struct KzuBinaryFileInfo;
struct KzcInputStream;


/* TODO: Remove when message system has been refactored. */
/** Loads property from binary. This is an old function for compatibility and will be removed. */
kzsError kzuProjectLoaderReadProperty_old(struct KzuProject* project, struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file,
                                          struct KzuProperty** out_property);

/** Loads property from binary. */
kzsError kzuProjectLoaderReadProperty(struct KzuProject* project, struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file, const void* object,
                                      struct KzuPropertyType** out_propertyType);

/** Loads a list of properties to the given property collection. */
kzsError kzuProjectLoaderReadProperties(struct KzuProject* project, struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file, const void* object);

/** Loads property from binary. */
kzsError kzuProjectLoaderReadPropertyType(struct KzuProject* project, struct KzcInputStream* inputStream, const struct KzuBinaryFileInfo* file,
                                          struct KzuPropertyType** out_propertyType);

/** Loads property type from binary. */
kzsException kzuProjectLoaderLoadPropertyType(struct KzuProject* project, kzString path, struct KzuPropertyType** out_propertyType);

/** Unloads property type from project. */
kzsException kzuProjectLoaderUnloadPropertyType(struct KzuProject* project, const struct KzuPropertyType* propertyType);


#endif
