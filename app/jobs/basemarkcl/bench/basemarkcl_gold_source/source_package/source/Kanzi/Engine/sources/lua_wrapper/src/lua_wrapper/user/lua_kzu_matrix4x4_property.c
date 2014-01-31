/*
** Lua binding: kzu_matrix4x4_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_matrix4x4_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_matrix4x4_property.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzBool (lua_State* tolua_S)
{
 kzBool* self = (kzBool*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcMatrix4x4 (lua_State* tolua_S)
{
 KzcMatrix4x4* self = (KzcMatrix4x4*) tolua_tousertype(tolua_S,1,0);
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
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuMatrix4x4Property");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzcMatrix4x4");
 tolua_usertype(tolua_S,"KzuMatrix4x4PropertyType");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuMatrix4x4PropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeCreate00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcMatrix4x4",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuMatrix4x4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzcMatrix4x4 defaultValue = *((struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,3,0));
  struct KzuMatrix4x4PropertyType* out_propertyType = ((struct KzuMatrix4x4PropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix4x4PropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuMatrix4x4PropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeDelete00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix4x4PropertyType* propertyType = ((struct KzuMatrix4x4PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix4x4PropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeFromPropertyType00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuMatrix4x4PropertyType* tolua_ret = (struct KzuMatrix4x4PropertyType*)  kzuMatrix4x4PropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMatrix4x4PropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeToPropertyType00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix4x4PropertyType* propertyType = ((struct KzuMatrix4x4PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuMatrix4x4PropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeGetDefaultValue00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4PropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix4x4PropertyType* propertyType = ((const struct KzuMatrix4x4PropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix4x4 tolua_ret =   kzuMatrix4x4PropertyTypeGetDefaultValue(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix4x4)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix4x4));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyCreate00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMatrix4x4PropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMatrix4x4PropertyType* propertyType = ((struct KzuMatrix4x4PropertyType*)  tolua_tousertype(tolua_S,2,0));
  const struct KzcMatrix4x4* matrix = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,3,0));
  struct KzuMatrix4x4Property* out_property = ((struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix4x4PropertyCreate(memoryManager,propertyType,matrix,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuMatrix4x4Property");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyDelete00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix4x4Property* property = ((struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMatrix4x4PropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertySetRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetRelative00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix4x4Property* matrixProperty = ((struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  kzBool relative = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMatrix4x4PropertySetRelative(matrixProperty,relative);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertySetRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyIsRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyIsRelative00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyIsRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix4x4Property* matrix = ((const struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuMatrix4x4PropertyIsRelative(matrix);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyIsRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyGetValue00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix4x4Property* matrix = ((const struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix4x4 tolua_ret =   kzuMatrix4x4PropertyGetValue(matrix);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix4x4)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix4x4));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyGetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyGetRelativeValue00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyGetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMatrix4x4Property* matrix = ((const struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix4x4 tolua_ret =   kzuMatrix4x4PropertyGetRelativeValue(matrix);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix4x4)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix4x4));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyGetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyToProperty00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix4x4Property* property = ((struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuMatrix4x4PropertyToProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyFromProperty00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuMatrix4x4Property* tolua_ret = (struct KzuMatrix4x4Property*)  kzuMatrix4x4PropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMatrix4x4Property");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetValue00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix4x4Property* matrixProperty = ((struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix4x4* matrix = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMatrix4x4PropertySetValue(matrixProperty,matrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMatrix4x4PropertySetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetRelativeValue00
static int tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMatrix4x4Property",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMatrix4x4Property* matrixProperty = ((struct KzuMatrix4x4Property*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix4x4* matrix = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMatrix4x4PropertySetRelativeValue(matrixProperty,matrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMatrix4x4PropertySetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_matrix4x4_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuMatrix4x4Property","KzuMatrix4x4Property","",NULL);
  tolua_beginmodule(tolua_S,"KzuMatrix4x4Property");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMatrix4x4PropertyType","KzuMatrix4x4PropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuMatrix4x4PropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyTypeCreate",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeCreate00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyTypeDelete",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeDelete00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyTypeFromPropertyType",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyTypeToPropertyType",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyTypeGetDefaultValue",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyCreate",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyCreate00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyDelete",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyDelete00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertySetRelative",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetRelative00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyIsRelative",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyIsRelative00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyGetValue",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyGetValue00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyGetRelativeValue",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyGetRelativeValue00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyToProperty",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyToProperty00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertyFromProperty",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertyFromProperty00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertySetValue",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetValue00);
  tolua_function(tolua_S,"kzuMatrix4x4PropertySetRelativeValue",tolua_kzu_matrix4x4_property_kzuMatrix4x4PropertySetRelativeValue00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_matrix4x4_property (lua_State* tolua_S) {
 return tolua_kzu_matrix4x4_property_open(tolua_S);
}
#endif

