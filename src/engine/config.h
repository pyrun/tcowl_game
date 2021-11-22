#ifndef CONFIG_H
#define CONFIG_H

#include <nlohmann/json.hpp>
#include "vec.h"

#define CONFIG_FILE "config.json"

namespace engine {
    class config {
        public:
            config();
            ~config();

            void load();
            void save();

            
            vec2 getVec2( std::string name, vec2 def = vec2{ 0, 0});
            std::string getString( std::string name, std::string def = "NoTitleSet");


            template< class T>
            T get( std::string name) {
                nlohmann::json l_array = p_json[name];
                return l_array.get<T>();
            }

            nlohmann::json *getJson() { return &p_json; }
        private:
            nlohmann::json p_json;
    };
};

#endif