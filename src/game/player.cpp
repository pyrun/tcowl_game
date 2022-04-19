#include "player.h"

using namespace engine;
using namespace game;

#include "../engine/timer.h"

player::player() {
    p_font = NULL;
}

player::~player() {
    
}

void player::begin( engine::font *font, engine::input*input, engine::entity_handler *entitys) {
    p_font = font;
    p_input = input;
    p_entity = entitys;

    p_player = p_entity->get( 1);
}

void player::draw( engine::graphic_draw *graphic) {
    static uint32_t l_time = 0;
    if( !p_font)
        return;

    p_font->print( { 100, 80}, "%d ms", helper::time::getDurrent(&l_time)-16);
    helper::time::reset( &l_time);

    //p_font->print( { 100, 50}, "test %.2f %.2f %d %d amount", p_input->axies.x, p_input->axies.y, graphic->getDisplacedElements() , p_entity->getAmount());

    p_font->draw( "äöü ß /() € t", vec2{ 0, 100});

    if( p_player) {
        action *l_action = p_player->objtype->getAction( p_player->action);

        graphic->getCamera()->setTarget( p_player->body->getPosition().toVec() + ( l_action->size / vec2{ 2, 2} ));
    }
}

void player::update() {
    if( p_player) {
        p_player->change = true;
        p_entity->bindInput( p_player, p_input->getInputMap());
    }
}