#include "world.h"

using namespace engine;

world::world() {
}

world::~world() {
}

void world::begin( graphic *graphic, uint32_t seed) {
    p_seed = seed;
    p_graphic = graphic;

    // todo load folder with tilesets
    p_tileset.load( p_graphic, "tile/grass/graphic.bmp");
}

void world::createRoom( vec2 position) {
    room *l_room;

    l_room = new room();
    l_room->init(position);

    p_rooms.push_back( l_room);
}

void world::draw( engine::graphic_draw *graphic) {
    for( int32_t x = 2; x < 18; x++)
        for( int32_t y = 2; y < 10; y++)
            graphic->draw( &p_tileset, engine::vec2{ 16*x, 16*y}, engine::vec2{ 16, 16}, engine::vec2{ ((16*x+16*y)%3)*16, (16*y)*0} );
}