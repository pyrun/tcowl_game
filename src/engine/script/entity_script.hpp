#ifndef ENTITY_SCRIPT_HPP
#define ENTITY_SCRIPT_HPP

#include <stdint.h>
#include <engine/script.hpp>

namespace engine {
    namespace script {
        LUALIB_API int entity_lib( lua_State *state);
    };
};

#endif