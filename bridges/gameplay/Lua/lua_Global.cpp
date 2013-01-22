#include "ScriptController.h"
#include "lua_Global.h"

#include "GameplayColouredCubesVolume.h"
#include "lua_GameplayColouredCubesVolume.h"

using namespace gameplay;

namespace Cubiquity
{

void luaRegister_lua_Global()
{
    ScriptUtil::registerFunction("raycast", lua__raycast);
}

int lua__raycast(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 4:
        {
            if (lua_type(state, 1) == LUA_TNONE &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                lua_type(state, 3) == LUA_TNUMBER &&
                (lua_type(state, 4) == LUA_TUSERDATA || lua_type(state, 4) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                GP_WARN("Attempting to get parameter 1 with unrecognized type SmoothTerrainVolume as an unsigned integer.");
                ScriptUtil::LuaArray<SmoothTerrainVolume> param1 = (SmoothTerrainVolume)luaL_checkunsigned(state, 1);

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

                bool result = raycast(param1, *param2, param3, *param4);

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua__raycast - Failed to match the given parameters to a valid function signature.");
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
