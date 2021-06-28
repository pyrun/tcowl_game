#ifndef SDL_INIT_H
#define SDL_INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

namespace sdl {
    extern bool init();
    extern bool deinit();
}

#endif