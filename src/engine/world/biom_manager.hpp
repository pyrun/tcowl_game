#ifndef WORLD_BIOM_MANAGER_HPP
#define WORLD_BIOM_MANAGER_HPP

#include <vector>
#include "biom.hpp"

#define ENGINE_BIOM_FILE_DESCRIPTION "definition.json"

namespace engine {
    class biom_manager {
        public:
            biom_manager();
            ~biom_manager();

            bool loadFolder( std::string path);
            void loadType( std::string file);
            bool loadScript( biom *biom);
            void init();

            uint32_t getAmount() { return (uint32_t)p_biomes.size(); }
            biom *get( uint16_t id) { return &p_biomes[id]; }

            void update();
        private:
            bool p_init;

            biom *create();
            bool remove( biom *target);

            std::vector<biom> p_biomes;
    };
};

#endif