#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "sdl_init.h"
#include "engine/graphic.h"
#include "engine/log.h"
#include "engine/input.h"
#include "engine/image.h"
#include "engine/font.h"
#include "engine/entity.h"

#include "game/lobby.h"

int main( int argc, char* args[] ) {
    sdl::init();

    engine::graphic p_graphic;
    engine::input p_input;
    engine::font p_font;
    engine::font_setting p_font_setting = { .size{ 7, 9} };
    engine::entity_handler p_entity;

    snprintf( p_graphic.getConfig()->titel, 64, "The Commemoration of White Light");

    p_graphic.init();
    p_input.init();
    p_font.init( &p_graphic, p_font_setting);

    game::lobby p_lobby;    
    p_lobby.init( &p_font, p_input.getInputMap(), &p_entity);

    p_graphic.addObject( &p_lobby);
    p_entity.loadEntity( "entity/human/human.json");

    // https://donjon.bin.sh/fantasy/name/#type=set;set=deity
    // https://opengameart.org/content/2d-modified-dark-forest-tileset
    // https://opengameart.org/content/forest-tileset-for-rpgs
    
    while( p_input.getEvents()->quit == false) {
        p_input.update();
        if( p_input.getEvents()->windows_size_change) {
            p_input.getEvents()->windows_size_change = false;
            p_graphic.init();
        }
        p_graphic.update();
        SDL_Delay( 10);
    }

    sdl::deinit();

    return 0;
}
