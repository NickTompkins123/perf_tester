/*
** Lua binding: kzu_ui_component_type
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_ui_component_type_open (lua_State* tolua_S);

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
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component_base.h>
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

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzuComponentTypeIdentifier (lua_State* tolua_S)
{
 KzuComponentTypeIdentifier* self = (KzuComponentTypeIdentifier*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuPropertyTypeCollection");
 tolua_usertype(tolua_S,"KzuUiComponentUninitializeCallback");
 tolua_usertype(tolua_S,"KzuUiComponentNode");
 tolua_usertype(tolua_S,"KzuUiEvent");
 tolua_usertype(tolua_S,"KzuUiComponentType");
 tolua_usertype(tolua_S,"KzuUiAction");
 tolua_usertype(tolua_S,"KzuUiComponentUpdateCallback");
 tolua_usertype(tolua_S,"KzuComponentTypeIdentifier");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuUiComponentHandleEventsCallback");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"KzuEngineMessageQueue");
 tolua_usertype(tolua_S,"KzuUiComponentInitializeCallback");
 tolua_usertype(tolua_S,"KzcHashMap");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuUiComponentTypeCreate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeCreate00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeCreate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentType* out_componentType = ((struct KzuUiComponentType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeCreate(memoryManager,&out_componentType);
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
    tolua_pushusertype(tolua_S,(void*)out_componentType,"KzuUiComponentType");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeCreate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeDelete00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiComponentType* componentType = ((struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeDelete(componentType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetParent */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetParent00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetParent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentType* parent = ((struct KzuUiComponentType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentTypeSetParent(componentType,parent);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetParent'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeAddAction */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeAddAction00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeAddAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuUiAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  struct KzuUiAction* action = ((struct KzuUiAction*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeAddAction(componentType,name,action);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeAddAction'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeTakeOwnership */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeTakeOwnership00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeTakeOwnership00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuUiComponentTypeTakeOwnership(componentType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeTakeOwnership'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeIsValid */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeIsValid00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeIsValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuUiComponentTypeIsValid(componentType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeIsValid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeTransferData */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeTransferData00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeTransferData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiComponentType* targetComponentType = ((struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentType* sourceComponentType = ((struct KzuUiComponentType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentTypeTransferData(targetComponentType,sourceComponentType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeTransferData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeAddPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeAddPropertyType00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeAddPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyType* propertyType = ((struct KzuPropertyType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeAddPropertyType(componentType,propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeAddPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeGetPropertyTypeCollection */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeGetPropertyTypeCollection00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeGetPropertyTypeCollection00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyTypeCollection* tolua_ret = (struct KzuPropertyTypeCollection*)  kzuUiComponentTypeGetPropertyTypeCollection(componentType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyTypeCollection");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeGetPropertyTypeCollection'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeGetPropertyTypes */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeGetPropertyTypes00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeGetPropertyTypes00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzcDynamicArray* tolua_ret = (struct KzcDynamicArray*)  kzuUiComponentTypeGetPropertyTypes(componentType);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzcDynamicArray");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeGetPropertyTypes'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeGetInheritedCallback */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeGetInheritedCallback00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeGetInheritedCallback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuUiComponentType* componentType = ((struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  enum KzuUiComponentCallback callback = ((enum KzuUiComponentCallback) (int)  tolua_tonumber(tolua_S,2,0));
  {
   struct KzuUiComponentType* tolua_ret = (struct KzuUiComponentType*)  kzuUiComponentTypeGetInheritedCallback(componentType,callback);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuUiComponentType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeGetInheritedCallback'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeGetTypeIdentifier */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeGetTypeIdentifier00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeGetTypeIdentifier00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  {
   KzuComponentTypeIdentifier tolua_ret = (KzuComponentTypeIdentifier)  kzuUiComponentTypeGetTypeIdentifier(componentType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzuComponentTypeIdentifier)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuComponentTypeIdentifier");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(KzuComponentTypeIdentifier));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuComponentTypeIdentifier");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeGetTypeIdentifier'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetTypeIdentifier */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetTypeIdentifier00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetTypeIdentifier00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuComponentTypeIdentifier",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  KzuComponentTypeIdentifier type = *((KzuComponentTypeIdentifier*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentTypeSetTypeIdentifier(componentType,type);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetTypeIdentifier'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetInitializeCallback */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetInitializeCallback00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetInitializeCallback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuUiComponentInitializeCallback",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  KzuUiComponentInitializeCallback function = *((KzuUiComponentInitializeCallback*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentTypeSetInitializeCallback(componentType,function);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetInitializeCallback'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetUninitializeCallback */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetUninitializeCallback00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetUninitializeCallback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuUiComponentUninitializeCallback",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  KzuUiComponentUninitializeCallback function = *((KzuUiComponentUninitializeCallback*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentTypeSetUninitializeCallback(componentType,function);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetUninitializeCallback'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetLogicUpdateCallback */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetLogicUpdateCallback00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetLogicUpdateCallback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuUiComponentUpdateCallback",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  KzuUiComponentUpdateCallback function = *((KzuUiComponentUpdateCallback*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentTypeSetLogicUpdateCallback(componentType,function);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetLogicUpdateCallback'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetHandleEventsCallback */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetHandleEventsCallback00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetHandleEventsCallback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuUiComponentHandleEventsCallback",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  KzuUiComponentHandleEventsCallback function = *((KzuUiComponentHandleEventsCallback*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuUiComponentTypeSetHandleEventsCallback(componentType,function);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetHandleEventsCallback'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetCallbackScript */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetCallbackScript00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetCallbackScript00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,2,0));
  enum KzuUiComponentCallback callback = ((enum KzuUiComponentCallback) (int)  tolua_tonumber(tolua_S,3,0));
  char* scriptPath = ((char*)  tolua_tostring(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeSetCallbackScript(memoryManager,componentType,callback,scriptPath);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetCallbackScript'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeSetCallbackInherited */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeSetCallbackInherited00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeSetCallbackInherited00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  enum KzuUiComponentCallback callback = ((enum KzuUiComponentCallback) (int)  tolua_tonumber(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeSetCallbackInherited(componentType,callback);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeSetCallbackInherited'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeCallCallbackInitialize */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackInitialize00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackInitialize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentNode* componentNode = ((struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeCallCallbackInitialize(componentType,componentNode,memoryManager);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeCallCallbackInitialize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeCallCallbackUninitialize */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackUninitialize00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackUninitialize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  struct KzuUiComponentNode* componentNode = ((struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeCallCallbackUninitialize(componentType,componentNode);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeCallCallbackUninitialize'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeCallCallbackLogicUpdate */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackLogicUpdate00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackLogicUpdate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuEngineMessageQueue",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  float deltaTime = ((float)  tolua_tonumber(tolua_S,3,0));
  struct KzuEngineMessageQueue* queue = ((struct KzuEngineMessageQueue*)  tolua_tousertype(tolua_S,4,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeCallCallbackLogicUpdate(componentType,componentNode,deltaTime,queue);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeCallCallbackLogicUpdate'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuUiComponentTypeCallCallbackHandleEvents */
#ifndef TOLUA_DISABLE_tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackHandleEvents00
static int tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackHandleEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuUiComponentType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuUiComponentNode",0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"KzuUiEvent",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"KzuEngineMessageQueue",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuUiComponentType* componentType = ((const struct KzuUiComponentType*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuUiComponentNode* componentNode = ((const struct KzuUiComponentNode*)  tolua_tousertype(tolua_S,2,0));
  char* eventType = ((char*)  tolua_tostring(tolua_S,3,0));
  struct KzuUiEvent* event = ((struct KzuUiEvent*)  tolua_tousertype(tolua_S,4,0));
  struct KzuEngineMessageQueue* queue = ((struct KzuEngineMessageQueue*)  tolua_tousertype(tolua_S,5,0));
  {
   kzsError tolua_ret = (kzsError)  kzuUiComponentTypeCallCallbackHandleEvents(componentType,componentNode,eventType,event,queue);
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
 tolua_error(tolua_S,"#ferror in function 'kzuUiComponentTypeCallCallbackHandleEvents'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_ui_component_type_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzuPropertyTypeCollection","KzuPropertyTypeCollection","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyTypeCollection");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyType","KzuPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyType");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuEngineMessageQueue","KzuEngineMessageQueue","",NULL);
  tolua_beginmodule(tolua_S,"KzuEngineMessageQueue");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiComponentNode","KzuUiComponentNode","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiComponentNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiEvent","KzuUiEvent","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiEvent");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiAction","KzuUiAction","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiAction");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcHashMap","KzcHashMap","",NULL);
  tolua_beginmodule(tolua_S,"KzcHashMap");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuUiComponentType","KzuUiComponentType","",NULL);
  tolua_beginmodule(tolua_S,"KzuUiComponentType");
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK",KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK);
  tolua_constant(tolua_S,"KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT",KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT);
  tolua_constant(tolua_S,"KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED",KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED);
  tolua_constant(tolua_S,"KZU_UI_COMPONENT_CALLBACK_INITIALIZE",KZU_UI_COMPONENT_CALLBACK_INITIALIZE);
  tolua_constant(tolua_S,"KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE",KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE);
  tolua_constant(tolua_S,"KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE",KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE);
  tolua_constant(tolua_S,"KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS",KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS);
  tolua_function(tolua_S,"kzuUiComponentTypeCreate",tolua_kzu_ui_component_type_kzuUiComponentTypeCreate00);
  tolua_function(tolua_S,"kzuUiComponentTypeDelete",tolua_kzu_ui_component_type_kzuUiComponentTypeDelete00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetParent",tolua_kzu_ui_component_type_kzuUiComponentTypeSetParent00);
  tolua_function(tolua_S,"kzuUiComponentTypeAddAction",tolua_kzu_ui_component_type_kzuUiComponentTypeAddAction00);
  tolua_function(tolua_S,"kzuUiComponentTypeTakeOwnership",tolua_kzu_ui_component_type_kzuUiComponentTypeTakeOwnership00);
  tolua_function(tolua_S,"kzuUiComponentTypeIsValid",tolua_kzu_ui_component_type_kzuUiComponentTypeIsValid00);
  tolua_function(tolua_S,"kzuUiComponentTypeTransferData",tolua_kzu_ui_component_type_kzuUiComponentTypeTransferData00);
  tolua_function(tolua_S,"kzuUiComponentTypeAddPropertyType",tolua_kzu_ui_component_type_kzuUiComponentTypeAddPropertyType00);
  tolua_function(tolua_S,"kzuUiComponentTypeGetPropertyTypeCollection",tolua_kzu_ui_component_type_kzuUiComponentTypeGetPropertyTypeCollection00);
  tolua_function(tolua_S,"kzuUiComponentTypeGetPropertyTypes",tolua_kzu_ui_component_type_kzuUiComponentTypeGetPropertyTypes00);
  tolua_function(tolua_S,"kzuUiComponentTypeGetInheritedCallback",tolua_kzu_ui_component_type_kzuUiComponentTypeGetInheritedCallback00);
  tolua_function(tolua_S,"kzuUiComponentTypeGetTypeIdentifier",tolua_kzu_ui_component_type_kzuUiComponentTypeGetTypeIdentifier00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetTypeIdentifier",tolua_kzu_ui_component_type_kzuUiComponentTypeSetTypeIdentifier00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetInitializeCallback",tolua_kzu_ui_component_type_kzuUiComponentTypeSetInitializeCallback00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetUninitializeCallback",tolua_kzu_ui_component_type_kzuUiComponentTypeSetUninitializeCallback00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetLogicUpdateCallback",tolua_kzu_ui_component_type_kzuUiComponentTypeSetLogicUpdateCallback00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetHandleEventsCallback",tolua_kzu_ui_component_type_kzuUiComponentTypeSetHandleEventsCallback00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetCallbackScript",tolua_kzu_ui_component_type_kzuUiComponentTypeSetCallbackScript00);
  tolua_function(tolua_S,"kzuUiComponentTypeSetCallbackInherited",tolua_kzu_ui_component_type_kzuUiComponentTypeSetCallbackInherited00);
  tolua_function(tolua_S,"kzuUiComponentTypeCallCallbackInitialize",tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackInitialize00);
  tolua_function(tolua_S,"kzuUiComponentTypeCallCallbackUninitialize",tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackUninitialize00);
  tolua_function(tolua_S,"kzuUiComponentTypeCallCallbackLogicUpdate",tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackLogicUpdate00);
  tolua_function(tolua_S,"kzuUiComponentTypeCallCallbackHandleEvents",tolua_kzu_ui_component_type_kzuUiComponentTypeCallCallbackHandleEvents00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_ui_component_type (lua_State* tolua_S) {
 return tolua_kzu_ui_component_type_open(tolua_S);
}
#endif

