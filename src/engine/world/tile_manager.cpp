#include "tile_manager.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <dirent.h>

#include "../helper.h"
#include "../helper_json.h"
#include "../log.h"

using json = nlohmann::json;
using namespace engine;

tile_manager::tile_manager() {
    p_type.clear();
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
    l_tile->setName( helper::json::getString( &l_json, "name", "noName").c_str());
    
    // Graphic
    std::string l_image_file;
    l_image_file = helper::json::getString( &l_json, "image", "error");
    if( l_image_file == "error") {// Datei nicht angegeben
        removetype( l_tile);
        return;
    }

    // Load image
    l_tile->getImage()->load( graphic, folder + l_image_file);

    // Alpha key
    uint8_t *l_color_key;
    l_color_key = helper::json::getNumberArrayN<uint8_t>( &l_json, "alpha-key", 3);
    if( l_color_key) {
        l_tile->getImage()->setAlphaKey( l_color_key[0], l_color_key[1], l_color_key[2]);
        delete l_color_key;
    }

    // Set Folder path
    l_tile->setFolderPath( folder);

    // Alle Grafiken/Animationen laden und hinzufügen
    if( l_json["graphic"].is_array()) {
        json l_json_root_graphic = l_json["graphic"];
        for( uint32_t i = 0; i < l_json["graphic"].size(); i++) {
            json l_graphic_json = l_json_root_graphic[i];
            tile_graphic l_graphic;

            // Laden der Werte
            l_graphic.name = helper::json::getString( &l_graphic_json, "name", "noName");
            l_graphic.position = helper::json::getVec2( &l_graphic_json, "position");
            l_graphic.length = helper::json::getUint32( &l_graphic_json, "length", 1);
            l_graphic.ticks = helper::json::getUint32( &l_graphic_json, "ticks-for-next-image", 0);

            log( log_level::log_debug, "tile_graphic '%s' %dx%d length %d durration %d",
                l_graphic.name.c_str(),
                l_graphic.position.x,
                l_graphic.position.y,
                l_graphic.length,
                l_graphic.ticks);
            l_tile->addGraphic( l_graphic);
        }
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
