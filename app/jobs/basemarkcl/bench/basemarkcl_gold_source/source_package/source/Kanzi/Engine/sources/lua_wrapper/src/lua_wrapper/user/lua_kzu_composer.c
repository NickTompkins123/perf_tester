/*
** Lua binding: kzu_composer
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_composer_open (lua_State* tolua_S);

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
#include <user/renderer/kzu_composer.h>
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
 tolua_usertype(tolua_S,"KzuObjectSource");
 tolua_usertype(tolua_S,"KzuTransformedScene");
 tolua_usertype(tolua_S,"KzsWindow");
 tolua_usertype(tolua_S,"KzuObjectSourceRuntimeData");
 tolua_usertype(tolua_S,"KzuRenderer");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzuComposer");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzcTexture");
 tolua_usertype(tolua_S,"KzuRenderPass");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzcResourceManager");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuComposerCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerCreate00
static int tolua_kzu_composer_kzuComposerCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyManager* propertyManager = ((struct KzuPropertyManager*)  tolua_tousertype(tolua_S,2,0));
  struct KzuComposer* out_composer = ((struct KzuComposer*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuComposerCreate(memoryManager,propertyManager,&out_composer);
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
    tolua_pushusertype(tolua_S,(void*)out_composer,"KzuComposer");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuComposerCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerDelete00
static int tolua_kzu_composer_kzuComposerDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuComposer* composer = ((struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuComposerDelete(composer);
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
 tolua_error(tolua_S,"#ferror in function 'kzuComposerDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerTakeOwnership */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerTakeOwnership00
