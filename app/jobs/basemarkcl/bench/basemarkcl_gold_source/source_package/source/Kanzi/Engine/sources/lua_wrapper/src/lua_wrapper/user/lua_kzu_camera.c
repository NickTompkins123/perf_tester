/*
** Lua binding: kzu_camera
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_camera_open (lua_State* tolua_S);

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
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_object_base.h>
#include <core/util/math/kzc_plane.h>
#include <core/util/math/kzc_matrix4x4.h>

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
 tolua_usertype(tolua_S,"KzcVector3");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzcMatrix4x4");
 tolua_usertype(tolua_S,"KzcPlane");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzuCameraNode");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuCameraNodeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeDelete00
static int tolua_kzu_camera_kzuCameraNodeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuCameraNode* cameraNode = ((struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeDelete(cameraNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeFromObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeFromObjectNode00
static int tolua_kzu_camera_kzuCameraNodeFromObjectNode00(lua_State* tolua_S)
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
   struct KzuCameraNode* tolua_ret = (struct KzuCameraNode*)  kzuCameraNodeFromObjectNode(objectNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuCameraNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeFromObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeToObjectNode */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeToObjectNode00
static int tolua_kzu_camera_kzuCameraNodeToObjectNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectNode* tolua_ret = (struct KzuObjectNode*)  kzuCameraNodeToObjectNode(cameraNode);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeToObjectNode'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetOrthogonalProjection */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetOrthogonalProjection00
static int tolua_kzu_camera_kzuCameraNodeSetOrthogonalProjection00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  enum KzuOrthogonalCoordinateSystem coordinateSystem = ((enum KzuOrthogonalCoordinateSystem) (int)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetOrthogonalProjection(cameraNode,coordinateSystem);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetOrthogonalProjection'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetPerspectiveProjection */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetPerspectiveProjection00
static int tolua_kzu_camera_kzuCameraNodeSetPerspectiveProjection00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetPerspectiveProjection(cameraNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetPerspectiveProjection'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetAspectRatio */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetAspectRatio00
static int tolua_kzu_camera_kzuCameraNodeSetAspectRatio00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float aspectRatio = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetAspectRatio(cameraNode,aspectRatio);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetAspectRatio'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetAspectRatio */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetAspectRatio00
static int tolua_kzu_camera_kzuCameraNodeGetAspectRatio00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuCameraNodeGetAspectRatio(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetAspectRatio'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetProjectionType */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetProjectionType00
static int tolua_kzu_camera_kzuCameraNodeGetProjectionType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuProjectionType tolua_ret = (enum KzuProjectionType)  kzuCameraNodeGetProjectionType(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetProjectionType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetProjectionType */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetProjectionType00
static int tolua_kzu_camera_kzuCameraNodeSetProjectionType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectionType projectionType = ((enum KzuProjectionType) (int)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetProjectionType(cameraNode,projectionType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetProjectionType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetOrthogonalCoordinateSystem */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetOrthogonalCoordinateSystem00
static int tolua_kzu_camera_kzuCameraNodeGetOrthogonalCoordinateSystem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuOrthogonalCoordinateSystem tolua_ret = (enum KzuOrthogonalCoordinateSystem)  kzuCameraNodeGetOrthogonalCoordinateSystemType(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetOrthogonalCoordinateSystem'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetOrthogonalCoordinateSystem */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetOrthogonalCoordinateSystem00
static int tolua_kzu_camera_kzuCameraNodeSetOrthogonalCoordinateSystem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  enum KzuOrthogonalCoordinateSystem system = ((enum KzuOrthogonalCoordinateSystem) (int)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetOrthogonalCoordinateSystemType(cameraNode,system);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetOrthogonalCoordinateSystem'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetOrientation */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetOrientation00
static int tolua_kzu_camera_kzuCameraNodeGetOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuCameraNodeGetOrientationAngle(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetOrientation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetOrientation */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetOrientation00
static int tolua_kzu_camera_kzuCameraNodeSetOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float angleInRadians = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetOrientationAngle(cameraNode,angleInRadians);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetOrientation'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeLookAt */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeLookAt00
static int tolua_kzu_camera_kzuCameraNodeLookAt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzcVector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"KzcVector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"KzcVector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzcVector3 position = *((struct KzcVector3*)  tolua_tousertype(tolua_S,2,0));
  struct KzcVector3 lookAtPoint = *((struct KzcVector3*)  tolua_tousertype(tolua_S,3,0));
  struct KzcVector3 upVector = *((struct KzcVector3*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeLookAt(cameraNode,position,lookAtPoint,upVector);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeLookAt'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeCalculateFrustumPlanes */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeCalculateFrustumPlanes00
static int tolua_kzu_camera_kzuCameraNodeCalculateFrustumPlanes00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcPlane",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzcMatrix4x4* cameraViewMatrix = ((const struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  struct KzcPlane* out_frustumPlanes = ((struct KzcPlane*)  tolua_tousertype(tolua_S,3,0));
  {
   kzuCameraNodeCalculateFrustumPlanes(cameraNode,cameraViewMatrix,out_frustumPlanes);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeCalculateFrustumPlanes'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeCalculateOrthogonalProjectionFromValues */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeCalculateOrthogonalProjectionFromValues00
static int tolua_kzu_camera_kzuCameraNodeCalculateOrthogonalProjectionFromValues00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,9,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,10,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float left = ((float)  tolua_tonumber(tolua_S,1,0));
  float right = ((float)  tolua_tonumber(tolua_S,2,0));
  float top = ((float)  tolua_tonumber(tolua_S,3,0));
  float bottom = ((float)  tolua_tonumber(tolua_S,4,0));
  enum KzuOrthogonalCoordinateSystem coordinateSystem = ((enum KzuOrthogonalCoordinateSystem) (int)  tolua_tonumber(tolua_S,5,0));
  enum KzuFieldOfViewType fovType = ((enum KzuFieldOfViewType) (int)  tolua_tonumber(tolua_S,6,0));
  float orthoHeight = ((float)  tolua_tonumber(tolua_S,7,0));
  float aspectRatio = ((float)  tolua_tonumber(tolua_S,8,0));
  struct KzcMatrix4x4* out_projectionMatrix = ((struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,9,0));
  {
   kzuCameraNodeCalculateOrthogonalProjectionFromValues(left,right,top,bottom,coordinateSystem,fovType,orthoHeight,aspectRatio,out_projectionMatrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeCalculateOrthogonalProjectionFromValues'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeCalculateOrthogonalProjection */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeCalculateOrthogonalProjection00
static int tolua_kzu_camera_kzuCameraNodeCalculateOrthogonalProjection00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,6,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float left = ((float)  tolua_tonumber(tolua_S,2,0));
  float right = ((float)  tolua_tonumber(tolua_S,3,0));
  float top = ((float)  tolua_tonumber(tolua_S,4,0));
  float bottom = ((float)  tolua_tonumber(tolua_S,5,0));
  struct KzcMatrix4x4* out_projectionMatrix = ((struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,6,0));
  {
   kzuCameraNodeCalculateOrthogonalProjection(cameraNode,left,right,top,bottom,out_projectionMatrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeCalculateOrthogonalProjection'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeCalculateProjectionMatrix */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeCalculateProjectionMatrix00
static int tolua_kzu_camera_kzuCameraNodeCalculateProjectionMatrix00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcMatrix4x4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  struct KzcMatrix4x4* out_projectionMatrix = ((struct KzcMatrix4x4*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuCameraNodeCalculateProjectionMatrix(cameraNode,out_projectionMatrix);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeCalculateProjectionMatrix'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetFieldOfViewType */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetFieldOfViewType00
static int tolua_kzu_camera_kzuCameraNodeSetFieldOfViewType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  enum KzuFieldOfViewType type = ((enum KzuFieldOfViewType) (int)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetFovType(cameraNode,type);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetFieldOfViewType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetFieldOfView */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetFieldOfView00
static int tolua_kzu_camera_kzuCameraNodeSetFieldOfView00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float fovInRadians = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetFov(cameraNode,fovInRadians);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetFieldOfView'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetFieldOfView */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetFieldOfView00
static int tolua_kzu_camera_kzuCameraNodeGetFieldOfView00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuCameraNodeGetFov(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetFieldOfView'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetFieldOfViewType */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetFieldOfViewType00
static int tolua_kzu_camera_kzuCameraNodeGetFieldOfViewType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuFieldOfViewType tolua_ret = (enum KzuFieldOfViewType)  kzuCameraNodeGetFovType(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetFieldOfViewType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetOrthoHeight */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetOrthoHeight00
static int tolua_kzu_camera_kzuCameraNodeSetOrthoHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float height = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetOrthogonalPlaneHeight(cameraNode,height);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetOrthoHeight'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetOrthoHeight */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetOrthoHeight00
static int tolua_kzu_camera_kzuCameraNodeGetOrthoHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuCameraNodeGetOrthogonalPlaneHeight(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetOrthoHeight'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetInheritAspectRatio */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetInheritAspectRatio00
static int tolua_kzu_camera_kzuCameraNodeSetInheritAspectRatio00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  kzBool disableAspectRatio = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetDisableAspectRatio(cameraNode,disableAspectRatio);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetInheritAspectRatio'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetInheritedAspectRatio */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetInheritedAspectRatio00
static int tolua_kzu_camera_kzuCameraNodeSetInheritedAspectRatio00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float inheritedAspectRatio = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzuCameraNodeSetInheritedAspectRatio(cameraNode,inheritedAspectRatio);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetInheritedAspectRatio'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeIsAspectRatioInherited */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeIsAspectRatioInherited00
static int tolua_kzu_camera_kzuCameraNodeIsAspectRatioInherited00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuCameraNodeGetDisableAspectRatio(cameraNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeIsAspectRatioInherited'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetNearClip */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetNearClip00
static int tolua_kzu_camera_kzuCameraNodeSetNearClip00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float nearClipDistance = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetZNear(cameraNode,nearClipDistance);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetNearClip'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeSetFarClip */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeSetFarClip00
static int tolua_kzu_camera_kzuCameraNodeSetFarClip00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  float farClipDistance = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeSetZFar(cameraNode,farClipDistance);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeSetFarClip'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetNearClip */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetNearClip00
static int tolua_kzu_camera_kzuCameraNodeGetNearClip00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuCameraNodeGetZNear(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetNearClip'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeGetFarClip */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetFarClip00
static int tolua_kzu_camera_kzuCameraNodeGetFarClip00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  kzuCameraNodeGetZFar(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetFarClip'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */


/* function: kzuCameraNodeGetStereoscopicFocusDistance */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeGetStereoscopicFocusDistance00
static int tolua_kzu_camera_kzuCameraNodeGetStereoscopicFocusDistance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* cameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  {
   float tolua_ret = (float)  0.0f;//kzuCameraNodeGetStereoscopicFocusDistance(cameraNode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeGetStereoscopicFocusDistance'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuCameraNodeCloneData */
#ifndef TOLUA_DISABLE_tolua_kzu_camera_kzuCameraNodeCloneData00
static int tolua_kzu_camera_kzuCameraNodeCloneData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuCameraNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuCameraNode* sourceCameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuCameraNode* destinationCameraNode = ((const struct KzuCameraNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuCameraNodeCloneData(sourceCameraNode,destinationCameraNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuCameraNodeCloneData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_camera_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL",KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL);
  tolua_constant(tolua_S,"KZU_CAMERA_FIELD_OF_VIEW_VERTICAL",KZU_CAMERA_FIELD_OF_VIEW_VERTICAL);
  tolua_constant(tolua_S,"KZU_PROJECTION_PERSPECTIVE",KZU_PROJECTION_PERSPECTIVE);
  tolua_constant(tolua_S,"KZU_PROJECTION_ORTHOGRAPHIC",KZU_PROJECTION_ORTHOGRAPHIC);
  tolua_constant(tolua_S,"KZU_ORTHOGONAL_COORDINATE_SYSTEM_ABSOLUTE",KZU_ORTHOGONAL_COORDINATE_SYSTEM_ABSOLUTE);
  tolua_constant(tolua_S,"KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE",KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE);
  tolua_constant(tolua_S,"KZU_FRUSTUM_PLANE_NEAR",KZU_FRUSTUM_PLANE_NEAR);
  tolua_constant(tolua_S,"KZU_FRUSTUM_PLANE_FAR",KZU_FRUSTUM_PLANE_FAR);
  tolua_constant(tolua_S,"KZU_FRUSTUM_PLANE_TOP",KZU_FRUSTUM_PLANE_TOP);
  tolua_constant(tolua_S,"KZU_FRUSTUM_PLANE_BOTTOM",KZU_FRUSTUM_PLANE_BOTTOM);
  tolua_constant(tolua_S,"KZU_FRUSTUM_PLANE_LEFT",KZU_FRUSTUM_PLANE_LEFT);
  tolua_constant(tolua_S,"KZU_FRUSTUM_PLANE_RIGHT",KZU_FRUSTUM_PLANE_RIGHT);
  tolua_constant(tolua_S,"KZU_FRUSTUM_PLANE_COUNT",KZU_FRUSTUM_PLANE_COUNT);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMatrix4x4","KzcMatrix4x4","",NULL);
  tolua_beginmodule(tolua_S,"KzcMatrix4x4");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcVector3","KzcVector3","",NULL);
  tolua_beginmodule(tolua_S,"KzcVector3");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuCameraNode","KzuCameraNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuCameraNode");
  tolua_endmodule(tolua_S);

 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_camera (lua_State* tolua_S) {
 return tolua_kzu_camera_open(tolua_S);
}
#endif

