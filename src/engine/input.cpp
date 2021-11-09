#include "input.h"

#include <string>
#include <math.h>

#include "log.h"
#include "helper.h"

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
            case SDL_KEYUP: {
                key( input_key_up, p_event.key.keysym.sym);
            } break;
            default: {
                // Something else
            } break;
        }
        switch( p_event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                p_map_event.windows_callback_size.x = p_event.window.data1;
                p_map_event.windows_callback_size.y = p_event.window.data2;
                p_map_event.windows_size_change = true;
            } break;
            default:
            break;
        }
    }
}

void input::reset() {
    p_map_event.quit = false;
    p_map_event.windows_size_change = false;

    p_map_input.up = false;
    p_map_input.down = false;
    p_map_input.left = false;
    p_map_input.right = false;

    p_map_input.axies = { 0.f, 0.f};
}

void input::key( input_key_state state, SDL_Keycode key) {
    bool l_update_axis = false;
    // todo einstelbar
    switch( key) {
        case SDLK_w: {
            p_map_input.up = state == input_key_state::input_key_down;
            l_update_axis = true;
        } break;
        case SDLK_s: {
            p_map_input.down = state == input_key_state::input_key_down;
            l_update_axis = true;
        } break;
        case SDLK_a: {
            p_map_input.left = state == input_key_state::input_key_down;
            l_update_axis = true;
        } break;
        case SDLK_d: {
            p_map_input.right = state == input_key_state::input_key_down;
            l_update_axis = true;
        } break;
        default: break;
    }

    if( l_update_axis)
        key_axis();
}

void input::key_axis() {
    int8_t l_x = (p_map_input.left?-ENGINE_INPUT_OFFSET_KEYS:0) + (p_map_input.right?ENGINE_INPUT_OFFSET_KEYS:0);
    int8_t l_y = (p_map_input.up?-ENGINE_INPUT_OFFSET_KEYS:0) + (p_map_input.down?ENGINE_INPUT_OFFSET_KEYS:0);

    // special case that both inputs are zero
    if( l_x == 0 && l_y == 0) {
        p_map_input.axies = { 0.f, 0.f};
        return;
    }

    // calculate angle from x and y
    float l_arc = atan2( l_y, l_x);
    p_map_input.axies.y = sin(l_arc)*1;
    p_map_input.axies.x = cos(l_arc)*1;
}