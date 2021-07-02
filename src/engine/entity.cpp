#include "entity.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "log.h"

using json = nlohmann::json;

using namespace engine;

entity::entity() {

}

entity::~entity() {

}

entity_handler::entity_handler() {

}

entity_handler::~entity_handler() {
    p_entity.clear();
}

void entity_handler::loadEntity( std::string file) {
    std::ifstream l_file( file);
    if( l_file.is_open() == false) {
        log( log_level::log_warn, "entity_handler::loadEntity %s konnte nicht geoefnet werden", file.c_str());
        return;
    }

    json l_json;
    l_file >> l_json;

    entity *l_entity = createEntity();

    // Name
    if( !l_json["name"].is_null() &&
        l_json["name"].is_string())
        l_entity->setName( l_json["name"].get<std::string>().c_str());
    else
        l_entity->setName( "NoName");
    log( log_trace, "Name: %s", l_entity->getName());

    // Id
    if( !l_json["id"].is_null() &&
        l_json["id"].is_number_unsigned()) {
        l_entity->setId( l_json["id"].get<uint32_t>());
    } else {
        removeEntity( l_entity);
        return;
    }
    log( log_trace, "Id: %d", l_entity->getId());
    
    //if( !l_json)
}

entity *entity_handler::createEntity() {
    entity l_entity;
    p_entity.push_back(l_entity);
    return &p_entity[ p_entity.size()-1];
}

bool entity_handler::removeEntity( entity *target) {
    for( uint32_t i = 0; i < p_entity.size(); i++) {
        entity *l_entity = &p_entity[i];
        if( l_entity == target) {
            p_entity.erase( p_entity.begin()+i);
            return true;
        }
    }
    return false;
}