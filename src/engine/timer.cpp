#include "timer.h"

#include <SDL2/SDL.h>

#define HELPER_GET_TICKS SDL_GetTicks

using namespace helper;

namespace helper {
    namespace time {
        void reset(uint32_t *timePast) {
            *timePast = SDL_GetTicks();
        }

        bool check( uint32_t *timePast, uint32_t durrent ) {
            return (HELPER_GET_TICKS() - *timePast) >= durrent;
        }

        uint32_t getDurrent( uint32_t *timePast) {
            return (HELPER_GET_TICKS() - *timePast);
        }

        uint32_t getTick() { return HELPER_GET_TICKS(); }
    };
};

timer::timer() {
    p_started = false;
    p_paused = false;
}

timer::~timer() {

}

bool timer::start() { // Reset
    time::reset( &p_time_start);
    p_time = 0;
    p_paused = false;
    if( p_started)
        return false;
    // start signalisieren
    p_started = true;
    return true;
}

bool timer::stop() {
    if( p_started == false)
        return false;
    p_started = false;
    p_paused = false;
    return true;
}

bool timer::pause() {
    if( p_paused)
        return false;
    p_paused = true;
    p_time += time::getDurrent( &p_time_start);
    return true;
}

bool timer::unpause() {
    if( !p_paused)
        return false;
    p_paused = false;
    time::reset( &p_time_start);
    return true;
}

uint32_t timer::getDurrent() {
    uint32_t l_time = p_time;
    if( !p_paused) {
        l_time += time::getDurrent( &p_time_start);
    }
    return l_time;
}