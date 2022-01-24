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

            // special types like arrays and vectors
            vec2 getVec2( std::string name, vec2 def = vec2{ 0, 0});

            // normal types are handleable with it
            template< class T>
            T get( std::string name, T defaultValue) {
                try {
                    nlohmann::json l_obj = p_json.at(name);
                    if( !l_obj.is_null())
                        return l_obj.get<T>();
                } catch (std::exception& e) {
                    return defaultValue;
                }
                return defaultValue;
            }

            nlohmann::json *getJson() { return &p_json; }
        private:
            nlohmann::json p_json;
    };
};

#endif