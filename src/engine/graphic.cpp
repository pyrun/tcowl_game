#include "graphic.h"

#include <string>

#include "log.h"

using namespace engine;

void graphic_draw::draw( graphic_image *image, vec2 pos, vec2 size, vec2 shift) {
    if( p_camera_position.x > pos.x+size.x ||
        p_camera_position.y > pos.y+size.y ||
        p_camera_position.x+p_camera_size.x < pos.x ||
        p_camera_position.y+p_camera_size.y < pos.y)
        return; // Nicht anzeigen

    SDL_Rect l_srcrect = { shift.x, shift.y, size.x, size.y};
    SDL_Rect l_dstrect = { pos.x, pos.y, size.x, size.y};

    SDL_RenderCopy( p_renderer, image->getTexture(), &l_srcrect, &l_dstrect);
    p_displayed_elements_counter++;
}

graphic::graphic() {
    p_window = NULL;
    p_renderer = NULL;

    reset();
}

graphic::~graphic() {

}

void graphic::init() {
    uint32_t l_windows_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if( p_window == NULL) {
        p_window = SDL_CreateWindow( p_config.titel,
                                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    ENGINE_GRAPHIC_DEFAULT_W, ENGINE_GRAPHIC_DEFAULT_H,
                                    l_windows_flags);
    }
    if( p_window == NULL) {
        engine::log( engine::log_fatal, "Could not create window: %s\n", SDL_GetError());
        exit(1);
    }
    p_camera_position = { 0, 0};
    //SDL_GetWindowSize( p_window, &p_camera_size.x, &p_camera_size.y);
    p_camera_size = p_config.native_resolution;
    if( p_renderer == NULL)
        p_renderer = SDL_CreateRenderer( p_window, -1, SDL_RENDERER_ACCELERATED);
    if( p_renderer == NULL ) {
        engine::log( engine::log_fatal, "Could not create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetRenderDrawColor( p_renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderSetLogicalSize( p_renderer, p_config.native_resolution.x, p_config.native_resolution.y);
    SDL_RenderSetIntegerScale( p_renderer, SDL_bool::SDL_TRUE);
    SDL_RenderPresent( p_renderer );
}

void graphic::reset() {
    snprintf( p_config.titel, ENGINE_GRAPHIC_DEFAULT_LENGTH, "%s", ENGINE_GRAPHIC_DEFAULT_TITEL);
    p_displayed_elements = 0;
    p_displayed_elements_counter = 0;
}

void graphic::update( float dt) {
    //Clear screen
    SDL_RenderClear( p_renderer );
    p_displayed_elements = p_displayed_elements_counter;
    p_displayed_elements_counter = 0;

    // Render pipeline
    for( uint32_t i = 0; i < p_graphic_objects.size(); i++) {
        graphic_object *l_obj = p_graphic_objects[i];
        l_obj->draw( dt, this);
    }

    //Update screen
    SDL_RenderPresent( p_renderer );
}
