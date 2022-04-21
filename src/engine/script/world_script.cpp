#include "world_script.h"

#include <string>
#include "../log.h"
#include "../world/world.h"

using namespace engine;

#ifdef __cplusplus
extern "C"{
#endif

static int lua_setTile( lua_State *state) {
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isnumber( state, 3)) {
        log( log_warn, "lua_setTile call wrong argument");
        return 0;
    }
    tile_manager *l_tilem = engine::used_world_handler->getTileManager();
    if( l_tilem == nullptr) {
        log( log_warn, "lua_setTile tile_manager is null");
        return 0;
    }
    tile *l_tile = l_tilem->getById( lua_tonumber( state, 1));
    if( l_tile == nullptr) {
        log( log_warn, "lua_setTile tile is null");
        return 0;
    }
    engine::used_world_handler->setTile( lua_tonumber( state, 2), lua_tonumber( state, 3), l_tile);
    return 0;
}

#ifdef __cplusplus
}
#endif

static const struct luaL_Reg world_lib_funcs[] = {
    {"setTile", lua_setTile},
    {NULL, NULL}
    };

LUALIB_API int engine::script::world_lib( lua_State *L) {
    luaL_newlib( L, world_lib_funcs);
	lua_setglobal( L, "world");

    return 1;
}