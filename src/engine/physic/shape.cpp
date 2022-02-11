#include "shape.h"

using namespace physic;

void shape_rect::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    graphic->drawRect( position.toVec() + getOffset().toVec(), p_rect.toVec());
}

engine::fvec2 shape_rect::getAABB() {
    return p_rect;
}

void sharp_circle::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    // todo
}

engine::fvec2 sharp_circle::getAABB() {
    return { 10.f, 10.f};
}