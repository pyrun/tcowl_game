#include <stdio.h>
#include <string.h>

#include "sdl_init.h"

#include "game/app.h"

int main( int argc, char* args[] ) {
    sdl::init();
    game::app l_app;

    // check args
    if( argc > 1)
        l_app.loadAgrs( args[1]);

    // startup
    l_app.begin();
    
    // main loop
    while(l_app.update());

    // deinit the res
    l_app.close();
    sdl::deinit();

    return 0;
}
