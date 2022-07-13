#include "player.hpp"

using namespace engine;
using namespace game;

#include <engine/log.hpp>
#include <engine/timer.hpp>

player::player() {
    p_font = NULL;
    p_item_move.item = nullptr;
    p_item_move.origin = nullptr;
    p_state =  player_state::player_state_idle;
}

player::~player() {
    
}

void player::begin( engine::font *font, engine::input*input, engine::entity_handler *entitys) {
    p_font = font;
    p_input = input;
    p_entity = entitys;

    p_player = p_entity->get( 2);

    if( p_player && p_player->inventory) {
        engine::inventory_entry l_entry;
        l_entry.objtype = entitys->getTypeByName("bread");
        p_player->inventory->add( { 5, 3}, &l_entry);

        /*p_player->inventory->add( { 2, 3}, &l_entry);
        l_entry.objtype = entitys->getType(102);
        p_player->inventory->add( { 4, 2}, &l_entry);
        l_entry.objtype = entitys->getType(103);
        p_player->inventory->add( { 3, 3}, &l_entry);
        l_entry.objtype = entitys->getType(101);
        p_player->inventory->add( { 2, 2}, &l_entry);*/
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
    
    if( p_player) {
        switch(p_state) {
            case player_state::player_state_idle: {
                p_entity->bindInput( p_player, p_input->getInputMap());

                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_inventory))
                    p_state = player_state::player_state_inventory;
            } break;

            case player_state::player_state_inventory: {
                p_entity->bindInput( p_player, nullptr);
                if( p_player->inventory == nullptr) { // if no inventory -> cancle
                    p_state = player_state::player_state_idle;
                    break;
                }

                // move item
                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_attack) &&
                    p_item_move.item == nullptr) { // pick up item
                    engine::inventory_onClick_answer l_answer = p_player->inventory->onClick( l_mouse);
                    if( l_answer.item) { // remove item from inventory
                        p_item_move.item = new engine::inventory_entry;
                        *p_item_move.item = *l_answer.item;
                        p_item_move.item_origin_state = *l_answer.item;
                        p_item_move.pos = l_answer.point;
                        p_player->inventory->del( l_answer.item);
                        engine::log( log_debug, "%d %d", p_item_move.pos.x, p_item_move.pos.y);
                        p_item_move.origin = p_player->inventory;
                    }
                }
                if( p_input->edgeDetection( input_key_edge_detection_up, input_buttons_attack) &&
                     p_item_move.item) { // place item
                    vec2 l_tile_pos = p_player->inventory->getTilePos(l_mouse) - p_item_move.pos;
                    engine::inventory_entry *l_item_add = p_player->inventory->add( l_tile_pos, p_item_move.item); // try to add in inventory
                    if( l_item_add) { // if happend we change some settings
                        l_item_add->angle = p_item_move.item->angle;
                        clearItemMove();
                    } else { // return item prev. place
                        p_player->inventory->add( p_item_move.item_origin_state.pos, &p_item_move.item_origin_state);
                        clearItemMove();
                    }
                }
                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_special)) {// turn item
                    if( p_item_move.item) {
                        p_player->inventory->turn( p_item_move.item, true);
                        p_item_move.pos = vec2{ -p_item_move.pos.y, p_item_move.pos.x};
                    }
                }

                drawInventory( graphic); // draw inventory

                if( p_item_move.item != nullptr) {
                    vec2 l_tile_pos = p_player->inventory->getTilePos(l_mouse)-p_item_move.pos;
                    switch(  p_player->inventory->check( l_tile_pos, p_item_move.item)) {
                        case engine::inventory_grid_state::inventory_grid_state_taken: {
                            graphic->setDrawColor( 255, 64, 32, 255);
                            graphic->drawRect( l_camera+(l_mouse/ENTITY_INVENTORY_SIZE_VEC2)*ENTITY_INVENTORY_SIZE_VEC2, ENTITY_INVENTORY_SIZE_VEC2);
                        } break;
                        case engine::inventory_grid_state::inventory_grid_state_available: {
                            graphic->setDrawColor( 0, 255, 64, 255);
                            graphic->drawRect( l_camera+(l_mouse/ENTITY_INVENTORY_SIZE_VEC2)*ENTITY_INVENTORY_SIZE_VEC2, ENTITY_INVENTORY_SIZE_VEC2);
                        } break;
                        default:
                        break;
                    }
                    p_player->inventory->drawItem( graphic,
                        l_camera + l_mouse - (p_item_move.pos*ENTITY_INVENTORY_SIZE_VEC2) - ENTITY_INVENTORY_SIZE_VEC2/vec2{2,2}, //pos
                        p_item_move.item, // item
                        ENTITY_INVENTORY_SIZE_VEC2/vec2{ 2, 2}); // centre
                }

                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_inventory)) {
                    p_state = player_state::player_state_idle;
                    if( p_item_move.item != nullptr) {
                        // return item
                        p_player->inventory->add( p_item_move.item_origin_state.pos, &p_item_move.item_origin_state);
                        clearItemMove();
                    }
                }
            } break; // player_state::player_state_inventory

            default:
            break;
        }

        // centre camera to player
        action *l_action = p_player->objtype->getAction( p_player->action);
        engine::fvec2 l_pos = p_player->body->getPosition() + ( fvec2{ (float)l_action->size.x, (float)l_action->size.y} / fvec2{ 2.f, 2.f} );
        graphic->getCamera()->setTarget( l_pos);
    }
}

void player::update() {
    if( p_player) {
        p_player->change = true;
    }
}

void player::clearItemMove() {
    if( p_item_move.item)
        delete p_item_move.item;
    p_item_move.item = nullptr;
    p_item_move.origin = nullptr;
}

void player::drawInventory( engine::graphic_draw *graphic) {
    graphic->setDrawColor( 70, 30, 30, 100);
    graphic->drawFilledRect( vec2{ 0, 0} + graphic->getCamera()->getPosition().toVec(), graphic->getCamera()->getSize().toVec());
    p_player->inventory->draw( graphic);
}