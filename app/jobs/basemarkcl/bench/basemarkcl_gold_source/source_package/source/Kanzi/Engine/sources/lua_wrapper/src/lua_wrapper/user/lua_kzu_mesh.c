/*
** Lua binding: kzu_mesh
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_mesh_open (lua_State* tolua_S);

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
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_object_base.h>
#include <core/resource_manager/kzc_resource_memory_type.h>
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
 tolua_usertype(tolua_S,"KzcVertexBuffer");
 tolua_usertype(tolua_S,"KzuMaterial");
 tolua_usertype(tolua_S,"KzcVector3");
 tolua_usertype(tolua_S,"KzuMeshBone");
 tolua_usertype(tolua_S,"KzcIndexBuffer");
 tolua_usertype(tolua_S,"const*");
 tolua_usertype(tolua_S,"KzuMesh");
 tolua_usertype(tolua_S,"KzcHashMap");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"KzuPlaneAxes");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"kzU16");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzcResourceMemoryType");
 tolua_usertype(tolua_S,"KzuSkeleton");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"KzuMeshCluster");
 tolua_usertype(tolua_S,"KzuMeshNode");
 tolua_usertype(tolua_S,"KzcMatrix4x4");
 tolua_usertype(tolua_S,"KzcVertexList");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzcResourceManager");
 tolua_usertype(tolua_S,"KzuBoundingVolume");
 tolua_usertype(tolua_S,"KzuClusterDefinition");
}

/* function: kzuMeshCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshCreate00
static int tolua_kzu_mesh_kzuMeshCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcResourceMemoryType",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,3,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  enum KzcResourceMemoryType memoryType = *((enum KzcResourceMemoryType*)  tolua_tousertype(tolua_S,2,0));
  struct KzuMesh* out_mesh = ((struct KzuMesh*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshCreate(memoryManager,memoryType,&out_mesh);
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
    tolua_pushusertype(tolua_S,(void*)out_mesh,"KzuMesh");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */


