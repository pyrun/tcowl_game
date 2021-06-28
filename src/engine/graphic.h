#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <SDL2/SDL.h>
#include <vector>

#include "vec.h"

#define ENGINE_GRAPHIC_DEFAULT_TITEL    "NO_TITEL_SET"
#define ENGINE_GRAPHIC_DEFAULT_LENGTH   256
#define ENGINE_GRAPHIC_DEFAULT_W        640
#define ENGINE_GRAPHIC_DEFAULT_H        400

namespace engine {
    struct graphic_config {
        char titel[ENGINE_GRAPHIC_DEFAULT_LENGTH];
    };

    class graphic_image {
        public:
            graphic_image() {}
            ~graphic_image() {}

            SDL_Texture *getTexture() { return p_texture; }
        public:
            vec2 size;
            SDL_Texture *p_texture;
    };

    class graphic_draw {
        public:
            graphic_draw() {}
            ~graphic_draw() {}

            void draw( graphic_image *image, vec2 pos, vec2 size, vec2 shift = vec2{ 0, 0});

        public:
            SDL_Renderer *p_renderer;
    };

    class graphic_object {
        public:
            graphic_object() {}
            ~graphic_object() {}

            virtual void draw( graphic_draw *graphic) {}
    };

    class graphic : public graphic_draw {
        public:
            graphic();
            ~graphic();

            void init();

            void update();

            void addObject( graphic_object* object) { p_graphic_objects.push_back( object); }
        private:
            SDL_Window *p_window;
            graphic_config p_config;
            std::vector<graphic_object*> p_graphic_objects;
    };
};

#endif