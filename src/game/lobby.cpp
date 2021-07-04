#include "lobby.h"

using namespace engine;
using namespace game;

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
    if( !p_font)
        return;
    
    static vec2 pos = { 100, 100};
    if( p_input->x < 0)
        pos.x -= 1;
    if( p_input->x > 0)
        pos.x += 1;
    if( p_input->y < 0)
        pos.y -= 1;
    if( p_input->y > 0)
        pos.y += 1;
    
    p_font->print( { 100, 50}, "test %d %d %d amount", p_input->x, p_input->y, graphic->getDisplacedElements() /*, p_entity->getAmount()*/);

    p_font->draw( "{dies ist ein sehr langer test} mit ��� ? und anderen sonderzeichen/()", pos);
}