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

void lobby::draw( float dt, engine::graphic_draw *graphic) {
    static uint32_t l_time = 0;
    if( !p_font)
        return;
    if( p_entity->get( 0)) {
        float l_speed = 10.f;
        p_entity->get( 0)->velocity += p_input->axies*l_speed;
        p_entity->get( 0)->change = true;
    }

    p_font->print( { 100, 80}, "%d ms", helper::time::getDurrent(&l_time)-16);
    helper::time::reset( &l_time);

    p_font->print( { 100, 50}, "test %.2f %.2f %d %d amount", p_input->axies.x, p_input->axies.y, graphic->getDisplacedElements() , p_entity->getAmount());

    p_font->draw( "äöü ß /() € t", vec2{ 0, 100});
}