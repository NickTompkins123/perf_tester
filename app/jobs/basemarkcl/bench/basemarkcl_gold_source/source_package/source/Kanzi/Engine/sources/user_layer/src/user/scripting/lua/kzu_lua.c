/**
* \file
* Lua scripting.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_lua.h"

#undef ENABLE_LUA
#ifdef ENABLE_LUA

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_file.h>
#include <core/debug/kzc_log.h>

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_math.h>
#include <system/kzs_error_codes.h>

#include <lua.h>
#include <lualib.h>
#include <tolua++.h>


struct KzuLua
{
    lua_State* luaState; /**< Instance of Lua interpreter and state. */
};

/** Helper structure used for loading the Lua scripts from memory. */
struct KzuLuaReaderHelper
{
    void* data;  /**< Data block to be read. */
    kzUint size; /**< Size of the data block. */
    kzBool read; /**< Has the data block already been read. */
};


/** Allocation function for Lua internal use. */
static void* kzuLuaMalloc_internal(void* memoryManager, void* block, kzUint oldSize, kzUint newSize);
/** Helper function for reading Kua scripts from memory. */
const kzChar* kzuLuaReaderFunction_internal(lua_State *L, void *data, kzUint *size);


kzsError kzuLuaCreate(struct KzcMemoryManager* memoryManager, struct KzuLua** out_lua)
{
    kzsError result;
    struct KzuLua* lua;

    result = kzcMemoryAllocVariable(memoryManager, lua, "Lua");
    kzsErrorForward(result);

    lua->luaState = lua_newstate(kzuLuaMalloc_internal, memoryManager);
    kzsErrorTest(lua->luaState != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Lua state creation failed due to lack of memory.");

    luaL_openlibs(lua->luaState);

    *out_lua = lua;
    kzsSuccess();
}

kzsError kzuLuaDelete(struct KzuLua* lua)
{
    kzsError result;
    
    kzsAssert(kzcIsValidPointer(lua));

    lua_close(lua->luaState);

    result = kzcMemoryFreeVariable(lua);
    kzsErrorForward(result);

    kzsSuccess();
}

struct lua_State* kzuLuaGetLuaState(const struct KzuLua* lua)
{
    kzsAssert(kzcIsValidPointer(lua));
    return lua->luaState;
}

kzsError kzuLuaLoadFromFile(const struct KzuLua* lua, kzString filePath)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(lua));

    if(luaL_loadfile(lua->luaState, filePath) || lua_pcall(lua->luaState, 0, 0, 0))
    {
        result = kzcLog(kzcMemoryGetManager(lua), KZS_LOG_LEVEL_WARNING, "Lua loading failed: %s", lua_tostring(lua->luaState, -1));
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuLuaLoadFromString(const struct KzuLua* lua, kzString script)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(lua));

    if(luaL_loadstring(lua->luaState, script) || lua_pcall(lua->luaState, 0, 0, 0))
    {
        result = kzcLog(kzcMemoryGetManager(lua), KZS_LOG_LEVEL_WARNING, "Lua compile failed: %s", lua_tostring(lua->luaState, -1));
        kzsErrorForward(result);
    }
    kzsSuccess();
}

kzsError kzuLuaLoadFromMemory(const struct KzuLua* lua, void* data, kzUint size, kzString chunkName)
{
    kzsError result;
    struct KzuLuaReaderHelper helper;

    kzsAssert(kzcIsValidPointer(lua));

    helper.data = data;
    helper.size = size;
    helper.read = KZ_FALSE;

    if(lua_load(lua->luaState, kzuLuaReaderFunction_internal, &helper, chunkName) || lua_pcall(lua->luaState, 0, 0, 0))
    {
        result = kzcLog(kzcMemoryGetManager(lua), KZS_LOG_LEVEL_WARNING, "Lua compile failed: %s", lua_tostring(lua->luaState, -1));
        kzsErrorForward(result);
    }
    kzsSuccess();
}

