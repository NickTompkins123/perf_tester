/*
** Lua binding: kzu_ui_event_listener
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_ui_event_listener_open (lua_State* tolua_S);

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
#include <user/ui/kzu_ui_event_listener.h>
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
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuUiEventListener");
 tolua_usertype(tolua_S,"KzuEngineMessageQueue");
 tolua_usertype(tolua_S,"KzuUiEventListenerArgument");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzuUiComponentNode");
 tolua_usertype(tolua_S,"KzuUiAction");
 tolua_usertype(tolua_S,"KzuUiEvent");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuUiEventListenerCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerCreate00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzuUiEventListener* out_eventListener = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerCreate(memoryManager,name,&out_eventListener);
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
    tolua_pushusertype(tolua_S,(void*)out_eventListener,"KzuUiEventListener");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerDelete00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListener* eventListener = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerDelete(eventListener);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerCopy */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerCopy00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerCopy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiEventListener* sourceEventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,2,0));
  struct KzuUiEventListener* out_eventListenerCopy = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerCopy(memoryManager,sourceEventListener,&out_eventListenerCopy);
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
    tolua_pushusertype(tolua_S,(void*)out_eventListenerCopy,"KzuUiEventListener");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerCopy'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerAddArgument */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerAddArgument00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerAddArgument00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEventListener* eventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiEventListenerArgument* argument = ((struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerAddArgument(eventListener,argument);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerAddArgument'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerSetTargetComponent */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerSetTargetComponent00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerSetTargetComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListener* eventListener = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentNode* targetComponentNode = ((struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventListenerSetTargetComponent(eventListener,targetComponentNode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerSetTargetComponent'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerSetTargetComponentPath */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerSetTargetComponentPath00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerSetTargetComponentPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListener* eventListener = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  char* path = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerSetTargetComponentPath(eventListener,path);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerSetTargetComponentPath'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerGetTargetComponentPath */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerGetTargetComponentPath00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerGetTargetComponentPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEventListener* eventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  {
   char* tolua_ret = (char*)  kzuUiEventListenerGetTargetComponentPath(eventListener);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerGetTargetComponentPath'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerSetAction */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerSetAction00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerSetAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListener* eventListener = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiAction* action = ((struct KzuUiAction*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventListenerSetAction(eventListener,action);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerSetAction'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerGetAction */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerGetAction00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerGetAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEventListener* eventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuUiAction* tolua_ret = (struct KzuUiAction*)  kzuUiEventListenerGetAction(eventListener);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiAction");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerGetAction'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerGetTargetLocation */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerGetTargetLocation00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerGetTargetLocation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEventListener* eventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuUiActionLocation tolua_ret = (enum KzuUiActionLocation)  kzuUiEventListenerGetTargetLocation(eventListener);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerGetTargetLocation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerSetDelay */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerSetDelay00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerSetDelay00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListener* eventListener = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  unsigned int delay = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   kzuUiEventListenerSetDelay(eventListener,delay);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerSetDelay'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerGetDelay */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerGetDelay00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerGetDelay00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEventListener* eventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuUiEventListenerGetDelay(eventListener);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerGetDelay'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerArgumentCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentCreate00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzuUiEventListenerArgument* out_argument = ((struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerArgumentCreate(memoryManager,name,&out_argument);
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
    tolua_pushusertype(tolua_S,(void*)out_argument,"KzuUiEventListenerArgument");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerArgumentCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerArgumentDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentDelete00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListenerArgument* argument = ((struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerArgumentDelete(argument);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerArgumentDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerArgumentCopy */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentCopy00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentCopy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiEventListenerArgument* sourceArgument = ((const struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,2,0));
  struct KzuUiEventListenerArgument* out_argument = ((struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerArgumentCopy(memoryManager,sourceArgument,&out_argument);
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
    tolua_pushusertype(tolua_S,(void*)out_argument,"KzuUiEventListenerArgument");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerArgumentCopy'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerArgumentGetName */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentGetName00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentGetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEventListenerArgument* argument = ((const struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,1,0));
  {
   char* tolua_ret = (char*)  kzuUiEventListenerArgumentGetName(argument);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerArgumentGetName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerArgumentSetFixedValue */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetFixedValue00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetFixedValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListenerArgument* argument = ((struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,1,0));
  struct KzuProperty* property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiEventListenerArgumentSetFixedValue(argument,property);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerArgumentSetFixedValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerArgumentSetForwardEventValue */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetForwardEventValue00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetForwardEventValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListenerArgument* argument = ((struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,1,0));
  char* argumentName = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerArgumentSetForwardEventValue(argument,argumentName);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerArgumentSetForwardEventValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerArgumentSetForwardValue */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetForwardValue00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetForwardValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListenerArgument",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiEventListenerArgument* argument = ((struct KzuUiEventListenerArgument*)  tolua_tousertype(tolua_S,1,0));
  char* argumentName = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerArgumentSetForwardValue(argument,argumentName);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerArgumentSetForwardValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiEventListenerCallCallback */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_event_listener_kzuUiEventListenerCallCallback00
static int tolua_kzu_ui_event_listener_kzuUiEventListenerCallCallback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuEngineMessageQueue",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiEventListener* eventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiComponentNode* component = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  struct KzuUiEvent* event = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,3,0));
  char* eventType = ((char*)  tolua_tostring(tolua_S,4,0));
  struct KzuEngineMessageQueue* queue = ((struct KzuEngineMessageQueue*)  tolua_tousertype(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiEventListenerCallCallback(eventListener,component,event,eventType,queue);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiEventListenerCallCallback'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_ui_event_listener_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"KZU_UI_ARGUMENT_TYPE_FIXED",KZU_UI_ARGUMENT_TYPE_FIXED);
  tolua_constant(tolua_S,"KZU_UI_ARGUMENT_TYPE_FORWARD_ARGUMENT",KZU_UI_ARGUMENT_TYPE_FORWARD_ARGUMENT);
  tolua_constant(tolua_S,"KZU_UI_ARGUMENT_TYPE_FORWARD_PROPERTY",KZU_UI_ARGUMENT_TYPE_FORWARD_PROPERTY);
  tolua_constant(tolua_S,"KZU_UI_ACTION_LOCATION_ENGINE",KZU_UI_ACTION_LOCATION_ENGINE);
  tolua_constant(tolua_S,"KZU_UI_ACTION_LOCATION_APPLICATION",KZU_UI_ACTION_LOCATION_APPLICATION);
  tolua_constant(tolua_S,"KZU_UI_ACTION_LOCATION_COMPONENT",KZU_UI_ACTION_LOCATION_COMPONENT);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEvent","KzuUiEvent","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEvent");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiAction","KzuUiAction","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiAction");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuEngineMessageQueue","KzuEngineMessageQueue","",NULL);
  tolua_beginmodule(tolua_S,"KzuEngineMessageQueue");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiComponentNode","KzuUiComponentNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiComponentNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEventListenerArgument","KzuUiEventListenerArgument","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEventListenerArgument");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEventListener","KzuUiEventListener","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEventListener");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuUiEventListenerCreate",tolua_kzu_ui_event_listener_kzuUiEventListenerCreate00);
  tolua_function(tolua_S,"kzuUiEventListenerDelete",tolua_kzu_ui_event_listener_kzuUiEventListenerDelete00);
  tolua_function(tolua_S,"kzuUiEventListenerCopy",tolua_kzu_ui_event_listener_kzuUiEventListenerCopy00);
  tolua_function(tolua_S,"kzuUiEventListenerAddArgument",tolua_kzu_ui_event_listener_kzuUiEventListenerAddArgument00);
  tolua_function(tolua_S,"kzuUiEventListenerSetTargetComponent",tolua_kzu_ui_event_listener_kzuUiEventListenerSetTargetComponent00);
  tolua_function(tolua_S,"kzuUiEventListenerSetTargetComponentPath",tolua_kzu_ui_event_listener_kzuUiEventListenerSetTargetComponentPath00);
  tolua_function(tolua_S,"kzuUiEventListenerGetTargetComponentPath",tolua_kzu_ui_event_listener_kzuUiEventListenerGetTargetComponentPath00);
  tolua_function(tolua_S,"kzuUiEventListenerSetAction",tolua_kzu_ui_event_listener_kzuUiEventListenerSetAction00);
  tolua_function(tolua_S,"kzuUiEventListenerGetAction",tolua_kzu_ui_event_listener_kzuUiEventListenerGetAction00);
  tolua_function(tolua_S,"kzuUiEventListenerGetTargetLocation",tolua_kzu_ui_event_listener_kzuUiEventListenerGetTargetLocation00);
  tolua_function(tolua_S,"kzuUiEventListenerSetDelay",tolua_kzu_ui_event_listener_kzuUiEventListenerSetDelay00);
  tolua_function(tolua_S,"kzuUiEventListenerGetDelay",tolua_kzu_ui_event_listener_kzuUiEventListenerGetDelay00);
  tolua_function(tolua_S,"kzuUiEventListenerArgumentCreate",tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentCreate00);
  tolua_function(tolua_S,"kzuUiEventListenerArgumentDelete",tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentDelete00);
  tolua_function(tolua_S,"kzuUiEventListenerArgumentCopy",tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentCopy00);
  tolua_function(tolua_S,"kzuUiEventListenerArgumentGetName",tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentGetName00);
  tolua_function(tolua_S,"kzuUiEventListenerArgumentSetFixedValue",tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetFixedValue00);
  tolua_function(tolua_S,"kzuUiEventListenerArgumentSetForwardEventValue",tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetForwardEventValue00);
  tolua_function(tolua_S,"kzuUiEventListenerArgumentSetForwardValue",tolua_kzu_ui_event_listener_kzuUiEventListenerArgumentSetForwardValue00);
  tolua_function(tolua_S,"kzuUiEventListenerCallCallback",tolua_kzu_ui_event_listener_kzuUiEventListenerCallCallback00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_ui_event_listener (lua_State* tolua_S) {
 return tolua_kzu_ui_event_listener_open(tolua_S);
}
#endif

