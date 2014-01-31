/*
** Lua binding: kzu_scene
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_scene_open (lua_State* tolua_S);

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
#include <user/scene_graph/kzu_scene.h>
#include <core/util/collection/kzc_dynamic_array.h>
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
 tolua_usertype(tolua_S,"KzuComposer");
 tolua_usertype(tolua_S,"KzuTransformedObjectNode");
 tolua_usertype(tolua_S,"KzuUiScreen");
 tolua_usertype(tolua_S,"KzuCameraNode");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"KzuObjectSource");
 tolua_usertype(tolua_S,"KzuScene");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"KzsInputPointingDeviceState");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzU32");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuRenderer");
 tolua_usertype(tolua_S,"KzcMatrix4x4");
 tolua_usertype(tolua_S,"KzsInputKey");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzuAnimationPlayer");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzsInputKeyDeviceState");
}

/* function: kzuSceneCreateWithRoot */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneCreateWithRoot00
static int tolua_kzu_scene_kzuSceneCreateWithRoot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  char* name = ((char*)  tolua_tostring(tolua_S,3,0));
  struct KzuObjectNode* rootNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,4,0));
  struct KzuScene* out_scene = ((struct KzuScene*)  tolua_tousertype(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneCreateWithRoot(memoryManager,propertyManager,name,rootNode,&out_scene);
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
    tolua_pushusertype(tolua_S,(void*)out_scene,"KzuScene");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneCreateWithRoot'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneDelete00
static int tolua_kzu_scene_kzuSceneDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuScene* scene = ((struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneDelete(scene);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneTakeOwnership */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneTakeOwnership00
static int tolua_kzu_scene_kzuSceneTakeOwnership00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuSceneTakeOwnership(scene);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneTakeOwnership'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneIsValid */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneIsValid00
static int tolua_kzu_scene_kzuSceneIsValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuSceneIsValid(scene);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneIsValid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneTransferData */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneTransferData00
static int tolua_kzu_scene_kzuSceneTransferData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuScene* targetScene = ((struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  struct KzuScene* sourceScene = ((struct KzuScene*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuSceneTransferData(targetScene,sourceScene);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneTransferData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneAnimate */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneAnimate00
static int tolua_kzu_scene_kzuSceneAnimate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  unsigned int deltaTime = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneAnimate(scene,deltaTime);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneAnimate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneExtract */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneExtract00
static int tolua_kzu_scene_kzuSceneExtract00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcDynamicArray",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuTransformedObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzcMemoryManager* memoryManager = ((struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,2,0));
  struct KzcDynamicArray* extractedObjectList = ((struct KzcDynamicArray*)  tolua_tousertype(tolua_S,3,0));
  struct KzuTransformedObjectNode* out_extractedRootNode = ((struct KzuTransformedObjectNode*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneExtract(memoryManager,scene,extractedObjectList,&out_extractedRootNode);
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
    tolua_pushusertype(tolua_S,(void*)out_extractedRootNode,"KzuTransformedObjectNode");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneExtract'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneSetComposer */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneSetComposer00
static int tolua_kzu_scene_kzuSceneSetComposer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  struct KzuComposer* composer = ((struct KzuComposer*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuSceneSetComposer(scene,composer);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneSetComposer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetComposer */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetComposer00
static int tolua_kzu_scene_kzuSceneGetComposer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuComposer* tolua_ret = (struct KzuComposer*)  kzuSceneGetComposer(scene);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuComposer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetComposer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetRootNode */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetRootNode00
static int tolua_kzu_scene_kzuSceneGetRootNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuSceneGetRootNode(scene);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetRootNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneSetViewCamera */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneSetViewCamera00
static int tolua_kzu_scene_kzuSceneSetViewCamera00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  struct KzuCameraNode* camera = ((struct KzuCameraNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuSceneSetViewCamera(scene,camera);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneSetViewCamera'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneSetViewCamera2D */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneSetViewCamera2D00
static int tolua_kzu_scene_kzuSceneSetViewCamera2D00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  struct KzuCameraNode* camera2D = ((struct KzuCameraNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuSceneSetViewCamera2D(scene,camera2D);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneSetViewCamera2D'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetViewCamera */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetViewCamera00
static int tolua_kzu_scene_kzuSceneGetViewCamera00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuCameraNode* tolua_ret = (struct KzuCameraNode*)  kzuSceneGetViewCamera(scene);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuCameraNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetViewCamera'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetViewCamera2D */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetViewCamera2D00
static int tolua_kzu_scene_kzuSceneGetViewCamera2D00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuCameraNode* tolua_ret = (struct KzuCameraNode*)  kzuSceneGetViewCamera2D(scene);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuCameraNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetViewCamera2D'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneAddObject */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneAddObject00
static int tolua_kzu_scene_kzuSceneAddObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  struct KzuObjectNode* objectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneAddObject(scene,objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneAddObject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneRemoveObject */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneRemoveObject00
static int tolua_kzu_scene_kzuSceneRemoveObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneRemoveObject(scene,objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneRemoveObject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneFindObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneFindObjectNode00
static int tolua_kzu_scene_kzuSceneFindObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* objectToFind = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuSceneFindObjectNode(scene,objectToFind);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneFindObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetRootObjectSource */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetRootObjectSource00
static int tolua_kzu_scene_kzuSceneGetRootObjectSource00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectSource* tolua_ret = (struct KzuObjectSource*)  kzuSceneGetRootObjectSource(scene);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectSource");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetRootObjectSource'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneCalculateWorldTransformation */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneCalculateWorldTransformation00
static int tolua_kzu_scene_kzuSceneCalculateWorldTransformation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  struct KzcMatrix4x4* out_worldTransformation = ((struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,3,0));
  {
   kzBool tolua_ret = (kzBool)  kzuSceneCalculateWorldTransformation(scene,objectNode,out_worldTransformation);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneCalculateWorldTransformation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetAnimationPlayer */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetAnimationPlayer00
static int tolua_kzu_scene_kzuSceneGetAnimationPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuAnimationPlayer* tolua_ret = (struct KzuAnimationPlayer*)  kzuSceneGetAnimationPlayer(scene);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuAnimationPlayer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetAnimationPlayer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetUiScreen */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetUiScreen00
static int tolua_kzu_scene_kzuSceneGetUiScreen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuUiScreen* tolua_ret = (struct KzuUiScreen*)  kzuSceneGetUiScreen(scene);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiScreen");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetUiScreen'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneHandleKeyEvents */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneHandleKeyEvents00
static int tolua_kzu_scene_kzuSceneHandleKeyEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzsInputKey",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzsInputKeyDeviceState",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  enum KzsInputKey keyCode = *((enum KzsInputKey*)  tolua_tousertype(tolua_S,2,0));
  enum KzsInputKeyDeviceState keyState = *((enum KzsInputKeyDeviceState*)  tolua_tousertype(tolua_S,3,0));
  {
   kzuSceneHandleKeyEvents(scene,keyCode,keyState);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneHandleKeyEvents'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneHandlePointerEvents */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneHandlePointerEvents00
static int tolua_kzu_scene_kzuSceneHandlePointerEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
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
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  kzU32 buttons = *((kzU32*)  tolua_tousertype(tolua_S,2,0));
  enum KzsInputPointingDeviceState state = *((enum KzsInputPointingDeviceState*)  tolua_tousertype(tolua_S,3,0));
  int mouseX = ((int)  tolua_tonumber(tolua_S,4,0));
  int mouseY = ((int)  tolua_tonumber(tolua_S,5,0));
  {
   kzuSceneHandlePointerEvents(scene,buttons,state,mouseX,mouseY);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneHandlePointerEvents'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneArrangeNode */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneArrangeNode00
static int tolua_kzu_scene_kzuSceneArrangeNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTransformedObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuTransformedObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuTransformedObjectNode* transformedNode = ((struct KzuTransformedObjectNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzuTransformedObjectNode* transformedParentNode = ((struct KzuTransformedObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  0;//kzuSceneArrangeNode(transformedNode,transformedParentNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneArrangeNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneApplyConstraints */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneApplyConstraints00
static int tolua_kzu_scene_kzuSceneApplyConstraints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMemoryManager* quickMemoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneApplyConstraints(scene,quickMemoryManager);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneApplyConstraints'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneGetName */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneGetName00
static int tolua_kzu_scene_kzuSceneGetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  {
   char* tolua_ret = (char*)  kzuSceneGetName(scene);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuSceneGetName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuSceneSetName */
#ifndef TOLUA_DISABLE_tolua_kzu_scene_kzuSceneSetName00
static int tolua_kzu_scene_kzuSceneSetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuScene",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuScene* scene = ((const struct KzuScene*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuSceneSetName(scene,name);
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
 tolua_error(tolua_S,"#ferror in function 'kzuSceneSetName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_scene_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuCameraNode","KzuCameraNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuCameraNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTransformedObjectNode","KzuTransformedObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuComposer","KzuComposer","",NULL);
  tolua_beginmodule(tolua_S,"KzuComposer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuRenderer","KzuRenderer","",NULL);
  tolua_beginmodule(tolua_S,"KzuRenderer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiScreen","KzuUiScreen","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiScreen");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMatrix4x4","KzcMatrix4x4","",NULL);
  tolua_beginmodule(tolua_S,"KzcMatrix4x4");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuScene","KzuScene","",NULL);
  tolua_beginmodule(tolua_S,"KzuScene");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuSceneCreateWithRoot",tolua_kzu_scene_kzuSceneCreateWithRoot00);
  tolua_function(tolua_S,"kzuSceneDelete",tolua_kzu_scene_kzuSceneDelete00);
  tolua_function(tolua_S,"kzuSceneTakeOwnership",tolua_kzu_scene_kzuSceneTakeOwnership00);
  tolua_function(tolua_S,"kzuSceneIsValid",tolua_kzu_scene_kzuSceneIsValid00);
  tolua_function(tolua_S,"kzuSceneTransferData",tolua_kzu_scene_kzuSceneTransferData00);
  tolua_function(tolua_S,"kzuSceneAnimate",tolua_kzu_scene_kzuSceneAnimate00);
  tolua_function(tolua_S,"kzuSceneExtract",tolua_kzu_scene_kzuSceneExtract00);
  tolua_function(tolua_S,"kzuSceneSetComposer",tolua_kzu_scene_kzuSceneSetComposer00);
  tolua_function(tolua_S,"kzuSceneGetComposer",tolua_kzu_scene_kzuSceneGetComposer00);
  tolua_function(tolua_S,"kzuSceneGetRootNode",tolua_kzu_scene_kzuSceneGetRootNode00);
  tolua_function(tolua_S,"kzuSceneSetViewCamera",tolua_kzu_scene_kzuSceneSetViewCamera00);
  tolua_function(tolua_S,"kzuSceneSetViewCamera2D",tolua_kzu_scene_kzuSceneSetViewCamera2D00);
  tolua_function(tolua_S,"kzuSceneGetViewCamera",tolua_kzu_scene_kzuSceneGetViewCamera00);
  tolua_function(tolua_S,"kzuSceneGetViewCamera2D",tolua_kzu_scene_kzuSceneGetViewCamera2D00);
  tolua_function(tolua_S,"kzuSceneAddObject",tolua_kzu_scene_kzuSceneAddObject00);
  tolua_function(tolua_S,"kzuSceneRemoveObject",tolua_kzu_scene_kzuSceneRemoveObject00);
  tolua_function(tolua_S,"kzuSceneFindObjectNode",tolua_kzu_scene_kzuSceneFindObjectNode00);
  tolua_function(tolua_S,"kzuSceneGetRootObjectSource",tolua_kzu_scene_kzuSceneGetRootObjectSource00);
  tolua_function(tolua_S,"kzuSceneCalculateWorldTransformation",tolua_kzu_scene_kzuSceneCalculateWorldTransformation00);
  tolua_function(tolua_S,"kzuSceneGetAnimationPlayer",tolua_kzu_scene_kzuSceneGetAnimationPlayer00);
  tolua_function(tolua_S,"kzuSceneGetUiScreen",tolua_kzu_scene_kzuSceneGetUiScreen00);
  tolua_function(tolua_S,"kzuSceneHandleKeyEvents",tolua_kzu_scene_kzuSceneHandleKeyEvents00);
  tolua_function(tolua_S,"kzuSceneHandlePointerEvents",tolua_kzu_scene_kzuSceneHandlePointerEvents00);
  tolua_function(tolua_S,"kzuSceneArrangeNode",tolua_kzu_scene_kzuSceneArrangeNode00);
  tolua_function(tolua_S,"kzuSceneApplyConstraints",tolua_kzu_scene_kzuSceneApplyConstraints00);
  tolua_function(tolua_S,"kzuSceneGetName",tolua_kzu_scene_kzuSceneGetName00);
  tolua_function(tolua_S,"kzuSceneSetName",tolua_kzu_scene_kzuSceneSetName00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_scene (lua_State* tolua_S) {
 return tolua_kzu_scene_open(tolua_S);
}
#endif

