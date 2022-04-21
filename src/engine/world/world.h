#ifndef ENGINE_WORLD_H
#define ENGINE_WORLD_H

#include <vector>

#include "../image.h"
#include "../graphic.h"

#include "room.h"
#include "tile.h"
#include "tile_manager.h"
#include "biom_manager.h"

#define WORLD_SIZE 64 // both axies
#define WORLD_STRUCT_SIZE 5 // ID 2byte + Biom 2byte + Tick 1byte

namespace engine {
    struct world_tile {
        uint32_t ticks = 0;
        uint32_t animation_tick = 0;

        tile *bot = nullptr;

        biom *biom = nullptr;
    };

    class world : public engine::graphic_object {
        public:
            world();
            ~world();

            void begin( graphic *graphic, tile_manager *tilemananger, biom_manager *biom_manager, uint32_t seed = 0x0);
            void generate();
            void cleanup();

            bool readRawData( uint8_t *buffer, size_t length);
            std::vector<uint8_t> getRawData();

            void setTile(int x, int y, tile *tiledata);
            world_tile *getTile(int x, int y);

            void createRoom( vec2 position);

            void reload( engine::graphic_draw *graphic) override;
            void draw( engine::graphic_draw *graphic) override;

            void update();

            const uint32_t getPriority() override { return 32; }
            tile_manager *getTileManager() { return p_tileset; }
        private:
            world_tile *p_world_data;
            uint32_t p_seed;
            tile_manager *p_tileset;
            biom_manager *p_biom_manager;

            graphic *p_graphic;

            std::vector<room*> p_rooms;
    };

    extern world *used_world_handler;
};
#endif