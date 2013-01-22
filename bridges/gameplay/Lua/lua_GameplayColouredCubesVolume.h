#ifndef LUA_GAMEPLAYCOLOUREDCUBESVOLUME_H_
#define LUA_GAMEPLAYCOLOUREDCUBESVOLUME_H_

namespace Cubiquity
{

// Lua bindings for GameplayColouredCubesVolume.
int lua_GameplayColouredCubesVolume_performUpdate(lua_State* state);
int lua_GameplayColouredCubesVolume_static_create(lua_State* state);

void luaRegister_GameplayColouredCubesVolume();

}

#endif
