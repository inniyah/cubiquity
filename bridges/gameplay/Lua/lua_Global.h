#ifndef LUA_GLOBAL_H_
#define LUA_GLOBAL_H_


namespace Cubiquity
{

// Lua bindings for global functions.
int lua__raycast(lua_State* state);

void luaRegister_lua_Global();

}

#endif
