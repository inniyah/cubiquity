#ifndef LUA_GAMEPLAYVOLUMESERIALISATION_H_
#define LUA_GAMEPLAYVOLUMESERIALISATION_H_

namespace gameplay
{

// Lua bindings for GameplayVolumeSerialisation.
int lua_GameplayVolumeSerialisation__gc(lua_State* state);
int lua_GameplayVolumeSerialisation__init(lua_State* state);
int lua_GameplayVolumeSerialisation_static_gameplayLoadData(lua_State* state);

void luaRegister_GameplayVolumeSerialisation();

}

#endif
