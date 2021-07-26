#include "sdl_init.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

using namespace sdl;

bool sdl::init() {
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    if (SDLNet_Init() == -1) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return false;
    }

    // pseudo random generator
    srand( time( NULL ) );

    return true;
}

bool sdl::deinit() {
    SDLNet_Quit(); 
    SDL_Quit();
    return true;
}