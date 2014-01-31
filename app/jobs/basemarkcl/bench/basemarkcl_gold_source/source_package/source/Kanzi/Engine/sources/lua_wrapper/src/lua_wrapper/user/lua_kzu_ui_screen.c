/*
** Lua binding: kzu_ui_screen
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_ui_screen_open (lua_State* tolua_S);

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
#include <user/ui/kzu_ui_screen.h>
#include <user/ui/kzu_ui_screen.h>
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
 tolua_usertype(tolua_S,"KzuTransformedScene");
 tolua_usertype(tolua_S,"KzuProject");
 tolua_usertype(tolua_S,"KzuRenderer");
 tolua_usertype(tolua_S,"KzsInputKey");
 tolua_usertype(tolua_S,"KzuUiComponentNode");
 tolua_usertype(tolua_S,"KzuUiScreen");
 tolua_usertype(tolua_S,"KzuCameraNode");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"KzuObjectSource");
 tolua_usertype(tolua_S,"KzuEngineMessageQueue");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzsInputPointingDeviceState");
 tolua_usertype(tolua_S,"kzU32");
 tolua_usertype(tolua_S,"KzsWindow");
 tolua_usertype(tolua_S,"KzuScene");
 tolua_usertype(tolua_S,"KzuUiEvent");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzsInputKeyDeviceState");
}

/* function: kzuUiScreenCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenCreate00
static int tolua_kzu_ui_screen_kzuUiScreenCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiScreen* out_screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiScreenCreate(memoryManager,&out_screen);
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
    tolua_pushusertype(tolua_S,(void*)out_screen,"KzuUiScreen");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenDelete00
static int tolua_kzu_ui_screen_kzuUiScreenDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiScreenDelete(screen);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenSetObjectSource */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenSetObjectSource00
static int tolua_kzu_ui_screen_kzuUiScreenSetObjectSource00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectSource",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  struct KzuObjectSource* objectSource = ((struct KzuObjectSource*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiScreenSetObjectSource(screen,objectSource);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenSetObjectSource'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenSetPickingCamera */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenSetPickingCamera00
static int tolua_kzu_ui_screen_kzuUiScreenSetPickingCamera00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  struct KzuCameraNode* pickingCamera = ((struct KzuCameraNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiScreenSetPickingCamera(screen,pickingCamera);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenSetPickingCamera'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenSet2DPickingCamera */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenSet2DPickingCamera00
static int tolua_kzu_ui_screen_kzuUiScreenSet2DPickingCamera00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  struct KzuCameraNode* pickingCamera = ((struct KzuCameraNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiScreenSet2DPickingCamera(screen,pickingCamera);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenSet2DPickingCamera'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenEnablePicking */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenEnablePicking00
static int tolua_kzu_ui_screen_kzuUiScreenEnablePicking00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  kzBool value = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiScreenEnablePicking(screen,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenEnablePicking'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenGetPickingEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenGetPickingEnabled00
static int tolua_kzu_ui_screen_kzuUiScreenGetPickingEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiScreen* screen = ((const struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiScreenGetPickingEnabled(screen);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenGetPickingEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenHandleKeyEvents */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenHandleKeyEvents00
static int tolua_kzu_ui_screen_kzuUiScreenHandleKeyEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzsInputKey",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzsInputKeyDeviceState",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiScreen* screen = ((const struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  enum KzsInputKey keyCode = *((enum KzsInputKey*)  tolua_tousertype(tolua_S,2,0));
  enum KzsInputKeyDeviceState keyState = *((enum KzsInputKeyDeviceState*)  tolua_tousertype(tolua_S,3,0));
  {
   kzuUiScreenHandleKeyEvents(screen,keyCode,keyState);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenHandleKeyEvents'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenHandlePointerEvents */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenHandlePointerEvents00
static int tolua_kzu_ui_screen_kzuUiScreenHandlePointerEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzU32",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzsInputPointingDeviceState",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiScreen* screen = ((const struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  kzU32 buttons = *((kzU32*)  tolua_tousertype(tolua_S,2,0));
  enum KzsInputPointingDeviceState state = *((enum KzsInputPointingDeviceState*)  tolua_tousertype(tolua_S,3,0));
  int mouseX = ((int)  tolua_tonumber(tolua_S,4,0));
  int mouseY = ((int)  tolua_tonumber(tolua_S,5,0));
  {
   kzuUiScreenHandlePointerEvents(screen,buttons,state,mouseX,mouseY);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenHandlePointerEvents'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenHandleEvents */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenHandleEvents00
static int tolua_kzu_ui_screen_kzuUiScreenHandleEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuRenderer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzsWindow",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,6,"KzuEngineMessageQueue",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* root = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  const struct KzuTransformedScene* transformedScene = ((const struct KzuTransformedScene*)  tolua_tousertype(tolua_S,3,0));
  const struct KzuRenderer* renderer = ((const struct KzuRenderer*)  tolua_tousertype(tolua_S,4,0));
  const struct KzsWindow* window = ((const struct KzsWindow*)  tolua_tousertype(tolua_S,5,0));
  struct KzuEngineMessageQueue* queue = ((struct KzuEngineMessageQueue*)  tolua_tousertype(tolua_S,6,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiScreenHandleEvents(screen,root,transformedScene,renderer,window,queue);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenHandleEvents'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenLogicUpdate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenLogicUpdate00
static int tolua_kzu_ui_screen_kzuUiScreenLogicUpdate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuEngineMessageQueue",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiScreen* screen = ((const struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* root = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  float deltaTime = ((float)  tolua_tonumber(tolua_S,3,0));
  struct KzuEngineMessageQueue* queue = ((struct KzuEngineMessageQueue*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiScreenLogicUpdate(screen,root,deltaTime,queue);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenLogicUpdate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenSetFocusedControl */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenSetFocusedControl00
static int tolua_kzu_ui_screen_kzuUiScreenSetFocusedControl00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentNode* componentNode = ((struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiScreenSetFocusedControl(screen,componentNode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenSetFocusedControl'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenGetFocusedControl */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenGetFocusedControl00
static int tolua_kzu_ui_screen_kzuUiScreenGetFocusedControl00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiScreen* screen = ((const struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuUiComponentNode* tolua_ret = (struct KzuUiComponentNode*)  kzuUiScreenGetFocusedControl(screen);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiComponentNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenGetFocusedControl'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenFocusNextControl */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenFocusNextControl00
static int tolua_kzu_ui_screen_kzuUiScreenFocusNextControl00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiScreenFocusNextControl(screen);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenFocusNextControl'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenFocusPreviousControl */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenFocusPreviousControl00
static int tolua_kzu_ui_screen_kzuUiScreenFocusPreviousControl00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiScreen* screen = ((struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiScreenFocusPreviousControl(screen);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenFocusPreviousControl'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenReset */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenReset00
static int tolua_kzu_ui_screen_kzuUiScreenReset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiScreen* screen = ((const struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiScreenReset(screen);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenReset'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiScreenGetEvent */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_screen_kzuUiScreenGetEvent00
static int tolua_kzu_ui_screen_kzuUiScreenGetEvent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiScreen",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiScreen* screen = ((const struct KzuUiScreen*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuUiEvent* tolua_ret = (struct KzuUiEvent*)  kzuUiScreenGetEvent(screen);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiEvent");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiScreenGetEvent'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_ui_screen_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuUiEvent","KzuUiEvent","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEvent");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiComponentNode","KzuUiComponentNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiComponentNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuRenderer","KzuRenderer","",NULL);
  tolua_beginmodule(tolua_S,"KzuRenderer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuCameraNode","KzuCameraNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuCameraNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProject","KzuProject","",NULL);
  tolua_beginmodule(tolua_S,"KzuProject");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuScene","KzuScene","",NULL);
  tolua_beginmodule(tolua_S,"KzuScene");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTransformedScene","KzuTransformedScene","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedScene");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectSource","KzuObjectSource","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectSource");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuEngineMessageQueue","KzuEngineMessageQueue","",NULL);
  tolua_beginmodule(tolua_S,"KzuEngineMessageQueue");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzsWindow","KzsWindow","",NULL);
  tolua_beginmodule(tolua_S,"KzsWindow");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiScreen","KzuUiScreen","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiScreen");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuUiScreenCreate",tolua_kzu_ui_screen_kzuUiScreenCreate00);
  tolua_function(tolua_S,"kzuUiScreenDelete",tolua_kzu_ui_screen_kzuUiScreenDelete00);
  tolua_function(tolua_S,"kzuUiScreenSetObjectSource",tolua_kzu_ui_screen_kzuUiScreenSetObjectSource00);
  tolua_function(tolua_S,"kzuUiScreenSetPickingCamera",tolua_kzu_ui_screen_kzuUiScreenSetPickingCamera00);
  tolua_function(tolua_S,"kzuUiScreenSet2DPickingCamera",tolua_kzu_ui_screen_kzuUiScreenSet2DPickingCamera00);
  tolua_function(tolua_S,"kzuUiScreenEnablePicking",tolua_kzu_ui_screen_kzuUiScreenEnablePicking00);
  tolua_function(tolua_S,"kzuUiScreenGetPickingEnabled",tolua_kzu_ui_screen_kzuUiScreenGetPickingEnabled00);
  tolua_function(tolua_S,"kzuUiScreenHandleKeyEvents",tolua_kzu_ui_screen_kzuUiScreenHandleKeyEvents00);
  tolua_function(tolua_S,"kzuUiScreenHandlePointerEvents",tolua_kzu_ui_screen_kzuUiScreenHandlePointerEvents00);
  tolua_function(tolua_S,"kzuUiScreenHandleEvents",tolua_kzu_ui_screen_kzuUiScreenHandleEvents00);
  tolua_function(tolua_S,"kzuUiScreenLogicUpdate",tolua_kzu_ui_screen_kzuUiScreenLogicUpdate00);
  tolua_function(tolua_S,"kzuUiScreenSetFocusedControl",tolua_kzu_ui_screen_kzuUiScreenSetFocusedControl00);
  tolua_function(tolua_S,"kzuUiScreenGetFocusedControl",tolua_kzu_ui_screen_kzuUiScreenGetFocusedControl00);
  tolua_function(tolua_S,"kzuUiScreenFocusNextControl",tolua_kzu_ui_screen_kzuUiScreenFocusNextControl00);
  tolua_function(tolua_S,"kzuUiScreenFocusPreviousControl",tolua_kzu_ui_screen_kzuUiScreenFocusPreviousControl00);
  tolua_function(tolua_S,"kzuUiScreenReset",tolua_kzu_ui_screen_kzuUiScreenReset00);
  tolua_function(tolua_S,"kzuUiScreenGetEvent",tolua_kzu_ui_screen_kzuUiScreenGetEvent00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_ui_screen (lua_State* tolua_S) {
 return tolua_kzu_ui_screen_open(tolua_S);
}
#endif

