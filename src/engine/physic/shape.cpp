#include "shape.h"

using namespace physic;

void shape_rect::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    graphic->drawRect( position.toVec() + getOffset().toVec(), p_rect.toVec());
}

engine::fvec2 shape_rect::getAABB() {
    return p_rect;
}

void sharp_circle::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    // TODO: draw circle
}

engine::fvec2 sharp_circle::getAABB() {
    return { p_size, p_size};
}

engine::fvec2 sharp_line::getAABB() {
    return p_offsetpoint;
}

void sharp_line::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    // TODO: draw line
}