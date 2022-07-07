#ifndef ENGINE_WORLD_H
#define ENGINE_WORLD_H

#include <vector>


#include <engine/image.hpp>
#include <engine/graphic.hpp>
#include <engine/physic/body.hpp>
#include <engine/physic/hub.hpp>
#include <engine/network_ids.hpp>
#include <network/network.hpp>

#include "room.hpp"
#include "tile.hpp"
#include "tile_manager.hpp"
#include "biom_manager.hpp"

#define WORLD_SIZE 32 // both axies
#define WORLD_PHYSIC_BODYS 1024 // bodys - we don't need many, but in the worst case scenario.
#define WORLD_STRUCT_SIZE 5 // ID 2byte + Biom 2byte + Tick 1byte

namespace engine {
    struct world_tile {
        uint32_t ticks = 0;
        uint32_t animation_tick = 0;

        tile *bot = nullptr;

        biom *biom = nullptr;
    };

    struct world_physic_body {
        physic::body body;
        physic::shape_rect *shape;
    };

    class world : public engine::graphic_object, public network::synchronisation {
        public:
            world();
            ~world();

            void begin( graphic *graphic, tile_manager *tilemananger, biom_manager *biom_manager, uint32_t seed = 0x0);
            void generate(biom *biom);
            bool checkSolidTileReachable( vec2 position);
            void generate_collisionmap( physic::hub *hub);
            void cleanup();

            bool readRawData( uint8_t *buffer, size_t length);
            std::vector<uint8_t> getRawData();

            void setTile(int x, int y, tile *tiledata);
            world_tile *getTile(int x, int y);

            void createRoom( vec2 position);

            void reload( engine::graphic_draw *graphic) override;
            void draw( engine::graphic_draw *graphic) override;

            uint32_t outNetworkData( world_tile *tile, vec2 pos, uint8_t *dataDist);
            void inNetworkData( uint8_t *dataDist);

            void network_update( network::connection *network_interface) override;
            bool newClientCallback( network::client *client, network::connection *network_interface) override;
            void recvPacket( network::packet packet) override;

            void update();

            const uint32_t getPriority() override { return 32; }
            tile_manager *getTileManager() { return p_tileset; }
            vec2 getWorldSize() { return { WORLD_SIZE, WORLD_SIZE}; }
        private:
            world_tile *p_world_data;
            world_physic_body *p_physic_bodys;

            uint32_t p_seed;
            tile_manager *p_tileset;
            biom_manager *p_biom_manager;

            graphic *p_graphic;

            std::vector<room*> p_rooms;
    };

    extern world *used_world_handler;
};
#endif