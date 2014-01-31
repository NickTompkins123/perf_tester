/*
** Lua binding: kzu_light_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_light_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_light_property.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcDynamicArrayIterator (lua_State* tolua_S)
{
 KzcDynamicArrayIterator* self = (KzcDynamicArrayIterator*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuPropertyCollection");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"KzcDynamicArrayIterator");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuGroupProperty");
 tolua_usertype(tolua_S,"KzuLightPropertyType");
 tolua_usertype(tolua_S,"KzuLightProperty");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuLightNode");
}

/* function: kzuLightPropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyCreate00
static int tolua_kzu_light_property_kzuLightPropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuLightProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuLightPropertyType* propertyType = ((struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,2,0));
  struct KzuLightProperty* out_property = ((struct KzuLightProperty*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightPropertyCreate(memoryManager,propertyType,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuLightProperty");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyDelete00
static int tolua_kzu_light_property_kzuLightPropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuLightProperty* property = ((struct KzuLightProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightPropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyToProperty00
static int tolua_kzu_light_property_kzuLightPropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuLightProperty* lightProperty = ((struct KzuLightProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuLightPropertyToProperty(lightProperty);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyFromProperty00
static int tolua_kzu_light_property_kzuLightPropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuLightProperty* tolua_ret = (struct KzuLightProperty*)  kzuLightPropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuLightProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyGetLightPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyGetLightPropertyType00
static int tolua_kzu_light_property_kzuLightPropertyGetLightPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightProperty* lightProperty = ((const struct KzuLightProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuLightPropertyType* tolua_ret = (struct KzuLightPropertyType*)  kzuLightPropertyGetLightPropertyType(lightProperty);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuLightPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyGetLightPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyTypeCreate00
static int tolua_kzu_light_property_kzuLightPropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzuLightPropertyType* out_propertyType = ((struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightPropertyTypeCreate(memoryManager,name,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuLightPropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyTypeDelete00
static int tolua_kzu_light_property_kzuLightPropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuLightPropertyType* propertyType = ((struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightPropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyTypeAddProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyTypeAddProperty00
static int tolua_kzu_light_property_kzuLightPropertyTypeAddProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightPropertyType* property = ((const struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyType* childPropertyType = ((struct KzuPropertyType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightPropertyTypeAddProperty(property,childPropertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyTypeAddProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyTypeToPropertyType00
static int tolua_kzu_light_property_kzuLightPropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuLightPropertyType* propertyType = ((struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuLightPropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyTypeFromPropertyType00
static int tolua_kzu_light_property_kzuLightPropertyTypeFromPropertyType00(lua_State* tolua_S)
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
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuLightPropertyType* tolua_ret = (struct KzuLightPropertyType*)  kzuLightPropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuLightPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyTypeGetPropertyTypes */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyTypeGetPropertyTypes00
static int tolua_kzu_light_property_kzuLightPropertyTypeGetPropertyTypes00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightPropertyType* lightPropertyType = ((const struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcDynamicArrayIterator tolua_ret =   kzuLightPropertyTypeGetPropertyTypes(lightPropertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcDynamicArrayIterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcDynamicArrayIterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcDynamicArrayIterator));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcDynamicArrayIterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyTypeGetPropertyTypes'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightPropertyTypeGetPropertyTypeCount */
#ifndef TOLUA_DISABLE_tolua_kzu_light_property_kzuLightPropertyTypeGetPropertyTypeCount00
static int tolua_kzu_light_property_kzuLightPropertyTypeGetPropertyTypeCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightPropertyType* lightPropertyType = ((const struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuLightPropertyTypeGetPropertyTypeCount(lightPropertyType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightPropertyTypeGetPropertyTypeCount'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_light_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuPropertyCollection","KzuPropertyCollection","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyCollection");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuGroupProperty","KzuGroupProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuGroupProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuLightNode","KzuLightNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuLightNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcDynamicArray","KzcDynamicArray","",NULL);
  tolua_beginmodule(tolua_S,"KzcDynamicArray");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuLightProperty","KzuLightProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuLightProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuLightPropertyType","KzuLightPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuLightPropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuLightPropertyCreate",tolua_kzu_light_property_kzuLightPropertyCreate00);
  tolua_function(tolua_S,"kzuLightPropertyDelete",tolua_kzu_light_property_kzuLightPropertyDelete00);
  tolua_function(tolua_S,"kzuLightPropertyToProperty",tolua_kzu_light_property_kzuLightPropertyToProperty00);
  tolua_function(tolua_S,"kzuLightPropertyFromProperty",tolua_kzu_light_property_kzuLightPropertyFromProperty00);
  tolua_function(tolua_S,"kzuLightPropertyGetLightPropertyType",tolua_kzu_light_property_kzuLightPropertyGetLightPropertyType00);
  tolua_function(tolua_S,"kzuLightPropertyTypeCreate",tolua_kzu_light_property_kzuLightPropertyTypeCreate00);
  tolua_function(tolua_S,"kzuLightPropertyTypeDelete",tolua_kzu_light_property_kzuLightPropertyTypeDelete00);
  tolua_function(tolua_S,"kzuLightPropertyTypeAddProperty",tolua_kzu_light_property_kzuLightPropertyTypeAddProperty00);
  tolua_function(tolua_S,"kzuLightPropertyTypeToPropertyType",tolua_kzu_light_property_kzuLightPropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuLightPropertyTypeFromPropertyType",tolua_kzu_light_property_kzuLightPropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuLightPropertyTypeGetPropertyTypes",tolua_kzu_light_property_kzuLightPropertyTypeGetPropertyTypes00);
  tolua_function(tolua_S,"kzuLightPropertyTypeGetPropertyTypeCount",tolua_kzu_light_property_kzuLightPropertyTypeGetPropertyTypeCount00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_light_property (lua_State* tolua_S) {
 return tolua_kzu_light_property_open(tolua_S);
}
#endif

