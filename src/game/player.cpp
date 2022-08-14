#include "player.hpp"

#include <algorithm>

using namespace engine;
using namespace game;

#include <engine/log.hpp>
#include <engine/timer.hpp>
#include <engine/entity/action.hpp>

player::player() {
    p_font = NULL;
    p_item_move.item = nullptr;
    p_item_move.origin = nullptr;
    p_state = player_state::player_state_idle;
}

player::~player() {
    
}

void player::begin( engine::font *font, engine::input*input, engine::entity_handler *entitys, engine::world *world) {
    p_font = font;
    p_input = input;
    p_entity = entitys;
    p_world = world;

    p_player = p_entity->get( 1);
    p_transfer_target = nullptr;

    addBattleTarget( { .entity = p_entity->get( 1), .team = player_battle_team_ally });
    //addBattleTarget( { .entity = p_entity->get( 2), .team = player_battle_team_enemy});
    //addBattleTarget( { .entity = p_entity->get( 1), .team = player_battle_team_ally });
    //addBattleTarget( { .entity = p_entity->get( 2), .team = player_battle_team_enemy});
    //addBattleTarget( { .entity = p_entity->get( 1), .team = player_battle_team_ally });
    addBattleTarget( { .entity = p_entity->get( 2), .team = player_battle_team_enemy});
    //addBattleTarget( { .entity = p_entity->get( 1), .team = player_battle_team_ally });
    addBattleTarget( { .entity = p_entity->get( 2), .team = player_battle_team_enemy});

    p_state =  player_state::player_state_battle;



    if( p_player && p_player->inventory) {
        engine::inventory_entry l_entry;
        l_entry.objtype = entitys->getTypeByName("bread");
        l_entry.pos = vec2{ 5, 3};
        p_player->inventory->add( &l_entry);

        l_entry.objtype = entitys->getTypeByName("bread_sword");
        l_entry.pos = vec2{ 3, 1};
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

    p_player = p_entity->get( 1);
    if( p_player) {
        p_font->print( l_camera + vec2{ -120, 10} + vec2{ (int32_t)graphic->getCamera()->getSize().x, 0}, "Pos %.1f %.1f", p_player->body->position.x, p_player->body->position.y);
    
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
            
            case player_state_battle: {
                drawBattle( graphic);
            } break;

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
        entity->change = true;
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

    // TODO: set only the entity change true
    if( p_player)
        p_player->change = true;
    if( p_transfer_target)
        p_transfer_target->change = true;
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
    int32_t l_index = 0;
    for( engine::entity_parameter &l_par:p_player->parameter) {
        p_font->print( l_camera + vec2{ 40, 30+(10*l_index++)}, "%s %d", l_par.name.c_str(), l_par.value);
    }
}

void player::drawBattle( engine::graphic_draw *graphic) {
    vec2 l_camera = graphic->getCamera()->getPosition().toVec();
    vec2 l_camera_size = graphic->getCamera()->getSize().toVec();
    vec2 l_mouse = graphic->getMousePositionToLogicalMousePosition( p_input->getInputMap()->mouse);

    // dark background
    graphic->setDrawColor( 20, 20, 20, 200);
    graphic->drawFilledRect( graphic->getCamera()->getPosition().toVec(), graphic->getCamera()->getSize().toVec());

    // floor background
    graphic->setDrawColor( 20, 20, 20, 220);
    graphic->drawFilledRect( graphic->getCamera()->getPosition().toVec()+vec2{ 0, 100}, graphic->getCamera()->getSize().toVec());

    // battle background
    engine::biom *l_biom = p_world->getBiom();
        if( l_biom != nullptr) {
        if( l_biom->battle_bg.getTexture() == nullptr)
            l_biom->battle_bg.load( graphic, l_biom->battle_bg_file.c_str());

        if( l_biom->battle_bg.getTexture() != nullptr)
            graphic->draw(  &l_biom->battle_bg,
                            l_camera + vec2{ 0, l_camera_size.y - l_biom->battle_bg.size.y},
                            l_biom->battle_bg.size);
    }

    if( p_battle_icons.getTexture() == nullptr)
        p_battle_icons.load( graphic, "system/battle.png");

    bool l_click = false;
    if( p_input->edgeDetection( input_key_edge_detection_down, input_buttons_use))
        l_click = true;

    p_font->print( l_camera + vec2{ 10, 10}, "Battle Order:");

    uint32_t l_index = 0; 
    for( player_battle_obj &l_obj:p_battle.battle_objects) {
        if( l_obj.entity == nullptr)
            continue;
        
        p_font->print( l_camera + vec2{ 10, 20 + l_obj.draw_index *10}, "%d %d %s", l_index++, l_obj.draw_index, l_obj.entity->objtype->name.c_str());

        // draw entity
        action *l_action = l_obj.entity->objtype->findAction( "Battle");
        if( l_action == nullptr)
            l_action = &l_obj.entity->objtype->actions[0];
        
        if( helper::time::check( &l_obj.time, l_action->delay) ) {
            helper::time::reset( &l_obj.time);
            l_obj.tick++;
        }

        int32_t l_zoom = 2;
        float l_width = l_camera_size.x*0.8f;
        vec2 l_offset = vec2{ (int32_t)(l_width/(p_battle.battle_objects.size()-1) *(l_obj.draw_index) )-(int32_t)(l_width/2), 40+(int32_t)(sin(l_obj.draw_index*10)*20.f)};

        vec2 l_position = l_camera_size.half()-l_action->size.half()*vec2{ 2, 2} + l_offset;

        // draw hud
        graphic->draw(  &p_battle_icons,
            l_camera + l_position-vec2{ 0, 16},
            vec2{ 16, 16},
            vec2{ 0, 0});

        // object
        graphic->draw(  &l_obj.entity->objtype->image,
                        l_camera + l_position,
                        l_action->size,
                        l_action->postion + vec2{ (int32_t)(l_obj.tick%l_action->length) * l_action->size.x, 0},
                        0.0,
                        nullptr,
                        (l_obj.team == player_battle_team_enemy)^l_action->flip_vertical?graphic_flip::graphic_flip_vertical:graphic_flip_none,
                        l_zoom);
        if( p_battle.target == &l_obj) {
            graphic->setDrawColor( 255, 20, 20, 220);
            graphic->drawRect( l_camera + l_position, l_action->size*vec2{ l_zoom, l_zoom});
        }

        if( l_obj.entity->index != p_player->index &&
            l_click &&
            physic::testAABBAABB( fvec2{ (float)l_mouse.x, (float)l_mouse.y}, fvec2{ 1.f, 1.f},
                fvec2{ (float)l_position.x, (float)l_position.y}, fvec2{ (float)l_action->size.x*l_zoom, (float)l_action->size.y*l_zoom}))
            p_battle.target = &l_obj;
    }

    p_player->inventory->draw( graphic); // player inventory
}

void player::addBattleTarget( player_battle_obj obj) {
    p_battle.battle_objects.push_back( obj);
    std::vector<player_battle_obj> p_battle_draw = p_battle.battle_objects; // copy

    // set index
    uint32_t l_n = 0;
    for( player_battle_obj &l_obj:p_battle_draw)
        l_obj.draw_index = l_n++;

    // sort order by team
    struct {
        bool operator()(player_battle_obj a, player_battle_obj b) const { return a.team < b.team; }
    } customLess;
    std::sort(p_battle_draw.begin(), p_battle_draw.end(), customLess);

    // set draw index
    l_n = 0;
    for( player_battle_obj &l_obj:p_battle_draw)
        p_battle.battle_objects[l_obj.draw_index].draw_index = l_n++;
}