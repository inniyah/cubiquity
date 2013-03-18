#include "Base.h"
#include "ScriptController.h"
#include "lua_GameplaySmoothTerrainVolume.h"
#include "GameplaySmoothTerrainVolume.h"
#include "gameplay.h"

using namespace Cubiquity;

namespace gameplay
{

void luaRegister_GameplaySmoothTerrainVolume()
{
    const luaL_Reg lua_members[] = 
    {
        {"getRootNodeForLua", lua_GameplaySmoothTerrainVolume_getRootNodeForLua},
        {"getVolumeForLua", lua_GameplaySmoothTerrainVolume_getVolumeForLua},
        {"performUpdate", lua_GameplaySmoothTerrainVolume_performUpdate},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"create", lua_GameplaySmoothTerrainVolume_static_create},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    ScriptUtil::registerClass("GameplaySmoothTerrainVolume", lua_members, NULL, NULL, lua_statics, scopePath);
}

static GameplaySmoothTerrainVolume* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "GameplaySmoothTerrainVolume");
    luaL_argcheck(state, userdata != NULL, 1, "'GameplaySmoothTerrainVolume' expected.");
    return (GameplaySmoothTerrainVolume*)((ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_GameplaySmoothTerrainVolume_getRootNodeForLua(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                lua_type(state, 2) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                int param1 = (int)luaL_checkint(state, 2);

                GameplaySmoothTerrainVolume* instance = getInstance(state);
                void* returnPtr = (void*)instance->getRootNodeForLua(param1);
                if (returnPtr)
                {
                    ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                    object->instance = returnPtr;
                    object->owns = false;
                    luaL_getmetatable(state, "Node");
                    lua_setmetatable(state, -2);
                }
                else
                {
                    lua_pushnil(state);
                }

                return 1;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolume_getRootNodeForLua - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplaySmoothTerrainVolume_getVolumeForLua(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                lua_type(state, 2) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                int param1 = (int)luaL_checkint(state, 2);

                GameplaySmoothTerrainVolume* instance = getInstance(state);
                SmoothTerrainVolume* result = instance->getVolumeForLua(param1);

                // Push the return value onto the stack.
                lua_pushlightuserdata(state, result);
                return 1;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolume_getVolumeForLua - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplaySmoothTerrainVolume_performUpdate(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 3:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                lua_type(state, 3) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector3> param1 = ScriptUtil::getObjectPointer<Vector3>(2, "Vector3", true, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector3'.");
                    lua_error(state);
                }

                // Get parameter 2 off the stack.
                float param2 = (float)luaL_checknumber(state, 3);

                GameplaySmoothTerrainVolume* instance = getInstance(state);
                instance->performUpdate(*param1, param2);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolume_performUpdate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplaySmoothTerrainVolume_static_create(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNONE)
                {
                    // Get parameter 1 off the stack.
                    GP_WARN("Attempting to get parameter 1 with unrecognized type SmoothTerrainVolume as an unsigned integer.");
                    SmoothTerrainVolume* param1 = (SmoothTerrainVolume)luaL_checkunsigned(state, 1);

                    void* returnPtr = (void*)GameplaySmoothTerrainVolume::create(param1);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = false;
                        luaL_getmetatable(state, "GameplaySmoothTerrainVolume");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolume_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 8:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNUMBER &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    lua_type(state, 4) == LUA_TNUMBER &&
                    lua_type(state, 5) == LUA_TNUMBER &&
                    lua_type(state, 6) == LUA_TNUMBER &&
                    lua_type(state, 7) == LUA_TNUMBER &&
                    lua_type(state, 8) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    int param1 = (int)luaL_checkint(state, 1);

                    // Get parameter 2 off the stack.
                    int param2 = (int)luaL_checkint(state, 2);

                    // Get parameter 3 off the stack.
                    int param3 = (int)luaL_checkint(state, 3);

                    // Get parameter 4 off the stack.
                    int param4 = (int)luaL_checkint(state, 4);

                    // Get parameter 5 off the stack.
                    int param5 = (int)luaL_checkint(state, 5);

                    // Get parameter 6 off the stack.
                    int param6 = (int)luaL_checkint(state, 6);

                    // Get parameter 7 off the stack.
                    unsigned int param7 = (unsigned int)luaL_checkunsigned(state, 7);

                    // Get parameter 8 off the stack.
                    unsigned int param8 = (unsigned int)luaL_checkunsigned(state, 8);

                    void* returnPtr = (void*)GameplaySmoothTerrainVolume::create(param1, param2, param3, param4, param5, param6, param7, param8);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "GameplaySmoothTerrainVolume");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolume_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1 or 8).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
