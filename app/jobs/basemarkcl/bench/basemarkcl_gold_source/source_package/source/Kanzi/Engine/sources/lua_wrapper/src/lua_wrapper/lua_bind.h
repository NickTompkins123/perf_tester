/**
* \file
* Binds lua functions
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_LUA_BIND_H
#define KZU_LUA_BIND_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <lua.h>

/** Binds engine functions to given Lua state. */
kzsError kzuBindLua(lua_State* tolua_S);


#endif
