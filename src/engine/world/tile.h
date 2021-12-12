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

    tile_graphic_type stringToTileGraphicType( std::string string) {
        if( string == "n")
            return tile_graphic_type_boder_n;
    }

    struct tile_graphic {
        std::string name;
        tile_graphic_type type;
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

            void addGraphic( tile_graphic tileg) { p_graphic.push_back(tileg); }
            tile_graphic *getGraphic( uint32_t i) { return &p_graphic[i]; }
        private:
            std::string p_name;
            image p_image;
            std::string p_folder;

            std::vector<tile_graphic> p_graphic;
    };
};

#endif