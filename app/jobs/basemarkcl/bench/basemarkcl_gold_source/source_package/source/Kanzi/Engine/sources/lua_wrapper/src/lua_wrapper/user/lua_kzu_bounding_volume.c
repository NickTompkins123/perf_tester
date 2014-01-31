/*
** Lua binding: kzu_bounding_volume
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_bounding_volume_open (lua_State* tolua_S);

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
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <system/debug/kzs_error.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_plane.h>

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
 tolua_usertype(tolua_S,"KzcVolume");
 tolua_usertype(tolua_S,"KzcVector3");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuBoundingVolumeSphere");
 tolua_usertype(tolua_S,"KzuBoundingVolumeAxisAlignedBox");
 tolua_usertype(tolua_S,"KzcMatrix4x4");
 tolua_usertype(tolua_S,"KzcVertexList");
 tolua_usertype(tolua_S,"KzuTransformedBoundingVolume");
 tolua_usertype(tolua_S,"KzcPlane");
 tolua_usertype(tolua_S,"KzuBoundingVolume");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuBoundingVolumeCreateEmpty */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeCreateEmpty00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeCreateEmpty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuBoundingVolume* out_volume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuBoundingVolumeCreateEmpty(memoryManager,&out_volume);
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
    tolua_pushusertype(tolua_S,(void*)out_volume,"KzuBoundingVolume");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeCreateEmpty'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeCreate00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVertexList",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcVertexList* vertexList = ((const struct KzcVertexList*)  tolua_tousertype(tolua_S,2,0));
  struct KzuBoundingVolume* out_volume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuBoundingVolumeCreate(memoryManager,vertexList,&out_volume);
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
    tolua_pushusertype(tolua_S,(void*)out_volume,"KzuBoundingVolume");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeDelete00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoundingVolume* boundingVolume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuBoundingVolumeDelete(boundingVolume);
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
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeCalculateTransformedBoundingVolume */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeCalculateTransformedBoundingVolume00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeCalculateTransformedBoundingVolume00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuTransformedBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoundingVolume* boundingVolume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix4x4* worldTransformation = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  struct KzuTransformedBoundingVolume* out_transformedBoundingVolume = ((struct KzuTransformedBoundingVolume*)  tolua_tousertype(tolua_S,3,0));
  {
   kzuBoundingVolumeCalculateTransformedBoundingVolume(boundingVolume,worldTransformation,out_transformedBoundingVolume);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeCalculateTransformedBoundingVolume'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuAxisAlignedBoundingBoxFromTransformedAABB */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuAxisAlignedBoundingBoxFromTransformedAABB00
static int tolua_kzu_bounding_volume_kzuAxisAlignedBoundingBoxFromTransformedAABB00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzcVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcVector3* aabbMin = ((const struct KzcVector3*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcVector3* aabbMax = ((const struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  const struct KzcMatrix4x4* transform = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,3,0));
  struct KzcVector3* out_aabbMin = ((struct KzcVector3*)  tolua_tousertype(tolua_S,4,0));
  struct KzcVector3* out_aabbMax = ((struct KzcVector3*)  tolua_tousertype(tolua_S,5,0));
  {
   kzuAxisAlignedBoundingBoxFromTransformedAABB(aabbMin,aabbMax,transform,out_aabbMin,out_aabbMax);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuAxisAlignedBoundingBoxFromTransformedAABB'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTransformedBoundingVolumeCreateFromAABB */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeCreateFromAABB00
static int tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeCreateFromAABB00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuTransformedBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcVector3* aabbMin = ((const struct KzcVector3*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcVector3* aabbMax = ((const struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  const struct KzcMatrix4x4* worldTransformation = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,3,0));
  struct KzuTransformedBoundingVolume* out_transformedBoundingVolume = ((struct KzuTransformedBoundingVolume*)  tolua_tousertype(tolua_S,4,0));
  {
   kzuTransformedBoundingVolumeCreateFromAABB(aabbMin,aabbMax,worldTransformation,out_transformedBoundingVolume);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTransformedBoundingVolumeCreateFromAABB'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTransformedBoundingVolumeGetAABB */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeGetAABB00
static int tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeGetAABB00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTransformedBoundingVolume",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcVector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTransformedBoundingVolume* transformedBoundingVolume = ((const struct KzuTransformedBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector3* out_minimum = ((struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  struct KzcVector3* out_maximum = ((struct KzcVector3*)  tolua_tousertype(tolua_S,3,0));
  {
   kzuTransformedBoundingVolumeGetAABB(transformedBoundingVolume,out_minimum,out_maximum);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuTransformedBoundingVolumeGetAABB'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTransformedBoundingVolumeIsAbovePlane */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeIsAbovePlane00
static int tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeIsAbovePlane00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTransformedBoundingVolume",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcPlane",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTransformedBoundingVolume* transformedBoundingVolume = ((const struct KzuTransformedBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcPlane* plane = ((const struct KzcPlane*)  tolua_tousertype(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuTransformedBoundingVolumeIsAbovePlane(transformedBoundingVolume,plane);
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
 tolua_error(tolua_S,"#ferror in function 'kzuTransformedBoundingVolumeIsAbovePlane'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuTransformedBoundingVolumeBoxGetCornerPoint */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeBoxGetCornerPoint00
static int tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeBoxGetCornerPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuTransformedBoundingVolume",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuTransformedBoundingVolume* boundingVolume = ((const struct KzuTransformedBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  unsigned int cornerIndex = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   struct KzcVector3 tolua_ret =   kzuTransformedBoundingVolumeBoxGetCornerPoint(boundingVolume,cornerIndex);
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
 tolua_error(tolua_S,"#ferror in function 'kzuTransformedBoundingVolumeBoxGetCornerPoint'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeSetVolume */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeSetVolume00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeSetVolume00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoundingVolume* boundingVolume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcVolume* volume = ((const struct KzcVolume*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuBoundingVolumeSetVolume(boundingVolume,volume);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeSetVolume'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeGetAxisAlignedBox */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeGetAxisAlignedBox00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeGetAxisAlignedBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoundingVolume* boundingVolume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuBoundingVolumeAxisAlignedBox* tolua_ret = (struct KzuBoundingVolumeAxisAlignedBox*)  kzuBoundingVolumeGetAxisAlignedBox(boundingVolume);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuBoundingVolumeAxisAlignedBox");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeGetAxisAlignedBox'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeGetSphere */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeGetSphere00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeGetSphere00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolume",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuBoundingVolume* boundingVolume = ((struct KzuBoundingVolume*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuBoundingVolumeSphere* tolua_ret = (struct KzuBoundingVolumeSphere*)  kzuBoundingVolumeGetSphere(boundingVolume);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuBoundingVolumeSphere");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeGetSphere'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolumeAxisAlignedBox",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuBoundingVolumeAxisAlignedBox* boundingVolume = ((const struct KzuBoundingVolumeAxisAlignedBox*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(boundingVolume);
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
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolumeAxisAlignedBox",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuBoundingVolumeAxisAlignedBox* boundingVolume = ((const struct KzuBoundingVolumeAxisAlignedBox*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcVector3 tolua_ret =   kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(boundingVolume);
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
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuBoundingVolumeSphereGetRadius */
#ifndef TOLUA_DISABLE_tolua_kzu_bounding_volume_kzuBoundingVolumeSphereGetRadius00
static int tolua_kzu_bounding_volume_kzuBoundingVolumeSphereGetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuBoundingVolumeSphere",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuBoundingVolumeSphere* boundingVolume = ((const struct KzuBoundingVolumeSphere*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuBoundingVolumeSphereGetRadius(boundingVolume);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuBoundingVolumeSphereGetRadius'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_bounding_volume_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"KZU_BOUNDING_VOLUME_TYPE_AXIS_ALIGNED_BOX",KZU_BOUNDING_VOLUME_TYPE_AXIS_ALIGNED_BOX);
  tolua_constant(tolua_S,"KZU_BOUNDING_VOLUME_TYPE_SPHERE",KZU_BOUNDING_VOLUME_TYPE_SPHERE);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcPlane","KzcPlane","",NULL);
  tolua_beginmodule(tolua_S,"KzcPlane");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcVertexList","KzcVertexList","",NULL);
  tolua_beginmodule(tolua_S,"KzcVertexList");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMatrix4x4","KzcMatrix4x4","",NULL);
  tolua_beginmodule(tolua_S,"KzcMatrix4x4");
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"KzcVector3","KzcVector3","",tolua_collect_KzcVector3);
  #else
  tolua_cclass(tolua_S,"KzcVector3","KzcVector3","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"KzcVector3");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcVolume","KzcVolume","",NULL);
  tolua_beginmodule(tolua_S,"KzcVolume");
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_LEFT",KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_LEFT);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_BACK_TOP_LEFT",KZU_BOUNDING_BOX_CORNER_BACK_TOP_LEFT);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_BACK_TOP_RIGHT",KZU_BOUNDING_BOX_CORNER_BACK_TOP_RIGHT);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_RIGHT",KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_RIGHT);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_LEFT",KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_LEFT);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_FRONT_TOP_LEFT",KZU_BOUNDING_BOX_CORNER_FRONT_TOP_LEFT);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_FRONT_TOP_RIGHT",KZU_BOUNDING_BOX_CORNER_FRONT_TOP_RIGHT);
  tolua_constant(tolua_S,"KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_RIGHT",KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_RIGHT);
  tolua_cclass(tolua_S,"KzuBoundingVolume","KzuBoundingVolume","",NULL);
  tolua_beginmodule(tolua_S,"KzuBoundingVolume");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuBoundingVolumeAxisAlignedBox","KzuBoundingVolumeAxisAlignedBox","",NULL);
  tolua_beginmodule(tolua_S,"KzuBoundingVolumeAxisAlignedBox");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuBoundingVolumeSphere","KzuBoundingVolumeSphere","",NULL);
  tolua_beginmodule(tolua_S,"KzuBoundingVolumeSphere");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuTransformedBoundingVolume","KzuTransformedBoundingVolume","",NULL);
  tolua_beginmodule(tolua_S,"KzuTransformedBoundingVolume");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuBoundingVolumeCreateEmpty",tolua_kzu_bounding_volume_kzuBoundingVolumeCreateEmpty00);
  tolua_function(tolua_S,"kzuBoundingVolumeCreate",tolua_kzu_bounding_volume_kzuBoundingVolumeCreate00);
  tolua_function(tolua_S,"kzuBoundingVolumeDelete",tolua_kzu_bounding_volume_kzuBoundingVolumeDelete00);
  tolua_function(tolua_S,"kzuBoundingVolumeCalculateTransformedBoundingVolume",tolua_kzu_bounding_volume_kzuBoundingVolumeCalculateTransformedBoundingVolume00);
  tolua_function(tolua_S,"kzuAxisAlignedBoundingBoxFromTransformedAABB",tolua_kzu_bounding_volume_kzuAxisAlignedBoundingBoxFromTransformedAABB00);
  tolua_function(tolua_S,"kzuTransformedBoundingVolumeCreateFromAABB",tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeCreateFromAABB00);
  tolua_function(tolua_S,"kzuTransformedBoundingVolumeGetAABB",tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeGetAABB00);
  tolua_function(tolua_S,"kzuTransformedBoundingVolumeIsAbovePlane",tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeIsAbovePlane00);
  tolua_function(tolua_S,"kzuTransformedBoundingVolumeBoxGetCornerPoint",tolua_kzu_bounding_volume_kzuTransformedBoundingVolumeBoxGetCornerPoint00);
  tolua_function(tolua_S,"kzuBoundingVolumeSetVolume",tolua_kzu_bounding_volume_kzuBoundingVolumeSetVolume00);
  tolua_function(tolua_S,"kzuBoundingVolumeGetAxisAlignedBox",tolua_kzu_bounding_volume_kzuBoundingVolumeGetAxisAlignedBox00);
  tolua_function(tolua_S,"kzuBoundingVolumeGetSphere",tolua_kzu_bounding_volume_kzuBoundingVolumeGetSphere00);
  tolua_function(tolua_S,"kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner",tolua_kzu_bounding_volume_kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner00);
  tolua_function(tolua_S,"kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner",tolua_kzu_bounding_volume_kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner00);
  tolua_function(tolua_S,"kzuBoundingVolumeSphereGetRadius",tolua_kzu_bounding_volume_kzuBoundingVolumeSphereGetRadius00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_bounding_volume (lua_State* tolua_S) {
 return tolua_kzu_bounding_volume_open(tolua_S);
}
#endif

