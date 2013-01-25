#include "Base.h"
#include "ScriptController.h"
#include "lua_GameplayRaycasting.h"
#include "GameplayRaycasting.h"

namespace gameplay
{

void luaRegister_GameplayRaycasting()
{
    const luaL_Reg lua_members[] = 
    {
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"gameplayRaycast", lua_GameplayRaycasting_static_gameplayRaycast},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    ScriptUtil::registerClass("GameplayRaycasting", lua_members, lua_GameplayRaycasting__init, lua_GameplayRaycasting__gc, lua_statics, scopePath);
}

static GameplayRaycasting* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "GameplayRaycasting");
    luaL_argcheck(state, userdata != NULL, 1, "'GameplayRaycasting' expected.");
    return (GameplayRaycasting*)((ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_GameplayRaycasting__gc(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                void* userdata = luaL_checkudata(state, 1, "GameplayRaycasting");
                luaL_argcheck(state, userdata != NULL, 1, "'GameplayRaycasting' expected.");
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    GameplayRaycasting* instance = (GameplayRaycasting*)object->instance;
                    SAFE_DELETE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplayRaycasting__gc - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplayRaycasting__init(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            void* returnPtr = (void*)new GameplayRaycasting();
            if (returnPtr)
            {
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                object->instance = returnPtr;
                object->owns = true;
                luaL_getmetatable(state, "GameplayRaycasting");
                lua_setmetatable(state, -2);
            }
            else
            {
                lua_pushnil(state);
            }

            return 1;
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 0).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplayRaycasting_static_gameplayRaycast(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 4:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TNIL) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                lua_type(state, 3) == LUA_TNUMBER &&
                (lua_type(state, 4) == LUA_TUSERDATA || lua_type(state, 4) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<GameplaySmoothTerrainVolume> param1 = ScriptUtil::getObjectPointer<GameplaySmoothTerrainVolume>(1, "GameplaySmoothTerrainVolume", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'GameplaySmoothTerrainVolume'.");
                    lua_error(state);
                }

                // Get parameter 2 off the stack.
                bool param2Valid;
                ScriptUtil::LuaArray<Ray> param2 = ScriptUtil::getObjectPointer<Ray>(2, "Ray", true, &param2Valid);
                if (!param2Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 2 to type 'Ray'.");
                    lua_error(state);
                }

                // Get parameter 3 off the stack.
                float param3 = (float)luaL_checknumber(state, 3);

                // Get parameter 4 off the stack.
                bool param4Valid;
                ScriptUtil::LuaArray<Vector3> param4 = ScriptUtil::getObjectPointer<Vector3>(4, "Vector3", true, &param4Valid);
                if (!param4Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 4 to type 'Vector3'.");
                    lua_error(state);
                }

                bool result = GameplayRaycasting::gameplayRaycast(param1, *param2, param3, *param4);

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_GameplayRaycasting_static_gameplayRaycast - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 4).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
