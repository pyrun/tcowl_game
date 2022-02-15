#ifndef INPUT_KEYBOARD_H
#define INPUT_KEYBOARD_H

#include <SDL2/SDL.h>

#include "input.h"

namespace engine {
    class input_keyboard : public input {
        public:
            input_keyboard();
            ~input_keyboard();

            void init() override;
            void update() override;
            void event( SDL_Event *event) override;
            void reset() override;

            input_map *getInputMap() override { return &p_map_input; }
            input_map *getPreviousInputMap() override { return &p_map_previous; }
        private:
            void key( input_key_state state, SDL_Keycode key);
            void key_axis();
        private:
            input_map p_map_input;
            input_map p_map_previous;
    };
}

#endif