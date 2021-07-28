#include "lobby.h"

using namespace engine;
using namespace game;

#include "../engine/timer.h"

lobby::lobby() {
    p_font = NULL;
}

lobby::~lobby() {
    
}

void lobby::init( engine::font *font, engine::input_map *input, engine::entity_handler *entitys) {
    p_font = font;
    p_input = input;
    p_entity = entitys;
}

void lobby::draw( engine::graphic_draw *graphic) {
    static uint32_t l_time = 0;
    if( !p_font)
        return;
    if( p_entity->get( 2)) {
        if( p_input->x < 0)
            p_entity->get( 2)->position.x -= 1;
        if( p_input->x > 0)
            p_entity->get( 2)->position.x += 1;
        if( p_input->y < 0)
            p_entity->get( 2)->position.y -= 1;
        if( p_input->y > 0)
            p_entity->get( 2)->position.y += 1;
        p_entity->get( 2)->change = true;
    }

    p_font->print( { 100, 80}, "%d ms", helper::time::getDurrent(&l_time)-16);
    helper::time::reset( &l_time);


    p_font->print( { 100, 50}, "test %d %d %d %d amount", p_input->x, p_input->y, graphic->getDisplacedElements() , p_entity->getAmount());

    p_font->draw( "{dies ist ein sehr langer test} mit ��� ? und anderen sonderzeichen/()", vec2{ 100, 100});
}