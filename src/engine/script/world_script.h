#ifndef WORLD_SCRIPT_H
#define WORLD_SCRIPT_H

#include "../script.h"
#include <stdint.h>

namespace engine {
    namespace script {
        LUALIB_API int world_lib( lua_State *state);
    };
};

#endif