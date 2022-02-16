#include "input.h"

using namespace engine;

void input_handler::add( input *inputobj) {
    inputobj->reset();
    p_input.push_back( inputobj);
}

input *input_handler::get( uint8_t index) {
    if( index >= p_input.size())
        return NULL;
    return p_input[index];
}

void input_handler::init() {
    reset();
}

void input_handler::update() {
    // Update
    for( uint32_t i = 0; i < p_input.size(); i++)
        p_input[i]->update();

    // handle events on queue
    while( SDL_PollEvent( &p_event )) {
        switch( p_event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                p_map_event.windows_callback_size.x = p_event.window.data1;
                p_map_event.windows_callback_size.y = p_event.window.data2;
                p_map_event.windows_size_change = true;
            } break;
            default:
            break;
        }

        switch( p_event.type) {
            case SDL_QUIT: {
                p_map_event.quit = true;
            } break;
            default: break;
        }

        // event
        for( uint32_t i = 0; i < p_input.size(); i++)
            p_input[i]->event( &p_event);
    }
}

void input_handler::reset() {
    p_map_event.quit = false;
    p_map_event.windows_size_change = false;
}