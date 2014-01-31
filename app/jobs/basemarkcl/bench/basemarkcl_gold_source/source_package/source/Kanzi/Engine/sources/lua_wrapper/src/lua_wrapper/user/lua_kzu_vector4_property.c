/*
** Lua binding: kzu_vector4_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_vector4_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_vector4_property.h>
#include <core/util/math/kzc_vector4.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzBool (lua_State* tolua_S)
{
 kzBool* self = (kzBool*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcVector4 (lua_State* tolua_S)
{
 KzcVector4* self = (KzcVector4*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuVector4PropertyType");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzcVector4");
 tolua_usertype(tolua_S,"KzuVector4Property");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuVector4PropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyTypeCreate00
static int tolua_kzu_vector4_property_kzuVector4PropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcVector4",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuVector4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzcVector4 defaultValue = *((struct KzcVector4*)  tolua_tousertype(tolua_S,3,0));
  struct KzuVector4PropertyType* out_propertyType = ((struct KzuVector4PropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector4PropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuVector4PropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyTypeDelete00
static int tolua_kzu_vector4_property_kzuVector4PropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4PropertyType* property = ((struct KzuVector4PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector4PropertyTypeDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyTypeFromPropertyType00
static int tolua_kzu_vector4_property_kzuVector4PropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuVector4PropertyType* tolua_ret = (struct KzuVector4PropertyType*)  kzuVector4PropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuVector4PropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyTypeToPropertyType00
static int tolua_kzu_vector4_property_kzuVector4PropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4PropertyType* propertyType = ((struct KzuVector4PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuVector4PropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyTypeGetDefaultValue00
static int tolua_kzu_vector4_property_kzuVector4PropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector4PropertyType* propertyType = ((const struct KzuVector4PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector4 tolua_ret =   kzuVector4PropertyTypeGetDefaultValue(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector4)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector4));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyCreate00
static int tolua_kzu_vector4_property_kzuVector4PropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuVector4PropertyType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcVector4",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuVector4PropertyType* propertyType = ((struct KzuVector4PropertyType*)  tolua_tousertype(tolua_S,2,0));
  struct KzcVector4 vector = *((struct KzcVector4*)  tolua_tousertype(tolua_S,3,0));
  struct KzuVector4Property* out_property = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector4PropertyCreate(memoryManager,propertyType,vector,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuVector4Property");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyDelete00
static int tolua_kzu_vector4_property_kzuVector4PropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4Property* property = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector4PropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertySetRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertySetRelative00
static int tolua_kzu_vector4_property_kzuVector4PropertySetRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4Property* property = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  kzBool relative = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector4PropertySetRelative(property,relative);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertySetRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyIsRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyIsRelative00
static int tolua_kzu_vector4_property_kzuVector4PropertyIsRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector4Property* property = ((const struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuVector4PropertyIsRelative(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyIsRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyGetValue00
static int tolua_kzu_vector4_property_kzuVector4PropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector4Property* vector = ((const struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector4 tolua_ret =   kzuVector4PropertyGetValue(vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector4)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector4));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyGetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyGetRelativeValue00
static int tolua_kzu_vector4_property_kzuVector4PropertyGetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector4Property* vector = ((const struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector4 tolua_ret =   kzuVector4PropertyGetRelativeValue(vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector4)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector4));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyGetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertySetValue00
static int tolua_kzu_vector4_property_kzuVector4PropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcVector4",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4Property* vectorProperty = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector4 vector = *((struct KzcVector4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector4PropertySetValue(vectorProperty,vector);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertySetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertySetRelativeValue00
static int tolua_kzu_vector4_property_kzuVector4PropertySetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcVector4",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4Property* vectorProperty = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector4 vector = *((struct KzcVector4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector4PropertySetRelativeValue(vectorProperty,vector);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertySetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyFromProperty00
static int tolua_kzu_vector4_property_kzuVector4PropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuVector4Property* tolua_ret = (struct KzuVector4Property*)  kzuVector4PropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuVector4Property");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertyToProperty00
static int tolua_kzu_vector4_property_kzuVector4PropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4Property* vector = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuVector4PropertyToProperty(vector);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertySetComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertySetComponentValue00
static int tolua_kzu_vector4_property_kzuVector4PropertySetComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4Property* vectorProperty = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuVector4PropertySetComponentValue(vectorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertySetComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector4PropertySetRelativeComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector4_property_kzuVector4PropertySetRelativeComponentValue00
static int tolua_kzu_vector4_property_kzuVector4PropertySetRelativeComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector4Property",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector4Property* vectorProperty = ((struct KzuVector4Property*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuVector4PropertySetRelativeComponentValue(vectorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector4PropertySetRelativeComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_vector4_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuVector4Property","KzuVector4Property","",NULL);
  tolua_beginmodule(tolua_S,"KzuVector4Property");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuVector4PropertyType","KzuVector4PropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuVector4PropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuVector4PropertyTypeCreate",tolua_kzu_vector4_property_kzuVector4PropertyTypeCreate00);
  tolua_function(tolua_S,"kzuVector4PropertyTypeDelete",tolua_kzu_vector4_property_kzuVector4PropertyTypeDelete00);
  tolua_function(tolua_S,"kzuVector4PropertyTypeFromPropertyType",tolua_kzu_vector4_property_kzuVector4PropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuVector4PropertyTypeToPropertyType",tolua_kzu_vector4_property_kzuVector4PropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuVector4PropertyTypeGetDefaultValue",tolua_kzu_vector4_property_kzuVector4PropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuVector4PropertyCreate",tolua_kzu_vector4_property_kzuVector4PropertyCreate00);
  tolua_function(tolua_S,"kzuVector4PropertyDelete",tolua_kzu_vector4_property_kzuVector4PropertyDelete00);
  tolua_function(tolua_S,"kzuVector4PropertySetRelative",tolua_kzu_vector4_property_kzuVector4PropertySetRelative00);
  tolua_function(tolua_S,"kzuVector4PropertyIsRelative",tolua_kzu_vector4_property_kzuVector4PropertyIsRelative00);
  tolua_function(tolua_S,"kzuVector4PropertyGetValue",tolua_kzu_vector4_property_kzuVector4PropertyGetValue00);
  tolua_function(tolua_S,"kzuVector4PropertyGetRelativeValue",tolua_kzu_vector4_property_kzuVector4PropertyGetRelativeValue00);
  tolua_function(tolua_S,"kzuVector4PropertySetValue",tolua_kzu_vector4_property_kzuVector4PropertySetValue00);
  tolua_function(tolua_S,"kzuVector4PropertySetRelativeValue",tolua_kzu_vector4_property_kzuVector4PropertySetRelativeValue00);
  tolua_function(tolua_S,"kzuVector4PropertyFromProperty",tolua_kzu_vector4_property_kzuVector4PropertyFromProperty00);
  tolua_function(tolua_S,"kzuVector4PropertyToProperty",tolua_kzu_vector4_property_kzuVector4PropertyToProperty00);
  tolua_function(tolua_S,"kzuVector4PropertySetComponentValue",tolua_kzu_vector4_property_kzuVector4PropertySetComponentValue00);
  tolua_function(tolua_S,"kzuVector4PropertySetRelativeComponentValue",tolua_kzu_vector4_property_kzuVector4PropertySetRelativeComponentValue00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_vector4_property (lua_State* tolua_S) {
 return tolua_kzu_vector4_property_open(tolua_S);
}
#endif

