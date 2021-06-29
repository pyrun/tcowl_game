#include "input.h"

#include <string>

#include "log.h"

using namespace engine;

input::input() {
    reset();
}

input::~input() {

}

void input::init() {

}

void input::update() {
    // Handle events on queue
    while( SDL_PollEvent( &p_event ) != 0 ) {
        // User requests quit
        switch( p_event.type) {
            case SDL_QUIT: {
                p_map_event.quit = true;
            } break;
            case SDL_KEYDOWN: {
                key( input_key_down, p_event.key.keysym.sym);
            } break;
            default: {
                // Something else
            } break;
        }
    }
}

void input::reset() {
    p_map_event.quit = false;
}

void input::key( input_key_state state, SDL_Keycode key) {
    // todo einstelbar
    switch( key) {
        case SDLK_w: {
            p_map_input.y = -128;
        } break;
        case SDLK_s: {
            p_map_input.y = 128;
        } break;
        case SDLK_a: {
            p_map_input.y = -128;
        } break;
        case SDLK_d: {
            p_map_input.y = 128;
        } break;
        default: break;
    }
}