#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "vec.h"
#include "camera.h"

#ifndef M_PI_2
#define M_PI_2 (M_PI/2.f)
#endif

#define ENGINE_GRAPHIC_DEFAULT_TITEL    "NO_TITEL_SET"
#define ENGINE_GRAPHIC_DEFAULT_TITEL_LENGTH   256

#define ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_W 320
#define ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_H 180
#define ENGINE_GRAPHIC_DEFAULT_W        640
#define ENGINE_GRAPHIC_DEFAULT_H        360

#define ENGINE_GRAPHIC_ARC_PRECISION_VALUE 20.f // precision value, value of 1 will draw a diamond, 27 makes smooth circles
#define ENGINE_GRAPHIC_ARC_PRECISION_SPEP (M_PI_2/ENGINE_GRAPHIC_ARC_PRECISION_VALUE)

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

            void setDrawColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a);

            void draw( graphic_image *image, vec2 pos, vec2 size, vec2 shift = vec2{ 0, 0});
            void drawRect( vec2 pos, vec2 rect);
            void drawEllipse( vec2 pos, fvec2 radius);
            void drawLine( vec2 pos, vec2 dest);

            uint32_t getDisplacedElements() { return p_displayed_elements; }
            camera *getCamera() { return &p_camera; }
        public:
            SDL_Renderer *p_renderer;
            uint32_t p_displayed_elements;
            uint32_t p_displayed_elements_counter;

            camera p_camera;
    };

    class graphic_object {
        public:
            graphic_object() {}
            ~graphic_object() {}

            virtual const uint32_t getPriority() { return 1024; }
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