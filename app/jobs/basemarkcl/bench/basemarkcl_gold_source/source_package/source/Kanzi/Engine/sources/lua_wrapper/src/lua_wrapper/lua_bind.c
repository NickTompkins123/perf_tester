/**
* \file
* Binds lua functions
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "lua_bind.h"

#include <lua_wrapper/user/lua_kzu_camera.h>
#include <lua_wrapper/user/lua_kzu_composer.h>
#include <lua_wrapper/user/lua_kzu_engine.h>
#include <lua_wrapper/user/lua_kzu_engine_message.h>
#include <lua_wrapper/user/lua_kzu_material.h>
#include <lua_wrapper/user/lua_kzu_object.h>
#include <lua_wrapper/user/lua_kzu_property.h>
#include <lua_wrapper/user/lua_kzu_scene.h>
#include <lua_wrapper/user/lua_kzu_user.h>
#include <lua_wrapper/user/lua_kzu_project.h>
#include <lua_wrapper/user/lua_kzu_ui_component.h>
#include <lua_wrapper/user/lua_kzu_ui_component_type.h>
#include <lua_wrapper/user/lua_kzu_ui_event.h>
#include <lua_wrapper/user/lua_kzu_ui_event_listener.h>
#include <lua_wrapper/user/lua_kzu_ui_screen.h>


kzsError kzuBindLua(lua_State* tolua_S)
{
    tolua_kzu_camera_open(tolua_S);
    tolua_kzu_composer_open(tolua_S);
    tolua_kzu_engine_open(tolua_S);
    tolua_kzu_engine_message_open(tolua_S);
    tolua_kzu_material_open(tolua_S);
    tolua_kzu_object_open(tolua_S);
    tolua_kzu_property_open(tolua_S);
    tolua_kzu_scene_open(tolua_S);
    tolua_kzu_user_open(tolua_S);
    tolua_kzu_project_open(tolua_S);

    tolua_kzu_ui_component_open(tolua_S);

    kzsSuccess();
}
