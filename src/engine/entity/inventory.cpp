#include "inventory.hpp"

using namespace engine;

#include <engine/graphic/image.hpp>

engine::image l_image;

void inventory_grid::reload( graphic_draw *graphic) {

}

void inventory_grid::draw( graphic_draw *graphic, bool top) {
    if( l_image.getTexture() == nullptr)
        l_image.load( graphic, "system/inventory.png");

    engine::vec2 l_pos;
    for(l_pos.x = 0; l_pos.x < p_grid->getW(); l_pos.x++ ) {
        for(l_pos.y = 0; l_pos.y < p_grid->getH(); l_pos.y++ ) {
            engine::inventory_grid_state *l_tile = p_grid->get( l_pos.x, l_pos.y);

            vec2 l_position = vec2{ graphic->getCamera()->getSize().toVec().x/2,  (int32_t)top?0:(int32_t)(graphic->getCamera()->getSize().toVec().y - p_grid->getH()*16)};

            graphic->draw( &l_image,
                (l_pos - vec2{ (int32_t)p_grid->getW()/2, 0}) * vec2{16, 16} + graphic->getCamera()->getPosition().toVec() + l_position,
                vec2{16, 16}); 
        }
    }
}