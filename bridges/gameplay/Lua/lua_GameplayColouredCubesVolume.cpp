#include "Base.h"
#include "ScriptController.h"
#include "lua_GameplayColouredCubesVolume.h"
#include "GameplayColouredCubesVolume.h"
#include "gameplay.h"

using namespace Cubiquity;

namespace gameplay
{

void luaRegister_GameplayColouredCubesVolume()
{
    const luaL_Reg lua_members[] = 
    {
        {"getRootNodeForLua", lua_GameplayColouredCubesVolume_getRootNodeForLua},
        {"getVoxel", lua_GameplayColouredCubesVolume_getVoxel},
        {"markAsModified", lua_GameplayColouredCubesVolume_markAsModified},
        {"performUpdate", lua_GameplayColouredCubesVolume_performUpdate},
        {"setVoxel", lua_GameplayColouredCubesVolume_setVoxel},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"create", lua_GameplayColouredCubesVolume_static_create},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    ScriptUtil::registerClass("GameplayColouredCubesVolume", lua_members, NULL, NULL, lua_statics, scopePath);
}

static GameplayColouredCubesVolume* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "GameplayColouredCubesVolume");
    luaL_argcheck(state, userdata != NULL, 1, "'GameplayColouredCubesVolume' expected.");
    return (GameplayColouredCubesVolume*)((ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_GameplayColouredCubesVolume_getRootNodeForLua(lua_State* state)
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

                GameplayColouredCubesVolume* instance = getInstance(state);
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

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_getRootNodeForLua - Failed to match the given parameters to a valid function signature.");
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

int lua_GameplayColouredCubesVolume_getVoxel(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 4:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                lua_type(state, 2) == LUA_TNUMBER &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                int param1 = (int)luaL_checkint(state, 2);

                // Get parameter 2 off the stack.
                int param2 = (int)luaL_checkint(state, 3);

                // Get parameter 3 off the stack.
                int param3 = (int)luaL_checkint(state, 4);

                GameplayColouredCubesVolume* instance = getInstance(state);
                void* returnPtr = (void*)new Vector4(instance->getVoxel(param1, param2, param3));
                if (returnPtr)
                {
                    ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                    object->instance = returnPtr;
                    object->owns = true;
                    luaL_getmetatable(state, "Vector4");
                    lua_setmetatable(state, -2);
                }
                else
                {
                    lua_pushnil(state);
                }

                return 1;
            }

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_getVoxel - Failed to match the given parameters to a valid function signature.");
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

int lua_GameplayColouredCubesVolume_markAsModified(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 7:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                lua_type(state, 2) == LUA_TNUMBER &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNUMBER &&
                lua_type(state, 5) == LUA_TNUMBER &&
                lua_type(state, 6) == LUA_TNUMBER &&
                lua_type(state, 7) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                int param1 = (int)luaL_checkint(state, 2);

                // Get parameter 2 off the stack.
                int param2 = (int)luaL_checkint(state, 3);

                // Get parameter 3 off the stack.
                int param3 = (int)luaL_checkint(state, 4);

                // Get parameter 4 off the stack.
                int param4 = (int)luaL_checkint(state, 5);

                // Get parameter 5 off the stack.
                int param5 = (int)luaL_checkint(state, 6);

                // Get parameter 6 off the stack.
                int param6 = (int)luaL_checkint(state, 7);

                GameplayColouredCubesVolume* instance = getInstance(state);
                instance->markAsModified(param1, param2, param3, param4, param5, param6);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_markAsModified - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 8:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                lua_type(state, 2) == LUA_TNUMBER &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNUMBER &&
                lua_type(state, 5) == LUA_TNUMBER &&
                lua_type(state, 6) == LUA_TNUMBER &&
                lua_type(state, 7) == LUA_TNUMBER &&
                lua_type(state, 8) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                int param1 = (int)luaL_checkint(state, 2);

                // Get parameter 2 off the stack.
                int param2 = (int)luaL_checkint(state, 3);

                // Get parameter 3 off the stack.
                int param3 = (int)luaL_checkint(state, 4);

                // Get parameter 4 off the stack.
                int param4 = (int)luaL_checkint(state, 5);

                // Get parameter 5 off the stack.
                int param5 = (int)luaL_checkint(state, 6);

                // Get parameter 6 off the stack.
                int param6 = (int)luaL_checkint(state, 7);

                // Get parameter 7 off the stack.
                int param7 = (int)luaL_checkint(state, 8);

                GameplayColouredCubesVolume* instance = getInstance(state);
                instance->markAsModified(param1, param2, param3, param4, param5, param6, param7);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_markAsModified - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 7 or 8).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplayColouredCubesVolume_performUpdate(lua_State* state)
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

                GameplayColouredCubesVolume* instance = getInstance(state);
                instance->performUpdate(*param1, param2);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_performUpdate - Failed to match the given parameters to a valid function signature.");
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

int lua_GameplayColouredCubesVolume_setVoxel(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 8:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                lua_type(state, 2) == LUA_TNUMBER &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNUMBER &&
                lua_type(state, 5) == LUA_TNUMBER &&
                lua_type(state, 6) == LUA_TNUMBER &&
                lua_type(state, 7) == LUA_TNUMBER &&
                lua_type(state, 8) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                int param1 = (int)luaL_checkint(state, 2);

                // Get parameter 2 off the stack.
                int param2 = (int)luaL_checkint(state, 3);

                // Get parameter 3 off the stack.
                int param3 = (int)luaL_checkint(state, 4);

                // Get parameter 4 off the stack.
                unsigned char param4 = (unsigned char)luaL_checkunsigned(state, 5);

                // Get parameter 5 off the stack.
                unsigned char param5 = (unsigned char)luaL_checkunsigned(state, 6);

                // Get parameter 6 off the stack.
                unsigned char param6 = (unsigned char)luaL_checkunsigned(state, 7);

                // Get parameter 7 off the stack.
                unsigned char param7 = (unsigned char)luaL_checkunsigned(state, 8);

                GameplayColouredCubesVolume* instance = getInstance(state);
                instance->setVoxel(param1, param2, param3, param4, param5, param6, param7);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_setVoxel - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 9:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                lua_type(state, 2) == LUA_TNUMBER &&
                lua_type(state, 3) == LUA_TNUMBER &&
                lua_type(state, 4) == LUA_TNUMBER &&
                lua_type(state, 5) == LUA_TNUMBER &&
                lua_type(state, 6) == LUA_TNUMBER &&
                lua_type(state, 7) == LUA_TNUMBER &&
                lua_type(state, 8) == LUA_TNUMBER &&
                lua_type(state, 9) == LUA_TNUMBER)
            {
                // Get parameter 1 off the stack.
                int param1 = (int)luaL_checkint(state, 2);

                // Get parameter 2 off the stack.
                int param2 = (int)luaL_checkint(state, 3);

                // Get parameter 3 off the stack.
                int param3 = (int)luaL_checkint(state, 4);

                // Get parameter 4 off the stack.
                unsigned char param4 = (unsigned char)luaL_checkunsigned(state, 5);

                // Get parameter 5 off the stack.
                unsigned char param5 = (unsigned char)luaL_checkunsigned(state, 6);

                // Get parameter 6 off the stack.
                unsigned char param6 = (unsigned char)luaL_checkunsigned(state, 7);

                // Get parameter 7 off the stack.
                unsigned char param7 = (unsigned char)luaL_checkunsigned(state, 8);

                // Get parameter 8 off the stack.
                int param8 = (int)luaL_checkint(state, 9);

                GameplayColouredCubesVolume* instance = getInstance(state);
                instance->setVoxel(param1, param2, param3, param4, param5, param6, param7, param8);
                
                return 0;
            }

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_setVoxel - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 8 or 9).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_GameplayColouredCubesVolume_static_create(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL) &&
                    (lua_type(state, 2) == LUA_TSTRING || lua_type(state, 2) == LUA_TNIL) &&
                    lua_type(state, 3) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    const char* param1 = ScriptUtil::getString(1, false);

                    // Get parameter 2 off the stack.
                    const char* param2 = ScriptUtil::getString(2, false);

                    // Get parameter 3 off the stack.
                    unsigned int param3 = (unsigned int)luaL_checkunsigned(state, 3);

                    void* returnPtr = (void*)GameplayColouredCubesVolume::create(param1, param2, param3);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "GameplayColouredCubesVolume");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_static_create - Failed to match the given parameters to a valid function signature.");
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
                    (lua_type(state, 7) == LUA_TSTRING || lua_type(state, 7) == LUA_TNIL) &&
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
                    const char* param7 = ScriptUtil::getString(7, false);

                    // Get parameter 8 off the stack.
                    unsigned int param8 = (unsigned int)luaL_checkunsigned(state, 8);

                    void* returnPtr = (void*)GameplayColouredCubesVolume::create(param1, param2, param3, param4, param5, param6, param7, param8);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "GameplayColouredCubesVolume");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_GameplayColouredCubesVolume_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 3 or 8).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
