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
    vec2 l_mouse = graphic->getMousePositionToLogicalMousePosition( p_input->getInputMap()->mouse);
    if( !p_font)
        return;

    p_font->print( l_camera + vec2{ -50, 0} + vec2{ (int32_t)graphic->getCamera()->getSize().x, 0}, "%2.1d ms", helper::time::getDurrent(&l_time));
    helper::time::reset( &l_time);
    
    static inventory_entry *l_item = nullptr;
    if( p_player) {
        static bool l_set = false;
        static vec2 l_pos1, l_pos2;
        // Mouse
        p_font->print( l_camera + vec2{ 10, 0}, "%3.d %3.d", p_input->getInputMap()->mouse.x, p_input->getInputMap()->mouse.y);
        if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_attack)) {
            l_pos1 = l_mouse;
            engine::log( log_debug, "mouse left click");
            l_set = true;

            if( l_item == nullptr) {
                inventory_entry *l_item_tmp = p_player->inventory->onClick( l_pos1);
                if( l_item_tmp) {
                    l_item = new inventory_entry;
                    *l_item = *l_item_tmp;
                    p_player->inventory->del( l_item_tmp);
                }
            } else {
                vec2 l_tile_pos = p_player->inventory->getTilePos(l_pos1);
                engine::log( log_debug, "%3.d %3.d", l_tile_pos.x, l_tile_pos.y);
                engine::inventory_entry *l_item_add = p_player->inventory->add( l_tile_pos, l_item->objtype);
                if( l_item_add) {
                    l_item_add->angle = l_item->angle;
                    delete l_item;
                    l_item = nullptr;
                }
            }
        }

        if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_special)) {
            if( l_item)
                p_player->inventory->turn( l_item, true);
        } 
        l_pos2 = l_mouse - l_pos1;
        if( p_input->edgeDetection( input_key_edge_detection_up, input_buttons_attack)) {
            engine::log( log_debug, "mouse left click lose");
            l_set = false;
        }

        
        drawInventory( graphic); // draw inventory
        graphic->setDrawColor( 255, 255, 0, 255);
        if(l_set)
            graphic->drawLine( l_camera+l_pos1, l_pos2);
        
        if( l_item)
            p_player->inventory->drawItem( graphic, l_camera + l_mouse, l_item,
            vec2{ 16, 16}/vec2{ 2, 2});

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

        if( p_player->inventory)
            p_entity->bindInput( p_player, nullptr);
    }
}

void player::drawInventory( engine::graphic_draw *graphic) {
    graphic->setDrawColor( 70, 30, 30, 100);
    graphic->drawFilledRect( vec2{ 0, 0} + graphic->getCamera()->getPosition().toVec(), graphic->getCamera()->getSize().toVec());
    p_player->inventory->draw( graphic);
}