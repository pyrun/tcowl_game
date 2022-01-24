#include "config.h"

#include <fstream>

using json = nlohmann::json;
using namespace engine;

config::config() {

}

config::~config() {
    
}

void config::load() {
    std::ifstream l_ifs(CONFIG_FILE);
    p_json = json::parse(l_ifs);
    l_ifs.close();
}

void config::save() {
    std::ofstream l_file(CONFIG_FILE);
    l_file << p_json.dump(4) << std::endl;
    l_file.close();
}

vec2 config::getVec2( std::string name, vec2 def) {
    vec2 l_value;
    json l_array = p_json[name];
    if( l_array.is_array() &&
        l_array.size() >= 2) {
        l_value.x = l_array[0].get<int32_t>();
        l_value.y = l_array[1].get<int32_t>();
    } else { // Datensatz ist falsch
        l_value = def;
    }
    return l_value;
}