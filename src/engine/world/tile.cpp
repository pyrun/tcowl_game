#include "tile.h"

using namespace engine;

tile::tile() {
}

tile::~tile() {
}

tile_graphic_type engine::stringToTileGraphicType( std::string string) {
    if( string == "special")
        return tile_graphic_type_special;
    if( string == "n")
        return tile_graphic_type_boder_n;
    if( string == "ne" || string == "en")
        return tile_graphic_type_boder_ne;
    if( string == "e")
        return tile_graphic_type_boder_e;
    if( string == "se" || string == "es")
        return tile_graphic_type_boder_se;
    if( string == "s")
        return tile_graphic_type_boder_s;
    if( string == "sw" || string == "ws")
        return tile_graphic_type_boder_sw;
    if( string == "w")
        return tile_graphic_type_boder_w;
    if( string == "nw" || string == "wn")
        return tile_graphic_type_boder_nw;
    return tile_graphic_type_dafault;
}