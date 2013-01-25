#ifndef LUA_GAMEPLAYSMOOTHTERRAINVOLUME_H_
#define LUA_GAMEPLAYSMOOTHTERRAINVOLUME_H_

namespace gameplay
{

// Lua bindings for GameplaySmoothTerrainVolume.
int lua_GameplaySmoothTerrainVolume_performUpdate(lua_State* state);
int lua_GameplaySmoothTerrainVolume_static_create(lua_State* state);

void luaRegister_GameplaySmoothTerrainVolume();

}

#endif
