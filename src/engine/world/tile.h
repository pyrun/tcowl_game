#ifndef TILE_H
#define TILE_H

#include <string>

#include "../graphic.h"
#include "../image.h"

#define ENGINE_TILE_SIZE 16
#define ENGINE_VEC2_TILE_SIZE vec2{ ENGINE_TILE_SIZE, ENGINE_TILE_SIZE}

namespace engine {
    enum tile_graphic_type {
        tile_graphic_type_special = 0,
        tile_graphic_type_dafault,
        tile_graphic_type_boder_n, // top
        tile_graphic_type_boder_ne,
        tile_graphic_type_boder_e, // right
        tile_graphic_type_boder_se,
        tile_graphic_type_boder_s, // down
        tile_graphic_type_boder_sw,
        tile_graphic_type_boder_w, // left
        tile_graphic_type_boder_nw,
    };

    tile_graphic_type stringToTileGraphicType( std::string string);

    struct tile_graphic {
        std::string name;
        tile_graphic_type type;
        vec2 position;
        uint32_t length;
        uint32_t ticks;
    };

    struct tile {
            uint16_t id;
            image image;
            bool solid;
            std::string folder;

            std::vector<tile_graphic> graphic;
    };
};

#endif