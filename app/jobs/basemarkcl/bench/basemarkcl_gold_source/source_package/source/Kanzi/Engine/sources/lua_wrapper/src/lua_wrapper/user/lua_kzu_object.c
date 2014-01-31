/*
** Lua binding: kzu_object
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_object_open (lua_State* tolua_S);

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
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_object_base.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_kzBool (lua_State* tolua_S)
{
 kzBool* self = (kzBool*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcDynamicArrayIterator (lua_State* tolua_S)
{
 KzcDynamicArrayIterator* self = (KzcDynamicArrayIterator*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzuObjectType (lua_State* tolua_S)
{
 KzuObjectType* self = (KzuObjectType*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcVector3 (lua_State* tolua_S)
{
 KzcVector3* self = (KzcVector3*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzcMatrix4x4 (lua_State* tolua_S)
{
 KzcMatrix4x4* self = (KzcMatrix4x4*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuBoundingVolume");
 tolua_usertype(tolua_S,"KzcVector3");
 tolua_usertype(tolua_S,"KzcDynamicArrayIterator");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"KzuObjectType");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzcMatrix4x4");
 tolua_usertype(tolua_S,"KzuTransformedObjectNode");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzcVolume");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzuRenderer");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuObjectTypeIsRenderable */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectTypeIsRenderable00
