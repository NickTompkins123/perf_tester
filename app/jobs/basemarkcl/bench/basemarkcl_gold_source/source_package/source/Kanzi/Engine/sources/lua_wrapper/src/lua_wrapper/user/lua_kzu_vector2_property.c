/*
** Lua binding: kzu_vector2_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_vector2_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_vector2_property.h>
#include <core/util/math/kzc_vector2.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_KzcVector2 (lua_State* tolua_S)
{
 KzcVector2* self = (KzcVector2*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

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
 tolua_usertype(tolua_S,"KzuVector2Property");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuVector2PropertyType");
 tolua_usertype(tolua_S,"KzcVector2");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuVector2PropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyTypeCreate00
static int tolua_kzu_vector2_property_kzuVector2PropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcVector2",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuVector2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzcVector2 defaultValue = *((struct KzcVector2*)  tolua_tousertype(tolua_S,3,0));
  struct KzuVector2PropertyType* out_propertyType = ((struct KzuVector2PropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector2PropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuVector2PropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyTypeDelete00
static int tolua_kzu_vector2_property_kzuVector2PropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2PropertyType* property = ((struct KzuVector2PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector2PropertyTypeDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyTypeFromPropertyType00
static int tolua_kzu_vector2_property_kzuVector2PropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuVector2PropertyType* tolua_ret = (struct KzuVector2PropertyType*)  kzuVector2PropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuVector2PropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyTypeToPropertyType00
static int tolua_kzu_vector2_property_kzuVector2PropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2PropertyType* propertyType = ((struct KzuVector2PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuVector2PropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyTypeGetDefaultValue00
static int tolua_kzu_vector2_property_kzuVector2PropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector2PropertyType* propertyType = ((const struct KzuVector2PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector2 tolua_ret =   kzuVector2PropertyTypeGetDefaultValue(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector2)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector2));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyCreate00
static int tolua_kzu_vector2_property_kzuVector2PropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuVector2PropertyType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcVector2",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuVector2PropertyType* propertyType = ((struct KzuVector2PropertyType*)  tolua_tousertype(tolua_S,2,0));
  struct KzcVector2 vector = *((struct KzcVector2*)  tolua_tousertype(tolua_S,3,0));
  struct KzuVector2Property* out_property = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector2PropertyCreate(memoryManager,propertyType,vector,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuVector2Property");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyDelete00
static int tolua_kzu_vector2_property_kzuVector2PropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2Property* property = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector2PropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertySetRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertySetRelative00
static int tolua_kzu_vector2_property_kzuVector2PropertySetRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2Property* property = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  kzBool relative = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector2PropertySetRelative(property,relative);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertySetRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyIsRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyIsRelative00
static int tolua_kzu_vector2_property_kzuVector2PropertyIsRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector2Property* property = ((const struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuVector2PropertyIsRelative(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyIsRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyFromProperty00
static int tolua_kzu_vector2_property_kzuVector2PropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuVector2Property* tolua_ret = (struct KzuVector2Property*)  kzuVector2PropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuVector2Property");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyToProperty00
static int tolua_kzu_vector2_property_kzuVector2PropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2Property* vector = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuVector2PropertyToProperty(vector);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyGetValue00
static int tolua_kzu_vector2_property_kzuVector2PropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector2Property* vector = ((const struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector2 tolua_ret =   kzuVector2PropertyGetValue(vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector2)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector2));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertyGetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertyGetRelativeValue00
static int tolua_kzu_vector2_property_kzuVector2PropertyGetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector2Property* vector = ((const struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector2 tolua_ret =   kzuVector2PropertyGetRelativeValue(vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector2)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector2));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertyGetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertySetValue00
static int tolua_kzu_vector2_property_kzuVector2PropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcVector2",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2Property* vectorProperty = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector2 vector = *((struct KzcVector2*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector2PropertySetValue(vectorProperty,vector);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertySetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertySetRelativeValue00
static int tolua_kzu_vector2_property_kzuVector2PropertySetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcVector2",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2Property* vectorProperty = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector2 vector = *((struct KzcVector2*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector2PropertySetRelativeValue(vectorProperty,vector);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertySetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertySetComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertySetComponentValue00
static int tolua_kzu_vector2_property_kzuVector2PropertySetComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2Property* vectorProperty = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuVector2PropertySetComponentValue(vectorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertySetComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector2PropertySetRelativeComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector2_property_kzuVector2PropertySetRelativeComponentValue00
static int tolua_kzu_vector2_property_kzuVector2PropertySetRelativeComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector2Property",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector2Property* vectorProperty = ((struct KzuVector2Property*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuVector2PropertySetRelativeComponentValue(vectorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector2PropertySetRelativeComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_vector2_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuVector2Property","KzuVector2Property","",NULL);
  tolua_beginmodule(tolua_S,"KzuVector2Property");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuVector2PropertyType","KzuVector2PropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuVector2PropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuVector2PropertyTypeCreate",tolua_kzu_vector2_property_kzuVector2PropertyTypeCreate00);
  tolua_function(tolua_S,"kzuVector2PropertyTypeDelete",tolua_kzu_vector2_property_kzuVector2PropertyTypeDelete00);
  tolua_function(tolua_S,"kzuVector2PropertyTypeFromPropertyType",tolua_kzu_vector2_property_kzuVector2PropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuVector2PropertyTypeToPropertyType",tolua_kzu_vector2_property_kzuVector2PropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuVector2PropertyTypeGetDefaultValue",tolua_kzu_vector2_property_kzuVector2PropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuVector2PropertyCreate",tolua_kzu_vector2_property_kzuVector2PropertyCreate00);
  tolua_function(tolua_S,"kzuVector2PropertyDelete",tolua_kzu_vector2_property_kzuVector2PropertyDelete00);
  tolua_function(tolua_S,"kzuVector2PropertySetRelative",tolua_kzu_vector2_property_kzuVector2PropertySetRelative00);
  tolua_function(tolua_S,"kzuVector2PropertyIsRelative",tolua_kzu_vector2_property_kzuVector2PropertyIsRelative00);
  tolua_function(tolua_S,"kzuVector2PropertyFromProperty",tolua_kzu_vector2_property_kzuVector2PropertyFromProperty00);
  tolua_function(tolua_S,"kzuVector2PropertyToProperty",tolua_kzu_vector2_property_kzuVector2PropertyToProperty00);
  tolua_function(tolua_S,"kzuVector2PropertyGetValue",tolua_kzu_vector2_property_kzuVector2PropertyGetValue00);
  tolua_function(tolua_S,"kzuVector2PropertyGetRelativeValue",tolua_kzu_vector2_property_kzuVector2PropertyGetRelativeValue00);
  tolua_function(tolua_S,"kzuVector2PropertySetValue",tolua_kzu_vector2_property_kzuVector2PropertySetValue00);
  tolua_function(tolua_S,"kzuVector2PropertySetRelativeValue",tolua_kzu_vector2_property_kzuVector2PropertySetRelativeValue00);
  tolua_function(tolua_S,"kzuVector2PropertySetComponentValue",tolua_kzu_vector2_property_kzuVector2PropertySetComponentValue00);
  tolua_function(tolua_S,"kzuVector2PropertySetRelativeComponentValue",tolua_kzu_vector2_property_kzuVector2PropertySetRelativeComponentValue00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_vector2_property (lua_State* tolua_S) {
 return tolua_kzu_vector2_property_open(tolua_S);
}
#endif

