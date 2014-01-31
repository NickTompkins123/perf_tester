/*
** Lua binding: kzu_color_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_color_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_color_property.h>
#include <core/util/color/kzc_color.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_KzcColorRGBA (lua_State* tolua_S)
{
 KzcColorRGBA* self = (KzcColorRGBA*) tolua_tousertype(tolua_S,1,0);
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
 tolua_usertype(tolua_S,"KzcColorRGBA");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuColorPropertyType");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzuColorProperty");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuColorPropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyTypeCreate00
static int tolua_kzu_color_property_kzuColorPropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcColorRGBA",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuColorPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzcColorRGBA defaultValue = *((struct KzcColorRGBA*)  tolua_tousertype(tolua_S,3,0));
  struct KzuColorPropertyType* out_propertyType = ((struct KzuColorPropertyType*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuColorPropertyTypeCreate(memoryManager,name,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuColorPropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyTypeDelete00
static int tolua_kzu_color_property_kzuColorPropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorPropertyType* propertyType = ((struct KzuColorPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuColorPropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyTypeFromPropertyType00
static int tolua_kzu_color_property_kzuColorPropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuColorPropertyType* tolua_ret = (struct KzuColorPropertyType*)  kzuColorPropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuColorPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyTypeToPropertyType00
static int tolua_kzu_color_property_kzuColorPropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorPropertyType* propertyType = ((struct KzuColorPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuColorPropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyTypeGetDefaultValue00
static int tolua_kzu_color_property_kzuColorPropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuColorPropertyType* propertyType = ((const struct KzuColorPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcColorRGBA tolua_ret =   kzuColorPropertyTypeGetDefaultValue(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcColorRGBA)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcColorRGBA");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcColorRGBA));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcColorRGBA");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyCreate00
static int tolua_kzu_color_property_kzuColorPropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuColorPropertyType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcColorRGBA",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuColorPropertyType* propertyType = ((struct KzuColorPropertyType*)  tolua_tousertype(tolua_S,2,0));
  struct KzcColorRGBA color = *((struct KzcColorRGBA*)  tolua_tousertype(tolua_S,3,0));
  struct KzuColorProperty* out_property = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuColorPropertyCreate(memoryManager,propertyType,color,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuColorProperty");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyDelete00
static int tolua_kzu_color_property_kzuColorPropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorProperty* property = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuColorPropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertySetRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertySetRelative00
static int tolua_kzu_color_property_kzuColorPropertySetRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorProperty* property = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  kzBool relative = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuColorPropertySetRelative(property,relative);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertySetRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyIsRelative */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyIsRelative00
static int tolua_kzu_color_property_kzuColorPropertyIsRelative00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuColorProperty* property = ((const struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuColorPropertyIsRelative(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyIsRelative'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertySetValue00
static int tolua_kzu_color_property_kzuColorPropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcColorRGBA",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorProperty* colorProperty = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcColorRGBA* color = ((const struct KzcColorRGBA*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuColorPropertySetValue(colorProperty,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertySetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertySetRelativeValue00
static int tolua_kzu_color_property_kzuColorPropertySetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcColorRGBA",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorProperty* colorProperty = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcColorRGBA* color = ((const struct KzcColorRGBA*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuColorPropertySetRelativeValue(colorProperty,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertySetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertySetComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertySetComponentValue00
static int tolua_kzu_color_property_kzuColorPropertySetComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorProperty* colorProperty = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuColorPropertySetComponentValue(colorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertySetComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertySetRelativeComponentValue */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertySetRelativeComponentValue00
static int tolua_kzu_color_property_kzuColorPropertySetRelativeComponentValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorProperty* colorProperty = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  unsigned int component = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzuColorPropertySetRelativeComponentValue(colorProperty,component,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertySetRelativeComponentValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyGetValue00
static int tolua_kzu_color_property_kzuColorPropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuColorProperty* color = ((const struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcColorRGBA tolua_ret =   kzuColorPropertyGetValue(color);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcColorRGBA)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcColorRGBA");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcColorRGBA));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcColorRGBA");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyGetRelativeValue */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyGetRelativeValue00
static int tolua_kzu_color_property_kzuColorPropertyGetRelativeValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuColorProperty* color = ((const struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcColorRGBA tolua_ret =   kzuColorPropertyGetRelativeValue(color);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcColorRGBA)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcColorRGBA");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcColorRGBA));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcColorRGBA");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyGetRelativeValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyToProperty00
static int tolua_kzu_color_property_kzuColorPropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuColorProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuColorProperty* property = ((struct KzuColorProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuColorPropertyToProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuColorPropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_color_property_kzuColorPropertyFromProperty00
static int tolua_kzu_color_property_kzuColorPropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuColorProperty* tolua_ret = (struct KzuColorProperty*)  kzuColorPropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuColorProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuColorPropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_color_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuColorProperty","KzuColorProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuColorProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuColorPropertyType","KzuColorPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuColorPropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuColorPropertyTypeCreate",tolua_kzu_color_property_kzuColorPropertyTypeCreate00);
  tolua_function(tolua_S,"kzuColorPropertyTypeDelete",tolua_kzu_color_property_kzuColorPropertyTypeDelete00);
  tolua_function(tolua_S,"kzuColorPropertyTypeFromPropertyType",tolua_kzu_color_property_kzuColorPropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuColorPropertyTypeToPropertyType",tolua_kzu_color_property_kzuColorPropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuColorPropertyTypeGetDefaultValue",tolua_kzu_color_property_kzuColorPropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuColorPropertyCreate",tolua_kzu_color_property_kzuColorPropertyCreate00);
  tolua_function(tolua_S,"kzuColorPropertyDelete",tolua_kzu_color_property_kzuColorPropertyDelete00);
  tolua_function(tolua_S,"kzuColorPropertySetRelative",tolua_kzu_color_property_kzuColorPropertySetRelative00);
  tolua_function(tolua_S,"kzuColorPropertyIsRelative",tolua_kzu_color_property_kzuColorPropertyIsRelative00);
  tolua_function(tolua_S,"kzuColorPropertySetValue",tolua_kzu_color_property_kzuColorPropertySetValue00);
  tolua_function(tolua_S,"kzuColorPropertySetRelativeValue",tolua_kzu_color_property_kzuColorPropertySetRelativeValue00);
  tolua_function(tolua_S,"kzuColorPropertySetComponentValue",tolua_kzu_color_property_kzuColorPropertySetComponentValue00);
  tolua_function(tolua_S,"kzuColorPropertySetRelativeComponentValue",tolua_kzu_color_property_kzuColorPropertySetRelativeComponentValue00);
  tolua_function(tolua_S,"kzuColorPropertyGetValue",tolua_kzu_color_property_kzuColorPropertyGetValue00);
  tolua_function(tolua_S,"kzuColorPropertyGetRelativeValue",tolua_kzu_color_property_kzuColorPropertyGetRelativeValue00);
  tolua_function(tolua_S,"kzuColorPropertyToProperty",tolua_kzu_color_property_kzuColorPropertyToProperty00);
  tolua_function(tolua_S,"kzuColorPropertyFromProperty",tolua_kzu_color_property_kzuColorPropertyFromProperty00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_color_property (lua_State* tolua_S) {
 return tolua_kzu_color_property_open(tolua_S);
}
#endif

