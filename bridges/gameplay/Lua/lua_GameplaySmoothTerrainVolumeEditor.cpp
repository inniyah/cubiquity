#include "Base.h"
#include "ScriptController.h"
#include "lua_GameplaySmoothTerrainVolumeEditor.h"
#include "GameplaySmoothTerrainVolumeEditor.h"

using namespace gameplay;

namespace Cubiquity
{

void luaRegister_GameplaySmoothTerrainVolumeEditor()
{
    const luaL_Reg lua_members[] = 
    {
        {"addMaterial", lua_GameplaySmoothTerrainVolumeEditor_addMaterial},
        {"applyPaint", lua_GameplaySmoothTerrainVolumeEditor_applyPaint},
        {"smooth", lua_GameplaySmoothTerrainVolumeEditor_smooth},
        {"subtractMaterial", lua_GameplaySmoothTerrainVolumeEditor_subtractMaterial},
        {NULL, NULL}
    };
    const luaL_Reg* lua_statics = NULL;
    std::vector<std::string> scopePath;

    ScriptUtil::registerClass("GameplaySmoothTerrainVolumeEditor", lua_members, lua_GameplaySmoothTerrainVolumeEditor__init, lua_GameplaySmoothTerrainVolumeEditor__gc, lua_statics, scopePath);
}

static GameplaySmoothTerrainVolumeEditor* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "GameplaySmoothTerrainVolumeEditor");
    luaL_argcheck(state, userdata != NULL, 1, "'GameplaySmoothTerrainVolumeEditor' expected.");
    return (GameplaySmoothTerrainVolumeEditor*)((ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_GameplaySmoothTerrainVolumeEditor__gc(lua_State* state)
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
                void* userdata = luaL_checkudata(state, 1, "GameplaySmoothTerrainVolumeEditor");
                luaL_argcheck(state, userdata != NULL, 1, "'GameplaySmoothTerrainVolumeEditor' expected.");
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    GameplaySmoothTerrainVolumeEditor* instance = (GameplaySmoothTerrainVolumeEditor*)object->instance;
                    SAFE_DELETE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolumeEditor__gc - Failed to match the given parameters to a valid function signature.");
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

int lua_GameplaySmoothTerrainVolumeEditor__init(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<GameplaySmoothTerrainVolume> param1 = ScriptUtil::getObjectPointer<GameplaySmoothTerrainVolume>(1, "GameplaySmoothTerrainVolume", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'GameplaySmoothTerrainVolume'.");
                    lua_error(state);
                }

                void* returnPtr = (void*)new GameplaySmoothTerrainVolumeEditor(param1);
                if (returnPtr)
                {
                    ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                    object->instance = returnPtr;
                    object->owns = true;
                    luaL_getmetatable(state, "GameplaySmoothTerrainVolumeEditor");
                    lua_setmetatable(state, -2);
                }
                else
                {
                    lua_pushnil(state);
                }

                return 1;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolumeEditor__init - Failed to match the given parameters to a valid function signature.");
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

int lua_GameplaySmoothTerrainVolumeEditor_addMaterial(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 6:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNONE &&
                lua_type(state, 5) == LUA_TNUMBER &&
                lua_type(state, 6) == LUA_TNUMBER)
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

                // Get parameter 3 off the stack.
                GP_WARN("Attempting to get parameter 3 with unrecognized type uint32_t as an unsigned integer.");
                uint32_t param3 = (uint32_t)luaL_checkunsigned(state, 4);

                // Get parameter 4 off the stack.
                float param4 = (float)luaL_checknumber(state, 5);

                // Get parameter 5 off the stack.
                float param5 = (float)luaL_checknumber(state, 6);

                GameplaySmoothTerrainVolumeEditor* instance = getInstance(state);
                instance->addMaterial(*param1, param2, param3, param4, param5);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolumeEditor_addMaterial - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 6).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplaySmoothTerrainVolumeEditor_applyPaint(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 6:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNONE &&
                lua_type(state, 5) == LUA_TNUMBER &&
                lua_type(state, 6) == LUA_TNUMBER)
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

                // Get parameter 3 off the stack.
                GP_WARN("Attempting to get parameter 3 with unrecognized type uint32_t as an unsigned integer.");
                uint32_t param3 = (uint32_t)luaL_checkunsigned(state, 4);

                // Get parameter 4 off the stack.
                float param4 = (float)luaL_checknumber(state, 5);

                // Get parameter 5 off the stack.
                float param5 = (float)luaL_checknumber(state, 6);

                GameplaySmoothTerrainVolumeEditor* instance = getInstance(state);
                instance->applyPaint(*param1, param2, param3, param4, param5);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolumeEditor_applyPaint - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 6).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplaySmoothTerrainVolumeEditor_smooth(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 6:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNUMBER &&
                lua_type(state, 5) == LUA_TNUMBER &&
                lua_type(state, 6) == LUA_TNUMBER)
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

                // Get parameter 3 off the stack.
                float param3 = (float)luaL_checknumber(state, 4);

                // Get parameter 4 off the stack.
                float param4 = (float)luaL_checknumber(state, 5);

                // Get parameter 5 off the stack.
                float param5 = (float)luaL_checknumber(state, 6);

                GameplaySmoothTerrainVolumeEditor* instance = getInstance(state);
                instance->smooth(*param1, param2, param3, param4, param5);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolumeEditor_smooth - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 6).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplaySmoothTerrainVolumeEditor_subtractMaterial(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 5:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNUMBER &&
                lua_type(state, 5) == LUA_TNUMBER)
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

                // Get parameter 3 off the stack.
                float param3 = (float)luaL_checknumber(state, 4);

                // Get parameter 4 off the stack.
                float param4 = (float)luaL_checknumber(state, 5);

                GameplaySmoothTerrainVolumeEditor* instance = getInstance(state);
                instance->subtractMaterial(*param1, param2, param3, param4);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplaySmoothTerrainVolumeEditor_subtractMaterial - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 5).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
