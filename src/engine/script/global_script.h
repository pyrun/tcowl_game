
#ifndef GLOBAL_SCRIPT_H
#define GLOBAL_SCRIPT_H

#include "../script.h"
#include <stdint.h>

namespace engine {
    namespace script {
        void global_lib( lua_State *state);
    };
};

#endif