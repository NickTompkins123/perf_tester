/*
** Lua binding: kzu_vector3_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_vector3_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_vector3_property.h>
#include <core/util/math/kzc_vector3.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_KzcVector3 (lua_State* tolua_S)
{
 KzcVector3* self = (KzcVector3*) tolua_tousertype(tolua_S,1,0);
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
 tolua_usertype(tolua_S,"KzcVector3");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuVector3PropertyType");
 tolua_usertype(tolua_S,"KzuVector3Property");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuVector3PropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyTypeCreate00
static int tolua_kzu_vector3_property_kzuVector3PropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"KzcVector3",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,5,"KzuVector3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  enum KzuVector3Type vectorType = ((enum KzuVector3Type) (int)  tolua_tonumber(tolua_S,3,0));
  struct KzcVector3 defaultValue = *((struct KzcVector3*)  tolua_tousertype(tolua_S,4,0));
  struct KzuVector3PropertyType* out_propertyType = ((struct KzuVector3PropertyType*)  tolua_tousertype(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector3PropertyTypeCreate(memoryManager,name,vectorType,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuVector3PropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyTypeDelete00
static int tolua_kzu_vector3_property_kzuVector3PropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3PropertyType* propertyType = ((struct KzuVector3PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector3PropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyTypeFromPropertyType00
static int tolua_kzu_vector3_property_kzuVector3PropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuVector3PropertyType* tolua_ret = (struct KzuVector3PropertyType*)  kzuVector3PropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuVector3PropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyTypeToPropertyType00
static int tolua_kzu_vector3_property_kzuVector3PropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3PropertyType* propertyType = ((struct KzuVector3PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuVector3PropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyTypeGetDefaultValue00
static int tolua_kzu_vector3_property_kzuVector3PropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector3PropertyType* propertyType = ((const struct KzuVector3PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuVector3PropertyTypeGetDefaultValue(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector3));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyTypeGetVectorType */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyTypeGetVectorType00
static int tolua_kzu_vector3_property_kzuVector3PropertyTypeGetVectorType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector3PropertyType* vector = ((const struct KzuVector3PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuVector3Type tolua_ret = (enum KzuVector3Type)  kzuVector3PropertyTypeGetVectorType(vector);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyTypeGetVectorType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyCreate00
static int tolua_kzu_vector3_property_kzuVector3PropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuVector3PropertyType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcVector3",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuVector3PropertyType* propertyType = ((struct KzuVector3PropertyType*)  tolua_tousertype(tolua_S,2,0));
  struct KzcVector3 vector = *((struct KzcVector3*)  tolua_tousertype(tolua_S,3,0));
  struct KzuVector3Property* out_property = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector3PropertyCreate(memoryManager,propertyType,vector,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuVector3Property");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyDelete00
static int tolua_kzu_vector3_property_kzuVector3PropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3Property* property = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuVector3PropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyFromProperty00
static int tolua_kzu_vector3_property_kzuVector3PropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuVector3Property* tolua_ret = (struct KzuVector3Property*)  kzuVector3PropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuVector3Property");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyToProperty00
static int tolua_kzu_vector3_property_kzuVector3PropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3Property* vector = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuVector3PropertyToProperty(vector);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertySetRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertySetRelative00
static int tolua_kzu_vector3_property_kzuVector3PropertySetRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3Property* property = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  kzBool relative = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector3PropertySetRelative(property,relative);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertySetRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyIsRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyIsRelative00
static int tolua_kzu_vector3_property_kzuVector3PropertyIsRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector3Property* property = ((const struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuVector3PropertyIsRelative(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyIsRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyGetValue00
static int tolua_kzu_vector3_property_kzuVector3PropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector3Property* vector = ((const struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuVector3PropertyGetValue(vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector3));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertyGetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertyGetRelativeValue00
static int tolua_kzu_vector3_property_kzuVector3PropertyGetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuVector3Property* vector = ((const struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuVector3PropertyGetRelativeValue(vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcVector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcVector3));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcVector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertyGetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertySetValue00
static int tolua_kzu_vector3_property_kzuVector3PropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3Property* vectorProperty = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector3 vector = *((struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector3PropertySetValue(vectorProperty,vector);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertySetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertySetRelativeValue00
static int tolua_kzu_vector3_property_kzuVector3PropertySetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3Property* vectorProperty = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector3 vector = *((struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuVector3PropertySetRelativeValue(vectorProperty,vector);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertySetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertySetComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertySetComponentValue00
static int tolua_kzu_vector3_property_kzuVector3PropertySetComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3Property* vectorProperty = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuVector3PropertySetComponentValue(vectorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertySetComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuVector3PropertySetRelativeComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_vector3_property_kzuVector3PropertySetRelativeComponentValue00
static int tolua_kzu_vector3_property_kzuVector3PropertySetRelativeComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuVector3Property",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuVector3Property* vectorProperty = ((struct KzuVector3Property*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuVector3PropertySetRelativeComponentValue(vectorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuVector3PropertySetRelativeComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_vector3_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuVector3Property","KzuVector3Property","",NULL);
  tolua_beginmodule(tolua_S,"KzuVector3Property");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuVector3PropertyType","KzuVector3PropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuVector3PropertyType");
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"KZU_VECTOR3_TYPE_NORMAL",KZU_VECTOR3_TYPE_NORMAL);
  tolua_constant(tolua_S,"KZU_VECTOR3_TYPE_POSITION_FROM_MATRIX4X4",KZU_VECTOR3_TYPE_POSITION_FROM_MATRIX4X4);
  tolua_constant(tolua_S,"KZU_VECTOR3_TYPE_DIRECTION_FROM_MATRIX4X4",KZU_VECTOR3_TYPE_DIRECTION_FROM_MATRIX4X4);
  tolua_function(tolua_S,"kzuVector3PropertyTypeCreate",tolua_kzu_vector3_property_kzuVector3PropertyTypeCreate00);
  tolua_function(tolua_S,"kzuVector3PropertyTypeDelete",tolua_kzu_vector3_property_kzuVector3PropertyTypeDelete00);
  tolua_function(tolua_S,"kzuVector3PropertyTypeFromPropertyType",tolua_kzu_vector3_property_kzuVector3PropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuVector3PropertyTypeToPropertyType",tolua_kzu_vector3_property_kzuVector3PropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuVector3PropertyTypeGetDefaultValue",tolua_kzu_vector3_property_kzuVector3PropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuVector3PropertyTypeGetVectorType",tolua_kzu_vector3_property_kzuVector3PropertyTypeGetVectorType00);
  tolua_function(tolua_S,"kzuVector3PropertyCreate",tolua_kzu_vector3_property_kzuVector3PropertyCreate00);
  tolua_function(tolua_S,"kzuVector3PropertyDelete",tolua_kzu_vector3_property_kzuVector3PropertyDelete00);
  tolua_function(tolua_S,"kzuVector3PropertyFromProperty",tolua_kzu_vector3_property_kzuVector3PropertyFromProperty00);
  tolua_function(tolua_S,"kzuVector3PropertyToProperty",tolua_kzu_vector3_property_kzuVector3PropertyToProperty00);
  tolua_function(tolua_S,"kzuVector3PropertySetRelative",tolua_kzu_vector3_property_kzuVector3PropertySetRelative00);
  tolua_function(tolua_S,"kzuVector3PropertyIsRelative",tolua_kzu_vector3_property_kzuVector3PropertyIsRelative00);
  tolua_function(tolua_S,"kzuVector3PropertyGetValue",tolua_kzu_vector3_property_kzuVector3PropertyGetValue00);
  tolua_function(tolua_S,"kzuVector3PropertyGetRelativeValue",tolua_kzu_vector3_property_kzuVector3PropertyGetRelativeValue00);
  tolua_function(tolua_S,"kzuVector3PropertySetValue",tolua_kzu_vector3_property_kzuVector3PropertySetValue00);
  tolua_function(tolua_S,"kzuVector3PropertySetRelativeValue",tolua_kzu_vector3_property_kzuVector3PropertySetRelativeValue00);
  tolua_function(tolua_S,"kzuVector3PropertySetComponentValue",tolua_kzu_vector3_property_kzuVector3PropertySetComponentValue00);
  tolua_function(tolua_S,"kzuVector3PropertySetRelativeComponentValue",tolua_kzu_vector3_property_kzuVector3PropertySetRelativeComponentValue00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_vector3_property (lua_State* tolua_S) {
 return tolua_kzu_vector3_property_open(tolua_S);
}
#endif

