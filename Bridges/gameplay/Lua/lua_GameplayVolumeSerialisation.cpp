#include "Base.h"
#include "ScriptController.h"
#include "lua_GameplayVolumeSerialisation.h"
#include ""

namespace Cubiquity
{

void luaRegister_GameplayVolumeSerialisation()
{
    const luaL_Reg lua_members[] = 
    {
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"gameplayLoadData", lua_GameplayVolumeSerialisation_static_gameplayLoadData},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    ScriptUtil::registerClass("GameplayVolumeSerialisation", lua_members, lua_GameplayVolumeSerialisation__init, lua_GameplayVolumeSerialisation__gc, lua_statics, scopePath);
}

static GameplayVolumeSerialisation* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "GameplayVolumeSerialisation");
    luaL_argcheck(state, userdata != NULL, 1, "'GameplayVolumeSerialisation' expected.");
    return (GameplayVolumeSerialisation*)((ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_GameplayVolumeSerialisation__gc(lua_State* state)
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
                void* userdata = luaL_checkudata(state, 1, "GameplayVolumeSerialisation");
                luaL_argcheck(state, userdata != NULL, 1, "'GameplayVolumeSerialisation' expected.");
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    GameplayVolumeSerialisation* instance = (GameplayVolumeSerialisation*)object->instance;
                    SAFE_DELETE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplayVolumeSerialisation__gc - Failed to match the given parameters to a valid function signature.");
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

int lua_GameplayVolumeSerialisation__init(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            void* returnPtr = (void*)new GameplayVolumeSerialisation();
            if (returnPtr)
            {
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                object->instance = returnPtr;
                object->owns = true;
                luaL_getmetatable(state, "GameplayVolumeSerialisation");
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

int lua_GameplayVolumeSerialisation_static_gameplayLoadData(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    ScriptUtil::LuaArray<const char> param1 = ScriptUtil::getString(1, false);

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<GameplayColouredCubesVolume> param2 = ScriptUtil::getObjectPointer<GameplayColouredCubesVolume>(2, "GameplayColouredCubesVolume", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    GameplayVolumeSerialisation::gameplayLoadData(param1, param2);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    ScriptUtil::LuaArray<const char> param1 = ScriptUtil::getString(1, false);

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<GameplaySmoothTerrainVolume> param2 = ScriptUtil::getObjectPointer<GameplaySmoothTerrainVolume>(2, "GameplaySmoothTerrainVolume", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    GameplayVolumeSerialisation::gameplayLoadData(param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_GameplayVolumeSerialisation_static_gameplayLoadData - Failed to match the given parameters to a valid function signature.");
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

}
