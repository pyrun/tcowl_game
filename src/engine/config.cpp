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