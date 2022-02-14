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

    p_font->print( { 100, 80}, "%d ms", helper::time::getDurrent(&l_time)-16);
    helper::time::reset( &l_time);

    p_font->print( { 100, 50}, "test %.2f %.2f %d %d amount", p_input->axies.x, p_input->axies.y, graphic->getDisplacedElements() , p_entity->getAmount());

    p_font->draw( "äöü ß /() € t", vec2{ 0, 100});
}

void lobby::update() {
    if( p_entity->get( 1)) {
        p_entity->get( 1)->change = true;
        p_entity->bindInput( p_entity->get( 1), p_input);
    }
}