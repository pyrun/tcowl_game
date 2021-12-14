#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "vec.h"

#define ENGINE_GRAPHIC_DEFAULT_TITEL    "NO_TITEL_SET"
#define ENGINE_GRAPHIC_DEFAULT_TITEL_LENGTH   256

#define ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_W 320
#define ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_H 180
#define ENGINE_GRAPHIC_DEFAULT_W        640
#define ENGINE_GRAPHIC_DEFAULT_H        360

namespace engine {
    struct graphic_config {
        char titel[ENGINE_GRAPHIC_DEFAULT_TITEL_LENGTH];
        vec2 native_resolution = vec2{ ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_W, ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_H};
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

            void setDrawColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a);
            void drawRect( vec2 pos, vec2 rect);


            uint32_t getDisplacedElements() { return p_displayed_elements; }

            vec2 getCameraPosition() { return p_camera_position; }
            vec2 getCameraSize() { return p_camera_size; }
        public:
            SDL_Renderer *p_renderer;
            vec2 p_camera_position;
            vec2 p_camera_size;
            uint32_t p_displayed_elements;
            uint32_t p_displayed_elements_counter;
    };

    class graphic_object {
        public:
            graphic_object() {}
            ~graphic_object() {}

            virtual void reload( graphic_draw *graphic) = 0;
            virtual void draw( graphic_draw *graphic) = 0;
    };

    class graphic : public graphic_draw {
        public:
            graphic();
            ~graphic();

            void init();
            void reset();

            void update( float dt);

            void reload();

            bool addObject( graphic_object* object);
            bool delObject( graphic_object* object);
            bool checkObject( graphic_object* object);

            void setTitle( std::string title) { snprintf( p_config.titel, ENGINE_GRAPHIC_DEFAULT_TITEL_LENGTH, title.c_str()); }
        private:
            SDL_Window *p_window;
            graphic_config p_config;
            std::vector<graphic_object*> p_graphic_objects;
    };
};

#endif