#ifndef WORLD_SCRIPT_HPP
#define WORLD_SCRIPT_HPP

#include <stdint.h>
#include <engine/script.hpp>

namespace engine {
    namespace script {
        LUALIB_API int world_lib( lua_State *state);
    };
};

#endif