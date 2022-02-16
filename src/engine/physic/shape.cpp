#include "shape.h"

using namespace physic;

void shape_rect::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    graphic->drawRect( position.toVec() + getOffset().toVec(), p_rect.toVec());
}

engine::fvec2 shape_rect::getAABB() {
    return p_rect;
}

void sharp_circle::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    engine::fvec2 l_size = { p_size, p_size};
    graphic->drawellipse( position.toVec() + getOffset().toVec() + l_size.toVec(), l_size );
}

engine::fvec2 sharp_circle::getAABB() {
    return { p_size*2, p_size*2};
}

engine::fvec2 sharp_line::getAABB() {
    return p_offsetpoint;
}

void sharp_line::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    drawLine( { 10, 10}, {50, 50});
}