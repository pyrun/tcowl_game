#ifndef ENGINE_WORLD_H
#define ENGINE_WORLD_H

#include <vector>

#include <engine/graphic/image.hpp>
#include <engine/graphic/graphic.hpp>
#include <engine/physic/body.hpp>
#include <engine/physic/hub.hpp>
#include <engine/network_ids.hpp>
#include <network/network.hpp>

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

        tile data;
    };

    struct world_physic_body {
        physic::body body;
        physic::shape_rect *shape;
    };

    struct world_data {
        world_tile *tiles;
        world_physic_body *physic_bodys;

        biom *biom;
    };

    class world : public engine::graphic_object, public network::synchronisation {
        public:
            world( graphic *graphic, tile_manager *tileset, biom_manager *biom_manager);
            ~world();

            void start(uint32_t seed = 0);
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

            void update( physic::hub *hub);

            const uint32_t getPriority() override { return 32; }
            tile_manager *getTileManager() { return p_tileset; }
            biom *getBiom() { return p_world_data==nullptr?nullptr:p_world_data->biom; }
            vec2 getWorldSize() { return { WORLD_SIZE, WORLD_SIZE}; }
        private:
            world_data *p_world_data;

            uint32_t p_seed;
            tile_manager *p_tileset;
            biom_manager *p_biom_manager;

            bool p_change;

            graphic *p_graphic;
    };

    extern world *used_world_handler;
};
#endif