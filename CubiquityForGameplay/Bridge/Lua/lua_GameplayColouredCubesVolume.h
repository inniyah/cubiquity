#ifndef LUA_GAMEPLAYCOLOUREDCUBESVOLUME_H_
#define LUA_GAMEPLAYCOLOUREDCUBESVOLUME_H_

namespace gameplay
{

// Lua bindings for GameplayColouredCubesVolume.
int lua_GameplayColouredCubesVolume_getRootNodeForLua(lua_State* state);
int lua_GameplayColouredCubesVolume_getVoxel(lua_State* state);
int lua_GameplayColouredCubesVolume_markAsModified(lua_State* state);
int lua_GameplayColouredCubesVolume_performUpdate(lua_State* state);
int lua_GameplayColouredCubesVolume_setVoxel(lua_State* state);
int lua_GameplayColouredCubesVolume_static_create(lua_State* state);

void luaRegister_GameplayColouredCubesVolume();

}

#endif
