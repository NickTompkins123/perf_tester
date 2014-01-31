/*
** Lua binding: kzu_float_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_float_property_open (lua_State* tolua_S);

#include <core/util/collection/kzc_dynamic_array.h>
#include <user/engine/kzu_engine_message_queue.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_property_base.h>
#include <user/properties/kzu_property.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_matrix3x3.h>
#include <core/util/math/kzc_matrix2x2.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/color/kzc_color.h>
#include <user/properties/kzu_float_property.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_kzBool (lua_State* tolua_S)
{
 kzBool* self = (kzBool*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuFloatProperty");
 tolua_usertype(tolua_S,"KzuFloatPropertyType");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuFloatPropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyTypeCreate00
static int tolua_kzu_float_property_kzuFloatPropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuFloatPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  float defaultValue = ((float)  tolua_tonumber(tolua_S,3,0));
  struct KzuFloatPropertyType* out_propertyType = ((struct KzuFloatPropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuFloatPropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((kzsError)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(kzsError));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuFloatPropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyTypeDelete00
static int tolua_kzu_float_property_kzuFloatPropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFloatPropertyType* propertyType = ((struct KzuFloatPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuFloatPropertyTypeDelete(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((kzsError)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(kzsError));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyTypeFromPropertyType00
static int tolua_kzu_float_property_kzuFloatPropertyTypeFromPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuPropertyType* propertyType = ((struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuFloatPropertyType* tolua_ret = (struct KzuFloatPropertyType*)  kzuFloatPropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuFloatPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyTypeToPropertyType00
static int tolua_kzu_float_property_kzuFloatPropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFloatPropertyType* propertyType = ((struct KzuFloatPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuFloatPropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyTypeGetDefaultValue00
static int tolua_kzu_float_property_kzuFloatPropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuFloatPropertyType* propertyType = ((const struct KzuFloatPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuFloatPropertyTypeGetDefaultValue(propertyType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyCreate00
static int tolua_kzu_float_property_kzuFloatPropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuFloatPropertyType",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuFloatPropertyType* propertyType = ((struct KzuFloatPropertyType*)  tolua_tousertype(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  struct KzuFloatProperty* out_property = ((struct KzuFloatProperty*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuFloatPropertyCreate(memoryManager,propertyType,value,&out_property);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((kzsError)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(kzsError));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuFloatProperty");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyDelete00
static int tolua_kzu_float_property_kzuFloatPropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFloatProperty* property = ((struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuFloatPropertyDelete(property);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((kzsError)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(kzsError));
     tolua_pushusertype(tolua_S,tolua_obj,"kzsError");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertySetRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertySetRelative00
static int tolua_kzu_float_property_kzuFloatPropertySetRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFloatProperty* property = ((struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  kzBool relative = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuFloatPropertySetRelative(property,relative);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertySetRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyIsRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyIsRelative00
static int tolua_kzu_float_property_kzuFloatPropertyIsRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuFloatProperty* property = ((const struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuFloatPropertyIsRelative(property);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((kzBool)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"kzBool");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(kzBool));
     tolua_pushusertype(tolua_S,tolua_obj,"kzBool");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyIsRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyGetValue00
static int tolua_kzu_float_property_kzuFloatPropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuFloatProperty* floatProperty = ((const struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuFloatPropertyGetValue(floatProperty);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyGetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyGetRelativeValue00
static int tolua_kzu_float_property_kzuFloatPropertyGetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuFloatProperty* floatProperty = ((const struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuFloatPropertyGetRelativeValue(floatProperty);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyGetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertySetValue00
static int tolua_kzu_float_property_kzuFloatPropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFloatProperty* floatProperty = ((struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  float value = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzuFloatPropertySetValue(floatProperty,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertySetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertySetRelativeValue00
static int tolua_kzu_float_property_kzuFloatPropertySetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFloatProperty* floatProperty = ((struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  float value = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzuFloatPropertySetRelativeValue(floatProperty,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertySetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyToProperty00
static int tolua_kzu_float_property_kzuFloatPropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFloatProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFloatProperty* property = ((struct KzuFloatProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuFloatPropertyToProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuFloatPropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_float_property_kzuFloatPropertyFromProperty00
static int tolua_kzu_float_property_kzuFloatPropertyFromProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProperty* property = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuFloatProperty* tolua_ret = (struct KzuFloatProperty*)  kzuFloatPropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuFloatProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuFloatPropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_float_property_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyType","KzuPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuFloatProperty","KzuFloatProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuFloatProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuFloatPropertyType","KzuFloatPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuFloatPropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuFloatPropertyTypeCreate",tolua_kzu_float_property_kzuFloatPropertyTypeCreate00);
  tolua_function(tolua_S,"kzuFloatPropertyTypeDelete",tolua_kzu_float_property_kzuFloatPropertyTypeDelete00);
  tolua_function(tolua_S,"kzuFloatPropertyTypeFromPropertyType",tolua_kzu_float_property_kzuFloatPropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuFloatPropertyTypeToPropertyType",tolua_kzu_float_property_kzuFloatPropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuFloatPropertyTypeGetDefaultValue",tolua_kzu_float_property_kzuFloatPropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuFloatPropertyCreate",tolua_kzu_float_property_kzuFloatPropertyCreate00);
  tolua_function(tolua_S,"kzuFloatPropertyDelete",tolua_kzu_float_property_kzuFloatPropertyDelete00);
  tolua_function(tolua_S,"kzuFloatPropertySetRelative",tolua_kzu_float_property_kzuFloatPropertySetRelative00);
  tolua_function(tolua_S,"kzuFloatPropertyIsRelative",tolua_kzu_float_property_kzuFloatPropertyIsRelative00);
  tolua_function(tolua_S,"kzuFloatPropertyGetValue",tolua_kzu_float_property_kzuFloatPropertyGetValue00);
  tolua_function(tolua_S,"kzuFloatPropertyGetRelativeValue",tolua_kzu_float_property_kzuFloatPropertyGetRelativeValue00);
  tolua_function(tolua_S,"kzuFloatPropertySetValue",tolua_kzu_float_property_kzuFloatPropertySetValue00);
  tolua_function(tolua_S,"kzuFloatPropertySetRelativeValue",tolua_kzu_float_property_kzuFloatPropertySetRelativeValue00);
  tolua_function(tolua_S,"kzuFloatPropertyToProperty",tolua_kzu_float_property_kzuFloatPropertyToProperty00);
  tolua_function(tolua_S,"kzuFloatPropertyFromProperty",tolua_kzu_float_property_kzuFloatPropertyFromProperty00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_float_property (lua_State* tolua_S) {
 return tolua_kzu_float_property_open(tolua_S);
}
#endif

