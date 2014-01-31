/*
** Lua binding: kzu_engine_message
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_engine_message_open (lua_State* tolua_S);

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
#include <user/engine/kzu_engine_message.h>
#include <system/input/kzs_input.h>
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
 tolua_usertype(tolua_S,"KzuEngineMessage");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuEngineMessageCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_message_kzuEngineMessageCreate00
static int tolua_kzu_engine_message_kzuEngineMessageCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,4,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuEngineMessage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  enum KzuEngineMessageType type = ((enum KzuEngineMessageType) (int)  tolua_tonumber(tolua_S,2,0));
  unsigned int delayDuration = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  void* userData = ((void*)  tolua_touserdata(tolua_S,4,0));
  struct KzuEngineMessage* out_message = ((struct KzuEngineMessage*)  tolua_tousertype(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineMessageCreate(memoryManager,type,delayDuration,userData,&out_message);
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
    tolua_pushusertype(tolua_S,(void*)out_message,"KzuEngineMessage");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineMessageCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineMessageDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_message_kzuEngineMessageDelete00
static int tolua_kzu_engine_message_kzuEngineMessageDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngineMessage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngineMessage* message = ((struct KzuEngineMessage*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineMessageDelete(message);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineMessageDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineMessageGetType */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_message_kzuEngineMessageGetType00
static int tolua_kzu_engine_message_kzuEngineMessageGetType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngineMessage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngineMessage* message = ((const struct KzuEngineMessage*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuEngineMessageType tolua_ret = (enum KzuEngineMessageType)  kzuEngineMessageGetType(message);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineMessageGetType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineMessageAddArgument */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_message_kzuEngineMessageAddArgument00
static int tolua_kzu_engine_message_kzuEngineMessageAddArgument00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngineMessage",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngineMessage* message = ((const struct KzuEngineMessage*)  tolua_tousertype(tolua_S,1,0));
  char* argumentName = ((char*)  tolua_tostring(tolua_S,2,0));
  void* argument = ((void*)  tolua_touserdata(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineMessageAddArgument(message,argumentName,argument);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineMessageAddArgument'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineMessageGetArgument */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_message_kzuEngineMessageGetArgument00
static int tolua_kzu_engine_message_kzuEngineMessageGetArgument00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngineMessage",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngineMessage* message = ((const struct KzuEngineMessage*)  tolua_tousertype(tolua_S,1,0));
  char* argumentName = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   void* tolua_ret = (void*)  kzuEngineMessageGetArgument(message,argumentName);
   tolua_pushuserdata(tolua_S,(void*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineMessageGetArgument'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineMessageGetUserData */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_message_kzuEngineMessageGetUserData00
static int tolua_kzu_engine_message_kzuEngineMessageGetUserData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngineMessage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngineMessage* message = ((const struct KzuEngineMessage*)  tolua_tousertype(tolua_S,1,0));
  {
   void* tolua_ret = (void*)  kzuEngineMessageGetUserData(message);
   tolua_pushuserdata(tolua_S,(void*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineMessageGetUserData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineMessageUpdate */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_message_kzuEngineMessageUpdate00
static int tolua_kzu_engine_message_kzuEngineMessageUpdate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngineMessage",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngineMessage* message = ((struct KzuEngineMessage*)  tolua_tousertype(tolua_S,1,0));
  unsigned int deltaTime = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuEngineMessageUpdate(message,deltaTime);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineMessageUpdate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_engine_message_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_TYPE_CHANGE_SCENE",KZU_ENGINE_MESSAGE_TYPE_CHANGE_SCENE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_START_ANIMATION",KZU_ENGINE_MESSAGE_START_ANIMATION);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_SET_PROPERTY_VALUE",KZU_ENGINE_MESSAGE_SET_PROPERTY_VALUE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_SWITCH_BOOLEAN_PROPERTY",KZU_ENGINE_MESSAGE_SWITCH_BOOLEAN_PROPERTY);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_SET_BOOLEAN_PROPERTY",KZU_ENGINE_MESSAGE_SET_BOOLEAN_PROPERTY);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_START_TRANSITION",KZU_ENGINE_MESSAGE_START_TRANSITION);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_SET_SLIDER_VALUE",KZU_ENGINE_MESSAGE_SET_SLIDER_VALUE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_INCREASE_SLIDER_VALUE",KZU_ENGINE_MESSAGE_INCREASE_SLIDER_VALUE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_DECREASE_SLIDER_VALUE",KZU_ENGINE_MESSAGE_DECREASE_SLIDER_VALUE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_CHANGE_RENDER_PASS_CAMERA",KZU_ENGINE_MESSAGE_CHANGE_RENDER_PASS_CAMERA);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_CHANGE_2D_PICKING_CAMERA",KZU_ENGINE_MESSAGE_CHANGE_2D_PICKING_CAMERA);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_SET_TRANSFORMATION_TO_IDENTITY",KZU_ENGINE_MESSAGE_SET_TRANSFORMATION_TO_IDENTITY);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_EXECUTE_SCRIPT",KZU_ENGINE_MESSAGE_EXECUTE_SCRIPT);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_CHANGE_SCENE_CAMERA",KZU_ENGINE_MESSAGE_CHANGE_SCENE_CAMERA);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_SWITCH_TOGGLE_BUTTON_STATE",KZU_ENGINE_MESSAGE_SWITCH_TOGGLE_BUTTON_STATE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_DROPDOWN_OPEN",KZU_ENGINE_MESSAGE_DROPDOWN_OPEN);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_DROPDOWN_CLOSE",KZU_ENGINE_MESSAGE_DROPDOWN_CLOSE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_DROPDOWN_SWITCH_OPEN_STATE",KZU_ENGINE_MESSAGE_DROPDOWN_SWITCH_OPEN_STATE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_DROPDOWN_SET_SELECTED_ITEM",KZU_ENGINE_MESSAGE_DROPDOWN_SET_SELECTED_ITEM);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_RENDER_PASS_SET_ENABLED_STATE",KZU_ENGINE_MESSAGE_RENDER_PASS_SET_ENABLED_STATE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_RENDER_PASS_TOGGLE_ENABLED_STATE",KZU_ENGINE_MESSAGE_RENDER_PASS_TOGGLE_ENABLED_STATE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_RENDER_PASS_RENDER_FOR_ONE_FRAME_AND_DISABLE",KZU_ENGINE_MESSAGE_RENDER_PASS_RENDER_FOR_ONE_FRAME_AND_DISABLE);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_CHANGE_SCENE_COMPOSER",KZU_ENGINE_MESSAGE_CHANGE_SCENE_COMPOSER);
  tolua_constant(tolua_S,"KZU_ENGINE_MESSAGE_SET_TEXTURE_PROPERTY",KZU_ENGINE_MESSAGE_SET_TEXTURE_PROPERTY);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuEngineMessage","KzuEngineMessage","",NULL);
  tolua_beginmodule(tolua_S,"KzuEngineMessage");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuEngineMessageCreate",tolua_kzu_engine_message_kzuEngineMessageCreate00);
  tolua_function(tolua_S,"kzuEngineMessageDelete",tolua_kzu_engine_message_kzuEngineMessageDelete00);
  tolua_function(tolua_S,"kzuEngineMessageGetType",tolua_kzu_engine_message_kzuEngineMessageGetType00);
  tolua_function(tolua_S,"kzuEngineMessageAddArgument",tolua_kzu_engine_message_kzuEngineMessageAddArgument00);
  tolua_function(tolua_S,"kzuEngineMessageGetArgument",tolua_kzu_engine_message_kzuEngineMessageGetArgument00);
  tolua_function(tolua_S,"kzuEngineMessageGetUserData",tolua_kzu_engine_message_kzuEngineMessageGetUserData00);
  tolua_function(tolua_S,"kzuEngineMessageUpdate",tolua_kzu_engine_message_kzuEngineMessageUpdate00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_engine_message (lua_State* tolua_S) {
 return tolua_kzu_engine_message_open(tolua_S);
}
#endif

