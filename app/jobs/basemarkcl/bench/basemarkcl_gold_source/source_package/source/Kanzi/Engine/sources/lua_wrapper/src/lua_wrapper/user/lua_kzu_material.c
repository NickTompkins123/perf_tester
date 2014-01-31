/*
** Lua binding: kzu_material
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_material_open (lua_State* tolua_S);

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
#include <user/material/kzu_material.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/color/kzc_color.h>
#include <system/kzs_types.h>
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
 tolua_usertype(tolua_S,"KzuMaterialType");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuMaterial");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuMaterialCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialCreate00
static int tolua_kzu_material_kzuMaterialCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuMaterialType",0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  struct KzuMaterialType* type = ((struct KzuMaterialType*)  tolua_tousertype(tolua_S,3,0));
  char* name = ((char*)  tolua_tostring(tolua_S,4,0));
  struct KzuMaterial* out_material = ((struct KzuMaterial*)  tolua_tousertype(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMaterialCreate(memoryManager,propertyManager,type,name,&out_material);
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
    tolua_pushusertype(tolua_S,(void*)out_material,"KzuMaterial");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialDelete00
static int tolua_kzu_material_kzuMaterialDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMaterial* material = ((struct KzuMaterial*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMaterialDelete(material);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialTakeOwnership */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialTakeOwnership00
static int tolua_kzu_material_kzuMaterialTakeOwnership00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMaterial* material = ((const struct KzuMaterial*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuMaterialTakeOwnership(material);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialTakeOwnership'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialIsValid */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialIsValid00
static int tolua_kzu_material_kzuMaterialIsValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMaterial* material = ((const struct KzuMaterial*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuMaterialIsValid(material);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialIsValid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialTransferData */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialTransferData00
static int tolua_kzu_material_kzuMaterialTransferData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMaterial",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMaterial* targetMaterial = ((struct KzuMaterial*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMaterial* sourceMaterial = ((struct KzuMaterial*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMaterialTransferData(targetMaterial,sourceMaterial);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialTransferData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialCopy */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialCopy00
static int tolua_kzu_material_kzuMaterialCopy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMaterial",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuMaterial* materialToCopy = ((const struct KzuMaterial*)  tolua_tousertype(tolua_S,2,0));
  struct KzuMaterial* out_material = ((struct KzuMaterial*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMaterialCopy(memoryManager,materialToCopy,&out_material);
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
    tolua_pushusertype(tolua_S,(void*)out_material,"KzuMaterial");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialCopy'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialGetPropertyManager */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialGetPropertyManager00
static int tolua_kzu_material_kzuMaterialGetPropertyManager00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMaterial* material = ((const struct KzuMaterial*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyManager* tolua_ret = (struct KzuPropertyManager*)  kzuMaterialGetPropertyManager(material);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialGetPropertyManager'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialGetName */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialGetName00
static int tolua_kzu_material_kzuMaterialGetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMaterial* material = ((const struct KzuMaterial*)  tolua_tousertype(tolua_S,1,0));
  {
   char* tolua_ret = (char*)  kzuMaterialGetName(material);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialGetName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMaterialGetMaterialType */
#ifndef TOLUA_DISABLE_tolua_kzu_material_kzuMaterialGetMaterialType00
static int tolua_kzu_material_kzuMaterialGetMaterialType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMaterial* material = ((const struct KzuMaterial*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuMaterialType* tolua_ret = (struct KzuMaterialType*)  kzuMaterialGetMaterialType(material);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMaterialType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMaterialGetMaterialType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_material_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuMaterialType","KzuMaterialType","",NULL);
  tolua_beginmodule(tolua_S,"KzuMaterialType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyType","KzuPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMaterial","KzuMaterial","",NULL);
  tolua_beginmodule(tolua_S,"KzuMaterial");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuMaterialCreate",tolua_kzu_material_kzuMaterialCreate00);
  tolua_function(tolua_S,"kzuMaterialDelete",tolua_kzu_material_kzuMaterialDelete00);
  tolua_function(tolua_S,"kzuMaterialTakeOwnership",tolua_kzu_material_kzuMaterialTakeOwnership00);
  tolua_function(tolua_S,"kzuMaterialIsValid",tolua_kzu_material_kzuMaterialIsValid00);
  tolua_function(tolua_S,"kzuMaterialTransferData",tolua_kzu_material_kzuMaterialTransferData00);
  tolua_function(tolua_S,"kzuMaterialCopy",tolua_kzu_material_kzuMaterialCopy00);
  tolua_function(tolua_S,"kzuMaterialGetPropertyManager",tolua_kzu_material_kzuMaterialGetPropertyManager00);
  tolua_function(tolua_S,"kzuMaterialGetName",tolua_kzu_material_kzuMaterialGetName00);
  tolua_function(tolua_S,"kzuMaterialGetMaterialType",tolua_kzu_material_kzuMaterialGetMaterialType00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_material (lua_State* tolua_S) {
 return tolua_kzu_material_open(tolua_S);
}
#endif

