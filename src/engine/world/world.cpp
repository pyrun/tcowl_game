#include "world.h"
#include "../noise.h"
#include "../helper.h"
#include "../timer.h"

using namespace engine;

world *engine::used_world_handler = NULL;

world::world() {
}

world::~world() {
}

void world::begin( graphic *graphic, tile_manager *tileset, biom_manager *biom_manager, uint32_t seed) {
    p_seed = seed;
    p_graphic = graphic;
    p_tileset = tileset;
    p_biom_manager = biom_manager;
    p_world_data = new world_tile[WORLD_SIZE*WORLD_SIZE];

    // set biom to map
    generate();
}

void world::generate() {
    uint32_t l_bioms_amount = p_biom_manager->getAmount();
    for( uint32_t x = 0; x < WORLD_SIZE; x++) {
        for( uint32_t y = 0; y < WORLD_SIZE; y++) {
            uint32_t l_biom_index = (uint32_t)(helper::perlin2d(x, y, 0.1, 4)*l_bioms_amount);
            biom *l_biom = p_biom_manager->get( l_biom_index);
            world_tile *l_tile = getTile( x, y);
            l_tile->biom = l_biom;
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
        return nullptr;
    return &p_world_data[WORLD_SIZE * x + y];  
}

void world::reload( graphic_draw *graphic) {
    p_tileset->reload( graphic);
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

void world::update() {
    engine::used_world_handler = this;

    p_biom_manager->update();

    for( uint32_t x = 0; x < WORLD_SIZE; x++) {
        for( uint32_t y = 0; y < WORLD_SIZE; y++) {
            world_tile *l_tile = getTile( x, y);
            if( l_tile->bot == nullptr && l_tile->biom) {
                script::function( "set", l_tile->biom->getLuaState(), x, y);
            }
        }
    }

    engine::used_world_handler = nullptr;
}