kzInt kzuLuaGetGlobalInteger(const struct KzuLua* lua, kzString name)
{
    kzInt returnValue;
    
    kzsAssert(kzcIsValidPointer(lua));

    lua_getglobal(lua->luaState, name);
    if(!lua_isnumber(lua->luaState, -1))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "LUA: Requested global integer was not number.");
    }
    returnValue = (kzInt)lua_tointeger(lua->luaState, -1);
    
    return returnValue;
}

kzFloat kzuLuaGetGlobalFloat(const struct KzuLua* lua, kzString name)
{
    kzFloat returnValue;

    kzsAssert(kzcIsValidPointer(lua));

    lua_getglobal(lua->luaState, name);
    if(!lua_isnumber(lua->luaState, -1))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "LUA: Requested global float was not number.");
    }
    returnValue = (kzFloat)lua_tonumber(lua->luaState, -1);

    return returnValue;
}

kzString kzuLuaGetGlobalString(const struct KzuLua* lua, kzString name)
{
    kzString returnValue;

    kzsAssert(kzcIsValidPointer(lua));

    lua_getglobal(lua->luaState, name);
    if(!lua_isstring(lua->luaState, -1))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "LUA: Requested global string was not string.");
    }
    returnValue = (kzString)lua_tostring(lua->luaState, -1);

    return returnValue;
}

void* kzuLuaGetGlobalUserData(const struct KzuLua* lua, kzString name)
{
    void* returnValue;

    kzsAssert(kzcIsValidPointer(lua));

    lua_getglobal(lua->luaState, name);
    if(!lua_isuserdata(lua->luaState, -1))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "LUA: Requested global userdata was not userdata.");
    }
    returnValue = lua_touserdata(lua->luaState, -1);

    return returnValue;
}

void* kzuLuaGetGlobalUserType(const struct KzuLua* lua, kzString name, kzString typeName)
{
    void* returnValue;

    kzsAssert(kzcIsValidPointer(lua));

    lua_getglobal(lua->luaState, name);
    if(!tolua_isusertype(lua->luaState, -1, typeName, 0, KZ_NULL))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "LUA: Requested global usertype was not usertype.");
    }
    returnValue = tolua_tousertype(lua->luaState, -1, (kzMutableString)typeName);

    return returnValue;
}

void kzuLuaSetGlobalInteger(const struct KzuLua* lua, kzString name, kzInt value)
{
    kzsAssert(kzcIsValidPointer(lua));
    lua_pushinteger(lua->luaState, (lua_Integer)value);
    lua_setglobal(lua->luaState, name);
}

void kzuLuaSetGlobalFloat(const struct KzuLua* lua, kzString name, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(lua));
    lua_pushnumber(lua->luaState, (lua_Number)value);
    lua_setglobal(lua->luaState, name);
}

void kzuLuaSetGlobalString(const struct KzuLua* lua, kzString name, kzString value)
{
    kzsAssert(kzcIsValidPointer(lua));
    lua_pushstring(lua->luaState, value);
    lua_setglobal(lua->luaState, name);
}

void kzuLuaSetGlobalUserData(const struct KzuLua* lua, kzString name, void* value)
{
    kzsAssert(kzcIsValidPointer(lua));
    lua_pushlightuserdata(lua->luaState, value);
    lua_setglobal(lua->luaState, name);
}

void kzuLuaSetGlobalUserType(const struct KzuLua* lua, kzString name, void* value, kzString typeName)
{
    kzsAssert(kzcIsValidPointer(lua));
    tolua_pushusertype(lua->luaState, value, typeName);
    lua_setglobal(lua->luaState, name);
}

