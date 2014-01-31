/*
** Lua binding: kzu_ui_component
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_ui_component_open (lua_State* tolua_S);

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
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_base.h>
#include <user/scene_graph/kzu_object_base.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzBool (lua_State* tolua_S)
{
 kzBool* self = (kzBool*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzuUiComponentArrangeFunction (lua_State* tolua_S)
{
 KzuUiComponentArrangeFunction* self = (KzuUiComponentArrangeFunction*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzuUiComponentMeasureFunction (lua_State* tolua_S)
{
 KzuUiComponentMeasureFunction* self = (KzuUiComponentMeasureFunction*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzcVector3");
 tolua_usertype(tolua_S,"KzuTransformedObjectNode");
 tolua_usertype(tolua_S,"KzuUiComponentNode");
 tolua_usertype(tolua_S,"KzuUiEvent");
 tolua_usertype(tolua_S,"KzuUiComponentType");
 tolua_usertype(tolua_S,"KzuUiAction");
 tolua_usertype(tolua_S,"KzuUiEventListener");
 tolua_usertype(tolua_S,"KzuEngineMessageQueue");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzuUiComponentArrangeFunction");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuUiComponentMeasureFunction");
}


/* function: kzuUiComponentNodeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeDelete00
static int tolua_kzu_ui_component_kzuUiComponentNodeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiComponentNode* uiComponentNode = ((struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeDelete(uiComponentNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeSetComponentType */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeSetComponentType00
static int tolua_kzu_ui_component_kzuUiComponentNodeSetComponentType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentType* componentType = ((struct KzuUiComponentType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentNodeSetComponentType(componentNode,componentType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeSetComponentType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeInitialize */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeInitialize00
static int tolua_kzu_ui_component_kzuUiComponentNodeInitialize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiComponentNode* componentNode = ((struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeInitialize(componentNode,memoryManager);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeInitialize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeUninitialize */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeUninitialize00
static int tolua_kzu_ui_component_kzuUiComponentNodeUninitialize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiComponentNode* componentNode = ((struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeUninitialize(componentNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeUninitialize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeLogicUpdate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeLogicUpdate00
static int tolua_kzu_ui_component_kzuUiComponentNodeLogicUpdate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuEngineMessageQueue",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  float deltaTime = ((float)  tolua_tonumber(tolua_S,2,0));
  struct KzuEngineMessageQueue* queue = ((struct KzuEngineMessageQueue*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeLogicUpdate(componentNode,deltaTime,queue);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeLogicUpdate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeHandleEvents */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeHandleEvents00
static int tolua_kzu_ui_component_kzuUiComponentNodeHandleEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuEngineMessageQueue",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiEvent* event = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,2,0));
  char* eventType = ((char*)  tolua_tostring(tolua_S,3,0));
  struct KzuEngineMessageQueue* queue = ((struct KzuEngineMessageQueue*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeHandleEvents(componentNode,event,eventType,queue);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeHandleEvents'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeAddEventListener */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeAddEventListener00
static int tolua_kzu_ui_component_kzuUiComponentNodeAddEventListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  char* event = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzuUiEventListener* eventListener = ((struct KzuUiEventListener*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeAddEventListener(componentNode,event,eventListener);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeAddEventListener'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeRemoveEventListener */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeRemoveEventListener00
static int tolua_kzu_ui_component_kzuUiComponentNodeRemoveEventListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiEventListener",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  char* event = ((char*)  tolua_tostring(tolua_S,2,0));
  const struct KzuUiEventListener* eventListener = ((const struct KzuUiEventListener*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeRemoveEventListener(componentNode,event,eventListener);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeRemoveEventListener'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeGetEventListeners */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeGetEventListeners00
static int tolua_kzu_ui_component_kzuUiComponentNodeGetEventListeners00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcDynamicArray",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  struct KzcDynamicArray* out_array = ((struct KzcDynamicArray*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentNodeGetEventListeners(memoryManager,componentNode,&out_array);
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
    tolua_pushusertype(tolua_S,(void*)out_array,"KzcDynamicArray");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeGetEventListeners'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeFromObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeFromObjectNode00
static int tolua_kzu_ui_component_kzuUiComponentNodeFromObjectNode00(lua_State* tolua_S)
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
   struct KzuUiComponentNode* tolua_ret = (struct KzuUiComponentNode*)  kzuUiComponentNodeFromObjectNode(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiComponentNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeFromObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeToObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeToObjectNode00
static int tolua_kzu_ui_component_kzuUiComponentNodeToObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuUiComponentNodeToObjectNode(componentNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeToObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeGetType */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeGetType00
static int tolua_kzu_ui_component_kzuUiComponentNodeGetType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuUiComponentType* tolua_ret = (struct KzuUiComponentType*)  kzuUiComponentNodeGetType(componentNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiComponentType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeGetType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeSetMeasureFunction */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeSetMeasureFunction00
static int tolua_kzu_ui_component_kzuUiComponentNodeSetMeasureFunction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuUiComponentMeasureFunction",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  KzuUiComponentMeasureFunction function = *((KzuUiComponentMeasureFunction*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentNodeSetMeasureFunction(componentNode,function);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeSetMeasureFunction'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeSetArrangeFunction */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeSetArrangeFunction00
static int tolua_kzu_ui_component_kzuUiComponentNodeSetArrangeFunction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuUiComponentArrangeFunction",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  KzuUiComponentArrangeFunction function = *((KzuUiComponentArrangeFunction*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentNodeSetArrangeFunction(componentNode,function);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeSetArrangeFunction'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeGetMeasureFunction */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeGetMeasureFunction00
static int tolua_kzu_ui_component_kzuUiComponentNodeGetMeasureFunction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  {
   KzuUiComponentMeasureFunction tolua_ret = (KzuUiComponentMeasureFunction)  kzuUiComponentNodeGetMeasureFunction(componentNode);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzuUiComponentMeasureFunction)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuUiComponentMeasureFunction");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(KzuUiComponentMeasureFunction));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuUiComponentMeasureFunction");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeGetMeasureFunction'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentNodeGetArrangeFunction */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiComponentNodeGetArrangeFunction00
static int tolua_kzu_ui_component_kzuUiComponentNodeGetArrangeFunction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,1,0));
  {
   KzuUiComponentArrangeFunction tolua_ret = (KzuUiComponentArrangeFunction)  kzuUiComponentNodeGetArrangeFunction(componentNode);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzuUiComponentArrangeFunction)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuUiComponentArrangeFunction");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(KzuUiComponentArrangeFunction));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuUiComponentArrangeFunction");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentNodeGetArrangeFunction'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiFindChildNode */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_kzuUiFindChildNode00
static int tolua_kzu_ui_component_kzuUiFindChildNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* parentObject = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* objectToFind = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiFindChildNode(parentObject,objectToFind);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiFindChildNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_ui_component_open (lua_State* tolua_S)
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
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuEngineMessageQueue","KzuEngineMessageQueue","",NULL);
  tolua_beginmodule(tolua_S,"KzuEngineMessageQueue");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiComponentType","KzuUiComponentType","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiComponentType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiAction","KzuUiAction","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiAction");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEvent","KzuUiEvent","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEvent");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEventListener","KzuUiEventListener","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEventListener");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTransformedObjectNode","KzuTransformedObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcVector3","KzcVector3","",NULL);
  tolua_beginmodule(tolua_S,"KzcVector3");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcDynamicArray","KzcDynamicArray","",NULL);
  tolua_beginmodule(tolua_S,"KzcDynamicArray");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiComponentNode","KzuUiComponentNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiComponentNode");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuUiComponentNodeDelete",tolua_kzu_ui_component_kzuUiComponentNodeDelete00);
  tolua_function(tolua_S,"kzuUiComponentNodeSetComponentType",tolua_kzu_ui_component_kzuUiComponentNodeSetComponentType00);
  tolua_function(tolua_S,"kzuUiComponentNodeInitialize",tolua_kzu_ui_component_kzuUiComponentNodeInitialize00);
  tolua_function(tolua_S,"kzuUiComponentNodeUninitialize",tolua_kzu_ui_component_kzuUiComponentNodeUninitialize00);
  tolua_function(tolua_S,"kzuUiComponentNodeLogicUpdate",tolua_kzu_ui_component_kzuUiComponentNodeLogicUpdate00);
  tolua_function(tolua_S,"kzuUiComponentNodeHandleEvents",tolua_kzu_ui_component_kzuUiComponentNodeHandleEvents00);
  tolua_function(tolua_S,"kzuUiComponentNodeAddEventListener",tolua_kzu_ui_component_kzuUiComponentNodeAddEventListener00);
  tolua_function(tolua_S,"kzuUiComponentNodeRemoveEventListener",tolua_kzu_ui_component_kzuUiComponentNodeRemoveEventListener00);
  tolua_function(tolua_S,"kzuUiComponentNodeGetEventListeners",tolua_kzu_ui_component_kzuUiComponentNodeGetEventListeners00);
  tolua_function(tolua_S,"kzuUiComponentNodeFromObjectNode",tolua_kzu_ui_component_kzuUiComponentNodeFromObjectNode00);
  tolua_function(tolua_S,"kzuUiComponentNodeToObjectNode",tolua_kzu_ui_component_kzuUiComponentNodeToObjectNode00);
  tolua_function(tolua_S,"kzuUiComponentNodeGetType",tolua_kzu_ui_component_kzuUiComponentNodeGetType00);
  tolua_function(tolua_S,"kzuUiComponentNodeSetMeasureFunction",tolua_kzu_ui_component_kzuUiComponentNodeSetMeasureFunction00);
  tolua_function(tolua_S,"kzuUiComponentNodeSetArrangeFunction",tolua_kzu_ui_component_kzuUiComponentNodeSetArrangeFunction00);
  tolua_function(tolua_S,"kzuUiComponentNodeGetMeasureFunction",tolua_kzu_ui_component_kzuUiComponentNodeGetMeasureFunction00);
  tolua_function(tolua_S,"kzuUiComponentNodeGetArrangeFunction",tolua_kzu_ui_component_kzuUiComponentNodeGetArrangeFunction00);
  tolua_function(tolua_S,"kzuUiFindChildNode",tolua_kzu_ui_component_kzuUiFindChildNode00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_ui_component (lua_State* tolua_S) {
 return tolua_kzu_ui_component_open(tolua_S);
}
#endif

