#ifndef ENTITY_SCRIPT_H
#define ENTITY_SCRIPT_H

#include "../script.h"
#include <stdint.h>

namespace engine {
    namespace script {
        LUALIB_API int entity_lib( lua_State *state);
    };
};

#endif