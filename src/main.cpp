#include <stdio.h>
#include <string.h>

#include "sdl_init.h"

#include "game/app.hpp"

int main( int argc, char* args[] ) {
    sdl::init();
    game::app l_app;

    if( sizeof(std::size_t) != sizeof(int32_t)) // TODO: better check
        while(1);

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