KZ_CALLBACK static void* kzuLuaMalloc_internal(void* memoryManager, void* block, kzUint oldSize, kzUint newSize)
{
    kzsError result;
    void* returnValue;

    /*
    Requirements for the allocator function:
     - block is NULL if and only if oldSize is zero. 
     - newSize is zero, the allocator must return NULL
     - if oldSize is not zero, it should free the block pointed to by block
     - when newSize is not zero, the allocator returns NULL if and only if it cannot fill the request
     - when newSize is not zero and oldSize is zero, the allocator should behave like malloc
     - when newSize and oldSize are not zero, the allocator behaves like realloc. 
     TODO: The following requirement is not true here. What can be done?
      - Lua assumes that the allocator never fails when oldSize >= newSize. 
    */

    if (newSize == 0) 
    {
        if (oldSize != 0)
        {
            result = kzcMemoryFreePointer(block);
            KZ_UNUSED_RETURN_VALUE(result); /* Ignore, as the call-back can't handle errors. */
        }
        returnValue = KZ_NULL;
    }
    else
    {
        /* TODO: Use kzcMemoryReallocPointer */
        void* newBlock;
        result = kzcMemoryAllocPointer(memoryManager, &newBlock, newSize, "LUA Internals");
        if(block != KZ_NULL)
        {
            kzsMemcpy(newBlock, block, kzsMinU(oldSize, newSize));
            result = kzcMemoryFreePointer(block);
            KZ_UNUSED_RETURN_VALUE(result); /* Ignore, as the call-back can't handle errors. */
        }

        returnValue = newBlock;
        
        /* TODO: Return KZ_NULL if cannot allocate memory. Currently asserts. Needs changes to memory managers. */
    }

    return returnValue;
}

