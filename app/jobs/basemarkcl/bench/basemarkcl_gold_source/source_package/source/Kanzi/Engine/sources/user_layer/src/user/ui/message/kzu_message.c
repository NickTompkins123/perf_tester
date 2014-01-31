/**
* \file
* Message object.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_message.h"
#include "kzu_message_private.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_vector4_property.h>
#include <user/properties/kzu_matrix2x2_property.h>
#include <user/properties/kzu_matrix3x3_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_void_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>


struct KzuMessageArgument
{
    const struct KzuPropertyType* type;

    union
    {
        kzFloat floatData;                  /**< Float data.*/
        kzInt intData;                      /**< Int data.*/
        kzBool boolData;                    /**< Bool data.*/
        struct KzcColorRGBA colorData;      /**< Color data.*/
        struct KzcVector2 vector2Data;      /**< Vector2 data.*/
        struct KzcVector3 vector3Data;      /**< Vector3 data.*/
        struct KzcVector4 vector4Data;      /**< Vector4 data.*/
        struct KzcMatrix2x2 matrix2x2Data;  /**< Matrix2x2 data.*/
        struct KzcMatrix3x3 matrix3x3Data;  /**< Matrix3x3 data.*/
        struct KzcMatrix4x4 matrix4x4Data;  /**< Matrix4x4 data.*/
        kzMutableString stringData;         /**< String data.*/
        void* voidData;                     /**< Void data.*/
    } data;
};

kzsError kzuMessageCreate_private(const struct KzcMemoryManager* memoryManager, struct KzuMessageDispatcher* messageDispatcher,
                                  const struct KzuMessageType* messageType, struct KzuObjectNode* source, struct KzuMessage** out_message)
{
    kzsError result;
    struct KzuMessage* message;

    result = kzcMemoryAllocVariable(memoryManager, message, "Message");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &message->arguments);
    kzsErrorForward(result);

    message->valid = KZ_TRUE;
    message->dispatcher = messageDispatcher;
    message->messageType = messageType;
    message->source = source;
    message->handled = KZ_FALSE;
    message->valid = KZ_TRUE;

    *out_message = message;
    kzsSuccess();
}

struct KzuMessageDispatcher* kzuMessageGetDispatcher(const struct KzuMessage* message)
{
    kzsAssert(kzcIsValidPointer(message));

    return message->dispatcher;
}

const struct KzuMessageType* kzuMessageGetType(const struct KzuMessage* message)
{
    kzsAssert(kzcIsValidPointer(message));

    return message->messageType;
}

struct KzuObjectNode* kzuMessageGetSource(const struct KzuMessage* message)
{
    kzsAssert(kzcIsValidPointer(message));

    return message->source;
}

void kzuMessageSetHandled(struct KzuMessage* message, kzBool handled)
{
    kzsAssert(kzcIsValidPointer(message));

    message->handled = handled;
}

kzBool kzuMessageIsHandled(const struct KzuMessage* message)
{
    kzsAssert(kzcIsValidPointer(message));

    return message->handled;
}

void kzuMessageSetValid(struct KzuMessage* message, kzBool valid)
{
    kzsAssert(kzcIsValidPointer(message));

    message->valid = valid;
}

kzBool kzuMessageIsValid(const struct KzuMessage* message)
{
    kzsAssert(kzcIsValidPointer(message));

    return message->valid;
}

static struct KzuMessageArgument* kzuMessageFindArgument_internal(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzuMessageArgument* argument = KZ_NULL;
    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(message->arguments);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuMessageArgument* currentArgument = kzcDynamicArrayIteratorGetValue(it);
        if (currentArgument->type == argumentType)
        {
            argument = currentArgument;
            break;
        }
    }

    return argument;
}

static kzsError kzuMessageAcquireArgument_internal(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzuMessageArgument** out_argument)
{
    kzsError result;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);
    if (argument == KZ_NULL)
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(message);
        
        result = kzcMemoryAllocVariable(memoryManager, argument, "Message argument");
        kzsErrorForward(result);

        argument->type = argumentType;

        result = kzcDynamicArrayAdd(message->arguments, argument);
        kzsErrorForward(result);
    }

    *out_argument = argument;
    kzsSuccess();
}

kzFloat kzuMessageGetFloatArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    kzFloat value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);
    
    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_FLOAT);
    
    if (argument != KZ_NULL)
    {
        value = argument->data.floatData;
    }
    else
    {
        value = kzuFloatPropertyTypeGetDefaultValue(kzuFloatPropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetFloatArgument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, kzFloat value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_FLOAT);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.floatData = value;

    kzsSuccess();
}

kzInt kzuMessageGetIntArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    kzInt value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_INT);

    if (argument != KZ_NULL)
    {
        value = argument->data.intData;
    }
    else
    {
        value = kzuIntPropertyTypeGetDefaultValue(kzuIntPropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetIntArgument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, kzInt value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_INT);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.intData = value;

    kzsSuccess();
}

