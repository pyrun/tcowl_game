#ifndef ENGINE_JSON_HPP
#define ENGINE_JSON_HPP

#include <nlohmann/json.hpp>
#include <engine/vec.hpp>

namespace engine {
    class json {
        public:
            json();
            ~json();

            void load( std::string name);
            void save( std::string name);

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
            bool p_change;
    };
};

#endif