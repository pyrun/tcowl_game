#include "json.h"

#include <fstream>

using namespace engine;

json::json() {

}

json::~json() {
    
}

void json::load( std::string name) {
    std::ifstream l_ifs(name);
    p_json = nlohmann::json::parse(l_ifs);
    l_ifs.close();
}

void json::save( std::string name) {
    std::ofstream l_file(name);
    l_file << p_json.dump(4) << std::endl;
    l_file.close();
}

vec2 json::getVec2( std::string name, vec2 def) {
    vec2 l_value;
    nlohmann::json l_array = p_json[name];
    if( l_array.is_array() &&
        l_array.size() >= 2) {
        l_value.x = l_array[0].get<int32_t>();
        l_value.y = l_array[1].get<int32_t>();
    } else { // Datensatz ist falsch
        l_value = def;
    }
    return l_value;
}