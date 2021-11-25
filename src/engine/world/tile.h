#ifndef TILE_H
#define TILE_H

#include <string>

#include "../graphic.h"

namespace engine {
    class tile {
        public:
            tile();
            ~tile();
            
            void setName(std::string name) { p_name = name; }
            std::string getName() { return p_name; }
        private:
            std::string p_name;
            graphic_image *p_image;
    };
};

#endif