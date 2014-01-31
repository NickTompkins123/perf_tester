/*
** Lua binding: kzu_matrix2x2_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_matrix2x2_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_matrix2x2_property.h>
#include <core/util/math/kzc_matrix2x2.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcMatrix2x2 (lua_State* tolua_S)
{
 KzcMatrix2x2* self = (KzcMatrix2x2*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzcMatrix2x2");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuMatrix2x2PropertyType");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzuMatrix2x2Property");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuMatrix2x2PropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeCreate00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcMatrix2x2",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuMatrix2x2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzcMatrix2x2 defaultValue = *((struct KzcMatrix2x2*)  tolua_tousertype(tolua_S,3,0));
  struct KzuMatrix2x2PropertyType* out_propertyType = ((struct KzuMatrix2x2PropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix2x2PropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuMatrix2x2PropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeDelete00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix2x2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix2x2PropertyType* propertyType = ((struct KzuMatrix2x2PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix2x2PropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeFromPropertyType00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuMatrix2x2PropertyType* tolua_ret = (struct KzuMatrix2x2PropertyType*)  kzuMatrix2x2PropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMatrix2x2PropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeToPropertyType00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix2x2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix2x2PropertyType* propertyType = ((struct KzuMatrix2x2PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuMatrix2x2PropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeGetDefaultValue00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix2x2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix2x2PropertyType* propertyType = ((const struct KzuMatrix2x2PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix2x2 tolua_ret =   kzuMatrix2x2PropertyTypeGetDefaultValue(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix2x2)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix2x2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix2x2));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix2x2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyCreate00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMatrix2x2PropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMatrix2x2",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuMatrix2x2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMatrix2x2PropertyType* propertyType = ((struct KzuMatrix2x2PropertyType*)  tolua_tousertype(tolua_S,2,0));
  const struct KzcMatrix2x2* matrix = ((const struct KzcMatrix2x2*)  tolua_tousertype(tolua_S,3,0));
  struct KzuMatrix2x2Property* out_property = ((struct KzuMatrix2x2Property*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix2x2PropertyCreate(memoryManager,propertyType,matrix,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuMatrix2x2Property");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyDelete00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix2x2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix2x2Property* property = ((struct KzuMatrix2x2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix2x2PropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyGetValue00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix2x2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix2x2Property* matrix = ((const struct KzuMatrix2x2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix2x2 tolua_ret =   kzuMatrix2x2PropertyGetValue(matrix);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix2x2)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix2x2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix2x2));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix2x2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertySetValue00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix2x2Property",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix2x2",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix2x2Property* matrixProperty = ((struct KzuMatrix2x2Property*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix2x2* matrix = ((const struct KzcMatrix2x2*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMatrix2x2PropertySetValue(matrixProperty,matrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyToProperty00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix2x2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix2x2Property* matrix = ((struct KzuMatrix2x2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuMatrix2x2PropertyToProperty(matrix);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix2x2PropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyFromProperty00
static int tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuMatrix2x2Property* tolua_ret = (struct KzuMatrix2x2Property*)  kzuMatrix2x2PropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMatrix2x2Property");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix2x2PropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_matrix2x2_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuMatrix2x2Property","KzuMatrix2x2Property","",NULL);
  tolua_beginmodule(tolua_S,"KzuMatrix2x2Property");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMatrix2x2PropertyType","KzuMatrix2x2PropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuMatrix2x2PropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyTypeCreate",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeCreate00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyTypeDelete",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeDelete00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyTypeFromPropertyType",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyTypeToPropertyType",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyTypeGetDefaultValue",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyCreate",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyCreate00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyDelete",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyDelete00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyGetValue",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyGetValue00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertySetValue",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertySetValue00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyToProperty",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyToProperty00);
  tolua_function(tolua_S,"kzuMatrix2x2PropertyFromProperty",tolua_kzu_matrix2x2_property_kzuMatrix2x2PropertyFromProperty00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_matrix2x2_property (lua_State* tolua_S) {
 return tolua_kzu_matrix2x2_property_open(tolua_S);
}
#endif