/* function: kzuMeshDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshDelete00
static int tolua_kzu_mesh_kzuMeshDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMesh* mesh = ((struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshDelete(mesh);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshNodeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshNodeDelete00
static int tolua_kzu_mesh_kzuMeshNodeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMeshNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMeshNode* meshNode = ((struct KzuMeshNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshNodeDelete(meshNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshNodeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshTakeOwnership */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshTakeOwnership00
static int tolua_kzu_mesh_kzuMeshTakeOwnership00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuMeshTakeOwnership(mesh);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshTakeOwnership'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshIsValid */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshIsValid00
static int tolua_kzu_mesh_kzuMeshIsValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuMeshIsValid(mesh);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshIsValid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshTransferData */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshTransferData00
static int tolua_kzu_mesh_kzuMeshTransferData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMesh* targetMesh = ((struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMesh* sourceMesh = ((struct KzuMesh*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMeshTransferData(targetMesh,sourceMesh);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshTransferData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshAssignData */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshAssignData00
static int tolua_kzu_mesh_kzuMeshAssignData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcResourceManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcVertexList",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuClusterDefinition",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,6,"KzcHashMap",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzcResourceManager* resourceManager = ((struct KzcResourceManager*)  tolua_tousertype(tolua_S,2,0));
  struct KzcVertexList* vertexList = ((struct KzcVertexList*)  tolua_tousertype(tolua_S,3,0));
  const struct KzuClusterDefinition* clusterDefinitions = ((const struct KzuClusterDefinition*)  tolua_tousertype(tolua_S,4,0));
  unsigned int clusterCount = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
  const struct KzcHashMap* vertexAttributeMapping = ((const struct KzcHashMap*)  tolua_tousertype(tolua_S,6,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshAssignData(mesh,resourceManager,vertexList,clusterDefinitions,clusterCount,vertexAttributeMapping);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshAssignData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshSetMorphTargets */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshSetMorphTargets00
static int tolua_kzu_mesh_kzuMeshSetMorphTargets00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const*",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  unsigned int numMorphTargets = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  struct KzuMesh* const* morphTargets = *((struct KzuMesh* const**)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshSetMorphTargets(mesh,numMorphTargets,morphTargets);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshSetMorphTargets'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshInterpolateMorphTargets */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshInterpolateMorphTargets00
static int tolua_kzu_mesh_kzuMeshInterpolateMorphTargets00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMesh",0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,3,0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,4,0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,2,0));
  void* out_positions = ((void*)  tolua_touserdata(tolua_S,3,0));
  void* out_normals = ((void*)  tolua_touserdata(tolua_S,4,0));
  void* out_tangents = ((void*)  tolua_touserdata(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshInterpolateMorphTargets(memoryManager,mesh,&out_positions,&out_normals,&out_tangents);
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
   tolua_pushuserdata(tolua_S,(void*)out_positions);
   tolua_pushuserdata(tolua_S,(void*)out_normals);
   tolua_pushuserdata(tolua_S,(void*)out_tangents);
  }
 }
 return 4;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshInterpolateMorphTargets'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshNodeSetMesh */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshNodeSetMesh00
static int tolua_kzu_mesh_kzuMeshNodeSetMesh00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMeshNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMeshNode* meshNode = ((const struct KzuMeshNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMesh* mesh = ((struct KzuMesh*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMeshNodeSetMesh(meshNode,mesh);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshNodeSetMesh'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshNodeGetMesh */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshNodeGetMesh00
static int tolua_kzu_mesh_kzuMeshNodeGetMesh00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMeshNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMeshNode* meshNode = ((const struct KzuMeshNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuMesh* tolua_ret = (struct KzuMesh*)  kzuMeshNodeGetMesh(meshNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMesh");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshNodeGetMesh'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshNodeFromObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshNodeFromObjectNode00
static int tolua_kzu_mesh_kzuMeshNodeFromObjectNode00(lua_State* tolua_S)
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
   struct KzuMeshNode* tolua_ret = (struct KzuMeshNode*)  kzuMeshNodeFromObjectNode(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMeshNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshNodeFromObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshNodeToObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshNodeToObjectNode00
static int tolua_kzu_mesh_kzuMeshNodeToObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMeshNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMeshNode* meshNode = ((struct KzuMeshNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuMeshNodeToObjectNode(meshNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshNodeToObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshAssignBoneData */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshAssignBoneData00
static int tolua_kzu_mesh_kzuMeshAssignBoneData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const*",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,4,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  unsigned int boneCount = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  struct KzuObjectNode* const* bones = *((struct KzuObjectNode* const**)  tolua_tousertype(tolua_S,3,0));
  const struct KzcMatrix4x4* invBindMatrices = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshAssignBoneData(mesh,boneCount,bones,invBindMatrices);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshAssignBoneData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetSkeletonBones */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetSkeletonBones00
static int tolua_kzu_mesh_kzuMeshGetSkeletonBones00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuMeshBone* tolua_ret = (struct KzuMeshBone*)  kzuMeshGetSkeletonBones(mesh);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMeshBone");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetSkeletonBones'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetSkeleton */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetSkeleton00
static int tolua_kzu_mesh_kzuMeshGetSkeleton00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuSkeleton* tolua_ret = (struct KzuSkeleton*)  kzuMeshGetSkeleton(mesh);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuSkeleton");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetSkeleton'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshSetBindShapeMatrix */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshSetBindShapeMatrix00
static int tolua_kzu_mesh_kzuMeshSetBindShapeMatrix00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix4x4* bindShapeMatrix = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMeshSetBindShapeMatrix(mesh,bindShapeMatrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshSetBindShapeMatrix'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetBindShapeMatrix */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetBindShapeMatrix00
static int tolua_kzu_mesh_kzuMeshGetBindShapeMatrix00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcMatrix4x4 tolua_ret =   kzuMeshGetBindShapeMatrix(mesh);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetBindShapeMatrix'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetVertexBuffer */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetVertexBuffer00
static int tolua_kzu_mesh_kzuMeshGetVertexBuffer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVertexBuffer* tolua_ret = (struct KzcVertexBuffer*)  kzuMeshGetVertexBuffer(mesh);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcVertexBuffer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetVertexBuffer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetIndexBuffer */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetIndexBuffer00
static int tolua_kzu_mesh_kzuMeshGetIndexBuffer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcIndexBuffer* tolua_ret = (struct KzcIndexBuffer*)  kzuMeshGetIndexBuffer(mesh);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcIndexBuffer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetIndexBuffer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshClusterGetIndexBuffer */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshClusterGetIndexBuffer00
static int tolua_kzu_mesh_kzuMeshClusterGetIndexBuffer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMeshCluster",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMeshCluster* meshCluster = ((const struct KzuMeshCluster*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcIndexBuffer* tolua_ret = (struct KzcIndexBuffer*)  kzuMeshClusterGetIndexBuffer(meshCluster);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcIndexBuffer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshClusterGetIndexBuffer'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshSetBoundingVolume */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshSetBoundingVolume00
static int tolua_kzu_mesh_kzuMeshSetBoundingVolume00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzuBoundingVolume* boundingVolume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMeshSetBoundingVolume(mesh,boundingVolume);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshSetBoundingVolume'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetBoundingVolume */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetBoundingVolume00
static int tolua_kzu_mesh_kzuMeshGetBoundingVolume00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuBoundingVolume* tolua_ret = (struct KzuBoundingVolume*)  kzuMeshGetBoundingVolume(mesh);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuBoundingVolume");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetBoundingVolume'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGenerateSphere */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGenerateSphere00
static int tolua_kzu_mesh_kzuMeshGenerateSphere00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcResourceManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzcResourceManager* resourceManager = ((struct KzcResourceManager*)  tolua_tousertype(tolua_S,2,0));
  float radius = ((float)  tolua_tonumber(tolua_S,3,0));
  const unsigned int numSegments = ((const unsigned int)  tolua_tonumber(tolua_S,4,0));
  const unsigned int numRows = ((const unsigned int)  tolua_tonumber(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshGenerateSphere(mesh,resourceManager,radius,numSegments,numRows);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGenerateSphere'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGenerateMorphSphere */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGenerateMorphSphere00
static int tolua_kzu_mesh_kzuMeshGenerateMorphSphere00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcResourceManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzcResourceManager* resourceManager = ((struct KzcResourceManager*)  tolua_tousertype(tolua_S,2,0));
  float radius = ((float)  tolua_tonumber(tolua_S,3,0));
  const unsigned int numSegments = ((const unsigned int)  tolua_tonumber(tolua_S,4,0));
  const unsigned int numRows = ((const unsigned int)  tolua_tonumber(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshGenerateMorphSphere(mesh,resourceManager,radius,numSegments,numRows);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGenerateMorphSphere'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGeneratePlaneWithOffset */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGeneratePlaneWithOffset00
static int tolua_kzu_mesh_kzuMeshGeneratePlaneWithOffset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcResourceManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,7,&tolua_err) || !tolua_isusertype(tolua_S,7,"KzuPlaneAxes",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,8,&tolua_err) || !tolua_isusertype(tolua_S,8,"kzBool",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,9,&tolua_err) || !tolua_isusertype(tolua_S,9,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,10,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzcResourceManager* resourceManager = ((struct KzcResourceManager*)  tolua_tousertype(tolua_S,2,0));
  float xOffset = ((float)  tolua_tonumber(tolua_S,3,0));
  float yOffset = ((float)  tolua_tonumber(tolua_S,4,0));
  float width = ((float)  tolua_tonumber(tolua_S,5,0));
  float height = ((float)  tolua_tonumber(tolua_S,6,0));
  enum KzuPlaneAxes planeAxes = *((enum KzuPlaneAxes*)  tolua_tousertype(tolua_S,7,0));
  kzBool invertU = *((kzBool*)  tolua_tousertype(tolua_S,8,0));
  kzBool invertV = *((kzBool*)  tolua_tousertype(tolua_S,9,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshGeneratePlaneWithOffset(mesh,resourceManager,xOffset,yOffset,width,height,planeAxes,invertU,invertV);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGeneratePlaneWithOffset'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGeneratePlane */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGeneratePlane00
static int tolua_kzu_mesh_kzuMeshGeneratePlane00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcResourceManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"KzuPlaneAxes",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,6,&tolua_err) || !tolua_isusertype(tolua_S,6,"kzBool",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,7,&tolua_err) || !tolua_isusertype(tolua_S,7,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzcResourceManager* resourceManager = ((struct KzcResourceManager*)  tolua_tousertype(tolua_S,2,0));
  float width = ((float)  tolua_tonumber(tolua_S,3,0));
  float height = ((float)  tolua_tonumber(tolua_S,4,0));
  enum KzuPlaneAxes planeAxes = *((enum KzuPlaneAxes*)  tolua_tousertype(tolua_S,5,0));
  kzBool invertU = *((kzBool*)  tolua_tousertype(tolua_S,6,0));
  kzBool invertV = *((kzBool*)  tolua_tousertype(tolua_S,7,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshGeneratePlane(mesh,resourceManager,width,height,planeAxes,invertU,invertV);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGeneratePlane'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGenerateBox */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGenerateBox00
static int tolua_kzu_mesh_kzuMeshGenerateBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcResourceManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzcResourceManager* resourceManager = ((struct KzcResourceManager*)  tolua_tousertype(tolua_S,2,0));
  float size = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshGenerateBox(mesh,resourceManager,size);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGenerateBox'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGenerateBoxFreeSize */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGenerateBoxFreeSize00
static int tolua_kzu_mesh_kzuMeshGenerateBoxFreeSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcResourceManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzcResourceManager* resourceManager = ((struct KzcResourceManager*)  tolua_tousertype(tolua_S,2,0));
  float sizeX = ((float)  tolua_tonumber(tolua_S,3,0));
  float sizeY = ((float)  tolua_tonumber(tolua_S,4,0));
  float sizeZ = ((float)  tolua_tonumber(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshGenerateBoxFreeSize(mesh,resourceManager,sizeX,sizeY,sizeZ);
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
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGenerateBoxFreeSize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshCalculateNormals */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshCalculateNormals00
static int tolua_kzu_mesh_kzuMeshCalculateNormals00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"kzU16",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,6,"KzcVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const kzU16* indices = ((const kzU16*)  tolua_tousertype(tolua_S,2,0));
  const unsigned int indiceCount = ((const unsigned int)  tolua_tonumber(tolua_S,3,0));
  const float vertices = ((const float)  tolua_tonumber(tolua_S,4,0));
  const unsigned int verticeCount = ((const unsigned int)  tolua_tonumber(tolua_S,5,0));
  struct KzcVector3* out_normal = ((struct KzcVector3*)  tolua_tousertype(tolua_S,6,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshCalculateNormals(memoryManager,indices,indiceCount,&vertices,verticeCount,&out_normal);
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
   tolua_pushnumber(tolua_S,(lua_Number)vertices);
    tolua_pushusertype(tolua_S,(void*)out_normal,"KzcVector3");
  }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshCalculateNormals'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetClusterCount */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetClusterCount00
static int tolua_kzu_mesh_kzuMeshGetClusterCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuMeshGetClusterCount(mesh);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetClusterCount'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetClusterAtIndex */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetClusterAtIndex00
static int tolua_kzu_mesh_kzuMeshGetClusterAtIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  unsigned int clusterIndex = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   struct KzuMeshCluster* tolua_ret = (struct KzuMeshCluster*)  kzuMeshGetClusterAtIndex(mesh,clusterIndex);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMeshCluster");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetClusterAtIndex'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshSetMaterial */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshSetMaterial00
static int tolua_kzu_mesh_kzuMeshSetMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMaterial* material = ((struct KzuMaterial*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMeshSetMaterial(mesh,material);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshSetMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshClusterSetMaterial */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshClusterSetMaterial00
static int tolua_kzu_mesh_kzuMeshClusterSetMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMeshCluster",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMaterial",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuMeshCluster* meshCluster = ((struct KzuMeshCluster*)  tolua_tousertype(tolua_S,1,0));
  struct KzuMaterial* material = ((struct KzuMaterial*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuMeshClusterSetMaterial(meshCluster,material);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshClusterSetMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetMaterial */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetMaterial00
static int tolua_kzu_mesh_kzuMeshGetMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuMaterial* tolua_ret = (struct KzuMaterial*)  kzuMeshGetMaterial(mesh);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMaterial");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshClusterGetMaterial */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshClusterGetMaterial00
static int tolua_kzu_mesh_kzuMeshClusterGetMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuMeshCluster",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuMeshCluster* meshCluster = ((const struct KzuMeshCluster*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuMaterial* tolua_ret = (struct KzuMaterial*)  kzuMeshClusterGetMaterial(meshCluster);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuMaterial");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshClusterGetMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuMeshGetVertices */
#ifndef TOLUA_DISABLE_tolua_kzu_mesh_kzuMeshGetVertices00
static int tolua_kzu_mesh_kzuMeshGetVertices00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuMesh",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcDynamicArray",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuMesh* mesh = ((const struct KzuMesh*)  tolua_tousertype(tolua_S,2,0));
  struct KzcDynamicArray* out_vertices = ((struct KzcDynamicArray*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuMeshGetVertices(memoryManager,mesh,&out_vertices);
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
    tolua_pushusertype(tolua_S,(void*)out_vertices,"KzcDynamicArray");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuMeshGetVertices'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_mesh_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMaterial","KzuMaterial","",NULL);
  tolua_beginmodule(tolua_S,"KzuMaterial");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuBoundingVolume","KzuBoundingVolume","",NULL);
  tolua_beginmodule(tolua_S,"KzuBoundingVolume");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcVertexBuffer","KzcVertexBuffer","",NULL);
  tolua_beginmodule(tolua_S,"KzcVertexBuffer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcIndexBuffer","KzcIndexBuffer","",NULL);
  tolua_beginmodule(tolua_S,"KzcIndexBuffer");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcVertexList","KzcVertexList","",NULL);
  tolua_beginmodule(tolua_S,"KzcVertexList");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcResourceManager","KzcResourceManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcResourceManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcHashMap","KzcHashMap","",NULL);
  tolua_beginmodule(tolua_S,"KzcHashMap");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcDynamicArray","KzcDynamicArray","",NULL);
  tolua_beginmodule(tolua_S,"KzcDynamicArray");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMeshNode","KzuMeshNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuMeshNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMesh","KzuMesh","",NULL);
  tolua_beginmodule(tolua_S,"KzuMesh");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuMeshCluster","KzuMeshCluster","",NULL);
  tolua_beginmodule(tolua_S,"KzuMeshCluster");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuClusterDefinition","KzuClusterDefinition","",NULL);
  tolua_beginmodule(tolua_S,"KzuClusterDefinition");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuMeshCreate",tolua_kzu_mesh_kzuMeshCreate00);
  tolua_function(tolua_S,"kzuMeshDelete",tolua_kzu_mesh_kzuMeshDelete00);
  tolua_function(tolua_S,"kzuMeshNodeDelete",tolua_kzu_mesh_kzuMeshNodeDelete00);
  tolua_function(tolua_S,"kzuMeshTakeOwnership",tolua_kzu_mesh_kzuMeshTakeOwnership00);
  tolua_function(tolua_S,"kzuMeshIsValid",tolua_kzu_mesh_kzuMeshIsValid00);
  tolua_function(tolua_S,"kzuMeshTransferData",tolua_kzu_mesh_kzuMeshTransferData00);
  tolua_function(tolua_S,"kzuMeshAssignData",tolua_kzu_mesh_kzuMeshAssignData00);
  tolua_function(tolua_S,"kzuMeshSetMorphTargets",tolua_kzu_mesh_kzuMeshSetMorphTargets00);
  tolua_function(tolua_S,"kzuMeshInterpolateMorphTargets",tolua_kzu_mesh_kzuMeshInterpolateMorphTargets00);
  tolua_function(tolua_S,"kzuMeshNodeSetMesh",tolua_kzu_mesh_kzuMeshNodeSetMesh00);
  tolua_function(tolua_S,"kzuMeshNodeGetMesh",tolua_kzu_mesh_kzuMeshNodeGetMesh00);
  tolua_function(tolua_S,"kzuMeshNodeFromObjectNode",tolua_kzu_mesh_kzuMeshNodeFromObjectNode00);
  tolua_function(tolua_S,"kzuMeshNodeToObjectNode",tolua_kzu_mesh_kzuMeshNodeToObjectNode00);
  tolua_function(tolua_S,"kzuMeshAssignBoneData",tolua_kzu_mesh_kzuMeshAssignBoneData00);
  tolua_function(tolua_S,"kzuMeshGetSkeletonBones",tolua_kzu_mesh_kzuMeshGetSkeletonBones00);
  tolua_function(tolua_S,"kzuMeshGetSkeleton",tolua_kzu_mesh_kzuMeshGetSkeleton00);
  tolua_function(tolua_S,"kzuMeshSetBindShapeMatrix",tolua_kzu_mesh_kzuMeshSetBindShapeMatrix00);
  tolua_function(tolua_S,"kzuMeshGetBindShapeMatrix",tolua_kzu_mesh_kzuMeshGetBindShapeMatrix00);
  tolua_function(tolua_S,"kzuMeshGetVertexBuffer",tolua_kzu_mesh_kzuMeshGetVertexBuffer00);
  tolua_function(tolua_S,"kzuMeshGetIndexBuffer",tolua_kzu_mesh_kzuMeshGetIndexBuffer00);
  tolua_function(tolua_S,"kzuMeshClusterGetIndexBuffer",tolua_kzu_mesh_kzuMeshClusterGetIndexBuffer00);
  tolua_function(tolua_S,"kzuMeshSetBoundingVolume",tolua_kzu_mesh_kzuMeshSetBoundingVolume00);
  tolua_function(tolua_S,"kzuMeshGetBoundingVolume",tolua_kzu_mesh_kzuMeshGetBoundingVolume00);
  tolua_function(tolua_S,"kzuMeshGenerateSphere",tolua_kzu_mesh_kzuMeshGenerateSphere00);
  tolua_function(tolua_S,"kzuMeshGenerateMorphSphere",tolua_kzu_mesh_kzuMeshGenerateMorphSphere00);
  tolua_function(tolua_S,"kzuMeshGeneratePlaneWithOffset",tolua_kzu_mesh_kzuMeshGeneratePlaneWithOffset00);
  tolua_function(tolua_S,"kzuMeshGeneratePlane",tolua_kzu_mesh_kzuMeshGeneratePlane00);
  tolua_function(tolua_S,"kzuMeshGenerateBox",tolua_kzu_mesh_kzuMeshGenerateBox00);
  tolua_function(tolua_S,"kzuMeshGenerateBoxFreeSize",tolua_kzu_mesh_kzuMeshGenerateBoxFreeSize00);
  tolua_function(tolua_S,"kzuMeshCalculateNormals",tolua_kzu_mesh_kzuMeshCalculateNormals00);
  tolua_function(tolua_S,"kzuMeshGetClusterCount",tolua_kzu_mesh_kzuMeshGetClusterCount00);
  tolua_function(tolua_S,"kzuMeshGetClusterAtIndex",tolua_kzu_mesh_kzuMeshGetClusterAtIndex00);
  tolua_function(tolua_S,"kzuMeshSetMaterial",tolua_kzu_mesh_kzuMeshSetMaterial00);
  tolua_function(tolua_S,"kzuMeshClusterSetMaterial",tolua_kzu_mesh_kzuMeshClusterSetMaterial00);
  tolua_function(tolua_S,"kzuMeshGetMaterial",tolua_kzu_mesh_kzuMeshGetMaterial00);
  tolua_function(tolua_S,"kzuMeshClusterGetMaterial",tolua_kzu_mesh_kzuMeshClusterGetMaterial00);
  tolua_function(tolua_S,"kzuMeshGetVertices",tolua_kzu_mesh_kzuMeshGetVertices00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_mesh (lua_State* tolua_S) {
 return tolua_kzu_mesh_open(tolua_S);
}
#endif

