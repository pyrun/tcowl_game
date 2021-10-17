#include "global_script.h"

#include <string>
#include "../log.h"

using namespace engine;

#ifdef __cplusplus
extern "C"{
#endif

static int lua_print(lua_State* state) {
    int l_nargs = lua_gettop( state);
    std::string l_text;

    for (int i = 1; i <= l_nargs; i++) {
        if( lua_isstring( state, i)) {
            l_text += lua_tostring( state, i);
        }
    }

    log( log_trace, "lua_print '%s'", l_text.c_str());

    // finish
    return 0;
}

#ifdef __cplusplus
}
#endif

void engine::script::global_lib( lua_State *L) {
    lua_pushcfunction(L, lua_print);
    lua_setglobal(L, "print");
}