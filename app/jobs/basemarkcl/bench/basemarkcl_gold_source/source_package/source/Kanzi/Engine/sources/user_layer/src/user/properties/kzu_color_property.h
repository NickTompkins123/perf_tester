/**
* \file
* Specifies color property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_COLOR_PROPERTY_H
#define KZU_COLOR_PROPERTY_H


#include <core/util/color/kzc_color.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuColorProperty
* Structure for color property.
*/
struct KzuColorProperty;
/** 
* \struct KzuColorPropertyType
* Structure for color property type.
*/
struct KzuColorPropertyType;


/** Creates color property type. */
kzsError kzuColorPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcColorRGBA defaultValue,
                                    struct KzuColorPropertyType** out_propertyType);
/** Deletes color property type. */
kzsError kzuColorPropertyTypeDelete(struct KzuColorPropertyType* propertyType);


/** Converts property type to color property type */
struct KzuColorPropertyType* kzuColorPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Converts color property type to property type. */
struct KzuPropertyType* kzuColorPropertyTypeToPropertyType(struct KzuColorPropertyType* propertyType);

/** Gets the default value of color property type. */
struct KzcColorRGBA kzuColorPropertyTypeGetDefaultValue(const struct KzuColorPropertyType* propertyType);


/** Creates a new material property color. */
kzsError kzuColorPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuColorPropertyType* propertyType, 
                                struct KzcColorRGBA color, struct KzuColorProperty** out_property);
/** Deletes color property. */
kzsError kzuColorPropertyDelete(struct KzuColorProperty* property);

/** Sets property relative. */
void kzuColorPropertySetRelative(struct KzuColorProperty* property, kzBool relative);
/** Is property relative. */
kzBool kzuColorPropertyIsRelative(const struct KzuColorProperty* property);

/** Sets color value for color property. */
void kzuColorPropertySetValue(struct KzuColorProperty* colorProperty, const struct KzcColorRGBA* color);
/** Sets color value for color property. */
void kzuColorPropertySetRelativeValue(struct KzuColorProperty* colorProperty, const struct KzcColorRGBA* color);
/** Sets value for given component of the color property. Red = 1, Green = 2, Blue = 3, Alpha = 4. Value is silently clamped to [0.0f,1.0f] range*/
void kzuColorPropertySetComponentValue(struct KzuColorProperty* colorProperty, kzUint component, kzFloat value);
/** Sets value for given component of the color property. Red = 1, Green = 2, Blue = 3, Alpha = 4. Value is silently clamped to [0.0f,1.0f] range*/
void kzuColorPropertySetRelativeComponentValue(struct KzuColorProperty* colorProperty, kzUint component, kzFloat value);
/** Returns a color value of color property. */
struct KzcColorRGBA kzuColorPropertyGetValue(const struct KzuColorProperty* color);
/** Returns a relative base color value of color property. */
struct KzcColorRGBA kzuColorPropertyGetRelativeValue(const struct KzuColorProperty* color);

/** Converts color property to property. */
struct KzuProperty* kzuColorPropertyToProperty(struct KzuColorProperty* property);
/** Converts property to color property */
struct KzuColorProperty* kzuColorPropertyFromProperty(const struct KzuProperty* property);


#endif
