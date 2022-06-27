#include "world.h"
#include "../noise.h"
#include "../helper.h"
#include "../timer.h"
#include "../log.h"

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

    p_physic_bodys = new world_physic_body[WORLD_PHYSIC_BODYS];

    graphic->getCamera()->setBorder( { WORLD_SIZE, WORLD_SIZE});

    // set up map
    biom *l_biom = p_biom_manager->get( 0/* rand()%p_biom_manager->getAmount()*/);
    generate( l_biom);

}

void world::generate( biom *biom) {
    for( uint32_t x = 0; x < WORLD_SIZE; x++) {
        for( uint32_t y = 0; y < WORLD_SIZE; y++) {
            world_tile *l_tile = getTile( x, y);
            l_tile->biom = biom;
        }
    }

    // once
    update();
}

bool world::checkSolidTileReachable( vec2 position) {
    static vec2 l_array[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    uint8_t l_solid = 0;
    
    world_tile *l_tile_stay = getTile( position.x, position.y);
    if( l_tile_stay == nullptr || l_tile_stay->bot->solid == false)
        return false; // tile not solid or nullptr

    for( int32_t i  = 0; i < 4; i++) {
        world_tile *l_tile = getTile( position.x+l_array[i].x, position.y+l_array[i].y);
        if( l_tile == nullptr)
            l_solid++;
        else if( l_tile->bot &&
            l_tile->bot->solid)
            l_solid++;
    }

    if( l_solid != 4)
        return true; // one side is exposed
    return false; // surrounded  with solid blocks
}

void world::generate_collisionmap( physic::hub *hub) {
    uint32_t l_index = 0, l_time;
    helper::time::reset( &l_time);

    for( uint32_t i = 0; i < WORLD_PHYSIC_BODYS; i++)
        hub->del( &p_physic_bodys[i].body);

    std::vector<vec2> l_skip_list; // tiles that alrdy has collision boxes

    for( int32_t x = 0; x < WORLD_SIZE; x++) {
        for( int32_t y = 0; y < WORLD_SIZE; y++) {
            bool l_skip = false;
            for( int i = 0; i < l_skip_list.size(); i++)
                if( l_skip_list[i] == vec2{ x, y})
                    l_skip = true;
            if( l_skip)
                continue;

            if( checkSolidTileReachable( { x, y}) ) {
                int32_t l_row = 1, l_line = 1;
                while( checkSolidTileReachable( { x, y+l_row})) { l_row++; };

                if( l_row == 1) {
                    while( checkSolidTileReachable( { x+l_line, y})) {
                        l_skip_list.push_back({ x+l_line, y});
                        l_line++;
                    };
                }

                physic::body *l_body = &p_physic_bodys[l_index].body;
                p_physic_bodys[l_index].shape = new physic::shape_rect( { (float)(ENGINE_TILE_SIZE*l_line), (float)(ENGINE_TILE_SIZE*l_row) });

                l_body->linkShape( p_physic_bodys[l_index].shape);
                l_body->setPosition( { (float)x*ENGINE_TILE_SIZE, (float)y*ENGINE_TILE_SIZE} );

                hub->add( l_body);

                // skip some tiles
                y += l_row - 1;

                l_index++;
                if( l_index >= WORLD_PHYSIC_BODYS) {
                    engine::log( log_debug, "world::generate_collisionmap reach max! %d %d", x, y);
                    return;
                }
            }
        }
    }
    engine::log( log_debug, "world::generate_collisionmap %dbodys %dms", l_index, helper::time::getDurrent( &l_time));
}

void world::cleanup() {
    if( p_world_data != nullptr)
        delete[] p_world_data;
}

bool world::readRawData( uint8_t *buffer, size_t length) {
    return false;
}

std::vector<uint8_t> world::getRawData() {
    std::vector<uint8_t> l_data;
    size_t l_size = WORLD_SIZE*WORLD_SIZE*WORLD_STRUCT_SIZE;

    l_data.reserve(l_size);

    for( uint32_t i = 0; i < WORLD_SIZE*WORLD_SIZE; i++) {
        // id
        l_data.push_back( (p_world_data[i].bot->id>>8));
        l_data.push_back( (p_world_data[i].bot->id));
        
        // biom
        l_data.push_back( (p_world_data[i].biom->getId()>>8));
        l_data.push_back( (p_world_data[i].biom->getId()));

        // tick
        l_data.push_back( p_world_data[i].animation_tick);
    }
    return l_data;
}

void world::createRoom( vec2 position) {
    room *l_room;

    l_room = new room();
    l_room->init(position);

    p_rooms.push_back( l_room);
}

void world::setTile(int x, int y, tile *tiledata) {
    if( WORLD_SIZE <= x ||
        WORLD_SIZE <= y ||
        x < 0 ||
        y < 0)
        return;
    world_tile *l_tile = &p_world_data[WORLD_SIZE * x + y];
    l_tile->bot = tiledata;

    // set face
    engine::tile_graphic *l_tile_graphic = &l_tile->bot->graphic[0]; // TODO check
    l_tile->animation_tick = rand()%l_tile_graphic->length;
}

world_tile *world::getTile(int x, int y) {
    if( WORLD_SIZE <= x ||
        WORLD_SIZE <= y ||
        x < 0 ||
        y < 0)
        return nullptr;
    return &p_world_data[WORLD_SIZE * x + y];  
}

void world::reload( graphic_draw *graphic) {
    p_tileset->reload( graphic);
}

void world::draw( engine::graphic_draw *graphic) {
    vec2 l_length_tiles = ( (graphic->getCamera()->getSize()+fvec2{ ENGINE_TILE_SIZE*2, ENGINE_TILE_SIZE*2}) / fvec2{ ENGINE_TILE_SIZE, ENGINE_TILE_SIZE}).toVec();
    vec2 l_pos_tiles = graphic->getCamera()->getPosition().toVec() / ENGINE_VEC2_TILE_SIZE;

    for( int32_t x = l_pos_tiles.x; x < l_length_tiles.x+l_pos_tiles.x; x++) {
        for( int32_t y = l_pos_tiles.y; y < l_length_tiles.y+l_pos_tiles.y; y++) {
            world_tile *l_data = getTile( x, y);
            if( !l_data)
                continue;
            tile *l_tile = l_data->bot;
            if( !l_tile)
                continue;

            engine::tile_graphic *l_tile_graphic = &l_tile->graphic[0]; // TODO check
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

            graphic->draw(  &l_tile->image,
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
                script::function( "Set", l_tile->biom->getLuaState(), x, y);
            }
        }
    }

    engine::used_world_handler = nullptr;
}