/*
** Lua binding: kzu_project
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:39.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_project_open (lua_State* tolua_S);

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
#include <user/project/kzu_project.h>
#include <core/util/collection/kzc_hash_map.h>
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

static int tolua_collect_KzuProjectObjectIterator (lua_State* tolua_S)
{
 KzuProjectObjectIterator* self = (KzuProjectObjectIterator*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

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
 tolua_usertype(tolua_S,"KzcRenderer");
 tolua_usertype(tolua_S,"KzuProject");
 tolua_usertype(tolua_S,"KzuBinaryDirectory");
 tolua_usertype(tolua_S,"KzcBitmapFontSystem");
 tolua_usertype(tolua_S,"KzcMemoryManager");
 tolua_usertype(tolua_S,"KzuObjectSource");
 tolua_usertype(tolua_S,"KzuScene");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"KzcHashSet");
 tolua_usertype(tolua_S,"KzcTruetypeSystem");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuObjectNode");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuUiManager");
 tolua_usertype(tolua_S,"KzuPropertyManager");
 tolua_usertype(tolua_S,"KzcResourceManager");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzuProjectObjectIterator");
}

/* function: kzuProjectDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectDelete00
static int tolua_kzu_project_kzuProjectDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProject* project = ((struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectDelete(project);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectClear */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectClear00
static int tolua_kzu_project_kzuProjectClear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectClear(project);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectClear'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetResourceManager */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetResourceManager00
static int tolua_kzu_project_kzuProjectGetResourceManager00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcResourceManager* tolua_ret = (struct KzcResourceManager*)  kzuProjectGetResourceManager(project);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcResourceManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetResourceManager'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetPropertyManager */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetPropertyManager00
static int tolua_kzu_project_kzuProjectGetPropertyManager00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyManager* tolua_ret = (struct KzuPropertyManager*)  kzuProjectGetPropertyManager(project);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetPropertyManager'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetBinaryDirectory */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetBinaryDirectory00
static int tolua_kzu_project_kzuProjectGetBinaryDirectory00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuBinaryDirectory* tolua_ret = (struct KzuBinaryDirectory*)  kzuProjectGetBinaryDirectory(project);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuBinaryDirectory");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetBinaryDirectory'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectAddObject */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectAddObject00
static int tolua_kzu_project_kzuProjectAddObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectObjectType type = ((enum KzuProjectObjectType) (int)  tolua_tonumber(tolua_S,2,0));
  void* object = ((void*)  tolua_touserdata(tolua_S,3,0));
  char* path = ((char*)  tolua_tostring(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectAddObject(project,type,object,path);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectAddObject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectRemoveObject */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectRemoveObject00
static int tolua_kzu_project_kzuProjectRemoveObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectObjectType type = ((enum KzuProjectObjectType) (int)  tolua_tonumber(tolua_S,2,0));
  char* path = ((char*)  tolua_tostring(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectRemoveObject(project,type,path);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectRemoveObject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectAddAnonymousObject */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectAddAnonymousObject00
static int tolua_kzu_project_kzuProjectAddAnonymousObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProject* project = ((struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectObjectType type = ((enum KzuProjectObjectType) (int)  tolua_tonumber(tolua_S,2,0));
  void* object = ((void*)  tolua_touserdata(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectAddAnonymousObject(project,type,object);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectAddAnonymousObject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectDeleteObject */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectDeleteObject00
static int tolua_kzu_project_kzuProjectDeleteObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectObjectType type = ((enum KzuProjectObjectType) (int)  tolua_tonumber(tolua_S,2,0));
  char* path = ((char*)  tolua_tostring(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectDeleteObject(project,type,path);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectDeleteObject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetObject */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetObject00
static int tolua_kzu_project_kzuProjectGetObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectObjectType type = ((enum KzuProjectObjectType) (int)  tolua_tonumber(tolua_S,2,0));
  char* path = ((char*)  tolua_tostring(tolua_S,3,0));
  {
   void* tolua_ret = (void*)  kzuProjectGetObject(project,type,path);
   tolua_pushuserdata(tolua_S,(void*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetObject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetObjectPath */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetObjectPath00
static int tolua_kzu_project_kzuProjectGetObjectPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isuserdata(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  const void* object = ((const void*)  tolua_touserdata(tolua_S,2,0));
  {
   char* tolua_ret = (char*)  kzuProjectGetObjectPath(project,object);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetObjectPath'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetRootSource */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetRootSource00
static int tolua_kzu_project_kzuProjectGetRootSource00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuObjectSource* tolua_ret = (struct KzuObjectSource*)  kzuProjectGetRootSource(project);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuObjectSource");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetRootSource'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectAddPermanentFilePath */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectAddPermanentFilePath00
static int tolua_kzu_project_kzuProjectAddPermanentFilePath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  char* filePath = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectAddPermanentFilePath(project,filePath);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectAddPermanentFilePath'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetPermanentFilePaths */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetPermanentFilePaths00
static int tolua_kzu_project_kzuProjectGetPermanentFilePaths00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcHashSet* tolua_ret = (struct KzcHashSet*)  kzuProjectGetPermanentFilePaths(project);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcHashSet");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetPermanentFilePaths'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectHasPermanentFilePath */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectHasPermanentFilePath00
static int tolua_kzu_project_kzuProjectHasPermanentFilePath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  char* filePath = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzBool tolua_ret = (kzBool)  kzuProjectHasPermanentFilePath(project,filePath);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectHasPermanentFilePath'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectRemovePermanentFilePath */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectRemovePermanentFilePath00
static int tolua_kzu_project_kzuProjectRemovePermanentFilePath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  char* filePath = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectRemovePermanentFilePath(project,filePath);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectRemovePermanentFilePath'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetPropertyType00
static int tolua_kzu_project_kzuProjectGetPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuProjectGetPropertyType(project,name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetObjectsOfType */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetObjectsOfType00
static int tolua_kzu_project_kzuProjectGetObjectsOfType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectObjectType type = ((enum KzuProjectObjectType) (int)  tolua_tonumber(tolua_S,2,0));
  {
   struct KzuProjectObjectIterator tolua_ret =   kzuProjectGetObjectsOfType(project,type);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzuProjectObjectIterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuProjectObjectIterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(struct KzuProjectObjectIterator));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuProjectObjectIterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetObjectsOfType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectObjectIterate_private */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectObjectIterate_private00
static int tolua_kzu_project_kzuProjectObjectIterate_private00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProjectObjectIterator",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProjectObjectIterator* iterator = ((struct KzuProjectObjectIterator*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuProjectObjectIterate_private(iterator);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectObjectIterate_private'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectObjectIteratorGetValue_private */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectObjectIteratorGetValue_private00
static int tolua_kzu_project_kzuProjectObjectIteratorGetValue_private00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProjectObjectIterator",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProjectObjectIterator* iterator = ((const struct KzuProjectObjectIterator*)  tolua_tousertype(tolua_S,1,0));
  {
   void* tolua_ret = (void*)  kzuProjectObjectIteratorGetValue_private(iterator);
   tolua_pushuserdata(tolua_S,(void*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectObjectIteratorGetValue_private'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetUiProject */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetUiProject00
static int tolua_kzu_project_kzuProjectGetUiProject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuUiManager* tolua_ret = (struct KzuUiManager*)  kzuProjectGetUiProject(project);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetUiProject'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */


/* function: kzuProjectGetBitmapFontSystem */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetBitmapFontSystem00
static int tolua_kzu_project_kzuProjectGetBitmapFontSystem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcBitmapFontSystem",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProject* project = ((struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  struct KzcBitmapFontSystem* out_bitmapFontSystem = ((struct KzcBitmapFontSystem*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectGetBitmapFontSystem(project,&out_bitmapFontSystem);
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
    tolua_pushusertype(tolua_S,(void*)out_bitmapFontSystem,"KzcBitmapFontSystem");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetBitmapFontSystem'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectSetTruetypeSystem */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectSetTruetypeSystem00
static int tolua_kzu_project_kzuProjectSetTruetypeSystem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcTruetypeSystem",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProject* project = ((struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  struct KzcTruetypeSystem* truetypeSystem = ((struct KzcTruetypeSystem*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectSetTruetypeSystem(project,truetypeSystem);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectSetTruetypeSystem'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetTruetypeSystem */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetTruetypeSystem00
static int tolua_kzu_project_kzuProjectGetTruetypeSystem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzcTruetypeSystem",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProject* project = ((struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  struct KzcTruetypeSystem* out_truetypeSystem = ((struct KzcTruetypeSystem*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectGetTruetypeSystem(project,&out_truetypeSystem);
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
    tolua_pushusertype(tolua_S,(void*)out_truetypeSystem,"KzcTruetypeSystem");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetTruetypeSystem'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectSetViewport */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectSetViewport00
static int tolua_kzu_project_kzuProjectSetViewport00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,7,&tolua_err) || !tolua_isusertype(tolua_S,7,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  float x = ((float)  tolua_tonumber(tolua_S,2,0));
  float y = ((float)  tolua_tonumber(tolua_S,3,0));
  float width = ((float)  tolua_tonumber(tolua_S,4,0));
  float height = ((float)  tolua_tonumber(tolua_S,5,0));
  float aspectRatio = ((float)  tolua_tonumber(tolua_S,6,0));
  kzBool relative = *((kzBool*)  tolua_tousertype(tolua_S,7,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectSetViewport(project,x,y,width,height,aspectRatio,relative);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectSetViewport'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectSetTimeMeasurementEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectSetTimeMeasurementEnabled00
static int tolua_kzu_project_kzuProjectSetTimeMeasurementEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProject* project = ((struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  kzBool enabled = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuProjectSetTimeMeasurementEnabled(project,enabled);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectSetTimeMeasurementEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectIsTimeMeasurementEnabled */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectIsTimeMeasurementEnabled00
static int tolua_kzu_project_kzuProjectIsTimeMeasurementEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuProjectIsTimeMeasurementEnabled(project);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectIsTimeMeasurementEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectAddMeasurementCumulativeTime_private */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectAddMeasurementCumulativeTime_private00
static int tolua_kzu_project_kzuProjectAddMeasurementCumulativeTime_private00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProject* project = ((struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  unsigned int startTime = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  enum KzuProjectMeasurementType measurementType = ((enum KzuProjectMeasurementType) (int)  tolua_tonumber(tolua_S,3,0));
  {
   kzuProjectAddMeasurementCumulativeTime_private(project,startTime,measurementType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectAddMeasurementCumulativeTime_private'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectPrintTimeMeasurementInfo */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectPrintTimeMeasurementInfo00
static int tolua_kzu_project_kzuProjectPrintTimeMeasurementInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuProjectPrintTimeMeasurementInfo(project);
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
 tolua_error(tolua_S,"#ferror in function 'kzuProjectPrintTimeMeasurementInfo'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuProjectGetTimeMeasurementInfo */
#ifndef TOLUA_DISABLE_tolua_kzu_project_kzuProjectGetTimeMeasurementInfo00
static int tolua_kzu_project_kzuProjectGetTimeMeasurementInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProject* project = ((const struct KzuProject*)  tolua_tousertype(tolua_S,1,0));
  enum KzuProjectMeasurementType measurementType = ((enum KzuProjectMeasurementType) (int)  tolua_tonumber(tolua_S,2,0));
  {
   unsigned int tolua_ret = (unsigned int)  kzuProjectGetTimeMeasurementInfo(project,measurementType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuProjectGetTimeMeasurementInfo'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_project_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_INVALID",KZU_PROJECT_OBJECT_TYPE_INVALID);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_IMAGE",KZU_PROJECT_OBJECT_TYPE_IMAGE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_SCENE",KZU_PROJECT_OBJECT_TYPE_SCENE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_TEXTURE",KZU_PROJECT_OBJECT_TYPE_TEXTURE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE",KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE",KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_MATERIAL",KZU_PROJECT_OBJECT_TYPE_MATERIAL);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE",KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_MESH",KZU_PROJECT_OBJECT_TYPE_MESH);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE",KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_COMPOSER",KZU_PROJECT_OBJECT_TYPE_COMPOSER);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE",KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_ANIMATION",KZU_PROJECT_OBJECT_TYPE_ANIMATION);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP",KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE",KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE",KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_FONT",KZU_PROJECT_OBJECT_TYPE_FONT);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_TRANSITION",KZU_PROJECT_OBJECT_TYPE_TRANSITION);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_SCRIPT",KZU_PROJECT_OBJECT_TYPE_SCRIPT);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_TRAJECTORY",KZU_PROJECT_OBJECT_TYPE_TRAJECTORY);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP",KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP);
  tolua_constant(tolua_S,"KZU_PROJECT_OBJECT_TYPE_SPLINE",KZU_PROJECT_OBJECT_TYPE_SPLINE);
  tolua_constant(tolua_S,"KZU_PROJECT_MEASUREMENT_IMAGE",KZU_PROJECT_MEASUREMENT_IMAGE);
  tolua_constant(tolua_S,"KZU_PROJECT_MEASUREMENT_COMPRESSED_IMAGE",KZU_PROJECT_MEASUREMENT_COMPRESSED_IMAGE);
  tolua_constant(tolua_S,"KZU_PROJECT_MEASUREMENT_MESH",KZU_PROJECT_MEASUREMENT_MESH);
  tolua_constant(tolua_S,"KZU_PROJECT_MEASUREMENT_ANIMATION",KZU_PROJECT_MEASUREMENT_ANIMATION);
  tolua_constant(tolua_S,"KZU_PROJECT_MEASUREMENT_OBJECT_NODE",KZU_PROJECT_MEASUREMENT_OBJECT_NODE);
  tolua_cclass(tolua_S,"KzuScene","KzuScene","",NULL);
  tolua_beginmodule(tolua_S,"KzuScene");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyType","KzuPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyManager","KzuPropertyManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuObjectNode","KzuObjectNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuObjectNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiManager","KzuUiManager","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuBinaryDirectory","KzuBinaryDirectory","",NULL);
  tolua_beginmodule(tolua_S,"KzuBinaryDirectory");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcResourceManager","KzcResourceManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcResourceManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcBitmapFontSystem","KzcBitmapFontSystem","",NULL);
  tolua_beginmodule(tolua_S,"KzcBitmapFontSystem");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcTruetypeSystem","KzcTruetypeSystem","",NULL);
  tolua_beginmodule(tolua_S,"KzcTruetypeSystem");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProject","KzuProject","",NULL);
  tolua_beginmodule(tolua_S,"KzuProject");
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"KzuProjectObjectIterator","KzuProjectObjectIterator","",tolua_collect_KzuProjectObjectIterator);
  #else
  tolua_cclass(tolua_S,"KzuProjectObjectIterator","KzuProjectObjectIterator","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"KzuProjectObjectIterator");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuProjectDelete",tolua_kzu_project_kzuProjectDelete00);
  tolua_function(tolua_S,"kzuProjectClear",tolua_kzu_project_kzuProjectClear00);
  tolua_function(tolua_S,"kzuProjectGetResourceManager",tolua_kzu_project_kzuProjectGetResourceManager00);
  tolua_function(tolua_S,"kzuProjectGetPropertyManager",tolua_kzu_project_kzuProjectGetPropertyManager00);
  tolua_function(tolua_S,"kzuProjectGetBinaryDirectory",tolua_kzu_project_kzuProjectGetBinaryDirectory00);
  tolua_function(tolua_S,"kzuProjectAddObject",tolua_kzu_project_kzuProjectAddObject00);
  tolua_function(tolua_S,"kzuProjectRemoveObject",tolua_kzu_project_kzuProjectRemoveObject00);
  tolua_function(tolua_S,"kzuProjectAddAnonymousObject",tolua_kzu_project_kzuProjectAddAnonymousObject00);
  tolua_function(tolua_S,"kzuProjectDeleteObject",tolua_kzu_project_kzuProjectDeleteObject00);
  tolua_function(tolua_S,"kzuProjectGetObject",tolua_kzu_project_kzuProjectGetObject00);
  tolua_function(tolua_S,"kzuProjectGetObjectPath",tolua_kzu_project_kzuProjectGetObjectPath00);
  tolua_function(tolua_S,"kzuProjectGetRootSource",tolua_kzu_project_kzuProjectGetRootSource00);
  tolua_function(tolua_S,"kzuProjectAddPermanentFilePath",tolua_kzu_project_kzuProjectAddPermanentFilePath00);
  tolua_function(tolua_S,"kzuProjectGetPermanentFilePaths",tolua_kzu_project_kzuProjectGetPermanentFilePaths00);
  tolua_function(tolua_S,"kzuProjectHasPermanentFilePath",tolua_kzu_project_kzuProjectHasPermanentFilePath00);
  tolua_function(tolua_S,"kzuProjectRemovePermanentFilePath",tolua_kzu_project_kzuProjectRemovePermanentFilePath00);
  tolua_function(tolua_S,"kzuProjectGetPropertyType",tolua_kzu_project_kzuProjectGetPropertyType00);
  tolua_function(tolua_S,"kzuProjectGetObjectsOfType",tolua_kzu_project_kzuProjectGetObjectsOfType00);
  
  
  tolua_function(tolua_S,"kzuProjectObjectIterate_private",tolua_kzu_project_kzuProjectObjectIterate_private00);
  tolua_function(tolua_S,"kzuProjectObjectIteratorGetValue_private",tolua_kzu_project_kzuProjectObjectIteratorGetValue_private00);
  tolua_function(tolua_S,"kzuProjectGetUiProject",tolua_kzu_project_kzuProjectGetUiProject00);
  tolua_function(tolua_S,"kzuProjectGetBitmapFontSystem",tolua_kzu_project_kzuProjectGetBitmapFontSystem00);
  tolua_function(tolua_S,"kzuProjectSetTruetypeSystem",tolua_kzu_project_kzuProjectSetTruetypeSystem00);
  tolua_function(tolua_S,"kzuProjectGetTruetypeSystem",tolua_kzu_project_kzuProjectGetTruetypeSystem00);
  tolua_function(tolua_S,"kzuProjectSetViewport",tolua_kzu_project_kzuProjectSetViewport00);
  tolua_function(tolua_S,"kzuProjectSetTimeMeasurementEnabled",tolua_kzu_project_kzuProjectSetTimeMeasurementEnabled00);
  tolua_function(tolua_S,"kzuProjectIsTimeMeasurementEnabled",tolua_kzu_project_kzuProjectIsTimeMeasurementEnabled00);
  tolua_function(tolua_S,"kzuProjectAddMeasurementCumulativeTime_private",tolua_kzu_project_kzuProjectAddMeasurementCumulativeTime_private00);
  tolua_function(tolua_S,"kzuProjectPrintTimeMeasurementInfo",tolua_kzu_project_kzuProjectPrintTimeMeasurementInfo00);
  tolua_function(tolua_S,"kzuProjectGetTimeMeasurementInfo",tolua_kzu_project_kzuProjectGetTimeMeasurementInfo00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_project (lua_State* tolua_S) {
 return tolua_kzu_project_open(tolua_S);
}
#endif

