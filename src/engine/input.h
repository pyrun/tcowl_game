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

    enum input_specials {
        input_specials_1 = 0,
        input_specials_2,
        input_specials_3,
        input_specials_4,
        input_specials_5,
        input_specials_6,
        input_specials_7,
        input_specials_8 ,
    };

    struct input_map {
        fvec2 axies;

        bool attack;

        // intern
        bool up;
        bool down;
        bool left;
        bool right;

        uint32_t specials;
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

            void setSpecials( uint32_t index, bool value);
            bool checkSepcial( uint32_t index, input_map *map);

            input_event *getEvents() { return &p_map_event; }
            input_map *getInputMap() { return &p_map_input; }
            input_map *getInputMapBefore() { return &p_map_input_before; }
        private:
            SDL_Event p_event;
            input_event p_map_event;
            input_map p_map_input;
            input_map p_map_input_before;
    };
};

#endif