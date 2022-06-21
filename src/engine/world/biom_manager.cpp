#include "biom_manager.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <dirent.h>

#include "../helper.h"
#include "../helper_json.h"
#include "../log.h"

using json = nlohmann::json;
using namespace engine;

biom_manager::biom_manager() {
    p_init = false;

    p_biomes.clear();
}

biom_manager::~biom_manager() {
}

bool biom_manager::loadFolder( std::string path) {
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
            if( helper::file_exists( (l_path + "/" + ENGINE_BIOM_FILE_DESCRIPTION).c_str()))
                loadType( l_path + "/");
            loadFolder( l_path);
        }
    }
    closedir(l_dir);

    return true;
}

void biom_manager::loadType( std::string folder) {
    std::ifstream l_file( folder + ENGINE_BIOM_FILE_DESCRIPTION);
    if( l_file.is_open() == false) {
        log( log_level::log_warn, "biom_manager::loadType %s konnte nicht geoefnet werden", (folder + ENGINE_BIOM_FILE_DESCRIPTION).c_str());
        return;
    }

    // Load file
    json l_json;
    l_file >> l_json;

    biom *l_biom = create();

    // Name
    l_biom->setId( helper::json::getUint32( &l_json, "id"));
    l_biom->setName( helper::json::getString( &l_json, "name", "noName").c_str());
    l_biom->setScriptName( folder + helper::json::getString( &l_json, "script", "noName").c_str());

    if( !loadScript( l_biom)) {
        remove( l_biom);
        return;
    }

    log( log_trace, "biom_manager::loadType %s %s", l_biom->getName().c_str(), l_biom->getScriptName().c_str());
}

bool biom_manager::loadScript( biom *biom) {
    std::string l_file;

    // check for null
    if( biom == NULL)
        return false;
    
    // Delete existing script beforehand if it is existing
    script::free( biom->getLuaState());

    // try load script
    biom->setLuaState( script::loadFile( biom->getScriptName().c_str()));
    if( biom->getLuaState() == NULL)
        return false;


    return true;
}

void biom_manager::init() {
    p_init = true;

    for( uint32_t i = 0; i < p_biomes.size(); i++) {
        biom *l_type = &p_biomes[i];
        // run once
        script::run( l_type->getLuaState());
        script::function( "Intilisation", l_type->getLuaState());
    }
}

biom *biom_manager::create() {
    biom l_type;
    p_biomes.push_back(l_type);
    return &p_biomes.back();
}

bool biom_manager::remove( biom *target) {
    for( uint32_t i = 0; i < p_biomes.size(); i++) {
        biom *l_type = &p_biomes[i];
        if( l_type == target) {
            p_biomes.erase( p_biomes.begin()+i);
            return true;
        }
    }
    return false;
}

void biom_manager::update() {
    if( !p_init)
        init();
}