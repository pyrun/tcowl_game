#include "inventory.hpp"

using namespace engine;

#include <engine/graphic/image.hpp>

engine::image l_image;

inventory_entry *inventory_grid::add( vec2 pos, inventory_entry *objtype) {
    if( check( pos, objtype) != inventory_grid_state_available)
        return nullptr;
    for (engine::vec2 const& l_hitbox : getItemHitboxList(objtype) ) {
        setState( pos+l_hitbox, inventory_grid_state::inventory_grid_state_taken);
    }
    inventory_entry l_enitry;
    l_enitry.objtype = objtype->objtype;
    l_enitry.pos = pos;
    l_enitry.angle = objtype->angle;
    p_items.push_back( l_enitry);
    return &p_items[p_items.size()-1];
}

bool inventory_grid::del( inventory_entry *item) {
    for(uint32_t i = 0; i < p_items.size(); i++) {
        if( p_items[i].pos == item->pos) {
            for (engine::vec2 const& l_hitbox : getItemHitboxList(item) ) {
                engine::inventory_grid_state * l_state = p_grid->get( p_items[i].pos.x+l_hitbox.x, p_items[i].pos.y+l_hitbox.y);
                if( l_state)
                    *l_state = engine::inventory_grid_state::inventory_grid_state_available;
            }
            p_items.erase( p_items.begin() + i);
            return true;
        }
    }
    return false;
}

vec2 inventory_grid::getTilePos( vec2 pos_abs) {
    return (pos_abs-p_draw_pos+vec2{ (int32_t)p_grid->getW()/2*ENTITY_INVENTORY_SIZE, 0} )/ENTITY_INVENTORY_SIZE_VEC2;
}

engine::inventory_grid_state inventory_grid::check( vec2 pos, inventory_entry *objtype) {
    for (engine::vec2 const& l_hitbox : getItemHitboxList(objtype)) {
        engine::inventory_grid_state *l_state = p_grid->get( pos.x+l_hitbox.x, pos.y+l_hitbox.y);
        if( l_state == nullptr)
            return engine::inventory_grid_state::inventory_grid_state_unavailable;
        if( *l_state != engine::inventory_grid_state::inventory_grid_state_available)
            return *l_state;
    }
    return engine::inventory_grid_state::inventory_grid_state_available;
}

void inventory_grid::setState( vec2 pos, inventory_grid_state state) {
    p_grid->set( pos.x, pos.y, state);
}

void inventory_grid::reload( graphic_draw *graphic) {

}

inventory_onClick_answer inventory_grid::onClick( vec2 pos) {
    inventory_onClick_answer l_return = { .item = nullptr};
    for(uint32_t i = 0; i < p_items.size(); i++) {
        inventory_entry *l_item = &p_items[i];
        for (engine::vec2 const& l_hitbox : getItemHitboxList(l_item)) {
            vec2 l_pos = (l_item->pos + l_hitbox - vec2{ (int32_t)p_grid->getW()/2, 0}) * ENTITY_INVENTORY_SIZE_VEC2 + p_draw_pos;
            if( pos.x > l_pos.x && 
                pos.y > l_pos.y &&
                pos.x < l_pos.x + ENTITY_INVENTORY_SIZE &&
                pos.y < l_pos.y + ENTITY_INVENTORY_SIZE) {
                l_return.item = l_item;
                l_return.point = l_hitbox;
                return l_return;
            }
        }
    }
    return l_return;
}



void inventory_grid::turn( inventory_entry *item, bool clockwise) {
    if( item == nullptr)
        return;
    switch( item->angle) {
        case inventory_angle::inventory_angle_0:
            item->angle = clockwise?inventory_angle::inventory_angle_90:inventory_angle::inventory_angle_270;
        break;
        case inventory_angle::inventory_angle_90:
            item->angle = clockwise?inventory_angle::inventory_angle_180:inventory_angle::inventory_angle_0;
        break;
        case inventory_angle::inventory_angle_180:
            item->angle = clockwise?inventory_angle::inventory_angle_270:inventory_angle::inventory_angle_90;
        break;
        case inventory_angle::inventory_angle_270:
            item->angle = clockwise?inventory_angle::inventory_angle_0:inventory_angle::inventory_angle_180;
        break;
        default:
        break;
    }
}

void inventory_grid::draw( graphic_draw *graphic, bool top) {
    if( l_image.getTexture() == nullptr)
        l_image.load( graphic, "system/inventory.png");
    
    calcDrawPos( graphic, top);

    engine::vec2 l_pos;
    vec2 p_draw_pos = vec2{ graphic->getCamera()->getSize().toVec().x/2,  (int32_t)top?0:(int32_t)(graphic->getCamera()->getSize().toVec().y - p_grid->getH()*16)};
    for(l_pos.x = 0; l_pos.x < p_grid->getW(); l_pos.x++ ) {
        for(l_pos.y = 0; l_pos.y < p_grid->getH(); l_pos.y++ ) {
            engine::inventory_grid_state *l_tile = p_grid->get( l_pos.x, l_pos.y);
            
            if( l_tile && *l_tile != engine::inventory_grid_state::inventory_grid_state_unavailable)
                graphic->draw( &l_image,
                    (l_pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * ENTITY_INVENTORY_SIZE_VEC2+ graphic->getCamera()->getPosition().toVec() + p_draw_pos,
                    vec2{16, 16},
                    vec2{*l_tile==engine::inventory_grid_state::inventory_grid_state_taken?16:0,0});
        }
    }


    for(uint32_t i = 0; i < p_items.size(); i++) {
        inventory_entry *l_item = &p_items[i];
        drawItem( graphic,
            (l_item->pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * ENTITY_INVENTORY_SIZE_VEC2 + graphic->getCamera()->getPosition().toVec() + p_draw_pos,
            l_item,
            ENTITY_INVENTORY_SIZE_VEC2 / vec2{ 2, 2});
    }
}

void inventory_grid::drawItem( graphic_draw *graphic, vec2 pos, inventory_entry *item, vec2 centre) {
    action *l_action = item->objtype->getAction( 0); // TODO: display action
    graphic->draw(  item->objtype->getImage(),
        pos,
        l_action->size,
        l_action->postion,
        (double)item->angle,
        &centre);
}

std::vector<vec2> inventory_grid::getItemHitboxList( inventory_entry *item) {
    std::vector<vec2> l_list;
    for (engine::vec2 const& l_hitbox : item->objtype->getItemHitbox() ) {
        switch( item->angle) {
            case engine::inventory_angle::inventory_angle_0:
                l_list.push_back( l_hitbox);
            break;
            case engine::inventory_angle::inventory_angle_90:
                l_list.push_back( vec2{ -l_hitbox.y, l_hitbox.x});
            break;
            case engine::inventory_angle::inventory_angle_180:
                l_list.push_back( vec2{ -l_hitbox.x, -l_hitbox.y});
            break;
            case engine::inventory_angle::inventory_angle_270:
                l_list.push_back( vec2{ l_hitbox.y, -l_hitbox.x });
            break;
            default:
            break;
        }
    }
    return l_list;
}

vec2 inventory_grid::calcDrawPos( graphic_draw *graphic, bool top) {
    p_draw_pos = vec2{ graphic->getCamera()->getSize().toVec().x/2,  (int32_t)top?0:(int32_t)(graphic->getCamera()->getSize().toVec().y - p_grid->getH()*16)};
    return p_draw_pos;
}