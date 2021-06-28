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
        //User requests quit
        if( p_event.type == SDL_QUIT)
            p_map_event.quit = true;
    }
}

void input::reset() {
    p_map_event.quit = false;
}