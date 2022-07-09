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

void inventory_grid::setState( vec2 pos, inventory_grid_state state) {
    p_grid->set( pos.x, pos.y, state);
}

void inventory_grid::reload( graphic_draw *graphic) {

}

void inventory_grid::draw( graphic_draw *graphic, bool top) {
    if( l_image.getTexture() == nullptr)
        l_image.load( graphic, "system/inventory.png");

    engine::vec2 l_pos;
    vec2 l_position = vec2{ graphic->getCamera()->getSize().toVec().x/2,  (int32_t)top?0:(int32_t)(graphic->getCamera()->getSize().toVec().y - p_grid->getH()*16)};
    for(l_pos.x = 0; l_pos.x < p_grid->getW(); l_pos.x++ ) {
        for(l_pos.y = 0; l_pos.y < p_grid->getH(); l_pos.y++ ) {
            engine::inventory_grid_state *l_tile = p_grid->get( l_pos.x, l_pos.y);
            
            if( l_tile && *l_tile != engine::inventory_grid_state::inventory_grid_state_unavailable)
                graphic->draw( &l_image,
                    (l_pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * vec2{16, 16} + graphic->getCamera()->getPosition().toVec() + l_position,
                    vec2{16, 16});
        }
    }


    for(uint32_t i = 0; i < p_items.size(); i++) {
        inventory_entry *l_item = &p_items[i];
        action *l_action = l_item->objtype->getAction( 0); // TODO: display action
        graphic->draw(  l_item->objtype->getImage(),
                        (l_item->pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * vec2{16, 16} + graphic->getCamera()->getPosition().toVec() + l_position,
                        l_action->size,
                        l_action->postion,
                        (double)l_item->angle);

    }
}