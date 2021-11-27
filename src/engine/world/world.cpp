#include "world.h"
#include "../noise.h"
#include "../helper.h"
#include "../timer.h"

using namespace engine;

world::world() {
}

world::~world() {
}

void world::begin( graphic *graphic, tile_manager *tileset, uint32_t seed) {
    p_seed = seed;
    p_graphic = graphic;
    p_tileset = tileset;
    p_world_data = new world_tile[WORLD_SIZE*WORLD_SIZE];

    for( uint32_t x = 0; x < WORLD_SIZE; x++) {
        for( uint32_t y = 0; y < WORLD_SIZE; y++) {
            setTile( x, y, p_tileset->get(0));
            world_tile *l_tile = getTile( x, y);
            engine::tile_graphic *l_tile_graphic = l_tile->bot->getGraphic(0);
            if( l_tile_graphic)
                l_tile->animation_tick = ((uint32_t)(helper::perlin2d(x*100, y*100, 0.1, 4)*l_tile_graphic->length));
        }
    }
}

void world::cleanup() {
    if( p_world_data != nullptr)
        delete[] p_world_data;
}

void world::createRoom( vec2 position) {
    room *l_room;

    l_room = new room();
    l_room->init(position);

    p_rooms.push_back( l_room);
}

void world::setTile(int x, int y, tile *tiledata) {
    if( WORLD_SIZE <= x ||
        WORLD_SIZE <= y)
        return;
    p_world_data[WORLD_SIZE * x + y].bot = tiledata;  
}

world_tile *world::getTile(int x, int y) {
    if( WORLD_SIZE <= x ||
        WORLD_SIZE <= y)
        return NULL;
    return &p_world_data[WORLD_SIZE * x + y];  
}

void world::draw( engine::graphic_draw *graphic) {
    vec2 l_length_tiles = graphic->getCameraSize();
    l_length_tiles = l_length_tiles / ENGINE_VEC2_TILE_SIZE;

    for( int32_t x = 0; x < l_length_tiles.x+1; x++) {
        for( int32_t y = 0; y < l_length_tiles.y+1; y++) {
            world_tile *l_data = getTile( x, y);
            if( !l_data)
                continue;
            tile *l_tile = l_data->bot;
            if( !l_tile)
                continue;

            engine::tile_graphic *l_tile_graphic = l_tile->getGraphic(0);
            if( !l_tile_graphic)
                continue;

            // Animation ticks
            if( l_tile_graphic->ticks &&
                helper::time::check( &l_data->ticks, l_tile_graphic->ticks)) {
                helper::time::reset( &l_data->ticks);
                l_data->animation_tick++;
                if( l_data->animation_tick >= l_tile_graphic->length)
                     l_data->animation_tick-=l_tile_graphic->length;
            }

            graphic->draw(  l_tile->getImage(),
                            engine::vec2{ ENGINE_TILE_SIZE*x, ENGINE_TILE_SIZE*y},
                            ENGINE_VEC2_TILE_SIZE,
                            l_tile_graphic->position
                            + engine::vec2{ (int32_t)l_data->animation_tick*ENGINE_TILE_SIZE, 0});
        }
    }
}