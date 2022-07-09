#include "player.hpp"

using namespace engine;
using namespace game;

#include <engine/log.hpp>
#include <engine/timer.hpp>

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

    if( p_player) {
        p_player->inventory->add( { 3, 3}, entitys->getType(100) );
    }
}

void player::draw( engine::graphic_draw *graphic) {
    static uint32_t l_time = 0;
    vec2 l_camera = graphic->getCamera()->getPosition().toVec();
    if( !p_font)
        return;

    p_font->print( l_camera + vec2{ -50, 0} + vec2{ (int32_t)graphic->getCamera()->getSize().x, 0}, "%2.1d ms", helper::time::getDurrent(&l_time));
    helper::time::reset( &l_time);

    if( p_player) {
        static bool l_set = false;
        static vec2 l_pos1, l_pos2;
        // Mouse
        p_font->print( l_camera + vec2{ 10, 0}, "%3.d %3.d", p_input->getInputMap()->mouse.x, p_input->getInputMap()->mouse.y);
        if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_attack)) {
            l_pos1 = l_camera + (p_input->getInputMap()->mouse / vec2{ (int32_t)graphic->getScale(), (int32_t)graphic->getScale()}) ;
            engine::log( log_debug, "mouse left click");
            l_set = true;
        }
        l_pos2 = l_camera + (p_input->getInputMap()->mouse / vec2{ (int32_t)graphic->getScale(), (int32_t)graphic->getScale()}) - l_pos1;
        if( p_input->edgeDetection( input_key_edge_detection_up, input_buttons_attack)) {
            engine::log( log_debug, "mouse left click lose");
            l_set = false;
        }
        
        drawInventory( graphic); // draw inventory
        graphic->setDrawColor( 255, 255, 0, 255);
        if(l_set)
            graphic->drawLine( l_pos1, l_pos2);

        // centre camera to player
        action *l_action = p_player->objtype->getAction( p_player->action);
        engine::fvec2 l_pos = p_player->body->getPosition() + ( fvec2{ (float)l_action->size.x, (float)l_action->size.y} / fvec2{ 2.f, 2.f} );
        graphic->getCamera()->setTarget( l_pos);
    }
}

void player::update() {
    if( p_player) {
        p_player->change = true;
        p_entity->bindInput( p_player, p_input->getInputMap());
    }
}

void player::drawInventory( engine::graphic_draw *graphic) {
    graphic->setDrawColor( 70, 30, 30, 100);
    graphic->drawFilledRect( vec2{ 0, 0} + graphic->getCamera()->getPosition().toVec(), graphic->getCamera()->getSize().toVec());
    p_player->inventory->draw( graphic);
}