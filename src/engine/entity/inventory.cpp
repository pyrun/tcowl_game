#include "inventory.hpp"

using namespace engine;

#include <engine/graphic/image.hpp>

engine::image l_image;

void inventory_grid::add( vec2 pos, engine::type *objtype) {
    setState( pos, inventory_grid_state::inventory_grid_state_taken);
    inventory_entry l_enitry;
    l_enitry.objtype = objtype;
    l_enitry.pos = pos;
    l_enitry.angle = inventory_angle_90;
    p_items.push_back( l_enitry);
}

bool inventory_grid::del( inventory_entry *item) {
    for(uint32_t i = 0; i < p_items.size(); i++) {
        if( p_items[i].pos == item->pos) {
            p_items.erase( p_items.begin() + i);
            return true;
        }
    }
    return false;
}

void inventory_grid::setState( vec2 pos, inventory_grid_state state) {
    p_grid->set( pos.x, pos.y, state);
}

void inventory_grid::reload( graphic_draw *graphic) {

}

inventory_entry *inventory_grid::onClick( vec2 pos) {
    for(uint32_t i = 0; i < p_items.size(); i++) {
        inventory_entry *l_item = &p_items[i];

        action *l_action = l_item->objtype->getAction( 0);

        vec2 l_pos = (l_item->pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * vec2{16, 16} + p_draw_pos;

        if( pos.x > l_pos.x && 
            pos.y > l_pos.y &&
            pos.x < l_pos.x + l_action->size.x &&
            pos.y < l_pos.y + l_action->size.y) {
            return l_item;
        }
    }
    return nullptr;
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
                    (l_pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * vec2{16, 16} + graphic->getCamera()->getPosition().toVec() + p_draw_pos,
                    vec2{16, 16});
        }
    }


    for(uint32_t i = 0; i < p_items.size(); i++) {
        inventory_entry *l_item = &p_items[i];
        drawItem( graphic, (l_item->pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * vec2{16, 16} + graphic->getCamera()->getPosition().toVec() + p_draw_pos, l_item);
    }
}

void inventory_grid::drawItem( graphic_draw *graphic, vec2 pos, inventory_entry *item, vec2 centre) {
    action *l_action = item->objtype->getAction( 0); // TODO: display action
    graphic->draw(  item->objtype->getImage(),
        pos-centre,
        l_action->size,
        l_action->postion,
        (double)item->angle);
}

vec2 inventory_grid::calcDrawPos( graphic_draw *graphic, bool top) {
    p_draw_pos = vec2{ graphic->getCamera()->getSize().toVec().x/2,  (int32_t)top?0:(int32_t)(graphic->getCamera()->getSize().toVec().y - p_grid->getH()*16)};
    return p_draw_pos;
}