#include "Base.h"
#include "lua_all_bindings.h"

namespace Cubiquity
{

void lua_RegisterAllBindings()
{
    luaRegister_GameplayColouredCubesVolume();
    luaRegister_GameplaySmoothTerrainVolume();
    luaRegister_GameplaySmoothTerrainVolumeEditor();
    luaRegister_lua_Global();
}

}

