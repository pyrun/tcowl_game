#include "tile_manager.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <dirent.h>

#include "../helper.h"
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