static int tolua_kzu_object_kzuObjectTypeIsRenderable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     (tolua_isvaluenil(tolua_S,1,&tolua_err) || !tolua_isusertype(tolua_S,1,"KzuObjectType",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  KzuObjectType objectType = *((KzuObjectType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectTypeIsRenderable(objectType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectTypeIsRenderable'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeDelete00
static int tolua_kzu_object_kzuObjectNodeDelete00(lua_State* tolua_S)
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
  struct KzuObjectNode* objectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeDelete(objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeTakeOwnership */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeTakeOwnership00
static int tolua_kzu_object_kzuObjectNodeTakeOwnership00(lua_State* tolua_S)
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
   kzuObjectNodeTakeOwnership(objectNode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeTakeOwnership'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeIsValid */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeIsValid00
static int tolua_kzu_object_kzuObjectNodeIsValid00(lua_State* tolua_S)
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
   kzBool tolua_ret = (kzBool)  kzuObjectNodeIsValid(objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeIsValid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeTransferData */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeTransferData00
static int tolua_kzu_object_kzuObjectNodeTransferData00(lua_State* tolua_S)
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
  struct KzuObjectNode* targetObjectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzuObjectNode* sourceObjectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuObjectNodeTransferData(targetObjectNode,sourceObjectNode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeTransferData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetPropertyManager */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetPropertyManager00
static int tolua_kzu_object_kzuObjectNodeGetPropertyManager00(lua_State* tolua_S)
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
   struct KzuPropertyManager* tolua_ret = (struct KzuPropertyManager*)  kzuObjectNodeGetPropertyManager(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetPropertyManager'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeCopyWithChildren */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeCopyWithChildren00
static int tolua_kzu_object_kzuObjectNodeCopyWithChildren00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* sourceObject = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  struct KzuObjectNode* out_objectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeCopyWithChildren(memoryManager,sourceObject,&out_objectNode);
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
    tolua_pushusertype(tolua_S,(void*)out_objectNode,"KzuObjectNode");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeCopyWithChildren'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetParent */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetParent00
static int tolua_kzu_object_kzuObjectNodeGetParent00(lua_State* tolua_S)
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
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuObjectNodeGetParent(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetParent'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetRootNode */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetRootNode00
static int tolua_kzu_object_kzuObjectNodeGetRootNode00(lua_State* tolua_S)
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
  struct KzuObjectNode* objectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuObjectNodeGetRootNode(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetRootNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeAddChild */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeAddChild00
static int tolua_kzu_object_kzuObjectNodeAddChild00(lua_State* tolua_S)
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
  struct KzuObjectNode* parentObject = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzuObjectNode* childNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeAddChild(parentObject,childNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeAddChild'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeAddChildAtIndex */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeAddChildAtIndex00
static int tolua_kzu_object_kzuObjectNodeAddChildAtIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuObjectNode* parentObject = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  struct KzuObjectNode* childNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeAddChildAtIndex(parentObject,index,childNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeAddChildAtIndex'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeRemoveChild */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeRemoveChild00
static int tolua_kzu_object_kzuObjectNodeRemoveChild00(lua_State* tolua_S)
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
  const struct KzuObjectNode* childObject = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeRemoveChild(parentObject,childObject);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeRemoveChild'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeRemoveAllChildren */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeRemoveAllChildren00
static int tolua_kzu_object_kzuObjectNodeRemoveAllChildren00(lua_State* tolua_S)
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
  const struct KzuObjectNode* parentObject = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeRemoveAllChildren(parentObject);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeRemoveAllChildren'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeHasChild */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeHasChild00
static int tolua_kzu_object_kzuObjectNodeHasChild00(lua_State* tolua_S)
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
  const struct KzuObjectNode* parentObjectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* childObjectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeHasChild(parentObjectNode,childObjectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeHasChild'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetChildCount */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetChildCount00
static int tolua_kzu_object_kzuObjectNodeGetChildCount00(lua_State* tolua_S)
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
  const struct KzuObjectNode* parentObject = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuObjectNodeGetChildCount(parentObject);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetChildCount'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetChildren */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetChildren00
static int tolua_kzu_object_kzuObjectNodeGetChildren00(lua_State* tolua_S)
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
   struct KzcDynamicArrayIterator tolua_ret =   kzuObjectNodeGetChildren(objectNode);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcDynamicArrayIterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcDynamicArrayIterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcDynamicArrayIterator));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcDynamicArrayIterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetChildren'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetChildAtIndex */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetChildAtIndex00
static int tolua_kzu_object_kzuObjectNodeGetChildAtIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuObjectNodeGetChildAtIndex(objectNode,index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetChildAtIndex'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetChildByName */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetChildByName00
static int tolua_kzu_object_kzuObjectNodeGetChildByName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuObjectNodeGetChildByName(objectNode,name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetChildByName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetName */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetName00
static int tolua_kzu_object_kzuObjectNodeSetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetName(objectNode,name);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetName */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetName00
static int tolua_kzu_object_kzuObjectNodeGetName00(lua_State* tolua_S)
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
   char* tolua_ret = (char*)  kzuObjectNodeGetName(objectNode);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetType */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetType00
static int tolua_kzu_object_kzuObjectNodeGetType00(lua_State* tolua_S)
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
   KzuObjectType tolua_ret = (KzuObjectType)  kzuObjectNodeGetType(objectNode);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzuObjectType)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuObjectType");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(KzuObjectType));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuObjectType");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetTransformation */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetTransformation00
static int tolua_kzu_object_kzuObjectNodeSetTransformation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix4x4* matrix = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetTransformation(objectNode,matrix);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetTransformation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetTransformation */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetTransformation00
static int tolua_kzu_object_kzuObjectNodeGetTransformation00(lua_State* tolua_S)
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
   struct KzcMatrix4x4 tolua_ret =   kzuObjectNodeGetTransformation(objectNode);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzcMatrix4x4)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzcMatrix4x4));
     tolua_pushusertype(tolua_S,tolua_obj,"KzcMatrix4x4");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetTransformation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetPositionVector */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetPositionVector00
static int tolua_kzu_object_kzuObjectNodeGetPositionVector00(lua_State* tolua_S)
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
   struct KzcVector3 tolua_ret =   kzuObjectNodeGetPositionVector(objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetPositionVector'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetIdentityTransformation */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetIdentityTransformation00
static int tolua_kzu_object_kzuObjectNodeSetIdentityTransformation00(lua_State* tolua_S)
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
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetIdentityTransformation(node);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetIdentityTransformation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetIdentity3x3Transformation */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetIdentity3x3Transformation00
static int tolua_kzu_object_kzuObjectNodeSetIdentity3x3Transformation00(lua_State* tolua_S)
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
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetIdentity3x3Transformation(node);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetIdentity3x3Transformation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeTranslate */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeTranslate00
static int tolua_kzu_object_kzuObjectNodeTranslate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float x = ((float)  tolua_tonumber(tolua_S,2,0));
  float y = ((float)  tolua_tonumber(tolua_S,3,0));
  float z = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeTranslate(node,x,y,z);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeTranslate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetPosition */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetPosition00
static int tolua_kzu_object_kzuObjectNodeSetPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float x = ((float)  tolua_tonumber(tolua_S,2,0));
  float y = ((float)  tolua_tonumber(tolua_S,3,0));
  float z = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetPosition(node,x,y,z);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetPosition'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeRotate */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeRotate00
static int tolua_kzu_object_kzuObjectNodeRotate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float angle = ((float)  tolua_tonumber(tolua_S,2,0));
  float x = ((float)  tolua_tonumber(tolua_S,3,0));
  float y = ((float)  tolua_tonumber(tolua_S,4,0));
  float z = ((float)  tolua_tonumber(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeRotate(node,angle,x,y,z);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeRotate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeScale */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeScale00
static int tolua_kzu_object_kzuObjectNodeScale00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float scale = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeScale(node,scale);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeScale'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeScaleXYZ */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeScaleXYZ00
static int tolua_kzu_object_kzuObjectNodeScaleXYZ00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float scaleX = ((float)  tolua_tonumber(tolua_S,2,0));
  float scaleY = ((float)  tolua_tonumber(tolua_S,3,0));
  float scaleZ = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeScaleXYZ(node,scaleX,scaleY,scaleZ);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeScaleXYZ'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeFindObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeFindObjectNode00
static int tolua_kzu_object_kzuObjectNodeFindObjectNode00(lua_State* tolua_S)
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
  const struct KzuObjectNode* rootNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* objectToFind = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeFindObjectNode(rootNode,objectToFind);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeFindObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeFindObjectNodeByName */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeFindObjectNodeByName00
static int tolua_kzu_object_kzuObjectNodeFindObjectNodeByName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuObjectNode* rootObjectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  char* objectName = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuObjectNodeFindObjectNodeByName(rootObjectNode,objectName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeFindObjectNodeByName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeFindObjectNodeContainingPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeFindObjectNodeContainingPropertyType00
static int tolua_kzu_object_kzuObjectNodeFindObjectNodeContainingPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuObjectNode* rootObjectNode = ((struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,2,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuObjectNodeFindObjectNodeContainingPropertyType(rootObjectNode,propertyType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeFindObjectNodeContainingPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetVisible */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetVisible00
static int tolua_kzu_object_kzuObjectNodeSetVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  kzBool visible = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetVisible(objectNode,visible);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetVisible'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeIsVisible */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeIsVisible00
static int tolua_kzu_object_kzuObjectNodeIsVisible00(lua_State* tolua_S)
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
   kzBool tolua_ret = (kzBool)  kzuObjectNodeIsVisible(objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeIsVisible'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetBoundingVolume */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetBoundingVolume00
static int tolua_kzu_object_kzuObjectNodeGetBoundingVolume00(lua_State* tolua_S)
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
   struct KzuBoundingVolume* tolua_ret = (struct KzuBoundingVolume*)  kzuObjectNodeGetBoundingVolume(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuBoundingVolume");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetBoundingVolume'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetBoundingVolumeUnion */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetBoundingVolumeUnion00
static int tolua_kzu_object_kzuObjectNodeGetBoundingVolumeUnion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVolume* out_volume = ((struct KzcVolume*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuObjectNodeGetBoundingVolumeUnion(objectNode,out_volume);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetBoundingVolumeUnion'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeRender */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeRender00
static int tolua_kzu_object_kzuObjectNodeRender00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuRenderer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuTransformedObjectNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuRenderer* renderer = ((struct KzuRenderer*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuTransformedObjectNode* transformedObjectNode = ((const struct KzuTransformedObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeRender(renderer,transformedObjectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeRender'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeCalculateRelativeTransformation */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeCalculateRelativeTransformation00
static int tolua_kzu_object_kzuObjectNodeCalculateRelativeTransformation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* rootObjectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  struct KzcMatrix4x4* out_transformation = ((struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,3,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeCalculateRelativeTransformation(rootObjectNode,objectNode,out_transformation);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeCalculateRelativeTransformation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeCalculateWorldTransformation */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeCalculateWorldTransformation00
static int tolua_kzu_object_kzuObjectNodeCalculateWorldTransformation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzcMatrix4x4* out_transformation = ((struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuObjectNodeCalculateWorldTransformation(objectNode,out_transformation);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeCalculateWorldTransformation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetPickable */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetPickable00
static int tolua_kzu_object_kzuObjectNodeSetPickable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  kzBool pickable = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetPickable(objectNode,pickable);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetPickable'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeIsPickable */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeIsPickable00
static int tolua_kzu_object_kzuObjectNodeIsPickable00(lua_State* tolua_S)
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
   kzBool tolua_ret = (kzBool)  kzuObjectNodeIsPickable(objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeIsPickable'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeIsPickable2D */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeIsPickable2D00
static int tolua_kzu_object_kzuObjectNodeIsPickable2D00(lua_State* tolua_S)
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
   kzBool tolua_ret = (kzBool)  kzuObjectNodeIsPickable2D(objectNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeIsPickable2D'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetPickable2D */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetPickable2D00
static int tolua_kzu_object_kzuObjectNodeSetPickable2D00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  kzBool pickable = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetPickable2D(objectNode,pickable);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetPickable2D'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeIsChildFromKzb */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeIsChildFromKzb00
static int tolua_kzu_object_kzuObjectNodeIsChildFromKzb00(lua_State* tolua_S)
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
  const struct KzuObjectNode* parent = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* child = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeIsChildFromKzb(parent,child);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeIsChildFromKzb'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeSetChildFromKzb */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeSetChildFromKzb00
static int tolua_kzu_object_kzuObjectNodeSetChildFromKzb00(lua_State* tolua_S)
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
  const struct KzuObjectNode* parent = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuObjectNode* child = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuObjectNodeSetChildFromKzb(parent,child);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeSetChildFromKzb'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetSize */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetSize00
static int tolua_kzu_object_kzuObjectNodeGetSize00(lua_State* tolua_S)
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
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuObjectNodeGetSize(node);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetSize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetMaximumBoundingBoxCoordinates */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetMaximumBoundingBoxCoordinates00
static int tolua_kzu_object_kzuObjectNodeGetMaximumBoundingBoxCoordinates00(lua_State* tolua_S)
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
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuObjectNodeGetMaximumBoundingBoxCoordinates(node);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetMaximumBoundingBoxCoordinates'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetMinimumBoundingBoxCoordinates */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetMinimumBoundingBoxCoordinates00
static int tolua_kzu_object_kzuObjectNodeGetMinimumBoundingBoxCoordinates00(lua_State* tolua_S)
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
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuObjectNodeGetMinimumBoundingBoxCoordinates(node);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetMinimumBoundingBoxCoordinates'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeIsLayout */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeIsLayout00
static int tolua_kzu_object_kzuObjectNodeIsLayout00(lua_State* tolua_S)
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
  const struct KzuObjectNode* node = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeIsLayout(node);
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
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeIsLayout'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetLayoutWidth */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetLayoutWidth00
static int tolua_kzu_object_kzuObjectNodeGetLayoutWidth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float width = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeGetLayoutWidth(objectNode,&width);
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
   tolua_pushnumber(tolua_S,(lua_Number)width);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetLayoutWidth'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetLayoutHeight */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetLayoutHeight00
static int tolua_kzu_object_kzuObjectNodeGetLayoutHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float height = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeGetLayoutHeight(objectNode,&height);
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
   tolua_pushnumber(tolua_S,(lua_Number)height);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetLayoutHeight'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetLayoutDepth */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetLayoutDepth00
static int tolua_kzu_object_kzuObjectNodeGetLayoutDepth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  float depth = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuObjectNodeGetLayoutDepth(objectNode,&depth);
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
   tolua_pushnumber(tolua_S,(lua_Number)depth);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetLayoutDepth'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetHorizontalAlignment */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetHorizontalAlignment00
static int tolua_kzu_object_kzuObjectNodeGetHorizontalAlignment00(lua_State* tolua_S)
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
   enum KzuObjectNodeHorizontalAlignment tolua_ret = (enum KzuObjectNodeHorizontalAlignment)  kzuObjectNodeGetHorizontalAlignment(objectNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetHorizontalAlignment'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetVerticalAlignment */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetVerticalAlignment00
static int tolua_kzu_object_kzuObjectNodeGetVerticalAlignment00(lua_State* tolua_S)
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
   enum KzuObjectNodeVerticalAlignment tolua_ret = (enum KzuObjectNodeVerticalAlignment)  kzuObjectNodeGetVerticalAlignment(objectNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetVerticalAlignment'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeGetDepthAlignment */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeGetDepthAlignment00
static int tolua_kzu_object_kzuObjectNodeGetDepthAlignment00(lua_State* tolua_S)
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
   enum KzuObjectNodeDepthAlignment tolua_ret = (enum KzuObjectNodeDepthAlignment)  kzuObjectNodeGetDepthAlignment(objectNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeGetDepthAlignment'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuObjectNodeCalculateUserRestrictedSize */
#ifndef TOLUA_DISABLE_tolua_kzu_object_kzuObjectNodeCalculateUserRestrictedSize00
static int tolua_kzu_object_kzuObjectNodeCalculateUserRestrictedSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuObjectNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzcVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuObjectNode* objectNode = ((const struct KzuObjectNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcVector3* aabbMin = ((const struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  const struct KzcVector3* aabbMax = ((const struct KzcVector3*)  tolua_tousertype(tolua_S,3,0));
  struct KzcVector3* out_aabbMin = ((struct KzcVector3*)  tolua_tousertype(tolua_S,4,0));
  struct KzcVector3* out_aabbMax = ((struct KzcVector3*)  tolua_tousertype(tolua_S,5,0));
  {
   kzuObjectNodeCalculateUserRestrictedSize(objectNode,aabbMin,aabbMax,out_aabbMin,out_aabbMax);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuObjectNodeCalculateUserRestrictedSize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_object_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuTransformedObjectNode","KzuTransformedObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuRenderer","KzuRenderer","",NULL);
  tolua_beginmodule(tolua_S,"KzuRenderer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyType","KzuPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcDynamicArray","KzcDynamicArray","",NULL);
  tolua_beginmodule(tolua_S,"KzcDynamicArray");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcVolume","KzcVolume","",NULL);
  tolua_beginmodule(tolua_S,"KzcVolume");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_LEFT",KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_LEFT);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_RIGHT",KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_RIGHT);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_CENTER",KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_CENTER);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_STRETCH",KZU_OBJECT_NODE_HORIZONTAL_ALIGNMENT_STRETCH);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_BOTTOM",KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_BOTTOM);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_TOP",KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_TOP);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_CENTER",KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_CENTER);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_STRETCH",KZU_OBJECT_NODE_VERTICAL_ALIGNMENT_STRETCH);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_DEPTH_ALIGNMENT_BACK",KZU_OBJECT_NODE_DEPTH_ALIGNMENT_BACK);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_DEPTH_ALIGNMENT_FRONT",KZU_OBJECT_NODE_DEPTH_ALIGNMENT_FRONT);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_DEPTH_ALIGNMENT_CENTER",KZU_OBJECT_NODE_DEPTH_ALIGNMENT_CENTER);
  tolua_constant(tolua_S,"KZU_OBJECT_NODE_DEPTH_ALIGNMENT_STRETCH",KZU_OBJECT_NODE_DEPTH_ALIGNMENT_STRETCH);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuObjectTypeIsRenderable",tolua_kzu_object_kzuObjectTypeIsRenderable00);
  tolua_function(tolua_S,"kzuObjectNodeDelete",tolua_kzu_object_kzuObjectNodeDelete00);
  tolua_function(tolua_S,"kzuObjectNodeTakeOwnership",tolua_kzu_object_kzuObjectNodeTakeOwnership00);
  tolua_function(tolua_S,"kzuObjectNodeIsValid",tolua_kzu_object_kzuObjectNodeIsValid00);
  tolua_function(tolua_S,"kzuObjectNodeTransferData",tolua_kzu_object_kzuObjectNodeTransferData00);
  tolua_function(tolua_S,"kzuObjectNodeGetPropertyManager",tolua_kzu_object_kzuObjectNodeGetPropertyManager00);
  tolua_function(tolua_S,"kzuObjectNodeCopyWithChildren",tolua_kzu_object_kzuObjectNodeCopyWithChildren00);
  tolua_function(tolua_S,"kzuObjectNodeGetParent",tolua_kzu_object_kzuObjectNodeGetParent00);
  tolua_function(tolua_S,"kzuObjectNodeGetRootNode",tolua_kzu_object_kzuObjectNodeGetRootNode00);
  tolua_function(tolua_S,"kzuObjectNodeAddChild",tolua_kzu_object_kzuObjectNodeAddChild00);
  tolua_function(tolua_S,"kzuObjectNodeAddChildAtIndex",tolua_kzu_object_kzuObjectNodeAddChildAtIndex00);
  tolua_function(tolua_S,"kzuObjectNodeRemoveChild",tolua_kzu_object_kzuObjectNodeRemoveChild00);
  tolua_function(tolua_S,"kzuObjectNodeRemoveAllChildren",tolua_kzu_object_kzuObjectNodeRemoveAllChildren00);
  tolua_function(tolua_S,"kzuObjectNodeHasChild",tolua_kzu_object_kzuObjectNodeHasChild00);
  tolua_function(tolua_S,"kzuObjectNodeGetChildCount",tolua_kzu_object_kzuObjectNodeGetChildCount00);
  tolua_function(tolua_S,"kzuObjectNodeGetChildren",tolua_kzu_object_kzuObjectNodeGetChildren00);
  tolua_function(tolua_S,"kzuObjectNodeGetChildAtIndex",tolua_kzu_object_kzuObjectNodeGetChildAtIndex00);
  tolua_function(tolua_S,"kzuObjectNodeGetChildByName",tolua_kzu_object_kzuObjectNodeGetChildByName00);
  tolua_function(tolua_S,"kzuObjectNodeSetName",tolua_kzu_object_kzuObjectNodeSetName00);
  tolua_function(tolua_S,"kzuObjectNodeGetName",tolua_kzu_object_kzuObjectNodeGetName00);
  tolua_function(tolua_S,"kzuObjectNodeGetType",tolua_kzu_object_kzuObjectNodeGetType00);
  tolua_function(tolua_S,"kzuObjectNodeSetTransformation",tolua_kzu_object_kzuObjectNodeSetTransformation00);
  tolua_function(tolua_S,"kzuObjectNodeGetTransformation",tolua_kzu_object_kzuObjectNodeGetTransformation00);
  tolua_function(tolua_S,"kzuObjectNodeGetPositionVector",tolua_kzu_object_kzuObjectNodeGetPositionVector00);
  tolua_function(tolua_S,"kzuObjectNodeSetIdentityTransformation",tolua_kzu_object_kzuObjectNodeSetIdentityTransformation00);
  tolua_function(tolua_S,"kzuObjectNodeSetIdentity3x3Transformation",tolua_kzu_object_kzuObjectNodeSetIdentity3x3Transformation00);
  tolua_function(tolua_S,"kzuObjectNodeTranslate",tolua_kzu_object_kzuObjectNodeTranslate00);
  tolua_function(tolua_S,"kzuObjectNodeSetPosition",tolua_kzu_object_kzuObjectNodeSetPosition00);
  tolua_function(tolua_S,"kzuObjectNodeRotate",tolua_kzu_object_kzuObjectNodeRotate00);
  tolua_function(tolua_S,"kzuObjectNodeScale",tolua_kzu_object_kzuObjectNodeScale00);
  tolua_function(tolua_S,"kzuObjectNodeScaleXYZ",tolua_kzu_object_kzuObjectNodeScaleXYZ00);
  tolua_function(tolua_S,"kzuObjectNodeFindObjectNode",tolua_kzu_object_kzuObjectNodeFindObjectNode00);
  tolua_function(tolua_S,"kzuObjectNodeFindObjectNodeByName",tolua_kzu_object_kzuObjectNodeFindObjectNodeByName00);
  tolua_function(tolua_S,"kzuObjectNodeFindObjectNodeContainingPropertyType",tolua_kzu_object_kzuObjectNodeFindObjectNodeContainingPropertyType00);
  tolua_function(tolua_S,"kzuObjectNodeSetVisible",tolua_kzu_object_kzuObjectNodeSetVisible00);
  tolua_function(tolua_S,"kzuObjectNodeIsVisible",tolua_kzu_object_kzuObjectNodeIsVisible00);
  tolua_function(tolua_S,"kzuObjectNodeGetBoundingVolume",tolua_kzu_object_kzuObjectNodeGetBoundingVolume00);
  tolua_function(tolua_S,"kzuObjectNodeGetBoundingVolumeUnion",tolua_kzu_object_kzuObjectNodeGetBoundingVolumeUnion00);
  tolua_function(tolua_S,"kzuObjectNodeRender",tolua_kzu_object_kzuObjectNodeRender00);
  tolua_function(tolua_S,"kzuObjectNodeCalculateRelativeTransformation",tolua_kzu_object_kzuObjectNodeCalculateRelativeTransformation00);
  tolua_function(tolua_S,"kzuObjectNodeCalculateWorldTransformation",tolua_kzu_object_kzuObjectNodeCalculateWorldTransformation00);
  tolua_function(tolua_S,"kzuObjectNodeSetPickable",tolua_kzu_object_kzuObjectNodeSetPickable00);
  tolua_function(tolua_S,"kzuObjectNodeIsPickable",tolua_kzu_object_kzuObjectNodeIsPickable00);
  tolua_function(tolua_S,"kzuObjectNodeIsPickable2D",tolua_kzu_object_kzuObjectNodeIsPickable2D00);
  tolua_function(tolua_S,"kzuObjectNodeSetPickable2D",tolua_kzu_object_kzuObjectNodeSetPickable2D00);
  tolua_function(tolua_S,"kzuObjectNodeIsChildFromKzb",tolua_kzu_object_kzuObjectNodeIsChildFromKzb00);
  tolua_function(tolua_S,"kzuObjectNodeSetChildFromKzb",tolua_kzu_object_kzuObjectNodeSetChildFromKzb00);
  tolua_function(tolua_S,"kzuObjectNodeGetSize",tolua_kzu_object_kzuObjectNodeGetSize00);
  tolua_function(tolua_S,"kzuObjectNodeGetMaximumBoundingBoxCoordinates",tolua_kzu_object_kzuObjectNodeGetMaximumBoundingBoxCoordinates00);
  tolua_function(tolua_S,"kzuObjectNodeGetMinimumBoundingBoxCoordinates",tolua_kzu_object_kzuObjectNodeGetMinimumBoundingBoxCoordinates00);
  tolua_function(tolua_S,"kzuObjectNodeIsLayout",tolua_kzu_object_kzuObjectNodeIsLayout00);
  tolua_function(tolua_S,"kzuObjectNodeGetLayoutWidth",tolua_kzu_object_kzuObjectNodeGetLayoutWidth00);
  tolua_function(tolua_S,"kzuObjectNodeGetLayoutHeight",tolua_kzu_object_kzuObjectNodeGetLayoutHeight00);
  tolua_function(tolua_S,"kzuObjectNodeGetLayoutDepth",tolua_kzu_object_kzuObjectNodeGetLayoutDepth00);
  tolua_function(tolua_S,"kzuObjectNodeGetHorizontalAlignment",tolua_kzu_object_kzuObjectNodeGetHorizontalAlignment00);
  tolua_function(tolua_S,"kzuObjectNodeGetVerticalAlignment",tolua_kzu_object_kzuObjectNodeGetVerticalAlignment00);
  tolua_function(tolua_S,"kzuObjectNodeGetDepthAlignment",tolua_kzu_object_kzuObjectNodeGetDepthAlignment00);
  tolua_function(tolua_S,"kzuObjectNodeCalculateUserRestrictedSize",tolua_kzu_object_kzuObjectNodeCalculateUserRestrictedSize00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_object (lua_State* tolua_S) {
 return tolua_kzu_object_open(tolua_S);
}
#endif

