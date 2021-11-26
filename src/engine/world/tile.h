#ifndef TILE_H
#define TILE_H

#include <string>

#include "../graphic.h"
#include "../image.h"

namespace engine {
    struct tile_graphic {
        std::string name;
        vec2 position;
        uint32_t length;
        uint32_t ticks;
    };

    class tile {
        public:
            tile();
            ~tile();

            void setFolderPath( std::string folder) { p_folder = folder; }
            std::string getFolderPath() { return p_folder; }

            void setName(std::string name) { p_name = name; }
            std::string getName() { return p_name; }
            image *getImage() { return &p_image; }
        private:
            std::string p_name;
            image p_image;
            std::string p_folder;

            std::vector<tile_graphic> p_graphic;
    };
};

#endif