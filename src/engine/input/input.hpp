#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <vector>

#include <engine/vec.hpp>

#define ENGINE_INPUT_OFFSET_KEYS    127

namespace engine {
    struct input_event {
        bool quit;
        vec2 windows_callback_size;
        bool windows_size_change;
    };

    enum input_buttons{
        input_buttons_attack = 0,
        input_buttons_up,
        input_buttons_down,
        input_buttons_left,
        input_buttons_right,

        input_buttons_special_0,
        input_buttons_special_1,
        input_buttons_special_2,
        input_buttons_special_3,
        input_buttons_special_4,
        input_buttons_special_5,
        input_buttons_special_6,
        input_buttons_special_7,
        
        input_buttons_end,
    };

    struct input_map { // default map
        fvec2 axies;
        bool buttons[input_buttons_end];
        vec2 mouse;
    };

    enum input_key_state {
        input_key_down = 0,
        input_key_up,
        input_key_unknow,
    };

    enum input_key_edge_detection {
        input_key_edge_detection_down = 0,
        input_key_edge_detection_up
    };

    class input {
        public:
            input() {}
            ~input() {}

            virtual void init() = 0;
            virtual void update() = 0;
            virtual void event( SDL_Event *event) = 0;
            virtual void reset() = 0;

            bool edgeDetection( input_key_edge_detection edge, input_buttons button) {
                bool l_input1 = getInputMap()->buttons[button];
                bool l_input2 = getPreviousInputMap()->buttons[button];
                if( edge == input_key_edge_detection_down) {
                    if( l_input1 && !l_input2)
                        return true;
                } else {
                    if( !l_input1 && l_input2)
                        return true;
                }
                return false;
            }
            virtual input_map *getInputMap() = 0;
            virtual input_map *getPreviousInputMap() = 0;
    };

    class input_handler {
        public:
            input_handler() {}
            ~input_handler() {}

            void add( input *inputobj);
            input *get( uint8_t index);
            void init();
            void update();
            void reset();

            input_event *getEvents() { return &p_map_event; }
        private:
            input_event p_map_event;
            std::vector<input*> p_input;
            SDL_Event p_event;
    };
};

#endif