#include "image.hpp"

#include <engine/log.hpp>

using namespace engine;

image::image() {
    p_texture = NULL;
}

image::~image() {

}

void image::setAlphaKey( uint8_t r, uint8_t g, uint8_t b) {
    p_color_key = { r, g, b, 1};
}

bool image::load( engine::graphic_draw *graphic, std::string file) {
    SDL_Surface *l_image = IMG_Load( file.c_str());
    if( l_image == NULL) {
        engine::log( log_warn, "Error loading image \"%s\" %s", file.c_str(), IMG_GetError());
        return false;
    }

    p_file = file;

    size.x = l_image->w;
    size.y = l_image->h;

    // Alpha
    SDL_SetColorKey( l_image, p_color_key.w?SDL_TRUE:SDL_FALSE, SDL_MapRGB( l_image->format, p_color_key.x, p_color_key.y, p_color_key.z));

    // Bild aus dem Arbeitsspeicher auf die Grafikkarte übertragen
    p_texture = SDL_CreateTextureFromSurface( graphic->getRenderer(), l_image);
    SDL_FreeSurface( l_image);
    if( p_texture == NULL) {
        engine::log( log_warn, "Error creating texture \"%s\" %s", file.c_str(), SDL_GetError());
        return false;
    }
    engine::log( log_trace, "Loading image \"%s\" successful", file.c_str());
    return true;
}

void image::reload( engine::graphic_draw *graphic) {
    SDL_DestroyTexture( p_texture);
    load( graphic, p_file);
}

void image::clear() {
    if( p_texture)
        SDL_DestroyTexture( p_texture);
}