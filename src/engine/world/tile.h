#ifndef TILE_H
#define TILE_H

#include <string>

#include "../graphic.h"

namespace engine {
    class tile {
        public:
            tile();
            ~tile();

        private:
            std::string p_name;
            graphic_image *p_image;
    };
};

#endif