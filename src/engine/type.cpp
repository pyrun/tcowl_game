#include "type.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <dirent.h>

#include "helper.h"
#include "log.h"

using json = nlohmann::json;

using namespace engine;

type::type() {

}

type::~type() {

}

type_handler::type_handler() {
    p_type.clear();
}

type_handler::~type_handler() {

}

bool type_handler::loadFolder(  graphic *graphic, std::string path) {
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
            if( helper::file_exists( (l_path + "/" + ENGINE_TYPE_FILE_DESCRIPTION).c_str()))
                loadtype( graphic, l_path + "/");
            loadFolder( graphic, l_path);
        }
    }
    closedir(l_dir);

    return true;
}

void type_handler::loadtype( graphic *graphic, std::string folder) {
    std::ifstream l_file( folder + ENGINE_TYPE_FILE_DESCRIPTION);
    if( l_file.is_open() == false) {
        log( log_level::log_warn, "type_handler::loadtype %s konnte nicht geoefnet werden", (folder + ENGINE_TYPE_FILE_DESCRIPTION).c_str());
        return;
    }

    json l_json;
    l_file >> l_json;

    type *l_type = createtype();

    // Name
    if( !l_json["name"].is_null() &&
        l_json["name"].is_string())
        l_type->setName( l_json["name"].get<std::string>().c_str());
    else
        l_type->setName( "NoName");
    log( log_trace, "Name: %s", l_type->getName());

    // Id
    if( !l_json["id"].is_null() &&
        l_json["id"].is_number_unsigned()) {
        l_type->setId( l_json["id"].get<uint32_t>());
    } else {
        removetype( l_type);
        return;
    }
    log( log_trace, "Id: %d", l_type->getId());

    // Graphic
    std::string l_image_file;
    if( !l_json["image"].is_null() &&
        l_json["image"].is_string()) {
         l_image_file = l_json["image"].get<std::string>();
    } else {
        removetype( l_type);
        return;
    }

    // Alpha key
    if( !l_json["alpha-key"].is_null() &&
        l_json["alpha-key"].is_array() &&
        l_json["alpha-key"].size() >= 3) {
        json l_json_root_color = l_json["alpha-key"];
        l_type->getImage()->setAlphaKey(    l_json_root_color[0].get<uint8_t>(),
                                            l_json_root_color[1].get<uint8_t>(),
                                            l_json_root_color[2].get<uint8_t>());
    }

    // Load image
    log( log_trace, "image: %s", (folder + l_image_file).c_str());
    l_type->getImage()->load( graphic, folder + l_image_file);


    // Alle Aktionen laden und hinzufügen
    if( !l_json["action"].is_null() &&
        l_json["action"].is_array()) {
        
        json l_json_root_action = l_json["action"];
        for( uint32_t i = 0; i < l_json["action"].size(); i++) {
            json l_action_json = l_json_root_action[i];
            action l_action;

            if( !l_action_json["id"].is_null()) {
                l_action.id  = l_action_json["id"].get<uint32_t>();
            } else {
                log( log_warn, "error loading action index %d, id", i);
                continue;
            }

            if( !l_action_json["name"].is_null())
                l_action.name = l_action_json["name"].get<std::string>();
            else
                l_action.name = "NoName";

            if( !l_action_json["position"].is_null()) {
                l_action.postion = { l_action_json["position"][0].get<int32_t>(), l_action_json["position"][1].get<int32_t>() };
            } else {
                log( log_warn, "error loading action index %d, position", i);
                continue;
            }

            if( !l_action_json["size"].is_null()) {
                l_action.size = { l_action_json["size"][0].get<int32_t>(), l_action_json["size"][1].get<int32_t>() };
            } else {
                log( log_warn, "error loading action index %d, size", i);
                continue;
            }
            
            if( !l_action_json["length"].is_null())
                l_action.length  = l_action_json["length"].get<uint32_t>();
            else
                l_action.length = 1;
            
            if( !l_action_json["ticks_for_next_image"].is_null())
                l_action.ticks_for_next_image  = l_action_json["ticks_for_next_image"].get<uint32_t>();
            else
                l_action.ticks_for_next_image = 1;
            log( log_level::log_debug, "action '%s' %dx%d %dx%d length %d durration %d",
                                                                                        l_action.name.c_str(),
                                                                                        l_action.postion.x,
                                                                                        l_action.postion.y,
                                                                                        l_action.size.x,
                                                                                        l_action.size.y,
                                                                                        l_action.length,
                                                                                        l_action.ticks_for_next_image);

            l_type->addAction( l_action);
        }
    }

    log( log_level::log_info, "Enitity %s with %d Action loaded", l_type->getName(), l_type->getAmountActions());
}

type *type_handler::createtype() {
    type l_type;
    p_type.push_back(l_type);
    return &p_type.back();
}

bool type_handler::removetype( type *target) {
    for( uint32_t i = 0; i < p_type.size(); i++) {
        type *l_type = &p_type[i];
        if( l_type == target) {
            p_type.erase( p_type.begin()+i);
            return true;
        }
    }
    return false;
}

type *type_handler::getById( uint32_t id) {
    for( uint32_t i = 0; i < p_type.size(); i++)
        if( p_type[i].getId() == id)
            return &p_type[i];
    return NULL;
}