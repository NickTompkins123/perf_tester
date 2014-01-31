/*
** Lua binding: kzu_engine
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_engine_open (lua_State* tolua_S);

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
#include <user/engine/kzu_engine.h>
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
 tolua_usertype(tolua_S,"KzuEngine");
 tolua_usertype(tolua_S,"KzuTransformedScene");
 tolua_usertype(tolua_S,"KzuPropertyCollection");
 tolua_usertype(tolua_S,"KzuMaterial");
 tolua_usertype(tolua_S,"KzuRenderer");
 tolua_usertype(tolua_S,"KzuTransformedObjectNode");
 tolua_usertype(tolua_S,"KzuPropertyGroup");
 tolua_usertype(tolua_S,"KzsWindow");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"KzuObjectSource");
 tolua_usertype(tolua_S,"KzuScene");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"KzcStopwatch");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuFixedPropertyTypes");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzcResourceManager");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuEngineMessageQueue");
}

/* function: kzuEngineCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineCreate00
static int tolua_kzu_engine_kzuEngineCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  unsigned int quickMemorySize = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,3,0));
  struct KzuEngine* out_engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineCreate(memoryManager,quickMemorySize,propertyManager,&out_engine);
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
    tolua_pushusertype(tolua_S,(void*)out_engine,"KzuEngine");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineDelete00
static int tolua_kzu_engine_kzuEngineDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineDelete(engine);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineSetActiveWindow */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineSetActiveWindow00
static int tolua_kzu_engine_kzuEngineSetActiveWindow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzsWindow",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  struct KzsWindow* window = ((struct KzsWindow*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuEngineSetActiveWindow(engine,window);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineSetActiveWindow'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetActiveWindow */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetActiveWindow00
static int tolua_kzu_engine_kzuEngineGetActiveWindow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzsWindow* tolua_ret = (struct KzsWindow*)  kzuEngineGetActiveWindow(engine);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzsWindow");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetActiveWindow'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineApplyGlobalProperties */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineApplyGlobalProperties00
static int tolua_kzu_engine_kzuEngineApplyGlobalProperties00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuPropertyManager* propertyManager = ((const struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  const void* object = ((const void*)  tolua_touserdata(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineApplyGlobalProperties(engine,propertyManager,object);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineApplyGlobalProperties'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineStartUpdate */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineStartUpdate00
static int tolua_kzu_engine_kzuEngineStartUpdate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineStartUpdate(engine);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineStartUpdate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineBeginFrame */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineBeginFrame00
static int tolua_kzu_engine_kzuEngineBeginFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineBeginFrame(engine);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineBeginFrame'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineEndFrame */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineEndFrame00
static int tolua_kzu_engine_kzuEngineEndFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineEndFrame(engine);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineEndFrame'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineExtractScene */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineExtractScene00
static int tolua_kzu_engine_kzuEngineExtractScene00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  struct KzuScene* scene = ((struct KzuScene*)  tolua_tousertype(tolua_S,2,0));
  struct KzuTransformedScene* transformedScene = ((struct KzuTransformedScene*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineExtractScene(engine,scene,transformedScene);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineExtractScene'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineRender */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineRender00
static int tolua_kzu_engine_kzuEngineRender00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuTransformedScene* transformedScene = ((const struct KzuTransformedScene*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineRender(engine,transformedScene);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineRender'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineUpdateUi */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineUpdateUi00
static int tolua_kzu_engine_kzuEngineUpdateUi00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuTransformedScene* transformedScene = ((const struct KzuTransformedScene*)  tolua_tousertype(tolua_S,2,0));
  unsigned int deltaTime = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineUpdateUi(engine,transformedScene,deltaTime);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineUpdateUi'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetTimer */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetTimer00
static int tolua_kzu_engine_kzuEngineGetTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcStopwatch* tolua_ret = (struct KzcStopwatch*)  kzuEngineGetTimer(engine);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcStopwatch");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetTimer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetDeltaTime */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetDeltaTime00
static int tolua_kzu_engine_kzuEngineGetDeltaTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuEngineGetDeltaTime(engine);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetDeltaTime'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetFramesPerSecond */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetFramesPerSecond00
static int tolua_kzu_engine_kzuEngineGetFramesPerSecond00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuEngineGetFramesPerSecond(engine);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetFramesPerSecond'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineSetGPUPerformanceMeasurementEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineSetGPUPerformanceMeasurementEnabled00
static int tolua_kzu_engine_kzuEngineSetGPUPerformanceMeasurementEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  kzBool enabled = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuEngineSetGPUPerformanceMeasurementEnabled(engine,enabled);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineSetGPUPerformanceMeasurementEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineIsGPUPerformanceMeasurementEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineIsGPUPerformanceMeasurementEnabled00
static int tolua_kzu_engine_kzuEngineIsGPUPerformanceMeasurementEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuEngineIsGPUPerformanceMeasurementEnabled(engine);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineIsGPUPerformanceMeasurementEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineIsCurrentFrameGPUEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineIsCurrentFrameGPUEnabled00
static int tolua_kzu_engine_kzuEngineIsCurrentFrameGPUEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuEngineIsCurrentFrameGPUEnabled(engine);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineIsCurrentFrameGPUEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetGPUPerformancePercentage */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetGPUPerformancePercentage00
static int tolua_kzu_engine_kzuEngineGetGPUPerformancePercentage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuEngineGetGPUPerformancePercentage(engine);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetGPUPerformancePercentage'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineSetGPUCallsEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineSetGPUCallsEnabled00
static int tolua_kzu_engine_kzuEngineSetGPUCallsEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuEngine* engine = ((struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  kzBool enabled = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuEngineSetGPUCallsEnabled(engine,enabled);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineSetGPUCallsEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetResourceManager */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetResourceManager00
static int tolua_kzu_engine_kzuEngineGetResourceManager00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcResourceManager* tolua_ret = (struct KzcResourceManager*)  kzuEngineGetResourceManager(engine);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcResourceManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetResourceManager'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetRenderer */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetRenderer00
static int tolua_kzu_engine_kzuEngineGetRenderer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuRenderer* tolua_ret = (struct KzuRenderer*)  kzuEngineGetRenderer(engine);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuRenderer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetRenderer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineDrawVisualizationGrid */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineDrawVisualizationGrid00
static int tolua_kzu_engine_kzuEngineDrawVisualizationGrid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuTransformedScene* transformedScene = ((const struct KzuTransformedScene*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineDrawVisualizationGrid(engine,transformedScene,1.0f);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineDrawVisualizationGrid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineDrawDebugObjects */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineDrawDebugObjects00
static int tolua_kzu_engine_kzuEngineDrawDebugObjects00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuTransformedScene* transformedScene = ((const struct KzuTransformedScene*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuEngineDrawDebugObjects(engine,transformedScene);
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
 tolua_error(tolua_S,"#ferror in function 'kzuEngineDrawDebugObjects'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuEngineGetMessageQueue */
#ifndef TOLUA_DISABLE_tolua_kzu_engine_kzuEngineGetMessageQueue00
static int tolua_kzu_engine_kzuEngineGetMessageQueue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuEngine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuEngine* engine = ((const struct KzuEngine*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuEngineMessageQueue* tolua_ret = (struct KzuEngineMessageQueue*)  kzuEngineGetMessageQueue(engine);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuEngineMessageQueue");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuEngineGetMessageQueue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_engine_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"KZU_ENGINE_DEFAULT_QUICK_MEMORY_SIZE",KZU_ENGINE_DEFAULT_QUICK_MEMORY_SIZE);
  tolua_cclass(tolua_S,"KzuScene","KzuScene","",NULL);
  tolua_beginmodule(tolua_S,"KzuScene");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTransformedObjectNode","KzuTransformedObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectSource","KzuObjectSource","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectSource");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMaterial","KzuMaterial","",NULL);
  tolua_beginmodule(tolua_S,"KzuMaterial");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuRenderer","KzuRenderer","",NULL);
  tolua_beginmodule(tolua_S,"KzuRenderer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuFixedPropertyTypes","KzuFixedPropertyTypes","",NULL);
  tolua_beginmodule(tolua_S,"KzuFixedPropertyTypes");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyCollection","KzuPropertyCollection","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyCollection");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyGroup","KzuPropertyGroup","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyGroup");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcResourceManager","KzcResourceManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcResourceManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcDynamicArray","KzcDynamicArray","",NULL);
  tolua_beginmodule(tolua_S,"KzcDynamicArray");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcStopwatch","KzcStopwatch","",NULL);
  tolua_beginmodule(tolua_S,"KzcStopwatch");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzsWindow","KzsWindow","",NULL);
  tolua_beginmodule(tolua_S,"KzsWindow");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTransformedScene","KzuTransformedScene","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedScene");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuEngine","KzuEngine","",NULL);
  tolua_beginmodule(tolua_S,"KzuEngine");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuEngineCreate",tolua_kzu_engine_kzuEngineCreate00);
  tolua_function(tolua_S,"kzuEngineDelete",tolua_kzu_engine_kzuEngineDelete00);
  tolua_function(tolua_S,"kzuEngineSetActiveWindow",tolua_kzu_engine_kzuEngineSetActiveWindow00);
  tolua_function(tolua_S,"kzuEngineGetActiveWindow",tolua_kzu_engine_kzuEngineGetActiveWindow00);
  tolua_function(tolua_S,"kzuEngineApplyGlobalProperties",tolua_kzu_engine_kzuEngineApplyGlobalProperties00);
  tolua_function(tolua_S,"kzuEngineStartUpdate",tolua_kzu_engine_kzuEngineStartUpdate00);
  tolua_function(tolua_S,"kzuEngineBeginFrame",tolua_kzu_engine_kzuEngineBeginFrame00);
  tolua_function(tolua_S,"kzuEngineEndFrame",tolua_kzu_engine_kzuEngineEndFrame00);
  tolua_function(tolua_S,"kzuEngineExtractScene",tolua_kzu_engine_kzuEngineExtractScene00);
  tolua_function(tolua_S,"kzuEngineRender",tolua_kzu_engine_kzuEngineRender00);
  tolua_function(tolua_S,"kzuEngineUpdateUi",tolua_kzu_engine_kzuEngineUpdateUi00);
  tolua_function(tolua_S,"kzuEngineGetTimer",tolua_kzu_engine_kzuEngineGetTimer00);
  tolua_function(tolua_S,"kzuEngineGetDeltaTime",tolua_kzu_engine_kzuEngineGetDeltaTime00);
  tolua_function(tolua_S,"kzuEngineGetFramesPerSecond",tolua_kzu_engine_kzuEngineGetFramesPerSecond00);
  tolua_function(tolua_S,"kzuEngineSetGPUPerformanceMeasurementEnabled",tolua_kzu_engine_kzuEngineSetGPUPerformanceMeasurementEnabled00);
  tolua_function(tolua_S,"kzuEngineIsGPUPerformanceMeasurementEnabled",tolua_kzu_engine_kzuEngineIsGPUPerformanceMeasurementEnabled00);
  tolua_function(tolua_S,"kzuEngineIsCurrentFrameGPUEnabled",tolua_kzu_engine_kzuEngineIsCurrentFrameGPUEnabled00);
  tolua_function(tolua_S,"kzuEngineGetGPUPerformancePercentage",tolua_kzu_engine_kzuEngineGetGPUPerformancePercentage00);
  tolua_function(tolua_S,"kzuEngineSetGPUCallsEnabled",tolua_kzu_engine_kzuEngineSetGPUCallsEnabled00);
  tolua_function(tolua_S,"kzuEngineGetResourceManager",tolua_kzu_engine_kzuEngineGetResourceManager00);
  tolua_function(tolua_S,"kzuEngineGetRenderer",tolua_kzu_engine_kzuEngineGetRenderer00);
  tolua_function(tolua_S,"kzuEngineDrawVisualizationGrid",tolua_kzu_engine_kzuEngineDrawVisualizationGrid00);
  tolua_function(tolua_S,"kzuEngineDrawDebugObjects",tolua_kzu_engine_kzuEngineDrawDebugObjects00);
  tolua_function(tolua_S,"kzuEngineGetMessageQueue",tolua_kzu_engine_kzuEngineGetMessageQueue00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_engine (lua_State* tolua_S) {
 return tolua_kzu_engine_open(tolua_S);
}
#endif

