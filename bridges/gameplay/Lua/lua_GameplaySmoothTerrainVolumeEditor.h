#ifndef LUA_GAMEPLAYSMOOTHTERRAINVOLUMEEDITOR_H_
#define LUA_GAMEPLAYSMOOTHTERRAINVOLUMEEDITOR_H_

namespace gameplay
{

// Lua bindings for GameplaySmoothTerrainVolumeEditor.
int lua_GameplaySmoothTerrainVolumeEditor__gc(lua_State* state);
int lua_GameplaySmoothTerrainVolumeEditor__init(lua_State* state);
int lua_GameplaySmoothTerrainVolumeEditor_addMaterial(lua_State* state);
int lua_GameplaySmoothTerrainVolumeEditor_applyPaint(lua_State* state);
int lua_GameplaySmoothTerrainVolumeEditor_smooth(lua_State* state);
int lua_GameplaySmoothTerrainVolumeEditor_subtractMaterial(lua_State* state);

void luaRegister_GameplaySmoothTerrainVolumeEditor();

}

#endif
