/*
** Lua binding: kzu_matrix3x3_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_matrix3x3_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_matrix3x3_property.h>
#include <core/util/math/kzc_matrix3x3.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcMatrix3x3 (lua_State* tolua_S)
{
 KzcMatrix3x3* self = (KzcMatrix3x3*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuMatrix3x3PropertyType");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzcMatrix3x3");
 tolua_usertype(tolua_S,"KzuMatrix3x3Property");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuMatrix3x3PropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeCreate00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcMatrix3x3",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuMatrix3x3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzcMatrix3x3 defaultValue = *((struct KzcMatrix3x3*)  tolua_tousertype(tolua_S,3,0));
  struct KzuMatrix3x3PropertyType* out_propertyType = ((struct KzuMatrix3x3PropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix3x3PropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuMatrix3x3PropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeDelete00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix3x3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix3x3PropertyType* propertyType = ((struct KzuMatrix3x3PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix3x3PropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeFromPropertyType00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuMatrix3x3PropertyType* tolua_ret = (struct KzuMatrix3x3PropertyType*)  kzuMatrix3x3PropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMatrix3x3PropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeToPropertyType00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix3x3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix3x3PropertyType* propertyType = ((struct KzuMatrix3x3PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuMatrix3x3PropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeGetDefaultValue00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix3x3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix3x3PropertyType* propertyType = ((const struct KzuMatrix3x3PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix3x3 tolua_ret =   kzuMatrix3x3PropertyTypeGetDefaultValue(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix3x3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix3x3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix3x3));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix3x3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyCreate00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMatrix3x3PropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMatrix3x3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuMatrix3x3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMatrix3x3PropertyType* propertyType = ((struct KzuMatrix3x3PropertyType*)  tolua_tousertype(tolua_S,2,0));
  const struct KzcMatrix3x3* matrix = ((const struct KzcMatrix3x3*)  tolua_tousertype(tolua_S,3,0));
  struct KzuMatrix3x3Property* out_property = ((struct KzuMatrix3x3Property*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix3x3PropertyCreate(memoryManager,propertyType,matrix,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuMatrix3x3Property");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyDelete00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix3x3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix3x3Property* property = ((struct KzuMatrix3x3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix3x3PropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyToProperty00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix3x3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix3x3Property* matrix = ((struct KzuMatrix3x3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuMatrix3x3PropertyToProperty(matrix);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyFromProperty00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuMatrix3x3Property* tolua_ret = (struct KzuMatrix3x3Property*)  kzuMatrix3x3PropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMatrix3x3Property");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyGetValue00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix3x3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix3x3Property* matrix = ((const struct KzuMatrix3x3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix3x3 tolua_ret =   kzuMatrix3x3PropertyGetValue(matrix);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix3x3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix3x3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix3x3));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix3x3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix3x3PropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertySetValue00
static int tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix3x3Property",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix3x3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix3x3Property* matrixProperty = ((struct KzuMatrix3x3Property*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix3x3* matrix = ((const struct KzcMatrix3x3*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMatrix3x3PropertySetValue(matrixProperty,matrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix3x3PropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_matrix3x3_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuMatrix3x3Property","KzuMatrix3x3Property","",NULL);
  tolua_beginmodule(tolua_S,"KzuMatrix3x3Property");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMatrix3x3PropertyType","KzuMatrix3x3PropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuMatrix3x3PropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyTypeCreate",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeCreate00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyTypeDelete",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeDelete00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyTypeFromPropertyType",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyTypeToPropertyType",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyTypeGetDefaultValue",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyCreate",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyCreate00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyDelete",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyDelete00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyToProperty",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyToProperty00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyFromProperty",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyFromProperty00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertyGetValue",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertyGetValue00);
  tolua_function(tolua_S,"kzuMatrix3x3PropertySetValue",tolua_kzu_matrix3x3_property_kzuMatrix3x3PropertySetValue00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_matrix3x3_property (lua_State* tolua_S) {
 return tolua_kzu_matrix3x3_property_open(tolua_S);
}
#endif

