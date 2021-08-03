#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include "vec.h"

#define ENGINE_INPUT_OFFSET_KEYS 127

namespace engine {
    struct input_event {
        bool quit;
        vec2 windows_callback_size;
        bool windows_size_change;
    };

    struct input_map {
        fvec2 axies;

        bool attack;

        // intern
        bool up;
        bool down;
        bool left;
        bool right;
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
            void key_axis();

            input_event *getEvents() { return &p_map_event; }
            input_map *getInputMap() { return &p_map_input; }
        private:
            SDL_Event p_event;
            input_event p_map_event;
            input_map p_map_input;
    };
};

#endif