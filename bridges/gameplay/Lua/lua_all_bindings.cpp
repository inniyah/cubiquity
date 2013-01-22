#include "Base.h"
#include "lua_all_bindings.h"

namespace Cubiquity
{

void lua_RegisterAllBindings()
{
    luaRegister_GameplayColouredCubesVolume();
    luaRegister_GameplayRaycasting();
    luaRegister_GameplaySmoothTerrainVolume();
    luaRegister_GameplaySmoothTerrainVolumeEditor();
    luaRegister_GameplayVolumeSerialisation();
    luaRegister_lua_Global();
}

}