kzBool kzuMessageGetBoolArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    kzBool value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_BOOL);

    if (argument != KZ_NULL)
    {
        value = argument->data.boolData;
    }
    else
    {
        value = kzuBoolPropertyTypeGetDefaultValue(kzuBoolPropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetBoolArgument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, kzBool value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_BOOL);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.boolData = value;

    kzsSuccess();
}

struct KzcColorRGBA kzuMessageGetColorArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzcColorRGBA value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_COLOR);

    if (argument != KZ_NULL)
    {
        value = argument->data.colorData;
    }
    else
    {
        value = kzuColorPropertyTypeGetDefaultValue(kzuColorPropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetColorArgument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzcColorRGBA value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_COLOR);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.colorData = value;

    kzsSuccess();
}

struct KzcVector2 kzuMessageGetVector2ArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzcVector2 value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VECTOR2);

    if (argument != KZ_NULL)
    {
        value = argument->data.vector2Data;
    }
    else
    {
        value = kzuVector2PropertyTypeGetDefaultValue(kzuVector2PropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetVector2Argument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzcVector2 value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VECTOR2);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.vector2Data = value;

    kzsSuccess();
}

struct KzcVector3 kzuMessageGetVector3ArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzcVector3 value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VECTOR3);

    if (argument != KZ_NULL)
    {
        value = argument->data.vector3Data;
    }
    else
    {
        value = kzuVector3PropertyTypeGetDefaultValue(kzuVector3PropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetVector3Argument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzcVector3 value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VECTOR3);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.vector3Data = value;

    kzsSuccess();
}

struct KzcVector4 kzuMessageGetVector4ArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzcVector4 value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VECTOR4);

    if (argument != KZ_NULL)
    {
        value = argument->data.vector4Data;
    }
    else
    {
        value = kzuVector4PropertyTypeGetDefaultValue(kzuVector4PropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetVector4Argument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzcVector4 value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VECTOR4);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.vector4Data = value;

    kzsSuccess();
}

struct KzcMatrix2x2 kzuMessageGetMatrix2x2ArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzcMatrix2x2 value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_MATRIX2X2);

    if (argument != KZ_NULL)
    {
        value = argument->data.matrix2x2Data;
    }
    else
    {
        value = kzuMatrix2x2PropertyTypeGetDefaultValue(kzuMatrix2x2PropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetMatrix2x2Argument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzcMatrix2x2 value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_MATRIX2X2);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.matrix2x2Data = value;

    kzsSuccess();
}

struct KzcMatrix3x3 kzuMessageGetMatrix3x3ArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzcMatrix3x3 value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_MATRIX3X3);

    if (argument != KZ_NULL)
    {
        value = argument->data.matrix3x3Data;
    }
    else
    {
        value = kzuMatrix3x3PropertyTypeGetDefaultValue(kzuMatrix3x3PropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetMatrix3x3Argument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzcMatrix3x3 value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_MATRIX3X3);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.matrix3x3Data = value;

    kzsSuccess();
}

struct KzcMatrix4x4 kzuMessageGetMatrix4x4ArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    struct KzcMatrix4x4 value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_MATRIX4X4);

    if (argument != KZ_NULL)
    {
        value = argument->data.matrix4x4Data;
    }
    else
    {
        value = kzuMatrix4x4PropertyTypeGetDefaultValue(kzuMatrix4x4PropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetMatrix4x4Argument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, struct KzcMatrix4x4 value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_MATRIX4X4);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.matrix4x4Data = value;

    kzsSuccess();
}

void* kzuMessageGetVoidArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    void* value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VOID);

    if (argument != KZ_NULL)
    {
        value = argument->data.voidData;
    }
    else
    {
        value = kzuVoidPropertyTypeGetDefaultValue(kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetVoidArgument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, void* value)
{
    kzsError result;
    struct KzuMessageArgument* argument;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_VOID);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    argument->data.voidData = value;

    kzsSuccess();
}

kzString kzuMessageGetStringArgumentDefault(const struct KzuMessage* message, const struct KzuPropertyType* argumentType)
{
    kzString value;
    struct KzuMessageArgument* argument = kzuMessageFindArgument_internal(message, argumentType);

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_STRING);

    if (argument != KZ_NULL)
    {
        value = argument->data.stringData;
    }
    else
    {
        value = kzuStringPropertyTypeGetDefaultValue(kzuStringPropertyTypeFromPropertyType((struct KzuPropertyType*)argumentType));
    }

    return value;
}

kzsError kzuMessageSetStringArgument(const struct KzuMessage* message, const struct KzuPropertyType* argumentType, kzString value)
{
    kzsError result;
    struct KzuMessageArgument* argument;
    kzMutableString newString;

    kzsAssert(kzuPropertyTypeGetDataType(argumentType) == KZU_PROPERTY_DATA_TYPE_STRING);

    result = kzuMessageAcquireArgument_internal(message, argumentType, &argument);
    kzsErrorForward(result);

    result = kzcStringCopy(kzcMemoryGetManager(message), value, &newString);
    kzsErrorForward(result);

    argument->data.stringData = newString;

    kzsSuccess();
}

