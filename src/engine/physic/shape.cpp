#include "shape.h"

using namespace physic;

void shape_rect::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    graphic->drawRect( position.toVec() + getOffset().toVec(), p_rect.toVec());
}

void sharp_circle::draw( engine::graphic_draw *graphic, engine::fvec2 position) {
    // todo

}