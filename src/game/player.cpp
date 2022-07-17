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

    p_player = p_entity->get( 1);
    p_transfer_target = p_entity->get( 2);

    if( p_player && p_player->inventory) {
        engine::inventory_entry l_entry;
        l_entry.objtype = entitys->getTypeByName("bread");
        l_entry.pos = vec2{ 5, 3};
        p_player->inventory->add( &l_entry);
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

                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_inventory)) {
                    p_state = player_state::player_state_inventory;
                    vec2 l_offset = {5, 5};
                    std::vector<engine::entity*> l_entitys = p_entity->find(
                        p_player->body->position.toVec()+p_player->body->shape->getOffset().toVec()-l_offset,
                        p_player->body->shape->getAABB().toVec()+l_offset*vec2{2,2});


                    for( engine::entity *l_entity: l_entitys) { // check if we can find an inventory
                        if( l_entity->inventory == nullptr || // no inventory
                            l_entity == p_player) // not himself
                            continue;
                        
                        engine::used_entity_handler = p_entity;
                        if( script::function( "OpenInventory",l_entity->lua_state, l_entity->index)) {
                            p_state = player_state_inventory_transfer;
                            p_transfer_target = l_entity;
                        }
                        engine::used_entity_handler = nullptr;

                    }
                }
            } break;

            case player_state::player_state_inventory_transfer: {
                if( p_transfer_target == nullptr ||
                    p_transfer_target->inventory == nullptr) {
                    p_state = player_state::player_state_inventory;
                    p_transfer_target = nullptr;
                    break;
                }
            } [[fallthrough]];
            case player_state::player_state_inventory: {
                p_entity->bindInput( p_player, nullptr);
                if( p_player->inventory == nullptr) { // if no inventory -> cancle
                    p_state = player_state::player_state_idle;
                    break;
                }
                if( p_state == player_state_inventory && p_transfer_target)
                    p_transfer_target = nullptr;

                // move item
                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_use) &&
                    p_item_move.item == nullptr) { // pick up item
                    if( !tryGetItemFromInventory( l_mouse, p_player))
                        tryGetItemFromInventory( l_mouse, p_transfer_target);
                }
                if( p_input->edgeDetection( input_key_edge_detection_up, input_buttons_use) &&
                     p_item_move.item) { // place item
                    if( tryPutItemToInventory( l_mouse, p_player));
                    else if( tryPutItemToInventory( l_mouse, p_transfer_target));
                    else {
                        p_item_move.origin->add( &p_item_move.item_origin_state); // TODO If it does not work, the object is deleted
                        clearItemMove();
                    }
                }
                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_action)) {// turn item
                    if( p_item_move.item) {
                        engine::inventory::turn( p_item_move.item, true);
                        p_item_move.pos = vec2{ -p_item_move.pos.y, p_item_move.pos.x}; // rotate offset x&y
                    }
                }

                drawInventory( graphic); // draw inventory// drag item draw

                if( p_item_move.item != nullptr) {
                    engine::inventory_grid_state l_state  = checkItemInInventory( l_mouse, p_player);
                    if( l_state == engine::inventory_grid_state::inventory_grid_state_unavailable)
                        l_state  = checkItemInInventory( l_mouse, p_transfer_target);
                    switch(  l_state) {
                        case engine::inventory_grid_state::inventory_grid_state_taken: {
                            graphic->setDrawColor( 255, 64, 32, 255);
                            SDL_SetTextureColorMod( p_item_move.item->objtype->image.getTexture(), 255, 64, 32);
                        } break;
                        default:
                        break;
                    }

                    p_player->inventory->drawItem( graphic,
                        l_camera + l_mouse - (p_item_move.pos*ENTITY_INVENTORY_SIZE_VEC2) - ENTITY_INVENTORY_SIZE_VEC2/vec2{2,2}, //pos
                        p_item_move.item, // item
                        ENTITY_INVENTORY_SIZE_VEC2/vec2{ 2, 2}); // centre
                    SDL_SetTextureColorMod( p_item_move.item->objtype->image.getTexture(), 255, 255, 255);
                }

                if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_inventory)) {
                    p_state = player_state::player_state_idle;
                    if( p_item_move.item != nullptr) {
                        // return item
                        p_item_move.origin->add( &p_item_move.item_origin_state); // TODO If it does not work, the object is deleted
                        clearItemMove();
                    }

                    if( p_transfer_target) {
                        engine::used_entity_handler = p_entity;
                        script::function( "CloseInventory", p_transfer_target->lua_state, p_transfer_target->index);
                        engine::used_entity_handler = nullptr;
                    }
                }
            } break; // player_state::player_state_inventory

            default:
            break;
        }

        // centre camera to player
        action *l_action = &p_player->objtype->actions[ p_player->action];
        engine::fvec2 l_pos = p_player->body->position + ( fvec2{ (float)l_action->size.x, (float)l_action->size.y} / fvec2{ 2.f, 2.f} );
        graphic->getCamera()->setTarget( l_pos);
    }
}

