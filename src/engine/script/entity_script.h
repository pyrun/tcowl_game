#ifndef ENTITY_SCRIPT_H
#define ENTITY_SCRIPT_H

#include "../script.h"
#include <stdint.h>

namespace engine {
    namespace script {
        void update( lua_State *state, int16_t id);
        void entity_lib( lua_State *state);
    };
};

#endif