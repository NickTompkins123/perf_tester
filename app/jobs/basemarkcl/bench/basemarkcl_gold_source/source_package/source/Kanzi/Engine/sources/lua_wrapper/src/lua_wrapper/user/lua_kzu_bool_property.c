/*
** Lua binding: kzu_bool_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_bool_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_bool_property.h>
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
 tolua_usertype(tolua_S,"KzuBoolPropertyType");
 tolua_usertype(tolua_S,"KzuBoolProperty");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuBoolPropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyTypeCreate00
static int tolua_kzu_bool_property_kzuBoolPropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"kzBool",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuBoolPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  kzBool defaultValue = *((kzBool*)  tolua_tousertype(tolua_S,3,0));
  struct KzuBoolPropertyType* out_propertyType = ((struct KzuBoolPropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuBoolPropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuBoolPropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyTypeDelete00
static int tolua_kzu_bool_property_kzuBoolPropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoolPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoolPropertyType* propertyType = ((struct KzuBoolPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuBoolPropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyTypeFromPropertyType00
static int tolua_kzu_bool_property_kzuBoolPropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuBoolPropertyType* tolua_ret = (struct KzuBoolPropertyType*)  kzuBoolPropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuBoolPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyTypeToPropertyType00
static int tolua_kzu_bool_property_kzuBoolPropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoolPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoolPropertyType* propertyType = ((struct KzuBoolPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuBoolPropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyTypeGetDefaultValue00
static int tolua_kzu_bool_property_kzuBoolPropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoolPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuBoolPropertyType* propertyType = ((const struct KzuBoolPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuBoolPropertyTypeGetDefaultValue(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyCreate00
static int tolua_kzu_bool_property_kzuBoolPropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuBoolPropertyType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"kzBool",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuBoolProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuBoolPropertyType* propertyType = ((struct KzuBoolPropertyType*)  tolua_tousertype(tolua_S,2,0));
  kzBool value = *((kzBool*)  tolua_tousertype(tolua_S,3,0));
  struct KzuBoolProperty* out_property = ((struct KzuBoolProperty*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuBoolPropertyCreate(memoryManager,propertyType,value,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuBoolProperty");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyDelete00
static int tolua_kzu_bool_property_kzuBoolPropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoolProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoolProperty* property = ((struct KzuBoolProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuBoolPropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertySetValue00
static int tolua_kzu_bool_property_kzuBoolPropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoolProperty",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoolProperty* boolProperty = ((struct KzuBoolProperty*)  tolua_tousertype(tolua_S,1,0));
  kzBool value = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuBoolPropertySetValue(boolProperty,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyGetValue00
static int tolua_kzu_bool_property_kzuBoolPropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoolProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuBoolProperty* boolProperty = ((const struct KzuBoolProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuBoolPropertyGetValue(boolProperty);
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
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyToProperty00
static int tolua_kzu_bool_property_kzuBoolPropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoolProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoolProperty* property = ((struct KzuBoolProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuBoolPropertyToProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoolPropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_bool_property_kzuBoolPropertyFromProperty00
static int tolua_kzu_bool_property_kzuBoolPropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuBoolProperty* tolua_ret = (struct KzuBoolProperty*)  kzuBoolPropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuBoolProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoolPropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_bool_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuBoolProperty","KzuBoolProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuBoolProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuBoolPropertyType","KzuBoolPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuBoolPropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuBoolPropertyTypeCreate",tolua_kzu_bool_property_kzuBoolPropertyTypeCreate00);
  tolua_function(tolua_S,"kzuBoolPropertyTypeDelete",tolua_kzu_bool_property_kzuBoolPropertyTypeDelete00);
  tolua_function(tolua_S,"kzuBoolPropertyTypeFromPropertyType",tolua_kzu_bool_property_kzuBoolPropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuBoolPropertyTypeToPropertyType",tolua_kzu_bool_property_kzuBoolPropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuBoolPropertyTypeGetDefaultValue",tolua_kzu_bool_property_kzuBoolPropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuBoolPropertyCreate",tolua_kzu_bool_property_kzuBoolPropertyCreate00);
  tolua_function(tolua_S,"kzuBoolPropertyDelete",tolua_kzu_bool_property_kzuBoolPropertyDelete00);
  tolua_function(tolua_S,"kzuBoolPropertySetValue",tolua_kzu_bool_property_kzuBoolPropertySetValue00);
  tolua_function(tolua_S,"kzuBoolPropertyGetValue",tolua_kzu_bool_property_kzuBoolPropertyGetValue00);
  tolua_function(tolua_S,"kzuBoolPropertyToProperty",tolua_kzu_bool_property_kzuBoolPropertyToProperty00);
  tolua_function(tolua_S,"kzuBoolPropertyFromProperty",tolua_kzu_bool_property_kzuBoolPropertyFromProperty00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_bool_property (lua_State* tolua_S) {
 return tolua_kzu_bool_property_open(tolua_S);
}
#endif

