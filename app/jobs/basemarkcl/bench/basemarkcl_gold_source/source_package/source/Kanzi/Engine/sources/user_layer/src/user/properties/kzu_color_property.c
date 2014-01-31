/**
* \file
* Specifies color property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_color_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_math.h>


/** Color property type. */
struct KzuColorPropertyType
{
    struct KzuPropertyType propertyType;        /**< Parent property type. */
};

struct KzuColorPropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuColorPropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    struct KzcColorRGBA defaultValue; /**< Default value. */
};

/** Color property structure. */
struct KzuColorProperty
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    struct KzcColorRGBA color;      /**< Color of this property. */

    kzBool isRelative; /**< Is property relative (for animations). */
    struct KzcColorRGBA relativeBase; /**< Object base value. */
};


kzsError kzuColorPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcColorRGBA defaultValue,
                                    struct KzuColorPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuColorPropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Color property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_COLOR, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuColorPropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuColorPropertyTypeDelete(struct KzuColorPropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuColorPropertyType* kzuColorPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuColorPropertyType*)propertyType;
}

struct KzuPropertyType* kzuColorPropertyTypeToPropertyType(struct KzuColorPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcColorRGBA kzuColorPropertyTypeGetDefaultValue(const struct KzuColorPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuColorPropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuColorPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuColorPropertyType* propertyType, 
                                struct KzcColorRGBA color, struct KzuColorProperty** out_property)
{
    kzsError result;
    struct KzuColorProperty* colorProperty;

    result = kzcMemoryAllocVariable(memoryManager, colorProperty, "ColorProperty");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&colorProperty->property, (struct KzuPropertyType*)propertyType);
    colorProperty->color = color;
    colorProperty->relativeBase = KZC_COLOR_BLACK_TRANSPARENT;
    colorProperty->isRelative = KZ_FALSE;

    *out_property = colorProperty;
    kzsSuccess();
}

kzsError kzuColorPropertyDelete(struct KzuColorProperty* property)
{
    kzsError result;

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuColorPropertySetRelative(struct KzuColorProperty* property, kzBool relative)
{
    kzsAssert(kzcIsValidPointer(property));
    property->isRelative = relative;
}

kzBool kzuColorPropertyIsRelative(const struct KzuColorProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->isRelative;
}

struct KzcColorRGBA kzuColorPropertyGetValue(const struct KzuColorProperty* color)
{
    struct KzcColorRGBA value;
    kzsAssert(kzcIsValidPointer(color));
    value = kzcColorRGBA(color->color.red + color->relativeBase.red, 
        color->color.green + color->relativeBase.green, 
        color->color.blue + color->relativeBase.blue, 
        color->color.alpha + color->relativeBase.alpha);
    return value;
}

struct KzcColorRGBA kzuColorPropertyGetRelativeValue(const struct KzuColorProperty* color)
{
    kzsAssert(kzcIsValidPointer(color));
    return color->relativeBase;
}

void kzuColorPropertySetValue(struct KzuColorProperty* colorProperty, const struct KzcColorRGBA* color)
{
    kzsAssert(kzcIsValidPointer(colorProperty));
    colorProperty->color = *color;
}

void kzuColorPropertySetRelativeValue(struct KzuColorProperty* colorProperty, const struct KzcColorRGBA* color)
{
    kzsAssert(kzcIsValidPointer(colorProperty));
    colorProperty->relativeBase = *color;
}

void kzuColorPropertySetComponentValue(struct KzuColorProperty* colorProperty, kzUint component, kzFloat value)
{
    kzFloat newValue = kzsClampf(value, 0.0f, 1.0f);

    kzsAssert(kzcIsValidPointer(colorProperty));

    switch (component)
    {
        case 0:
        {
            colorProperty->color.red = newValue;
            break;
        }
        case 1:
        {
            colorProperty->color.green = newValue;
            break;
        }
        case 2:
        {
            colorProperty->color.blue = newValue;
            break;
        }
        case 3:
        {
            colorProperty->color.alpha = newValue;
            break;
        }
        default:
        {
            kzsAssertText(KZ_FALSE, "Component out of valid range");
            break;
        }
    }
}

void kzuColorPropertySetRelativeComponentValue(struct KzuColorProperty* colorProperty, kzUint component, kzFloat value)
{
    kzFloat newValue = kzsClampf(value, 0.0f, 1.0f);

    kzsAssert(kzcIsValidPointer(colorProperty));

    switch (component)
    {
        case 0:
        {
            colorProperty->relativeBase.red = newValue;
            break;
        }
        case 1:
        {
            colorProperty->relativeBase.green = newValue;
            break;
        }
        case 2:
        {
            colorProperty->relativeBase.blue = newValue;
            break;
        }
        case 3:
        {
            colorProperty->relativeBase.alpha = newValue;
            break;
        }
        default:
        {
            kzsAssertText(KZ_FALSE, "Component out of valid range");
            break;
        }
    }
}

struct KzuProperty* kzuColorPropertyToProperty(struct KzuColorProperty* property)
{
    return &property->property;
}

struct KzuColorProperty* kzuColorPropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuColorProperty*)property;
}
