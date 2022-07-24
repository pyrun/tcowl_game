#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include <engine/vec.hpp>
#include <engine/graphic/camera.hpp>

#ifndef M_PI_2
#define M_PI_2 (M_PI/2.f)
#endif

#define ENGINE_GRAPHIC_DEFAULT_TITEL "NO_TITEL_SET"
#define ENGINE_GRAPHIC_DEFAULT_TITEL_LENGTH 256

#define ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_W 320
#define ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_H 180
#define ENGINE_GRAPHIC_DEFAULT_W 640
#define ENGINE_GRAPHIC_DEFAULT_H 360

#define ENGINE_GRAPHIC_ARC_PRECISION_VALUE 27.f // precision value, value of 1 will draw a diamond, 27 makes smooth circles
#define ENGINE_GRAPHIC_ARC_PRECISION_SPEP (M_PI_2/ENGINE_GRAPHIC_ARC_PRECISION_VALUE)

namespace engine {
    enum graphic_flip  {
        graphic_flip_none = 0,
        graphic_flip_horizontal,
        graphic_flip_vertical
    };

    struct graphic_config {
        char titel[ENGINE_GRAPHIC_DEFAULT_TITEL_LENGTH];
        vec2 native_resolution = vec2{ ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_W, ENGINE_GRAPHIC_DEFAULT_NATIVE_RESOLUTION_H};
    };

    class graphic_image {
        public:
            SDL_Texture *getTexture() { return p_texture; }
        public:
            vec2 size;
            SDL_Texture *p_texture;
    };

    class graphic_draw {
        public:
            virtual void setDrawColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;

            virtual void draw( graphic_image *image, vec2 pos, vec2 size, vec2 shift = vec2{ 0, 0}, double angle = 0, vec2 *center = nullptr, graphic_flip flip = graphic_flip_none) = 0;
            virtual void drawRect( vec2 pos, vec2 rect) = 0;
            virtual void drawFilledRect( vec2 pos, vec2 rect) = 0;
            virtual void drawEllipse( vec2 pos, fvec2 radius) = 0;
            virtual void drawLine( vec2 pos, vec2 dest) = 0;

            virtual vec2 getMousePositionToLogicalMousePosition( vec2 realMousePos) = 0;

            uint32_t getDisplacedElements() { return p_displayed_elements; }
            virtual camera *getCamera() = 0;
            virtual uint32_t getScale() = 0;

            virtual SDL_Renderer *getRenderer() = 0;
        public:
            uint32_t p_displayed_elements;
            uint32_t p_displayed_elements_counter;
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

            void setDrawColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

            void draw( graphic_image *image, vec2 pos, vec2 size, vec2 shift = vec2{ 0, 0}, double angle = 0, vec2 *center = nullptr, graphic_flip flip = graphic_flip_none) override;
            void drawRect( vec2 pos, vec2 rect) override;
            void drawFilledRect( vec2 pos, vec2 rect) override;
            void drawEllipse( vec2 pos, fvec2 radius) override;
            void drawLine( vec2 pos, vec2 dest) override;

            vec2 getMousePositionToLogicalMousePosition( vec2 realMousePos) override;

            camera *getCamera() override { return &p_camera; }
            uint32_t getScale() override { return p_windows_scale; };
            SDL_Renderer *getRenderer() override  { return p_renderer; }
        private:
            SDL_Renderer *p_renderer;
            camera p_camera;

            SDL_Window *p_window;
            vec2 p_windows_size;
            uint32_t p_windows_scale;

            graphic_config p_config;
            std::vector<graphic_object*> p_graphic_objects;
    };
};

#endif