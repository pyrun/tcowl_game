#include "input_keyboard.hpp"

#include <string>
#include <math.h>

#include <engine/log.hpp>
#include <engine/helper.hpp>

using namespace engine;

input_keyboard::input_keyboard() {
    reset();
}

input_keyboard::~input_keyboard() {

}

void input_keyboard::init() {

}

void input_keyboard::update() {
    p_map_previous = p_map_input;
}

void input_keyboard::event( SDL_Event *event) {
    // User requests quit
    switch( event->type) {
        case SDL_KEYDOWN: {
            key( input_key_down, event->key.keysym.sym);
        } break;
        case SDL_KEYUP: {
            key( input_key_up, event->key.keysym.sym);
        } break;
        case SDL_MOUSEBUTTONDOWN: {
            onMouseEvent( input_key_down);
        } break;
        case SDL_MOUSEBUTTONUP: {
            onMouseEvent( input_key_up);
        } break;
        case SDL_MOUSEMOTION: {
            onMouseEvent( input_key_unknow);
        } break;

        default: {
            // Something else
        } break;
    }
}

void input_keyboard::reset() {
    for( uint32_t i = 0; i < input_buttons_end; i++)
        p_map_input.buttons[i] = false;
    p_map_input.axies = { 0.f, 0.f};
}

void input_keyboard::key( input_key_state state, SDL_Keycode key) {
    bool l_update_axis = false;
    switch( key) {
        case SDLK_r: { p_map_input.buttons[input_buttons_action] = state == input_key_state::input_key_down; } break;
        case SDLK_q: { p_map_input.buttons[input_buttons_use] = state == input_key_state::input_key_down; } break;
        case SDLK_e: [[fallthrough]];
        case SDLK_i: { p_map_input.buttons[input_buttons_inventory] = state == input_key_state::input_key_down; } break;
        case SDLK_w: { p_map_input.buttons[input_buttons_up] = state == input_key_state::input_key_down; l_update_axis = true; } break;
        case SDLK_s: { p_map_input.buttons[input_buttons_down] = state == input_key_state::input_key_down; l_update_axis = true; } break;
        case SDLK_a: { p_map_input.buttons[input_buttons_left] = state == input_key_state::input_key_down; l_update_axis = true; } break;
        case SDLK_d: { p_map_input.buttons[input_buttons_right] = state == input_key_state::input_key_down; l_update_axis = true; } break;
        case SDLK_F1: { p_map_input.buttons[input_buttons_special_0] = state == input_key_state::input_key_down; } break;
        case SDLK_F2: { p_map_input.buttons[input_buttons_special_1] = state == input_key_state::input_key_down; } break;
        case SDLK_F3: { p_map_input.buttons[input_buttons_special_2] = state == input_key_state::input_key_down; } break;
        case SDLK_F4: { p_map_input.buttons[input_buttons_special_3] = state == input_key_state::input_key_down; } break;
        case SDLK_F5: { p_map_input.buttons[input_buttons_special_4] = state == input_key_state::input_key_down; } break;
        case SDLK_F6: { p_map_input.buttons[input_buttons_special_5] = state == input_key_state::input_key_down; } break;
        case SDLK_F7: { p_map_input.buttons[input_buttons_special_6] = state == input_key_state::input_key_down; } break;
        case SDLK_F8: { p_map_input.buttons[input_buttons_special_7] = state == input_key_state::input_key_down; } break;
        default: break;
    }

    if( l_update_axis)
        key_axis();
}

void input_keyboard::key_axis() {
    int8_t l_x = (p_map_input.buttons[input_buttons_left]?-ENGINE_INPUT_OFFSET_KEYS:0) + (p_map_input.buttons[input_buttons_right]?ENGINE_INPUT_OFFSET_KEYS:0);
    int8_t l_y = (p_map_input.buttons[input_buttons_up]?-ENGINE_INPUT_OFFSET_KEYS:0) + (p_map_input.buttons[input_buttons_down]?ENGINE_INPUT_OFFSET_KEYS:0);

    // special case that both inputs are zero
    if( l_x == 0 && l_y == 0) {
        p_map_input.axies = { 0.f, 0.f};
        return;
    }

    // calculate angle from x and y
    float l_arc = atan2( l_y, l_x);
    p_map_input.axies.y = sin(l_arc)*1;
    p_map_input.axies.x = cos(l_arc)*1;

    if( fabs( p_map_input.axies.y) < 0.1f)
        p_map_input.axies.y = 0;
    if( fabs( p_map_input.axies.x) < 0.1f)
        p_map_input.axies.x = 0;
}

void input_keyboard::onMouseEvent( input_key_state state) {
    // SDL_PumpEvents(); // make sure we have the latest mouse state.
    
    uint32_t l_mouse_click = SDL_GetMouseState( &p_map_input.mouse.x, &p_map_input.mouse.y);
    if( state != input_key_unknow) {
        p_map_input.buttons[input_buttons_use] = l_mouse_click & SDL_BUTTON_LMASK;
        p_map_input.buttons[input_buttons_action] = l_mouse_click & SDL_BUTTON_RMASK;
    }
}