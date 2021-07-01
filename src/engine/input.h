#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include "vec.h"

#define ENGINE_INPUT_OFFSET_KEYS 127

namespace engine {
    struct input_event {
        bool quit;
    };

    struct input_map {
        int8_t x;
        int8_t y;
        bool attack;
    };

    enum input_key_state {
        input_key_down = 0,
        input_key_up
    };

    class input {
        public:
            input();
            ~input();

            void init();

            void update();

            void reset();

            void key( input_key_state state, SDL_Keycode key);
            void key_axis( bool horizontal, input_key_state state, int8_t value);

            input_event getEvents() { return p_map_event; }
            input_map *getInputMap() { return &p_map_input; }
        private:
            SDL_Event p_event;
            input_event p_map_event;
            input_map p_map_input;
    };
};

#endif