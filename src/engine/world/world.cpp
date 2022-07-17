#include "world.hpp"

#include <engine/noise.hpp>
#include <engine/helper.hpp>
#include <engine/timer.hpp>
#include <engine/log.hpp>

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
    p_world_data = new world_data;
    p_world_data->tiles = new world_tile[WORLD_SIZE*WORLD_SIZE];
    p_world_data->physic_bodys = new world_physic_body[WORLD_PHYSIC_BODYS] {};

    graphic->getCamera()->setBorder( { WORLD_SIZE, WORLD_SIZE});

    // set up map
    p_world_data->biom = p_biom_manager->get(0);

    engine::used_world_handler = this;
    p_biom_manager->update();
    script::function( "Generation", p_world_data->biom->getLuaState(), WORLD_SIZE, WORLD_SIZE);
    engine::used_world_handler = nullptr;


}

bool world::checkSolidTileReachable( vec2 position) {
    static vec2 l_array[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    uint8_t l_solid = 0;
    
    world_tile *l_tile_stay = getTile( position.x, position.y);
    if( l_tile_stay == nullptr || l_tile_stay->data.solid == false)
        return false; // tile not solid or nullptr

    for( int32_t i  = 0; i < 4; i++) {
        world_tile *l_tile = getTile( position.x+l_array[i].x, position.y+l_array[i].y);
        if( l_tile == nullptr)
            l_solid++;
        else if( l_tile->data.solid)
            l_solid++;
    }

    if( l_solid != 4)
        return true; // one side is exposed
    return false; // surrounded  with solid blocks
}

void world::generate_collisionmap( physic::hub *hub) {
    uint32_t l_index = 0, l_time;
    helper::time::reset( &l_time);

    // TODO cleanup
    for( uint32_t i = 0; i < WORLD_PHYSIC_BODYS; i++)
        hub->del( &p_world_data->physic_bodys[i].body);

    std::vector<vec2> l_skip_list; // tiles that alrdy has collision boxes get to this list
    auto checkSkip = []( std::vector<vec2> list, int x, int y) {
        bool l_skip = false;
            for( int i = 0; i < list.size(); i++) {
                if( list[i] == vec2{ x, y})
                    l_skip = true;
            }
        return l_skip;
    };

    // Create physical bodies only on accessible tiles
    for( int32_t x = 0; x < WORLD_SIZE; x++) {
        for( int32_t y = 0; y < WORLD_SIZE; y++) {
            if( checkSkip( l_skip_list, x, y))
                continue;
            if( checkSolidTileReachable( { x, y}) ) {
                int32_t l_row = 1, l_line = 1;
                // check y
                while( checkSolidTileReachable( { x, y+l_row})) {
                    if( checkSkip( l_skip_list, x, y+l_row))
                        break;
                    l_skip_list.push_back({ x, y+l_row});
                    l_row++;
                };

                // if only one tile size in y check x
                if( l_row == 1) {
                    while( checkSolidTileReachable( { x+l_line, y})) {
                        if( checkSkip( l_skip_list, x+l_line, y))
                            break;
                        l_skip_list.push_back({ x+l_line, y});
                        l_line++;
                    };
                }

                physic::body *l_body = &p_world_data->physic_bodys[l_index].body;

                if( p_world_data->physic_bodys[l_index].shape != nullptr)
                    delete p_world_data->physic_bodys[l_index].shape;
                p_world_data->physic_bodys[l_index].shape = new physic::shape_rect( { (float)(ENGINE_TILE_SIZE*l_line), (float)(ENGINE_TILE_SIZE*l_row) });
                l_body->shape = p_world_data->physic_bodys[l_index].shape;
                l_body->position = { (float)x*ENGINE_TILE_SIZE, (float)y*ENGINE_TILE_SIZE};
                hub->add( l_body); // add to the hub

                // check amount of tiles
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

    // biom
    l_data.push_back( (p_world_data->biom->getId()>>8));
    l_data.push_back( (p_world_data->biom->getId()));

    for( uint32_t i = 0; i < WORLD_SIZE*WORLD_SIZE; i++) {
        // id
        l_data.push_back( (p_world_data->tiles[i].data.id>>8));
        l_data.push_back( (p_world_data->tiles[i].data.id));

        // tick
        l_data.push_back( p_world_data->tiles[i].animation_tick);
    }
    return l_data;
}

void world::setTile(int x, int y, tile *tiledata) {
    if( WORLD_SIZE <= x ||
        WORLD_SIZE <= y ||
        x < 0 ||
        y < 0)
        return;
    world_tile *l_tile = getTile( x, y);

    l_tile->data = *tiledata;

    // set face
    engine::tile_graphic *l_tile_graphic = &l_tile->data.graphic[0]; // TODO check
    l_tile->animation_tick = rand()%l_tile_graphic->length;
}

world_tile *world::getTile(int x, int y) {
    if( WORLD_SIZE <= x ||
        WORLD_SIZE <= y ||
        x < 0 ||
        y < 0)
        return nullptr;
    return &p_world_data->tiles[WORLD_SIZE * x + y];  
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
            tile *l_tile = &l_data->data;

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

uint32_t world::outNetworkData( world_tile *tile, vec2 pos, uint8_t *dataDist) {
    uint32_t l_offset = 0;

    helper::int16toUint8x2( tile->data.id, dataDist + l_offset); l_offset +=2;
    helper::int16toUint8x2( tile->animation_tick, dataDist + l_offset); l_offset +=2;
    helper::int16toUint8x2( pos.x, dataDist + l_offset); l_offset +=2;
    helper::int16toUint8x2( pos.y, dataDist + l_offset); l_offset +=2;

    return l_offset;
}

void world::inNetworkData( uint8_t *dataDist) {
    uint32_t l_offset = 0;
    int16_t l_id_biom, l_id, l_animation_tick;
    int16_t l_x, l_y;

    helper::uint8x2toInt16( dataDist + l_offset, &l_id); l_offset +=2;
    helper::uint8x2toInt16( dataDist + l_offset, &l_animation_tick); l_offset +=2;
    helper::uint8x2toInt16( dataDist + l_offset, &l_x); l_offset +=2;
    helper::uint8x2toInt16( dataDist + l_offset, &l_y); l_offset +=2;

    engine::tile *l_tile = p_tileset->getById( l_id);
    if( l_tile == nullptr)
        return;
    
    setTile( l_x, l_y, l_tile);
    engine::world_tile *l_world_tile = getTile( l_x, l_y);
    l_world_tile->animation_tick = l_animation_tick;

    // TODO Biom
}

void world::network_update( network::connection *network_interface) {
    
}

bool world::newClientCallback( network::client *client, network::connection *network_interface) {
    network::packet l_packet;

    for( int32_t x = 0; x < WORLD_SIZE; x++) {
        for( int32_t y = 0; y < WORLD_SIZE; y++) {
            world_tile *l_tile = getTile( x, y);

            l_packet.type = engine::network_id::network_id_world_data;
            l_packet.length = outNetworkData( l_tile, { x, y}, l_packet.data);
            l_packet.crc = network::getCRC8( l_packet);

            network_interface->sendPacket( l_packet, client);
        }
    }
    log( log_trace, "world::newClientCallback");
    return true;
}

void world::recvPacket( network::packet packet) {
    // todo check okay
    if( packet.type == engine::network_id::network_id_world_data )
        inNetworkData( packet.data);
}

void world::update() {
    engine::used_world_handler = this;

    p_biom_manager->update();

    engine::used_world_handler = nullptr;
}