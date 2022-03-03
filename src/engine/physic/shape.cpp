#include "shape.h"

using namespace physic;

void shape_rect::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    graphic->drawRect( position.toVec() + getOffset().toVec(), p_rect.toVec());
}

engine::fvec2 shape_rect::getAABB() {
    return p_rect;
}

void shape_circle::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    engine::fvec2 l_size = { p_size, p_size};
    graphic->drawEllipse( position.toVec() + getOffset().toVec() + l_size.toVec(), l_size );
}

engine::fvec2 shape_circle::getAABB() {
    return { p_size*2, p_size*2};
}

engine::fvec2 shape_line::getAABB() {
    return p_offsetpoint;
}

void shape_line::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    graphic->drawLine( position.toVec(), p_offsetpoint.toVec());
}