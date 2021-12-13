#include "helper_json.h"

using namespace engine;

uint32_t helper::json::getBool( nlohmann::json *json_ptr, std::string name, bool def) {
    try {
        nlohmann::json l_array = json_ptr->at(name);
        if( l_array.is_boolean()) {
            return l_array.get<bool>();
        }
    } catch (std::exception& e) {
        return def;
    }
    return def;
}

uint32_t helper::json::getUint32( nlohmann::json *json_ptr, std::string name, uint32_t def) {
    try {
        nlohmann::json l_array = json_ptr->at(name);
        if( l_array.is_number_unsigned()) {
            return l_array.get<uint32_t>();
        }
    } catch (std::exception& e) {
        return def;
    }
    return def;
}

float helper::json::getFloat( nlohmann::json *json_ptr, std::string name, float def) {
    try {
        nlohmann::json l_array = json_ptr->at(name);
        if( l_array.is_number()) {
            return l_array.get<float>();
        }
    } catch (std::exception& e) {
        return def;
    }
    return def;
}

engine::vec2 helper::json::getVec2( nlohmann::json *json_ptr, std::string name, engine::vec2 def) {
    engine::vec2 l_value;
    try {
        nlohmann::json l_array = json_ptr->at(name);
        if( l_array.is_array() &&
            l_array.size() >= 2) {
            l_value.x = l_array[0].get<int32_t>();
            l_value.y = l_array[1].get<int32_t>();
        } else { // Datensatz ist falsch
            l_value = def;
        }
    } catch (std::exception& e) {
        l_value = def;
    }
    
    return l_value;
}

std::string helper::json::getString( nlohmann::json *json_ptr, std::string name, std::string def) {
    try {
        nlohmann::json l_array = json_ptr->at(name);
        if( l_array.is_string()) {
            return l_array.get<std::string>();
        } else {
            return def;
        }
    } catch (std::exception& e) {
        return def;
    }
    return def;
}