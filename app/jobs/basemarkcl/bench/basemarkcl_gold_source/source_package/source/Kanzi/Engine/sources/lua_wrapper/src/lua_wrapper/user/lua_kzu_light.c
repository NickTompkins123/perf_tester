/*
** Lua binding: kzu_light
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_light_open (lua_State* tolua_S);

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
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_object_base.h>
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
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuLightPropertyType");
 tolua_usertype(tolua_S,"KzuLightNode");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzuLightProperty");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuObjectNode");
}

/* function: kzuLightNodeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeCreate00
static int tolua_kzu_light_kzuLightNodeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  char* name = ((char*)  tolua_tostring(tolua_S,3,0));
  struct KzuLightNode* out_lightNode = ((struct KzuLightNode*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightNodeCreate(memoryManager,propertyManager,name,&out_lightNode);
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
    tolua_pushusertype(tolua_S,(void*)out_lightNode,"KzuLightNode");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeDelete00
static int tolua_kzu_light_kzuLightNodeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuLightNode* lightNode = ((struct KzuLightNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightNodeDelete(lightNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeCreateDirectional */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeCreateDirectional00
static int tolua_kzu_light_kzuLightNodeCreateDirectional00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  char* name = ((char*)  tolua_tostring(tolua_S,3,0));
  struct KzuLightNode* out_lightNode = ((struct KzuLightNode*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightNodeCreateDirectional(memoryManager,propertyManager,name,&out_lightNode);
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
    tolua_pushusertype(tolua_S,(void*)out_lightNode,"KzuLightNode");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeCreateDirectional'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeCreatePoint */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeCreatePoint00
static int tolua_kzu_light_kzuLightNodeCreatePoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  char* name = ((char*)  tolua_tostring(tolua_S,3,0));
  struct KzuLightNode* out_lightNode = ((struct KzuLightNode*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightNodeCreatePoint(memoryManager,propertyManager,name,&out_lightNode);
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
    tolua_pushusertype(tolua_S,(void*)out_lightNode,"KzuLightNode");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeCreatePoint'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeCreateSpot */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeCreateSpot00
static int tolua_kzu_light_kzuLightNodeCreateSpot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  char* name = ((char*)  tolua_tostring(tolua_S,3,0));
  struct KzuLightNode* out_lightNode = ((struct KzuLightNode*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightNodeCreateSpot(memoryManager,propertyManager,name,&out_lightNode);
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
    tolua_pushusertype(tolua_S,(void*)out_lightNode,"KzuLightNode");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeCreateSpot'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeInitialize */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeInitialize00
static int tolua_kzu_light_kzuLightNodeInitialize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuLightPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightNode* lightNode = ((const struct KzuLightNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzuLightPropertyType* lightPropertyType = ((struct KzuLightPropertyType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuLightNodeInitialize(lightNode,lightPropertyType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeInitialize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeFromObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeFromObjectNode00
static int tolua_kzu_light_kzuLightNodeFromObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuLightNode* tolua_ret = (struct KzuLightNode*)  kzuLightNodeFromObjectNode(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuLightNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeFromObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeToObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeToObjectNode00
static int tolua_kzu_light_kzuLightNodeToObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuLightNode* lightNode = ((struct KzuLightNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuLightNodeToObjectNode(lightNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeToObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeGetLightPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeGetLightPropertyType00
static int tolua_kzu_light_kzuLightNodeGetLightPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightNode* lightNode = ((const struct KzuLightNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuLightPropertyType* tolua_ret = (struct KzuLightPropertyType*)  kzuLightNodeGetLightPropertyType(lightNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuLightPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeGetLightPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeIsEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeIsEnabled00
static int tolua_kzu_light_kzuLightNodeIsEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightNode* lightNode = ((const struct KzuLightNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuLightNodeIsEnabled(lightNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeIsEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuLightNodeSetShadowCasterEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_light_kzuLightNodeSetShadowCasterEnabled00
static int tolua_kzu_light_kzuLightNodeSetShadowCasterEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuLightNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuLightNode* lightNode = ((const struct KzuLightNode*)  tolua_tousertype(tolua_S,1,0));
  kzBool enabled = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuLightNodeSetShadowCasterEnabled(lightNode,enabled);
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
 tolua_error(tolua_S,"#ferror in function 'kzuLightNodeSetShadowCasterEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_light_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuLightPropertyType","KzuLightPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuLightPropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuLightProperty","KzuLightProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuLightProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuLightNode","KzuLightNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuLightNode");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuLightNodeCreate",tolua_kzu_light_kzuLightNodeCreate00);
  tolua_function(tolua_S,"kzuLightNodeDelete",tolua_kzu_light_kzuLightNodeDelete00);
  tolua_function(tolua_S,"kzuLightNodeCreateDirectional",tolua_kzu_light_kzuLightNodeCreateDirectional00);
  tolua_function(tolua_S,"kzuLightNodeCreatePoint",tolua_kzu_light_kzuLightNodeCreatePoint00);
  tolua_function(tolua_S,"kzuLightNodeCreateSpot",tolua_kzu_light_kzuLightNodeCreateSpot00);
  tolua_function(tolua_S,"kzuLightNodeInitialize",tolua_kzu_light_kzuLightNodeInitialize00);
  tolua_function(tolua_S,"kzuLightNodeFromObjectNode",tolua_kzu_light_kzuLightNodeFromObjectNode00);
  tolua_function(tolua_S,"kzuLightNodeToObjectNode",tolua_kzu_light_kzuLightNodeToObjectNode00);
  tolua_function(tolua_S,"kzuLightNodeGetLightPropertyType",tolua_kzu_light_kzuLightNodeGetLightPropertyType00);
  tolua_function(tolua_S,"kzuLightNodeIsEnabled",tolua_kzu_light_kzuLightNodeIsEnabled00);
  tolua_function(tolua_S,"kzuLightNodeSetShadowCasterEnabled",tolua_kzu_light_kzuLightNodeSetShadowCasterEnabled00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_light (lua_State* tolua_S) {
 return tolua_kzu_light_open(tolua_S);
}
#endif

