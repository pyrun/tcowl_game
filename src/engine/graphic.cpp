#include "graphic.h"

#include <string>
#include <math.h>
#include <algorithm>

#include "log.h"

using namespace engine;

void graphic_draw::draw( graphic_image *image, vec2 pos, vec2 size, vec2 shift) {
    if( p_camera.getPosition().x > pos.x+size.x ||
        p_camera.getPosition().y > pos.y+size.y ||
        p_camera.getPosition().x+p_camera.getSize().x < pos.x ||
        p_camera.getPosition().y+p_camera.getSize().y < pos.y)
        return; // Nicht anzeigen

    SDL_Rect l_srcrect = { shift.x, shift.y, size.x, size.y};
    SDL_Rect l_dstrect = { pos.x, pos.y, size.x, size.y};

    SDL_RenderCopy( p_renderer, image->getTexture(), &l_srcrect, &l_dstrect);
    p_displayed_elements_counter++;
}

void graphic_draw::setDrawColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor( p_renderer, r, g, b, a);
}

void graphic_draw::drawRect( vec2 pos, vec2 rect) {
    SDL_Rect l_rect {pos.x, pos.y, rect.x, rect.y};
    SDL_RenderDrawRect( p_renderer, &l_rect );
}

void graphic_draw::drawellipse( vec2 pos, fvec2 radius) { //draw one quadrant arc, and mirror the other 4 quadrants
    float l_theta = 0; // angle that will be increased each loop

    //starting point
    vec2 l_location = { (int32_t)(radius.x * cos(l_theta)), (int32_t)(radius.y * sin(l_theta))};
    vec2 l_new_location = l_location;

    //repeat until theta >= 90;
    for( l_theta = ENGINE_GRAPHIC_ARC_PRECISION_SPEP;  l_theta <= M_PI_2;  l_theta += ENGINE_GRAPHIC_ARC_PRECISION_SPEP) {//step through only a 90 arc (1 quadrant)
        //get new point location
        l_new_location.x = radius.x * cosf(l_theta) + 0.5; //new point (+.5 is a quick rounding method)
        l_new_location.y = radius.y * sinf(l_theta) + 0.5; //new point (+.5 is a quick rounding method)

        //draw line from previous point to new point, ONLY if point incremented
        if( (l_location.x != l_new_location.x) || (l_location.y != l_new_location.y) ) {//only draw if coordinate changed
            SDL_RenderDrawLine(p_renderer, pos.x + l_location.x, pos.y - l_location.y,    pos.x + l_new_location.x, pos.y - l_new_location.y );//quadrant TR
            SDL_RenderDrawLine(p_renderer, pos.x - l_location.x, pos.y - l_location.y,    pos.x - l_new_location.x, pos.y - l_new_location.y );//quadrant TL
            SDL_RenderDrawLine(p_renderer, pos.x - l_location.x, pos.y + l_location.y,    pos.x - l_new_location.x, pos.y + l_new_location.y );//quadrant BL
            SDL_RenderDrawLine(p_renderer, pos.x + l_location.x, pos.y + l_location.y,    pos.x + l_new_location.x, pos.y + l_new_location.y );//quadrant BR
        }
        //save previous points
        l_location = l_new_location;
    }
    //arc did not finish because of rounding, so finish the arc
    if( l_location.x != 0) {
        l_location.x = 0;
        SDL_RenderDrawLine(p_renderer, pos.x + l_location.x, pos.y - l_location.y,    pos.x + l_new_location.x, pos.y - l_new_location.y );//quadrant TR
        SDL_RenderDrawLine(p_renderer, pos.x - l_location.x, pos.y - l_location.y,    pos.x - l_new_location.x, pos.y - l_new_location.y );//quadrant TL
        SDL_RenderDrawLine(p_renderer, pos.x - l_location.x, pos.y + l_location.y,    pos.x - l_new_location.x, pos.y + l_new_location.y );//quadrant BL
        SDL_RenderDrawLine(p_renderer, pos.x + l_location.x, pos.y + l_location.y,    pos.x + l_new_location.x, pos.y + l_new_location.y );//quadrant BR
    }
}

void graphic_draw::drawLine( vec2 pos, vec2 vector) {
    SDL_RenderDrawLine( p_renderer, pos.x, pos.y, pos.x+vector.x, pos.y+vector.y);
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
    //SDL_GetWindowSize( p_window, &p_camera_size.x, &p_camera_size.y);
    p_camera.setSize( p_config.native_resolution);
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
    snprintf( p_config.titel, ENGINE_GRAPHIC_DEFAULT_TITEL_LENGTH, "%s", ENGINE_GRAPHIC_DEFAULT_TITEL);
    p_displayed_elements = 0;
    p_displayed_elements_counter = 0;
}

void graphic::update( float dt) {
    // Clear screen
    setDrawColor( 0, 0, 0, 255);
    SDL_RenderClear( p_renderer );
    p_displayed_elements = p_displayed_elements_counter;
    p_displayed_elements_counter = 0;
    
    // Render pipeline
    for( uint32_t i = 0; i < p_graphic_objects.size(); i++) {
        graphic_object *l_obj = p_graphic_objects[i];
        l_obj->draw( this);
    }

    // Update screen
    SDL_RenderPresent( p_renderer );
}

void graphic::reload() {
    // pipeline
    for( uint32_t i = 0; i < p_graphic_objects.size(); i++) {
        graphic_object *l_obj = p_graphic_objects[i];
        l_obj->reload( this);
    }
}


bool graphic::addObject( graphic_object* object) {
    for( uint32_t i = 0; i < p_graphic_objects.size(); i++) {
        if( p_graphic_objects[i] == object)
            return false;
    }
    p_graphic_objects.push_back( object);

    // sorting 
    std::sort( p_graphic_objects.begin(), p_graphic_objects.end(),
        []( graphic_object *lhs, graphic_object *rhs) -> bool
        {
            return lhs->getPriority() < rhs->getPriority();
        });

    return true;
}

bool graphic::delObject( graphic_object* object) {
    for( uint32_t i = 0; i < p_graphic_objects.size(); i++) {
        if( p_graphic_objects[i] == object) {
            p_graphic_objects.erase( p_graphic_objects.begin() + i);
            return true;
        }
    }
    return false;
}

bool graphic::checkObject( graphic_object* object) {
    for( uint32_t i = 0; i < p_graphic_objects.size(); i++) {
        if( p_graphic_objects[i] == object) {
            return true;
        }
    }
    return false;
}