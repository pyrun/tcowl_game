#include <stdio.h>
#include <string.h>

#include "sdl_init.h"

#include "game/app.h"

// https://donjon.bin.sh/fantasy/name/#type=set;set=deity
// https://opengameart.org/content/2d-modified-dark-forest-tileset
// https://opengameart.org/content/forest-tileset-for-rpgs

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

void test() {
      // initialization
      lua_State * L = luaL_newstate();
      luaL_openlibs(L);

      // execute script
      const char lua_script[] = "print('Hello World!')";
      int load_stat = luaL_loadbuffer(L,lua_script,strlen(lua_script),lua_script);
      lua_pcall(L, 0, 0, 0);

      // cleanup
      lua_close(L);
}

int main( int argc, char* args[] ) {
    sdl::init();
    game::app l_app;
    bool l_start_server = false;

    // check args
    for( uint32_t i = 0; i < argc; i++) {
        if( strcmp( args[i], "server") == 0)
            l_start_server = true;
    }

    // startup
    l_app.begin( l_start_server);

    // test code
    test();

    // main loop
    while(l_app.update());

    // deinit the res
    l_app.close();
    sdl::deinit();

    return 0;
}
