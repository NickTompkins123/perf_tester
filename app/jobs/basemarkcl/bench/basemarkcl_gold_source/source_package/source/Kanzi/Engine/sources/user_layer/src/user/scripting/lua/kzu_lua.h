/**
* \file
* Lua scripting.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_LUA_H
#define KZU_LUA_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzuProperty;
struct KzuAnimation;
struct KzcHashMap;

struct lua_State;


/**
* \struct KzuLua
* Structure for Lua script interpreter and state.
*/
struct KzuLua;


/* Lua user function prototype. */
typedef kzInt (*kzuLuaUserCFunction)(struct lua_State *state);


/** Creates and initializes Lua state structure. */
kzsError kzuLuaCreate(struct KzcMemoryManager* memoryManager, struct KzuLua** out_lua);
/** Deletes Lua state structure. */
kzsError kzuLuaDelete(struct KzuLua* lua);


/** Returns the Lua interpreter and state structure, which can be used to access the Lua calls directly. */
struct lua_State* kzuLuaGetLuaState(const struct KzuLua* lua);


/** Loads a file to Lua interpreter from given path. File can be lua source code or precompiled file. */
kzsError kzuLuaLoadFromFile(const struct KzuLua* lua, kzString filePath);
/** Loads and compiles script contained in parameter string to Lua interpreter. */
kzsError kzuLuaLoadFromString(const struct KzuLua* lua, kzString script);
/** Loads and compiles script contained in parameter string to Lua interpreter. BlockName parameter is used for error reporting. */
kzsError kzuLuaLoadFromMemory(const struct KzuLua* lua, void* data, kzUint size, kzString blockName);

/** Gets integer value from global scope of Lua state. */
kzInt kzuLuaGetGlobalInteger(const struct KzuLua* lua, kzString name);
/** Gets float value from global scope of Lua state. */
kzFloat kzuLuaGetGlobalFloat(const struct KzuLua* lua, kzString name);
/** Gets string value from global scope of Lua state. */
kzString kzuLuaGetGlobalString(const struct KzuLua* lua, kzString name);
/** Gets user data from global scope of Lua state. */
void* kzuLuaGetGlobalUserData(const struct KzuLua* lua, kzString name);
/** Gets user type from global scope of Lua state. */
void* kzuLuaGetGlobalUserType(const struct KzuLua* lua, kzString name, kzString typeName);

/** Sets integer value to global scope of Lua state with given variable name. */
void kzuLuaSetGlobalInteger(const struct KzuLua* lua, kzString name, kzInt value);
/** Sets float value to global scope of Lua state with given variable name. */
void kzuLuaSetGlobalFloat(const struct KzuLua* lua, kzString name, kzFloat value);
/** Sets string value to global scope of Lua state with given variable name. Allocates memory internally for storing the String. */
void kzuLuaSetGlobalString(const struct KzuLua* lua, kzString name, kzString value);
/** Sets user data to global scope of Lua state with given variable name. */
void kzuLuaSetGlobalUserData(const struct KzuLua* lua, kzString name, void* value);
/** Sets user type data to global scope of Lua state with given variable name. */
void kzuLuaSetGlobalUserType(const struct KzuLua* lua, kzString name, void* value, kzString typeName);


/** Adds a function to Lua state with given name. After the call the function can be called from Lua script. */
kzsError kzuLuaAddFunction(const struct KzuLua* lua, kzString functionName, kzuLuaUserCFunction userFunction);


/**
* Calls a function available in Lua state. 
* \param lua Lua state
* \param functionName Name of the function in Lua state
* \param parameterDescriptor Descriptor for function parameters and return values.   
*                            The descriptor is given as mapping "x>y" where x and y can be any sequence of the following options:
*                            - 'i' for kzInt
*                            - 'f' for kzFloat
*                            - 's' for kzString
*                            - 'u' for void data
*                            - 'P' for user type, must be preceeded with 'p' which must be string telling the type of user type in C. See the example below.
*                            The '>' symbol ends the parameter list and all characters after the symbol are treated as return values.
*                            Symbol '>' is allowed only once.
*                            For example "fis>f" would be treated as function with parameters kzFloat, kzInt, kzString and return value kzFloat.
* \param va_args The parameters in va_args list must match the parameters and return values listed in parameterDescriptor.
* Example1:
*   kzInt a = 0;
*   kzFloat b = 0;
*   kzString str = "test";
*   kzFloat returnValue;
*   kzuLuaCallFunction(state, "fun", "fis>f", a, b, str, &returnValue);
* Example2:
*   result = kzuLuaCallFunction(luaTest, "luaAddObject", "pPpP>pP", 
*       "const KzuScene", kzaApplicationGetScene(application), 
*       "KzuObjectNode", kzuMeshNodeToObjectNode(meshNode), 
*       "kzsError", &res);
*   kzsErrorForward(result);
*   kzsErrorForward(res);
*/
kzsError kzuLuaCallFunction(const struct KzuLua* lua, kzString functionName, kzString parameterDescriptor, ...);


#endif
