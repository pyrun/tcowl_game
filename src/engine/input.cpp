#include "input.h"

#include <string>

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
    
    p_map_input.x = 0;
    p_map_input.y = 0;
}

void input::key( input_key_state state, SDL_Keycode key) {
    // todo einstelbar
    switch( key) {
        case SDLK_w: {
            key_axis( false, state, -ENGINE_INPUT_OFFSET_KEYS);
        } break;
        case SDLK_s: {
            key_axis( false, state, ENGINE_INPUT_OFFSET_KEYS);
        } break;
        case SDLK_a: {
            key_axis( true, state, -ENGINE_INPUT_OFFSET_KEYS);
        } break;
        case SDLK_d: {
            key_axis( true, state, ENGINE_INPUT_OFFSET_KEYS);
        } break;
        default: break;
    }
}

void input::key_axis( bool horizontal, input_key_state state, int8_t value) {
    int8_t *l_axis = horizontal?&p_map_input.x:&p_map_input.y;
    if( state == input_key_state::input_key_down)
        *l_axis = value;
    else if( MIN( value, *l_axis) >= MAX( value, *l_axis)) { // Reset the value if it was previously set
        *l_axis = 0;
    }
}