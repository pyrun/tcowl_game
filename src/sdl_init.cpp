#include "sdl_init.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>

using namespace sdl;

bool sdl::init() {
    // todo 
    // init without graphic
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    int32_t l_flags = IMG_INIT_PNG;
    if( IMG_Init( l_flags) != l_flags) {
        printf("IMG_Init: %s\n", IMG_GetError());
        return false;
    }

    if (SDLNet_Init() == -1) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return false;
    }

    // pseudo random generator
    //srand( time( NULL ) );

    return true;
}

bool sdl::deinit() {
    SDLNet_Quit(); 
    SDL_Quit();
    return true;
}