#include "world.h"

using namespace engine;

world::world() {
}

world::~world() {
}

void world::begin( graphic *graphic) {
    p_graphic = graphic;
    // dummy
    // todo loade tiles
    p_image.load( p_graphic, "tile/grass/graphic.bmp");
}

void world::draw( engine::graphic_draw *graphic) {
    for( int32_t x = 2; x < 18; x++)
        for( int32_t y = 2; y < 10; y++)
            graphic->draw( &p_image, engine::vec2{ 16*x, 16*y}, engine::vec2{ 16, 16});
}