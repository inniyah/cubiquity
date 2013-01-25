#ifndef LUA_GAMEPLAYRAYCASTING_H_
#define LUA_GAMEPLAYRAYCASTING_H_

namespace gameplay
{

// Lua bindings for GameplayRaycasting.
int lua_GameplayRaycasting__gc(lua_State* state);
int lua_GameplayRaycasting__init(lua_State* state);
int lua_GameplayRaycasting_static_gameplayRaycast(lua_State* state);

void luaRegister_GameplayRaycasting();

}

#endif
