#include "type.hpp"

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <nlohmann/json.hpp>
#include <engine/helper.hpp>
#include <engine/helper_json.hpp>
#include <engine/log.hpp>

using json = nlohmann::json;

using namespace engine;

action *engine::type::findAction( std::string name) {
    for( action &l_action:actions)
        if( l_action.name == name)
            return &l_action;
    return nullptr;
}

type_handler::type_handler() {
    p_type.clear();
    p_highest_id = 1024; // for automatic numbering
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
    l_type->name = helper::json::getString( &l_json, "name", "noName");

    // Id
    uint32_t l_id;
    l_type->id = helper::json::getUint32( &l_json, "id", p_highest_id);
    if( l_type->id >= p_highest_id)
        p_highest_id = l_type->id+1;

    // Type
    std::string l_object_type = helper::json::getString( &l_json, "type", "object");
    if( l_object_type == "object")
        l_type->object_type = engine::type_objecttype::type_objecttype_object;
    else if( l_object_type == "item")
        l_type->object_type = engine::type_objecttype::type_objecttype_item;

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
        l_type->image.setAlphaKey( l_color_key[0], l_color_key[1], l_color_key[2]);
        delete l_color_key;
    }

    // depth_sorting_offset
    l_type->depth_sorting_offset = helper::json::getVec2( &l_json, "depth-sorting-offset");

    // Load image
    l_type->image.load( graphic, folder + l_image_file);

    // Set Folder path
    l_type->src_path = folder;

    // Physic data
    float *l_collision_data = helper::json::getNumberArray<float>( &l_json, "collision-data", 8);

    // Physic type/shape
    physic::shape *l_shape = nullptr;
    std::string l_collision_type = helper::json::getString( &l_json, "collision-type", "");
    if( !l_collision_data)
        l_collision_type.clear();
    if( l_collision_type == "circle") { // circle
        l_shape = new physic::shape_circle( l_collision_data[2]);
    } else if(l_collision_type == "rect") { // rect
        l_shape = new physic::shape_rect( { l_collision_data[2], l_collision_data[3]} );
    } else if(l_collision_type == "line") { // line
        l_shape = new physic::shape_line( { l_collision_data[2], l_collision_data[3]} );
    }
    if( l_shape)
        l_shape->setOffset( { l_collision_data[0], l_collision_data[1]});
    l_type->shape = l_shape;

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
            l_action.delay = helper::json::getUint32( &l_action_json, "delay", 1);
            l_action.bind_velocity = helper::json::getBool( &l_action_json, "bind-velocity", false);
            l_action.flip_vertical = helper::json::getBool( &l_action_json, "flip-verticaly", false);
            l_action.flip_horizontal = helper::json::getBool( &l_action_json, "flip-horizontal", false);

            log( log_level::log_debug, "action '%s' %dx%d %dx%d length %d durration %d",
                l_action.name.c_str(),
                l_action.postion.x,
                l_action.postion.y,
                l_action.size.x,
                l_action.size.y,
                l_action.length,
                l_action.delay);
            
            l_type->actions.push_back( l_action);
        }
    }

    // item-hitbox
    if( l_json["item-hitbox"].is_array()) {
        json l_json_root_item_hitbox = l_json["item-hitbox"];
        for( uint32_t i = 0; i < l_json["item-hitbox"].size(); i++) {
            json l_item_hitbox_json = l_json_root_item_hitbox[i];
            if( l_item_hitbox_json.is_array() && l_item_hitbox_json.size() == 2) {
                log( log_level::log_debug, "item-hitbox %d %d", l_item_hitbox_json[0].get<int32_t>(), l_item_hitbox_json[1].get<int32_t>());
                l_type->item_vertex.push_back( vec2{ l_item_hitbox_json[0].get<int32_t>(), l_item_hitbox_json[1].get<int32_t>()});
            }
        }
    } else if( l_type->object_type == engine::type_objecttype_item) {
        l_type->item_vertex.push_back( vec2{ 0, 0});
    }

    // inventory-size
    if( l_json["inventory-size"].is_array()) {
        json l_json_root_inventory_size = l_json["inventory-size"];
        if( l_json_root_inventory_size.size() == 2) {
            vec2 l_size = vec2{ l_json_root_inventory_size[0].get<int32_t>(), l_json_root_inventory_size[1].get<int32_t>()};
            log( log_level::log_debug, "inventory-size %d %d", l_size.x, l_size.y);
            l_type->inventory_size = l_size;
        }
    }

    log( log_level::log_info, "Entity id:%d name:%s action:%d type:%s loaded", l_type->id, l_type->name.c_str(), l_type->actions.size(), l_object_type.c_str());
}

void type_handler::reload( graphic_draw *graphic) {
    for( type &l_type: p_type)
        l_type.image.reload( graphic);
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
            if( l_type->shape) // free shape
                delete l_type->shape;
            p_type.erase( p_type.begin()+i);
            return true;
        }
    }
    return false;
}

type *type_handler::getById( uint16_t id) {
    for( type &l_type: p_type)
        if( l_type.id == id)
            return &l_type;
    return NULL;
}

type *type_handler::getByName( std::string name) {
    for( type &l_type: p_type)
        if( l_type.name == name)
            return &l_type;
    return NULL;
}