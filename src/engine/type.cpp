#include "type.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <dirent.h>

#include "helper.h"
#include "helper_json.h"
#include "log.h"

using json = nlohmann::json;

using namespace engine;

type::type() {
    p_shape = nullptr;
}

void type::cleanup() {
    if( p_shape)
        delete p_shape;
}

action *type::getAction( std::string name) {
    for( std::size_t i = 0; i < p_action.size(); i++) {
        action *l_action = &p_action[i];
        if( l_action->name == name)
            return l_action;
    }
    return NULL;
}

type_handler::type_handler() {
    p_type.clear();
}

type_handler::~type_handler() { }

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

    // Load file
    json l_json;
    l_file >> l_json;

    type *l_type = createtype();

    // Name
    l_type->setName( helper::json::getString( &l_json, "name", "noName").c_str());

    // Id
    uint32_t l_id;
    l_id = helper::json::getUint32( &l_json, "id", 0);
    if( l_id == 0) { // keine Nummer vergeben
        removetype( l_type);
        return;
    }
    l_type->setId( l_id);

    // Graphic
    std::string l_image_file;
    l_image_file = helper::json::getString( &l_json, "image", "error");
    if( l_image_file == "error") {// Datei nicht angegeben
        removetype( l_type);
        return;
    }

    // Alpha key
    uint8_t *l_color_key;
    l_color_key = helper::json::getNumberArrayN<uint8_t>( &l_json, "alpha-key", 3);
    if( l_color_key) {
        l_type->getImage()->setAlphaKey( l_color_key[0], l_color_key[1], l_color_key[2]);
        delete l_color_key;
    }

    // depth_sorting_offset
    l_type->setDepthSortingOffset( helper::json::getVec2( &l_json, "depth_sorting_offset"));

    // Load image
    l_type->getImage()->load( graphic, folder + l_image_file);

    // Set Folder path
    l_type->setFolderPath( folder);

    // Physic data
    float *l_collision_data = helper::json::getNumberArray<float>( &l_json, "collision-data", 8);

    // Physic type/shape
    physic::shape *l_shape = nullptr;
    std::string l_collision_type = helper::json::getString( &l_json, "collision-type", "");
    if( !l_collision_data)
        l_collision_type.clear();
    if( l_collision_type == "circle") { // circle
        l_shape = new physic::sharp_circle( l_collision_data[2]);
    } else if(l_collision_type == "rect") { // rect
        l_shape = new physic::shape_rect( { l_collision_data[2], l_collision_data[3]} );
    } else if(l_collision_type == "line") { // line
        l_shape = new physic::shape_line( { l_collision_data[2], l_collision_data[3]} );
    }
    if( l_shape)
        l_shape->setOffset( { l_collision_data[0], l_collision_data[1]});
    l_type->linkShape( l_shape);

    // Alle Aktionen laden und hinzufügen
    if( l_json["action"].is_array()) {
        json l_json_root_action = l_json["action"];
        for( uint32_t i = 0; i < l_json["action"].size(); i++) {
            json l_action_json = l_json_root_action[i];
            action l_action;
            l_action.id = i;

            l_action.name = helper::json::getString( &l_action_json, "name", "noName");
            l_action.postion = helper::json::getVec2( &l_action_json, "position");
            l_action.size = helper::json::getVec2( &l_action_json, "size", vec2{ 32, 32});
            l_action.length = helper::json::getUint32( &l_action_json, "length", 1);
            l_action.ticks_for_next_image = helper::json::getUint32( &l_action_json, "ticks_for_next_image", 1);
            l_action.bind_velocity = helper::json::getBool( &l_action_json, "bind_velocity", false);

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

    log( log_level::log_info, "Entity id:%d name:%s action:%d loaded", l_type->getId(), l_type->getName(), l_type->getAmountActions());
}

void type_handler::reload( graphic_draw *graphic) {
    for( uint32_t i = 0; i < p_type.size(); i++) {
        type *l_type = &p_type[i];
        // reload image
        l_type->getImage()->reload( graphic);
    }
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

type *type_handler::getById( uint16_t id) {
    for( uint32_t i = 0; i < p_type.size(); i++)
        if( p_type[i].getId() == id)
            return &p_type[i];
    return NULL;
}