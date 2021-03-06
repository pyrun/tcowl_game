#ifndef WORLD_TILE_MANAGER_HPP
#define WORLD_TILE_MANAGER_HPP

#include <vector>
#include "tile.hpp"

#define ENGINE_TILE_FILE_DESCRIPTION "definition.json"

namespace engine {
    class tile_manager {
        public:
            tile_manager();
            ~tile_manager();

            bool loadFolder( graphic *graphic, std::string path);
            void loadtype( graphic *graphic, std::string file);
            void reload( graphic_draw *graphic);

            uint32_t getAmount() { return (uint32_t)p_type.size(); }
            tile *get( uint32_t index) { return &p_type.at( index); }
            tile *getById( uint16_t id);
            tile *getByName( std::string name);
        private:
            tile *createtype();
            bool removetype( tile *target);

            std::vector<tile> p_type;

            uint16_t p_highest_id;
    };
};

#endif