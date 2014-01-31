/*
** Lua binding: kzu_texture_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_texture_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_texture_property.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

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
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuTexturePropertyType");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzuMatrix4x4PropertyType");
 tolua_usertype(tolua_S,"KzcTexture");
 tolua_usertype(tolua_S,"KzuTextureProperty");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuTexturePropertyTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeCreate00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuMatrix4x4PropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,6,"KzcTexture",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,7,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  enum KzuTextureOperation operation = ((enum KzuTextureOperation) (int)  tolua_tonumber(tolua_S,3,0));
  unsigned int textureUnit = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  struct KzuMatrix4x4PropertyType* matrixPropertyType = ((struct KzuMatrix4x4PropertyType*)  tolua_tousertype(tolua_S,5,0));
  struct KzcTexture* defaultValue = ((struct KzcTexture*)  tolua_tousertype(tolua_S,6,0));
  struct KzuTexturePropertyType* out_propertyType = ((struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,7,0));
  {
   kzsError tolua_ret = (kzsError)  kzuTexturePropertyTypeCreate(memoryManager,name,operation,textureUnit,matrixPropertyType,defaultValue,&out_propertyType);
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
    tolua_pushusertype(tolua_S,(void*)out_propertyType,"KzuTexturePropertyType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeDelete00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuTexturePropertyType* propertyType = ((struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuTexturePropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyTypeFromPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeFromPropertyType00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeFromPropertyType00(lua_State* tolua_S)
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
   struct KzuTexturePropertyType* tolua_ret = (struct KzuTexturePropertyType*)  kzuTexturePropertyTypeFromPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuTexturePropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeFromPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyTypeToPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeToPropertyType00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeToPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuTexturePropertyType* propertyType = ((struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuTexturePropertyTypeToPropertyType(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeToPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyTypeGetDefaultValue */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeGetDefaultValue00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeGetDefaultValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTexturePropertyType* propertyType = ((const struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcTexture* tolua_ret = (struct KzcTexture*)  kzuTexturePropertyTypeGetDefaultValue(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcTexture");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeGetDefaultValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyCreate00
static int tolua_kzu_texture_property_kzuTexturePropertyCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcTexture",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuTextureProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuTexturePropertyType* propertyType = ((struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,2,0));
  struct KzcTexture* texture = ((struct KzcTexture*)  tolua_tousertype(tolua_S,3,0));
  struct KzuTextureProperty* out_property = ((struct KzuTextureProperty*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuTexturePropertyCreate(memoryManager,propertyType,texture,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuTextureProperty");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyDelete00
static int tolua_kzu_texture_property_kzuTexturePropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTextureProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuTextureProperty* property = ((struct KzuTextureProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuTexturePropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyToProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyToProperty00
static int tolua_kzu_texture_property_kzuTexturePropertyToProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTextureProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuTextureProperty* texture = ((struct KzuTextureProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuProperty* tolua_ret = (struct KzuProperty*)  kzuTexturePropertyToProperty(texture);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyToProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyFromProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyFromProperty00
static int tolua_kzu_texture_property_kzuTexturePropertyFromProperty00(lua_State* tolua_S)
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
   struct KzuTextureProperty* tolua_ret = (struct KzuTextureProperty*)  kzuTexturePropertyFromProperty(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuTextureProperty");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyFromProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyGetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyGetValue00
static int tolua_kzu_texture_property_kzuTexturePropertyGetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTextureProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTextureProperty* texture = ((const struct KzuTextureProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcTexture* tolua_ret = (struct KzcTexture*)  kzuTexturePropertyGetValue(texture);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcTexture");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyGetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertySetValue */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertySetValue00
static int tolua_kzu_texture_property_kzuTexturePropertySetValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTextureProperty",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcTexture",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuTextureProperty* textureProperty = ((struct KzuTextureProperty*)  tolua_tousertype(tolua_S,1,0));
  struct KzcTexture* texture = ((struct KzcTexture*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuTexturePropertySetValue(textureProperty,texture);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertySetValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyTypeGetTextureUnit */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureUnit00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureUnit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTexturePropertyType* propertyType = ((const struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuTexturePropertyTypeGetTextureUnit(propertyType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeGetTextureUnit'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyTypeGetTextureOperation */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureOperation00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureOperation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTexturePropertyType* propertyType = ((const struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuTextureOperation tolua_ret = (enum KzuTextureOperation)  kzuTexturePropertyTypeGetTextureOperation(propertyType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeGetTextureOperation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTexturePropertyTypeGetTextureMatrix */
#ifndef TOLUA_DISABLE_tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureMatrix00
static int tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureMatrix00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTexturePropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTexturePropertyType* propertyType = ((const struct KzuTexturePropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuMatrix4x4PropertyType* tolua_ret = (struct KzuMatrix4x4PropertyType*)  kzuTexturePropertyTypeGetTextureMatrix(propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMatrix4x4PropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTexturePropertyTypeGetTextureMatrix'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_texture_property_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuMatrix4x4PropertyType","KzuMatrix4x4PropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuMatrix4x4PropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcTexture","KzcTexture","",NULL);
  tolua_beginmodule(tolua_S,"KzcTexture");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTextureProperty","KzuTextureProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuTextureProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTexturePropertyType","KzuTexturePropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuTexturePropertyType");
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"KZU_TEXTURE_OPERATION_MODULATE",KZU_TEXTURE_OPERATION_MODULATE);
  tolua_constant(tolua_S,"KZU_TEXTURE_OPERATION_REPLACE",KZU_TEXTURE_OPERATION_REPLACE);
  tolua_constant(tolua_S,"KZU_TEXTURE_OPERATION_DECAL",KZU_TEXTURE_OPERATION_DECAL);
  tolua_constant(tolua_S,"KZU_TEXTURE_OPERATION_ADD",KZU_TEXTURE_OPERATION_ADD);
  tolua_constant(tolua_S,"KZU_TEXTURE_OPERATION_CUSTOM",KZU_TEXTURE_OPERATION_CUSTOM);
  tolua_function(tolua_S,"kzuTexturePropertyTypeCreate",tolua_kzu_texture_property_kzuTexturePropertyTypeCreate00);
  tolua_function(tolua_S,"kzuTexturePropertyTypeDelete",tolua_kzu_texture_property_kzuTexturePropertyTypeDelete00);
  tolua_function(tolua_S,"kzuTexturePropertyTypeFromPropertyType",tolua_kzu_texture_property_kzuTexturePropertyTypeFromPropertyType00);
  tolua_function(tolua_S,"kzuTexturePropertyTypeToPropertyType",tolua_kzu_texture_property_kzuTexturePropertyTypeToPropertyType00);
  tolua_function(tolua_S,"kzuTexturePropertyTypeGetDefaultValue",tolua_kzu_texture_property_kzuTexturePropertyTypeGetDefaultValue00);
  tolua_function(tolua_S,"kzuTexturePropertyCreate",tolua_kzu_texture_property_kzuTexturePropertyCreate00);
  tolua_function(tolua_S,"kzuTexturePropertyDelete",tolua_kzu_texture_property_kzuTexturePropertyDelete00);
  tolua_function(tolua_S,"kzuTexturePropertyToProperty",tolua_kzu_texture_property_kzuTexturePropertyToProperty00);
  tolua_function(tolua_S,"kzuTexturePropertyFromProperty",tolua_kzu_texture_property_kzuTexturePropertyFromProperty00);
  tolua_function(tolua_S,"kzuTexturePropertyGetValue",tolua_kzu_texture_property_kzuTexturePropertyGetValue00);
  tolua_function(tolua_S,"kzuTexturePropertySetValue",tolua_kzu_texture_property_kzuTexturePropertySetValue00);
  tolua_function(tolua_S,"kzuTexturePropertyTypeGetTextureUnit",tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureUnit00);
  tolua_function(tolua_S,"kzuTexturePropertyTypeGetTextureOperation",tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureOperation00);
  tolua_function(tolua_S,"kzuTexturePropertyTypeGetTextureMatrix",tolua_kzu_texture_property_kzuTexturePropertyTypeGetTextureMatrix00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_texture_property (lua_State* tolua_S) {
 return tolua_kzu_texture_property_open(tolua_S);
}
#endif

