/*
** Lua binding: kzu_property
** Generated automatically by tolua++-1.0.92 on 04/14/11 10:14:40.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
 int  tolua_kzu_property_open (lua_State* tolua_S);

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
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_property_base.h>
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

static int tolua_collect_KzuPropertyPriority (lua_State* tolua_S)
{
 KzuPropertyPriority* self = (KzuPropertyPriority*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_kzsError (lua_State* tolua_S)
{
 kzsError* self = (kzsError*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_KzuPropertyDataType (lua_State* tolua_S)
{
 KzuPropertyDataType* self = (KzuPropertyDataType*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"KzuPropertyDataType");
 tolua_usertype(tolua_S,"KzcDynamicArray");
 tolua_usertype(tolua_S,"KzuProperty");
 tolua_usertype(tolua_S,"kzBool");
 tolua_usertype(tolua_S,"KzuFixedPropertyTypes");
 tolua_usertype(tolua_S,"KzuPropertyType");
 tolua_usertype(tolua_S,"kzU32");
 tolua_usertype(tolua_S,"KzuPropertyPriority");
 tolua_usertype(tolua_S,"kzsError");
 tolua_usertype(tolua_S,"KzcMemoryManager");
}

/* function: kzuPropertyDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyDelete00
static int tolua_kzu_property_kzuPropertyDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProperty* property = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuPropertyDelete(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyCopy */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyCopy00
static int tolua_kzu_property_kzuPropertyCopy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuProperty",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuProperty* sourceProperty = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,2,0));
  struct KzuProperty* out_property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,3,0));
  {
   kzsError tolua_ret = (kzsError)  kzuPropertyCopy(memoryManager,sourceProperty,&out_property);
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
    tolua_pushusertype(tolua_S,(void*)out_property,"KzuProperty");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyCopy'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyCopyContent */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyCopyContent00
static int tolua_kzu_property_kzuPropertyCopyContent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProperty* targetProperty = ((struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuProperty* sourceProperty = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuPropertyCopyContent(targetProperty,sourceProperty);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyCopyContent'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyCopyValue */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyCopyValue00
static int tolua_kzu_property_kzuPropertyCopyValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProperty* targetProperty = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  const struct KzuProperty* sourceProperty = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)  kzuPropertyCopyValue(targetProperty,sourceProperty);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyCopyValue'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertySetPriority */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertySetPriority00
static int tolua_kzu_property_kzuPropertySetPriority00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"KzuPropertyPriority",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProperty* property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  enum KzuPropertyPriority priority = *((enum KzuPropertyPriority*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuPropertySetPriority(property,priority);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertySetPriority'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyGetPriority */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyGetPriority00
static int tolua_kzu_property_kzuPropertyGetPriority00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProperty* property = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuPropertyPriority tolua_ret = (enum KzuPropertyPriority)  kzuPropertyGetPriority(property);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzuPropertyPriority)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuPropertyPriority");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(enum KzuPropertyPriority));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuPropertyPriority");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyGetPriority'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyGetPropertyType */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyGetPropertyType00
static int tolua_kzu_property_kzuPropertyGetPropertyType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProperty* property = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   struct KzuPropertyType* tolua_ret = (struct KzuPropertyType*)  kzuPropertyGetPropertyType(property);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"KzuPropertyType");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyGetPropertyType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeDelete */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeDelete00
static int tolua_kzu_property_kzuPropertyTypeDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuPropertyType* propertyType = ((struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzsError tolua_ret = (kzsError)  kzuPropertyTypeDelete(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeDelete'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeGetName */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeGetName00
static int tolua_kzu_property_kzuPropertyTypeGetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   char* tolua_ret = (char*)  kzuPropertyTypeGetName(propertyType);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeGetName'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeGetDataType */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeGetDataType00
static int tolua_kzu_property_kzuPropertyTypeGetDataType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   enum KzuPropertyDataType tolua_ret = (enum KzuPropertyDataType)  kzuPropertyTypeGetDataType(propertyType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((KzuPropertyDataType)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuPropertyDataType");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(enum KzuPropertyDataType));
     tolua_pushusertype(tolua_S,tolua_obj,"KzuPropertyDataType");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeGetDataType'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeSetInherited */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeSetInherited00
static int tolua_kzu_property_kzuPropertyTypeSetInherited00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  kzBool inherited = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuPropertyTypeSetInherited(propertyType,inherited);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeSetInherited'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeGetInherited */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeGetInherited00
static int tolua_kzu_property_kzuPropertyTypeGetInherited00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuPropertyTypeGetInherited(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeGetInherited'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyGetFromKzb */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyGetFromKzb00
static int tolua_kzu_property_kzuPropertyGetFromKzb00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProperty* property = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuPropertyGetFromKzb(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyGetFromKzb'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertySetFromKzb */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertySetFromKzb00
static int tolua_kzu_property_kzuPropertySetFromKzb00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProperty* property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuPropertySetFromKzb(property);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertySetFromKzb'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyDisableFromKzb */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyDisableFromKzb00
static int tolua_kzu_property_kzuPropertyDisableFromKzb00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProperty* property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuPropertyDisableFromKzb(property);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyDisableFromKzb'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertySetFlags */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertySetFlags00
static int tolua_kzu_property_kzuPropertySetFlags00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzU32",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProperty* property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  kzU32 flags = *((kzU32*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuPropertySetFlags(property,flags);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertySetFlags'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyIsInherited */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyIsInherited00
static int tolua_kzu_property_kzuPropertyIsInherited00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuProperty* property = ((const struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuPropertyIsInherited(property);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyIsInherited'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertySetInherited */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertySetInherited00
static int tolua_kzu_property_kzuPropertySetInherited00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuProperty",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"kzBool",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuProperty* property = ((struct KzuProperty*)  tolua_tousertype(tolua_S,1,0));
  kzBool inherited = *((kzBool*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuPropertySetInherited(property,inherited);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertySetInherited'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyCreateFixedProperties */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyCreateFixedProperties00
static int tolua_kzu_property_kzuPropertyCreateFixedProperties00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzcMemoryManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuFixedPropertyTypes",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzcMemoryManager* memoryManager = ((const struct KzcMemoryManager*)  tolua_tousertype(tolua_S,1,0));
  struct KzuFixedPropertyTypes* out_fixedPropertyTypes = ((struct KzuFixedPropertyTypes*)  tolua_tousertype(tolua_S,2,0));
  {
   kzsError tolua_ret = (kzsError)0;//  kzuPropertyCreateFixedProperties(memoryManager,&out_fixedPropertyTypes);
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
    tolua_pushusertype(tolua_S,(void*)out_fixedPropertyTypes,"KzuFixedPropertyTypes");
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyCreateFixedProperties'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyDeleteFixedProperties */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyDeleteFixedProperties00
static int tolua_kzu_property_kzuPropertyDeleteFixedProperties00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuFixedPropertyTypes",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuFixedPropertyTypes* fixedPropertyTypes = ((struct KzuFixedPropertyTypes*)  tolua_tousertype(tolua_S,1,0));
  {
      kzsError tolua_ret = (kzsError)  0;//kzuPropertyDeleteFixedProperties(fixedPropertyTypes);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyDeleteFixedProperties'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeTakeOwnership */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeTakeOwnership00
static int tolua_kzu_property_kzuPropertyTypeTakeOwnership00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzuPropertyTypeTakeOwnership(propertyType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeTakeOwnership'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeIsValid */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeIsValid00
static int tolua_kzu_property_kzuPropertyTypeIsValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const struct KzuPropertyType* propertyType = ((const struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  {
   kzBool tolua_ret = (kzBool)  kzuPropertyTypeIsValid(propertyType);
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
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeIsValid'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* function: kzuPropertyTypeTransferData */
#ifndef TOLUA_DISABLE_tolua_kzu_property_kzuPropertyTypeTransferData00
static int tolua_kzu_property_kzuPropertyTypeTransferData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"KzuPropertyType",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  struct KzuPropertyType* targetPropertyType = ((struct KzuPropertyType*)  tolua_tousertype(tolua_S,1,0));
  struct KzuPropertyType* sourcePropertyType = ((struct KzuPropertyType*)  tolua_tousertype(tolua_S,2,0));
  {
   kzuPropertyTypeTransferData(targetPropertyType,sourcePropertyType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'kzuPropertyTypeTransferData'.",&tolua_err);
 return 0;
#endif
}
#endif /* #ifndef TOLUA_DISABLE */

/* Open function */
 int tolua_kzu_property_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"KzcMemoryManager","KzcMemoryManager","",NULL);
  tolua_beginmodule(tolua_S,"KzcMemoryManager");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzcDynamicArray","KzcDynamicArray","",NULL);
  tolua_beginmodule(tolua_S,"KzcDynamicArray");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuProperty","KzuProperty","",NULL);
  tolua_beginmodule(tolua_S,"KzuProperty");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"KzuPropertyType","KzuPropertyType","",NULL);
  tolua_beginmodule(tolua_S,"KzuPropertyType");
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"kzuPropertyDelete",tolua_kzu_property_kzuPropertyDelete00);
  tolua_function(tolua_S,"kzuPropertyCopy",tolua_kzu_property_kzuPropertyCopy00);
  tolua_function(tolua_S,"kzuPropertyCopyContent",tolua_kzu_property_kzuPropertyCopyContent00);
  tolua_function(tolua_S,"kzuPropertyCopyValue",tolua_kzu_property_kzuPropertyCopyValue00);
  tolua_function(tolua_S,"kzuPropertySetPriority",tolua_kzu_property_kzuPropertySetPriority00);
  tolua_function(tolua_S,"kzuPropertyGetPriority",tolua_kzu_property_kzuPropertyGetPriority00);
  tolua_function(tolua_S,"kzuPropertyGetPropertyType",tolua_kzu_property_kzuPropertyGetPropertyType00);
  tolua_function(tolua_S,"kzuPropertyTypeDelete",tolua_kzu_property_kzuPropertyTypeDelete00);
  tolua_function(tolua_S,"kzuPropertyTypeGetName",tolua_kzu_property_kzuPropertyTypeGetName00);
  tolua_function(tolua_S,"kzuPropertyTypeGetDataType",tolua_kzu_property_kzuPropertyTypeGetDataType00);
  tolua_function(tolua_S,"kzuPropertyTypeSetInherited",tolua_kzu_property_kzuPropertyTypeSetInherited00);
  tolua_function(tolua_S,"kzuPropertyTypeGetInherited",tolua_kzu_property_kzuPropertyTypeGetInherited00);
  tolua_function(tolua_S,"kzuPropertyGetFromKzb",tolua_kzu_property_kzuPropertyGetFromKzb00);
  tolua_function(tolua_S,"kzuPropertySetFromKzb",tolua_kzu_property_kzuPropertySetFromKzb00);
  tolua_function(tolua_S,"kzuPropertyDisableFromKzb",tolua_kzu_property_kzuPropertyDisableFromKzb00);
  tolua_function(tolua_S,"kzuPropertySetFlags",tolua_kzu_property_kzuPropertySetFlags00);
  tolua_function(tolua_S,"kzuPropertyIsInherited",tolua_kzu_property_kzuPropertyIsInherited00);
  tolua_function(tolua_S,"kzuPropertySetInherited",tolua_kzu_property_kzuPropertySetInherited00);
  tolua_function(tolua_S,"kzuPropertyCreateFixedProperties",tolua_kzu_property_kzuPropertyCreateFixedProperties00);
  tolua_function(tolua_S,"kzuPropertyDeleteFixedProperties",tolua_kzu_property_kzuPropertyDeleteFixedProperties00);
  tolua_function(tolua_S,"kzuPropertyTypeTakeOwnership",tolua_kzu_property_kzuPropertyTypeTakeOwnership00);
  tolua_function(tolua_S,"kzuPropertyTypeIsValid",tolua_kzu_property_kzuPropertyTypeIsValid00);
  tolua_function(tolua_S,"kzuPropertyTypeTransferData",tolua_kzu_property_kzuPropertyTypeTransferData00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
  int luaopen_kzu_property (lua_State* tolua_S) {
 return tolua_kzu_property_open(tolua_S);
}
#endif