static int tolua_kzu_composer_kzuComposerTakeOwnership00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuComposerTakeOwnership(composer);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuComposerTakeOwnership'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerIsValid */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerIsValid00
static int tolua_kzu_composer_kzuComposerIsValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuComposerIsValid(composer);
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
 tolua_error(tolua_S,"#ferror in function 'kzuComposerIsValid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerTransferData */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerTransferData00
static int tolua_kzu_composer_kzuComposerTransferData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuComposer* targetComposer = ((struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  struct KzuComposer* sourceComposer = ((struct KzuComposer*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuComposerTransferData(targetComposer,sourceComposer);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuComposerTransferData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerGetPropertyManager */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerGetPropertyManager00
static int tolua_kzu_composer_kzuComposerGetPropertyManager00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyManager* tolua_ret = (struct KzuPropertyManager*)  kzuComposerGetPropertyManager(composer);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuComposerGetPropertyManager'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerAddChild */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerAddChild00
static int tolua_kzu_composer_kzuComposerAddChild00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  struct KzuComposer* childComposer = ((struct KzuComposer*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuComposerAddChild(composer,childComposer);
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
 tolua_error(tolua_S,"#ferror in function 'kzuComposerAddChild'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerRemoveChild */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerRemoveChild00
static int tolua_kzu_composer_kzuComposerRemoveChild00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuComposer* childComposer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuComposerRemoveChild(composer,childComposer);
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
 tolua_error(tolua_S,"#ferror in function 'kzuComposerRemoveChild'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerGetChildren */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerGetChildren00
static int tolua_kzu_composer_kzuComposerGetChildren00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcDynamicArray* tolua_ret = (struct KzcDynamicArray*)  kzuComposerGetChildren(composer);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcDynamicArray");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuComposerGetChildren'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerGetChildComposer */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerGetChildComposer00
static int tolua_kzu_composer_kzuComposerGetChildComposer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   struct KzuComposer* tolua_ret = (struct KzuComposer*)  kzuComposerGetChildComposer(composer,index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuComposer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuComposerGetChildComposer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerGetChildCount */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerGetChildCount00
static int tolua_kzu_composer_kzuComposerGetChildCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuComposerGetChildCount(composer);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuComposerGetChildCount'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerApply */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerApply00
static int tolua_kzu_composer_kzuComposerApply00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuRenderer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzsWindow",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  struct KzuRenderer* renderer = ((struct KzuRenderer*)  tolua_tousertype(tolua_S,2,0));
  const struct KzuTransformedScene* transformedScene = ((const struct KzuTransformedScene*)  tolua_tousertype(tolua_S,3,0));
  const struct KzsWindow* activeWindow = ((const struct KzsWindow*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuComposerApply(composer,renderer,transformedScene,activeWindow);
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
 tolua_error(tolua_S,"#ferror in function 'kzuComposerApply'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuComposerIterateChildren */
#ifndef TOLUA_DISABLE_tolua_kzu_composer_kzuComposerIterateChildren00
static int tolua_kzu_composer_kzuComposerIterateChildren00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuComposer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuRenderer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuTransformedScene",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzsWindow",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuComposer* composer = ((const struct KzuComposer*)  tolua_tousertype(tolua_S,1,0));
  struct KzuRenderer* renderer = ((struct KzuRenderer*)  tolua_tousertype(tolua_S,2,0));
  const struct KzuTransformedScene* transformedScene = ((const struct KzuTransformedScene*)  tolua_tousertype(tolua_S,3,0));
  const struct KzsWindow* activeWindow = ((const struct KzsWindow*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuComposerIterateChildren(composer,renderer,transformedScene,activeWindow);
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
 tolua_error(tolua_S,"#ferror in function 'kzuComposerIterateChildren'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_composer_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuRenderPass","KzuRenderPass","",NULL);
  tolua_beginmodule(tolua_S,"KzuRenderPass");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuRenderer","KzuRenderer","",NULL);
  tolua_beginmodule(tolua_S,"KzuRenderer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTransformedScene","KzuTransformedScene","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedScene");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectSourceRuntimeData","KzuObjectSourceRuntimeData","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectSourceRuntimeData");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyType","KzuPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcTexture","KzcTexture","",NULL);
  tolua_beginmodule(tolua_S,"KzcTexture");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcDynamicArray","KzcDynamicArray","",NULL);
  tolua_beginmodule(tolua_S,"KzcDynamicArray");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcResourceManager","KzcResourceManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcResourceManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzsWindow","KzsWindow","",NULL);
  tolua_beginmodule(tolua_S,"KzsWindow");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuComposer","KzuComposer","",NULL);
  tolua_beginmodule(tolua_S,"KzuComposer");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuComposerCreate",tolua_kzu_composer_kzuComposerCreate00);
  tolua_function(tolua_S,"kzuComposerDelete",tolua_kzu_composer_kzuComposerDelete00);
  tolua_function(tolua_S,"kzuComposerTakeOwnership",tolua_kzu_composer_kzuComposerTakeOwnership00);
  tolua_function(tolua_S,"kzuComposerIsValid",tolua_kzu_composer_kzuComposerIsValid00);
  tolua_function(tolua_S,"kzuComposerTransferData",tolua_kzu_composer_kzuComposerTransferData00);
  tolua_function(tolua_S,"kzuComposerGetPropertyManager",tolua_kzu_composer_kzuComposerGetPropertyManager00);
  tolua_function(tolua_S,"kzuComposerAddChild",tolua_kzu_composer_kzuComposerAddChild00);
  tolua_function(tolua_S,"kzuComposerRemoveChild",tolua_kzu_composer_kzuComposerRemoveChild00);
  tolua_function(tolua_S,"kzuComposerGetChildren",tolua_kzu_composer_kzuComposerGetChildren00);
  tolua_function(tolua_S,"kzuComposerGetChildComposer",tolua_kzu_composer_kzuComposerGetChildComposer00);
  tolua_function(tolua_S,"kzuComposerGetChildCount",tolua_kzu_composer_kzuComposerGetChildCount00);
  tolua_function(tolua_S,"kzuComposerApply",tolua_kzu_composer_kzuComposerApply00);
  tolua_function(tolua_S,"kzuComposerIterateChildren",tolua_kzu_composer_kzuComposerIterateChildren00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_composer (lua_State* tolua_S) {
 return tolua_kzu_composer_open(tolua_S);
}
#endif

