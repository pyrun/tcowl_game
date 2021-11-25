#ifndef HELPER_JSON_H
#define HELPER_JSON_H

#include "vec.h"

#include <nlohmann/json.hpp>

namespace helper {
    namespace json {
        uint32_t getBool( nlohmann::json *json_ptr, std::string name, bool def);
        uint32_t getUint32( nlohmann::json *json_ptr, std::string name, uint32_t def = 0);
        
        template< class T>
        T* getNumberArrayN( nlohmann::json *json_ptr, std::string name, uint32_t length = 1) {
            try {
                nlohmann::json l_array = json_ptr->at(name);
                if( l_array.is_array() &&
                    l_array.size() >= length) {
                    T *l_values = new T[length];
                    for( uint32_t i = 0; i < length; i++)
                        l_values[i] = l_array[i].get<T>();
                    return l_values;
                }
            } catch (std::exception& e) {
                return NULL;
            }
            return NULL;
        }
        
        template< class T>
        T* getNumberArray( nlohmann::json *json_ptr, std::string name, uint32_t maxlength = 1) {
            try {
                nlohmann::json l_array = json_ptr->at(name);
                if( l_array.is_array()) {
                    T *l_values = new T[maxlength];
                    for( uint32_t i = 0; i < l_array.size(); i++) {
                        if( i == maxlength)
                            return l_values;
                        l_values[i] = l_array[i].get<T>();
                    }
                    return l_values;
                }
            } catch (std::exception& e) {
                return NULL;
            }
            return NULL;
        }

        float getFloat( nlohmann::json *json_ptr, std::string name, float def = 0.0f);
        engine::vec2 getVec2( nlohmann::json *json_ptr, std::string name, engine::vec2 def = engine::vec2{ 0, 0});
        std::string getString( nlohmann::json *json_ptr, std::string name, std::string def = "default");
    }
}

#endif