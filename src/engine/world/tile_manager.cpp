#include "tile_manager.hpp"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <dirent.h>

#include <engine/helper.hpp>
#include <engine/helper_json.hpp>
#include <engine/log.hpp>

using json = nlohmann::json;
using namespace engine;

tile_manager::tile_manager() {
    p_type.clear();
    p_highest_id = 1024;
}

tile_manager::~tile_manager() {

}

bool tile_manager::loadFolder(  graphic *graphic, std::string path) {
    DIR *l_dir;
    struct dirent *l_entry;

    // if folder dont end with '/' add the character
    if( path.c_str()[path.size()-1] != '/')
        path = path + '/';
        
    // open dir
    l_dir = opendir( path.c_str());
    if ( l_dir == NULL) {
        engine::log( log_warn, "Cant open path folder %s", path.c_str());
        return false;
    }

    // open ech folder
    while ((l_entry = readdir(l_dir)) != NULL) {
        std::string l_path = path + l_entry->d_name;

        // dont go back folder
        if( l_entry->d_name[0] == '.')
            continue;
        
        // Check if file or folder
        if( helper::isDirectory( l_path.c_str())) {
            // Check if folder has type inside
            if( helper::file_exists( (l_path + "/" + ENGINE_TILE_FILE_DESCRIPTION).c_str()))
                loadtype( graphic, l_path + "/");
            loadFolder( graphic, l_path);
        }
    }
    closedir(l_dir);

    return true;
}

void tile_manager::loadtype( graphic *graphic, std::string folder) {
    std::ifstream l_file( folder + ENGINE_TILE_FILE_DESCRIPTION);
    if( l_file.is_open() == false) {
        log( log_level::log_warn, "tile_manager::loadtype %s konnte nicht geoefnet werden", (folder + ENGINE_TILE_FILE_DESCRIPTION).c_str());
        return;
    }
    
    // Load file
    json l_json;
    l_file >> l_json;

    tile *l_tile = createtype();

    // Name
    l_tile->id = helper::json::getUint32( &l_json, "id", p_highest_id);
    l_tile->name = helper::json::getString( &l_json, "name");
    if( l_tile->id >= p_highest_id)
        p_highest_id = l_tile->id+1;
    
    // Graphic
    std::string l_image_file;
    l_image_file = helper::json::getString( &l_json, "image", "error");
    if( l_image_file == "error") {// Datei nicht angegeben
        removetype( l_tile);
        return;
    }

    // Solid
    l_tile->solid = helper::json::getBool( &l_json, "solid", false);

    // Load image
    l_tile->image.load( graphic, folder + l_image_file);
    if( l_tile->image.getTexture() == nullptr) {
        log( log_level::log_warn, "tile_manager::loadtype %s graphic file cannot be loaded", (folder + ENGINE_TILE_FILE_DESCRIPTION).c_str());
        removetype( l_tile);
        return;
    }

    // Alpha key
    uint8_t *l_color_key;
    l_color_key = helper::json::getNumberArrayN<uint8_t>( &l_json, "alpha-key", 3);
    if( l_color_key) {
        l_tile->image.setAlphaKey( l_color_key[0], l_color_key[1], l_color_key[2]);
        delete l_color_key;
    }

    // Set Folder path
    l_tile->folder = folder;

    // Alle Grafiken/Animationen laden und hinzufügen
    if( l_json["graphic"].is_array()) {
        json l_json_root_graphic = l_json["graphic"];
        for( uint32_t i = 0; i < l_json["graphic"].size(); i++) {
            json l_graphic_json = l_json_root_graphic[i];
            tile_graphic l_graphic;

            // Laden der Werte
            l_graphic.name = helper::json::getString( &l_graphic_json, "name", "noName");
            
            std::string l_type = helper::json::getString( &l_graphic_json, "type", "");
            l_graphic.type = (engine::tile_graphic_type)helper::json::getUint32( &l_graphic_json, "type", engine::tile_graphic_type::tile_graphic_type_dafault);
            
            l_graphic.position = helper::json::getVec2( &l_graphic_json, "position");
            l_graphic.length = helper::json::getUint32( &l_graphic_json, "length", 1);
            l_graphic.ticks = helper::json::getUint32( &l_graphic_json, "delay", 0);

            log( log_level::log_debug, "tile_graphic '%s' %dx%d length %d durration %d type %s",
                l_graphic.name.c_str(),
                l_graphic.position.x,
                l_graphic.position.y,
                l_graphic.length,
                l_graphic.ticks,
                l_type.c_str());
            l_tile->graphic.push_back( l_graphic);
        }
    } else { // default
        tile_graphic l_graphic;
        l_graphic.length = 1;
        l_graphic.name = "idle";
        l_graphic.position = vec2{ 0, 0};
        l_graphic.ticks = 1;
        l_graphic.type = engine::tile_graphic_type::tile_graphic_type_dafault;
        l_tile->graphic.push_back( l_graphic);
    }

    log( log_level::log_warn, "tile_manager::loadtype \"%s\" #%d loaded", l_tile->name.c_str(), l_tile->id);
}

void tile_manager::reload( graphic_draw *graphic) {
    for( uint32_t i = 0; i < p_type.size(); i++) {
        tile *l_type = &p_type[i];
        // reload image
        l_type->image.reload( graphic);
    }
}

tile *tile_manager::createtype() {
    tile l_type;
    p_type.push_back(l_type);
    return &p_type.back();
}

bool tile_manager::removetype( tile *target) {
    for( uint32_t i = 0; i < p_type.size(); i++) {
        tile *l_type = &p_type[i];
        if( l_type == target) {
            p_type.erase( p_type.begin()+i);
            return true;
        }
    }
    return false;
}

tile *tile_manager::getById( uint16_t id) {
    for( tile &l_tile: p_type)
        if( l_tile.id == id)
            return &l_tile;
    return nullptr;
}

tile *tile_manager::getByName( std::string name) {
    for( tile &l_tile: p_type)
        if( l_tile.name == name)
            return &l_tile;
    return nullptr;
}