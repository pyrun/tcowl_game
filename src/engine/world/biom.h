#ifndef BIOM_H
#define BIOM_H

#include <string>
#include "../script.h"

namespace engine {
    class biom {
        public:
            biom();
            ~biom();

            void setId(uint16_t id) { p_id = id; }
            uint16_t getId() { return p_id; }

            void setName( std::string name) { p_name = name; }
            std::string getName() { return p_name; }

            void setScriptName( std::string name) { p_script_file = name; }
            std::string getScriptName() { return p_script_file; }
            void setLuaState( lua_State *state) { p_lua = state; }
            lua_State *getLuaState() { return p_lua; }

            void update();
        private:
            uint16_t p_id;
            std::string p_name;

            std::string p_script_file;
            lua_State *p_lua;
    };
};

#endif