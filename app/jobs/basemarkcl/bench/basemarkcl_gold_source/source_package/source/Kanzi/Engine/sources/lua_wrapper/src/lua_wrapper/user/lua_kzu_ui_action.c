/*
** Lua binding: kzu_ui_action
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_ui_action_open (lua_State* tolua_S);

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
#include <user/ui/kzu_ui_action.h>
#include <user/ui/kzu_ui_event.h>
#include <core/util/collection/kzc_hash_map.h>
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
 tolua_usertype(tolua_S,"KzcHashMap");
 tolua_usertype(tolua_S,"KzuUiComponentNode");
 tolua_usertype(tolua_S,"KzuUiAction");
 tolua_usertype(tolua_S,"KzuUiActionCallback");
 tolua_usertype(tolua_S,"KzuUiEvent");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuUiActionCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_action_kzuUiActionCreate00
static int tolua_kzu_ui_action_kzuUiActionCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuUiActionCallback",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"kzBool",0,&tolua_err)) ||
     !tolua_isuserdata(tolua_S,4,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuUiAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  KzuUiActionCallback actionCallback = *((KzuUiActionCallback*)  tolua_tousertype(tolua_S,2,0));
  kzBool requiresMessageQueue = *((kzBool*)  tolua_tousertype(tolua_S,3,0));
  void* userData = ((void*)  tolua_touserdata(tolua_S,4,0));
  struct KzuUiAction* out_action = ((struct KzuUiAction*)  tolua_tousertype(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiActionCreate(memoryManager,actionCallback,requiresMessageQueue,userData,&out_action);
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
    tolua_pushusertype(tolua_S,(void*)out_action,"KzuUiAction");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiActionCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiActionDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_action_kzuUiActionDelete00
static int tolua_kzu_ui_action_kzuUiActionDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiAction* action = ((struct KzuUiAction*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiActionDelete(action);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiActionDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiActionCopy */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_action_kzuUiActionCopy00
static int tolua_kzu_ui_action_kzuUiActionCopy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiAction",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiAction* sourceAction = ((const struct KzuUiAction*)  tolua_tousertype(tolua_S,2,0));
  struct KzuUiAction* out_action = ((struct KzuUiAction*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiActionCopy(memoryManager,sourceAction,&out_action);
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
    tolua_pushusertype(tolua_S,(void*)out_action,"KzuUiAction");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiActionCopy'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiActionGetRequiresMessageQueue */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_action_kzuUiActionGetRequiresMessageQueue00
static int tolua_kzu_ui_action_kzuUiActionGetRequiresMessageQueue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiAction* action = ((const struct KzuUiAction*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiActionGetRequiresMessageQueue(action);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiActionGetRequiresMessageQueue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiActionCall */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_action_kzuUiActionCall00
static int tolua_kzu_ui_action_kzuUiActionCall00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiAction",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,6,"KzcHashMap",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiAction* action = ((const struct KzuUiAction*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiComponentNode* component = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  void* userData = ((void*)  tolua_touserdata(tolua_S,3,0));
  char* eventType = ((char*)  tolua_tostring(tolua_S,4,0));
  struct KzuUiEvent* event = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,5,0));
  struct KzcHashMap* customParameters = ((struct KzcHashMap*)  tolua_tousertype(tolua_S,6,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiActionCall(action,component,userData,eventType,event,customParameters);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiActionCall'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiActionGetUserData */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_action_kzuUiActionGetUserData00
static int tolua_kzu_ui_action_kzuUiActionGetUserData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiAction* action = ((const struct KzuUiAction*)  tolua_tousertype(tolua_S,1,0));
  {
   void* tolua_ret = (void*)  kzuUiActionGetUserData(action);
   tolua_pushuserdata(tolua_S,(void*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiActionGetUserData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_ui_action_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiComponentNode","KzuUiComponentNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiComponentNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEvent","KzuUiEvent","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEvent");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiAction","KzuUiAction","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiAction");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuUiActionCreate",tolua_kzu_ui_action_kzuUiActionCreate00);
  tolua_function(tolua_S,"kzuUiActionDelete",tolua_kzu_ui_action_kzuUiActionDelete00);
  tolua_function(tolua_S,"kzuUiActionCopy",tolua_kzu_ui_action_kzuUiActionCopy00);
  tolua_function(tolua_S,"kzuUiActionGetRequiresMessageQueue",tolua_kzu_ui_action_kzuUiActionGetRequiresMessageQueue00);
  tolua_function(tolua_S,"kzuUiActionCall",tolua_kzu_ui_action_kzuUiActionCall00);
  tolua_function(tolua_S,"kzuUiActionGetUserData",tolua_kzu_ui_action_kzuUiActionGetUserData00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_ui_action (lua_State* tolua_S) {
 return tolua_kzu_ui_action_open(tolua_S);
}
#endif

