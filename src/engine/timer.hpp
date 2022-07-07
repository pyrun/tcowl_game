#ifndef HELPER_TIMER_HPP
#define HELPER_TIMER_HPP

#include <stdint.h>

namespace helper {
    namespace time {
        void reset(uint32_t *timePast);
        bool check( uint32_t *timePast, uint32_t durrent );
        uint32_t getDurrent( uint32_t *timePast);
        uint32_t getTick();
    };
    class timer {
        public:
            timer();
            ~timer();

            bool start();
            bool stop();

            bool pause();
            bool unpause();

            uint32_t getDurrent();

            bool isStarted() { return p_started; }
            bool isPaused() { return p_paused; }
        private:
            bool p_started;
            bool p_paused;
            uint32_t p_time_start;
            uint32_t p_time;
    };
};


#endif