void player::update() {
    if( p_player) {
        p_player->change = true;
    }
}

bool player::tryGetItemFromInventory( engine::vec2 pos, engine::entity *entity) {
    if( entity == nullptr ||
        entity->inventory == nullptr)
        return false;
    engine::inventory_onClick_answer l_answer = entity->inventory->onClick( pos);
    if( l_answer.item) { // remove item from inventory
        p_item_move.item = new engine::inventory_entry;
        *p_item_move.item = *l_answer.item;
        p_item_move.item_origin_state = *l_answer.item;
        p_item_move.pos = l_answer.point;
        entity->inventory->del( l_answer.item);
        p_item_move.origin = entity->inventory;
        return true;
    }
    return false;
}

bool player::tryPutItemToInventory( engine::vec2 pos, engine::entity *entity) {
    if( entity == nullptr ||
        entity->inventory == nullptr)
        return false;
    
    p_item_move.item->pos = entity->inventory->getTilePos(pos)-p_item_move.pos;
    
    engine::inventory_entry *l_item_add = entity->inventory->add( p_item_move.item); // try to add in inventory
    if( l_item_add) { // if happend we change some settings
        l_item_add->angle = p_item_move.item->angle;
        clearItemMove();
        return true;
    }
    return false;
}

engine::inventory_grid_state player::checkItemInInventory( engine::vec2 pos, engine::entity *entity) {
    if( entity == nullptr ||
        entity->inventory == nullptr)
        return inventory_grid_state::inventory_grid_state_unavailable;
    p_item_move.item->pos = entity->inventory->getTilePos( pos)-p_item_move.pos;

    return entity->inventory->check( p_item_move.item);
}



void player::clearItemMove() {
    if( p_item_move.item)
        delete p_item_move.item;
    p_item_move.item = nullptr;
    p_item_move.origin = nullptr;
}

void player::drawInventory( engine::graphic_draw *graphic) {
    vec2 l_camera = graphic->getCamera()->getPosition().toVec();

    static const int32_t l_border_offset = 40;
    graphic->setDrawColor( 20, 20, 20, 200);
    graphic->drawFilledRect( vec2{ l_border_offset, 0} + graphic->getCamera()->getPosition().toVec(), graphic->getCamera()->getSize().toVec() - vec2{  l_border_offset*2, 0});

    p_player->inventory->draw( graphic); // player inventory
    if( p_transfer_target &&
        p_transfer_target->inventory) {
        p_font->print( l_camera + vec2{120, 90}, "%s:", p_transfer_target->objtype->name.c_str());
        p_transfer_target->inventory->draw( graphic, vec2{ 0, 100});  // target inventory
    }
    
    p_font->print( l_camera + vec2{ 40, 20}, "states:");
    p_font->print( l_camera + vec2{ 40, 30}, "strength %d", 3);
    p_font->print( l_camera + vec2{ 40, 40}, "skill %d", 2);
    p_font->print( l_camera + vec2{ 40, 50}, "magic %d", 2);
}