kzsError kzuLuaCallFunction(const struct KzuLua* lua, kzString functionName, kzString parameterDescriptor, ...)
{
    kzsError result;
    kzInt numberResults;
    va_list vaList;
    kzMutableString nextParameterName = KZ_NULL;
    kzInt parameterCount = 0;
    kzInt resultCount = 0;

    kzsAssert(kzcIsValidPointer(lua));

    /* TODO: Use kzs_arg.h */
    va_start(vaList, parameterDescriptor);
    lua_getglobal(lua->luaState, functionName);

    {
        kzString parameterDesc = parameterDescriptor;
        kzBool parameters = KZ_TRUE;
        while(*parameterDesc != KZ_NULL)
        {
            kzChar c = *parameterDesc++;
            switch(c)
            {
                case 'P':
                {
                    /* Skip string type descriptors. */
                    break;
                }
                case '>':
                {
                    parameters = KZ_FALSE;
                    break;
                }
                default:
                {
                    if(parameters)
                    {
                        parameterCount++;
                    }
                    else
                    {
                        resultCount++;
                    }
                    break;
                }    
            }
        }
    }

    while(*parameterDescriptor != KZ_NULL)
    {
        kzBool breakOut = KZ_FALSE;
        luaL_checkstack(lua->luaState, 1, "Too many arguments");

        switch(*parameterDescriptor++)
        {
            case 'f':
            {
                /*lint -e{632,662,826}*/
                lua_pushnumber(lua->luaState, (lua_Number)va_arg(vaList, kzDouble));
                break;
            }
            case 'i':
            {
                /*lint -e{632,662,826}*/
                lua_pushinteger(lua->luaState, va_arg(vaList, kzInt));
                break;
            }
            case 's':
            {
                /*lint -e{632,662,826}*/
                lua_pushstring(lua->luaState, va_arg(vaList, char*));
                break;
            }
            case 'u':
            {
                /*lint -e{632,662,826}*/
                lua_pushlightuserdata(lua->luaState, va_arg(vaList, void*));
                break;
            }
            case 'p':
            {
                /*lint -e{632,662,826}*/
                nextParameterName = (kzMutableString)va_arg(vaList, char*);
                break;
            }
            case 'P':
            {
                kzsAssertText(nextParameterName != KZ_NULL, "kzuLuaCallFunction, P was not preceded by p");
                /*lint -e{632,662,826}*/
                tolua_pushusertype(lua->luaState, va_arg(vaList, void*), nextParameterName);
                nextParameterName = KZ_NULL;
                break;
            }
            case '>':
            {
                breakOut = KZ_TRUE;
                break;
            }
            default:
            {
                kzChar foundCharacter = *(parameterDescriptor - 1);
                result = kzcLog(kzcMemoryGetManager(lua), KZS_LOG_LEVEL_WARNING, "Invalid option: '%c'", foundCharacter);
                kzsErrorForward(result);
                kzsAssert(KZ_FALSE);
            }
        }

        if(breakOut)
        {
            break;
        }
    }

    if(lua_pcall(lua->luaState, parameterCount, resultCount, 0) != 0)
    {
        result = kzcLog(kzcMemoryGetManager(lua), KZS_LOG_LEVEL_WARNING, "Lua error calling Lua-function: %s", lua_tostring(lua->luaState, -1));
        kzsErrorForward(result);
    }

    numberResults = -resultCount;
    while(*parameterDescriptor != KZ_NULL)
    {
        switch(*parameterDescriptor++)
        {
            case 'f':
            {
                if(!lua_isnumber(lua->luaState, numberResults))
                {
                    kzsLog(KZS_LOG_LEVEL_WARNING, "Lua wrong type encountered for return parameter. Float, but return value was not number.");
                }
                /*lint -e{632,662,826}*/
                *va_arg(vaList, kzFloat*) = (kzFloat)lua_tonumber(lua->luaState, numberResults);
                break;
            }
            case 'i':
            {
                if(!lua_isnumber(lua->luaState, numberResults))
                {
                    kzsLog(KZS_LOG_LEVEL_WARNING, "Lua wrong type encountered for return parameter. Integer, but return value was not number.");
                }
                /*lint -e{632,662,826}*/
                *va_arg(vaList, kzInt*) = lua_tointeger(lua->luaState, numberResults);
                break;
            }
            case 's':
            {
                if(!lua_isstring(lua->luaState, numberResults))
                {
                    kzsLog(KZS_LOG_LEVEL_WARNING, "Lua wrong type encountered for return parameter. String, but return value was not string.");
                }
                /*lint -e{632,662,826}*/
                *va_arg(vaList, kzString*) = lua_tostring(lua->luaState, numberResults);
                break;
            }
            case 'u':
            {
                if(!lua_islightuserdata(lua->luaState, numberResults))
                {
                    kzsLog(KZS_LOG_LEVEL_WARNING, "Lua wrong type encountered for return parameter. UserData, but return value was not userData.");
                }
                /*lint -e{632,662,826}*/
                *va_arg(vaList, kzString*) = lua_touserdata(lua->luaState, numberResults);
                break;
            }
            case 'p':
            {
                /*lint -e{632,662,826}*/
                nextParameterName = (kzMutableString)va_arg(vaList, char*);
                break;
            }
            case 'P':
            {
                kzsAssertText(nextParameterName != KZ_NULL, "kzuLuaCallFunction, P was not preceded by p");
                if(!tolua_isusertype(lua->luaState, numberResults, nextParameterName, 0, KZ_NULL))
                {
                    kzsLog(KZS_LOG_LEVEL_WARNING, "Lua wrong type encountered for return parameter. UserData, but return value was not UserData.");
                }
                {
                    void* value = tolua_tousertype(lua->luaState, numberResults, nextParameterName);
                    /*lint -e{632,662,826}*/
                    *va_arg(vaList, void**) = value;
                }
                nextParameterName = KZ_NULL;
                break;
            }
            default:
            {
                kzChar foundCharacter = *(parameterDescriptor - 1);
                result = kzcLog(kzcMemoryGetManager(lua), KZS_LOG_LEVEL_WARNING, "Invalid option: '%c'", foundCharacter);
                kzsErrorForward(result);
                kzsAssert(KZ_FALSE);
            }
        }
    }

    va_end(vaList);
    kzsSuccess();
}

kzsError kzuLuaAddFunction(const struct KzuLua* lua, kzString functionName, kzuLuaUserCFunction userFunction)
{
    kzsAssert(kzcIsValidPointer(lua));

    lua_pushcfunction(lua->luaState, (lua_CFunction)userFunction);
    lua_setglobal(lua->luaState, functionName);

    kzsSuccess();
}

KZ_CALLBACK const kzChar* kzuLuaReaderFunction_internal(lua_State *luaState, void *data, kzUint *size)
{
    void* returnData;
    struct KzuLuaReaderHelper* helper = (struct KzuLuaReaderHelper*)data;

    if(!helper->read)
    {
        *size = helper->size;
        returnData = helper->data;
    }
    else
    {
        *size = 0;
        returnData = KZ_NULL;
    }
    return returnData;
}

#else

KZ_EMPTY_SOURCE_FILE;

#endif
