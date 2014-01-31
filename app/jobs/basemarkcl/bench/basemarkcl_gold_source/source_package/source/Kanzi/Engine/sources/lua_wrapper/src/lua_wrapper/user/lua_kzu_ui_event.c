/*
** Lua binding: kzu_ui_event
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_ui_event_open (lua_State* tolua_S);

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
#include <user/ui/kzu_ui_event.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_ray.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzBool (lua_State* tolua_S)
{
 kzBool* self = (kzBool*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcRay (lua_State* tolua_S)
{
 KzcRay* self = (KzcRay*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcVector3 (lua_State* tolua_S)
{
 KzcVector3* self = (KzcVector3*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuPropertyCollection");
 tolua_usertype(tolua_S,"KzcVector3");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzsInputKey");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"KzcRay");
 tolua_usertype(tolua_S,"KzuUiEvent");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuUiEventCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventCreate00
static int tolua_kzu_ui_event_kzuUiEventCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiEvent* out_uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventCreate(memoryManager,&out_uiEvent);
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
    tolua_pushusertype(tolua_S,(void*)out_uiEvent,"KzuUiEvent");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventDelete00
static int tolua_kzu_ui_event_kzuUiEventDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventDelete(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventResetEvent */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventResetEvent00
static int tolua_kzu_ui_event_kzuUiEventResetEvent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventResetEvent(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventResetEvent'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventResetFrame */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventResetFrame00
static int tolua_kzu_ui_event_kzuUiEventResetFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventResetFrame(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventResetFrame'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsPointerPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsPointerPressed00
static int tolua_kzu_ui_event_kzuUiEventIsPointerPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsPointerPressed(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsPointerPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsPointerReleased */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsPointerReleased00
static int tolua_kzu_ui_event_kzuUiEventIsPointerReleased00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsPointerReleased(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsPointerReleased'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsPointerMoved */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsPointerMoved00
static int tolua_kzu_ui_event_kzuUiEventIsPointerMoved00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsPointerMoved(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsPointerMoved'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsPointerDragged */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsPointerDragged00
static int tolua_kzu_ui_event_kzuUiEventIsPointerDragged00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsPointerDragged(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsPointerDragged'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetPointerX */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetPointerX00
static int tolua_kzu_ui_event_kzuUiEventGetPointerX00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   int tolua_ret = (int)  kzuUiEventGetPointerX(uiEvent);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetPointerX'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetPointerY */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetPointerY00
static int tolua_kzu_ui_event_kzuUiEventGetPointerY00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   int tolua_ret = (int)  kzuUiEventGetPointerY(uiEvent);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetPointerY'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetPointerDragStartX */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetPointerDragStartX00
static int tolua_kzu_ui_event_kzuUiEventGetPointerDragStartX00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   int tolua_ret = (int)  kzuUiEventGetPointerDragStartX(uiEvent);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetPointerDragStartX'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetPointerDragStartY */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetPointerDragStartY00
static int tolua_kzu_ui_event_kzuUiEventGetPointerDragStartY00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   int tolua_ret = (int)  kzuUiEventGetPointerDragStartY(uiEvent);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetPointerDragStartY'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventPointerPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventPointerPressed00
static int tolua_kzu_ui_event_kzuUiEventPointerPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  enum KzuUiEventButton button = ((enum KzuUiEventButton) (int)  tolua_tonumber(tolua_S,2,0));
  int x = ((int)  tolua_tonumber(tolua_S,3,0));
  int y = ((int)  tolua_tonumber(tolua_S,4,0));
  {
   kzuUiEventPointerPressed(uiEvent,button,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventPointerPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventPointerReleased */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventPointerReleased00
static int tolua_kzu_ui_event_kzuUiEventPointerReleased00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  enum KzuUiEventButton button = ((enum KzuUiEventButton) (int)  tolua_tonumber(tolua_S,2,0));
  int x = ((int)  tolua_tonumber(tolua_S,3,0));
  int y = ((int)  tolua_tonumber(tolua_S,4,0));
  {
   kzuUiEventPointerReleased(uiEvent,button,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventPointerReleased'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventPointerMoved */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventPointerMoved00
static int tolua_kzu_ui_event_kzuUiEventPointerMoved00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   kzuUiEventPointerMoved(uiEvent,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventPointerMoved'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventPointerDragged */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventPointerDragged00
static int tolua_kzu_ui_event_kzuUiEventPointerDragged00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  enum KzuUiEventButton button = ((enum KzuUiEventButton) (int)  tolua_tonumber(tolua_S,2,0));
  int x = ((int)  tolua_tonumber(tolua_S,3,0));
  int y = ((int)  tolua_tonumber(tolua_S,4,0));
  {
   kzuUiEventPointerDragged(uiEvent,button,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventPointerDragged'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventKeyPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventKeyPressed00
static int tolua_kzu_ui_event_kzuUiEventKeyPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzsInputKey",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  enum KzsInputKey key = *((enum KzsInputKey*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventKeyPressed(uiEvent,key);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventKeyPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventKeyReleased */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventKeyReleased00
static int tolua_kzu_ui_event_kzuUiEventKeyReleased00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzsInputKey",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  enum KzsInputKey key = *((enum KzsInputKey*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventKeyReleased(uiEvent,key);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventKeyReleased'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventResetLeftAndRight_private */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventResetLeftAndRight_private00
static int tolua_kzu_ui_event_kzuUiEventResetLeftAndRight_private00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuUiEventResetLeftAndRight_private(uiEvent);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventResetLeftAndRight_private'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsLeftPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsLeftPressed00
static int tolua_kzu_ui_event_kzuUiEventIsLeftPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsLeftPressed(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsLeftPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsRightPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsRightPressed00
static int tolua_kzu_ui_event_kzuUiEventIsRightPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsRightPressed(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsRightPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsTabPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsTabPressed00
static int tolua_kzu_ui_event_kzuUiEventIsTabPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsTabPressed(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsTabPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsActionPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsActionPressed00
static int tolua_kzu_ui_event_kzuUiEventIsActionPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsActionPressed(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsActionPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventResetActionPressed_private */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventResetActionPressed_private00
static int tolua_kzu_ui_event_kzuUiEventResetActionPressed_private00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuUiEventResetActionPressed_private(uiEvent);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventResetActionPressed_private'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventIsShiftPressed */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventIsShiftPressed00
static int tolua_kzu_ui_event_kzuUiEventIsShiftPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiEventIsShiftPressed(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventIsShiftPressed'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetPickedObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetPickedObjectNode00
static int tolua_kzu_ui_event_kzuUiEventGetPickedObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuUiEventGetPickedObjectNode(uiEvent);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetPickedObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventSetPickedObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventSetPickedObjectNode00
static int tolua_kzu_ui_event_kzuUiEventSetPickedObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  struct KzuObjectNode* node = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventSetPickedObjectNode(uiEvent,node);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventSetPickedObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventSetPickedObjectNodeOffset */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventSetPickedObjectNodeOffset00
static int tolua_kzu_ui_event_kzuUiEventSetPickedObjectNodeOffset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcVector3* offset = ((const struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventSetPickedObjectNodeOffset(uiEvent,offset);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventSetPickedObjectNodeOffset'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetPickedObjectNodeOffset */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetPickedObjectNodeOffset00
static int tolua_kzu_ui_event_kzuUiEventGetPickedObjectNodeOffset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuUiEventGetPickedObjectNodeOffset(uiEvent);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetPickedObjectNodeOffset'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetDragStartPickedObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetDragStartPickedObjectNode00
static int tolua_kzu_ui_event_kzuUiEventGetDragStartPickedObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuUiEventGetDragStartPickedObjectNode(uiEvent);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetDragStartPickedObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventSetStoredPickingRay */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventSetStoredPickingRay00
static int tolua_kzu_ui_event_kzuUiEventSetStoredPickingRay00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcRay",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEvent* uiEvent = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcRay* ray = ((const struct KzcRay*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventSetStoredPickingRay(uiEvent,ray);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventSetStoredPickingRay'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetStoredPickingRay */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetStoredPickingRay00
static int tolua_kzu_ui_event_kzuUiEventGetStoredPickingRay00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcRay tolua_ret =   kzuUiEventGetStoredPickingRay(uiEvent);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcRay)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcRay");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcRay));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcRay");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetStoredPickingRay'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventGetEventPropertyCollection */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventGetEventPropertyCollection00
static int tolua_kzu_ui_event_kzuUiEventGetEventPropertyCollection00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyCollection* tolua_ret = (struct KzuPropertyCollection*)  kzuUiEventGetEventPropertyCollection(uiEvent);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyCollection");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventGetEventPropertyCollection'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventAddProperty */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_kzuUiEventAddProperty00
static int tolua_kzu_ui_event_kzuUiEventAddProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEvent* uiEvent = ((const struct KzuUiEvent*)  tolua_tousertype(tolua_S,1,0));
  struct KzuProperty* property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventAddProperty(uiEvent,property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventAddProperty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_ui_event_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"KZU_UI_EVENT_BUTTON_1",KZU_UI_EVENT_BUTTON_1);
  tolua_constant(tolua_S,"KZU_UI_EVENT_BUTTON_2",KZU_UI_EVENT_BUTTON_2);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"KzcRay","KzcRay","",tolua_collect_KzcRay);
  #else
  tolua_cclass(tolua_S,"KzcRay","KzcRay","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"KzcRay");
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"KzcVector3","KzcVector3","",tolua_collect_KzcVector3);
  #else
  tolua_cclass(tolua_S,"KzcVector3","KzcVector3","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"KzcVector3");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyCollection","KzuPropertyCollection","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyCollection");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEvent","KzuUiEvent","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEvent");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuUiEventCreate",tolua_kzu_ui_event_kzuUiEventCreate00);
  tolua_function(tolua_S,"kzuUiEventDelete",tolua_kzu_ui_event_kzuUiEventDelete00);
  tolua_function(tolua_S,"kzuUiEventResetEvent",tolua_kzu_ui_event_kzuUiEventResetEvent00);
  tolua_function(tolua_S,"kzuUiEventResetFrame",tolua_kzu_ui_event_kzuUiEventResetFrame00);
  tolua_function(tolua_S,"kzuUiEventIsPointerPressed",tolua_kzu_ui_event_kzuUiEventIsPointerPressed00);
  tolua_function(tolua_S,"kzuUiEventIsPointerReleased",tolua_kzu_ui_event_kzuUiEventIsPointerReleased00);
  tolua_function(tolua_S,"kzuUiEventIsPointerMoved",tolua_kzu_ui_event_kzuUiEventIsPointerMoved00);
  tolua_function(tolua_S,"kzuUiEventIsPointerDragged",tolua_kzu_ui_event_kzuUiEventIsPointerDragged00);
  tolua_function(tolua_S,"kzuUiEventGetPointerX",tolua_kzu_ui_event_kzuUiEventGetPointerX00);
  tolua_function(tolua_S,"kzuUiEventGetPointerY",tolua_kzu_ui_event_kzuUiEventGetPointerY00);
  tolua_function(tolua_S,"kzuUiEventGetPointerDragStartX",tolua_kzu_ui_event_kzuUiEventGetPointerDragStartX00);
  tolua_function(tolua_S,"kzuUiEventGetPointerDragStartY",tolua_kzu_ui_event_kzuUiEventGetPointerDragStartY00);
  tolua_function(tolua_S,"kzuUiEventPointerPressed",tolua_kzu_ui_event_kzuUiEventPointerPressed00);
  tolua_function(tolua_S,"kzuUiEventPointerReleased",tolua_kzu_ui_event_kzuUiEventPointerReleased00);
  tolua_function(tolua_S,"kzuUiEventPointerMoved",tolua_kzu_ui_event_kzuUiEventPointerMoved00);
  tolua_function(tolua_S,"kzuUiEventPointerDragged",tolua_kzu_ui_event_kzuUiEventPointerDragged00);
  tolua_function(tolua_S,"kzuUiEventKeyPressed",tolua_kzu_ui_event_kzuUiEventKeyPressed00);
  tolua_function(tolua_S,"kzuUiEventKeyReleased",tolua_kzu_ui_event_kzuUiEventKeyReleased00);
  tolua_function(tolua_S,"kzuUiEventResetLeftAndRight_private",tolua_kzu_ui_event_kzuUiEventResetLeftAndRight_private00);
  tolua_function(tolua_S,"kzuUiEventIsLeftPressed",tolua_kzu_ui_event_kzuUiEventIsLeftPressed00);
  tolua_function(tolua_S,"kzuUiEventIsRightPressed",tolua_kzu_ui_event_kzuUiEventIsRightPressed00);
  tolua_function(tolua_S,"kzuUiEventIsTabPressed",tolua_kzu_ui_event_kzuUiEventIsTabPressed00);
  tolua_function(tolua_S,"kzuUiEventIsActionPressed",tolua_kzu_ui_event_kzuUiEventIsActionPressed00);
  tolua_function(tolua_S,"kzuUiEventResetActionPressed_private",tolua_kzu_ui_event_kzuUiEventResetActionPressed_private00);
  tolua_function(tolua_S,"kzuUiEventIsShiftPressed",tolua_kzu_ui_event_kzuUiEventIsShiftPressed00);
  tolua_function(tolua_S,"kzuUiEventGetPickedObjectNode",tolua_kzu_ui_event_kzuUiEventGetPickedObjectNode00);
  tolua_function(tolua_S,"kzuUiEventSetPickedObjectNode",tolua_kzu_ui_event_kzuUiEventSetPickedObjectNode00);
  tolua_function(tolua_S,"kzuUiEventSetPickedObjectNodeOffset",tolua_kzu_ui_event_kzuUiEventSetPickedObjectNodeOffset00);
  tolua_function(tolua_S,"kzuUiEventGetPickedObjectNodeOffset",tolua_kzu_ui_event_kzuUiEventGetPickedObjectNodeOffset00);
  tolua_function(tolua_S,"kzuUiEventGetDragStartPickedObjectNode",tolua_kzu_ui_event_kzuUiEventGetDragStartPickedObjectNode00);
  tolua_function(tolua_S,"kzuUiEventSetStoredPickingRay",tolua_kzu_ui_event_kzuUiEventSetStoredPickingRay00);
  tolua_function(tolua_S,"kzuUiEventGetStoredPickingRay",tolua_kzu_ui_event_kzuUiEventGetStoredPickingRay00);
  tolua_function(tolua_S,"kzuUiEventGetEventPropertyCollection",tolua_kzu_ui_event_kzuUiEventGetEventPropertyCollection00);
  tolua_function(tolua_S,"kzuUiEventAddProperty",tolua_kzu_ui_event_kzuUiEventAddProperty00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_ui_event (lua_State* tolua_S) {
 return tolua_kzu_ui_event_open(tolua_S);
}
#endif

