#ifndef ENGINE_SCRIPT_H
#define ENGINE_SCRIPT_H

#include <string>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
};


namespace engine {
    namespace script {
        void run( lua_State *state);

        void function( std::string function, lua_State *state, int16_t id);

        void free( lua_State *state);
        lua_State *loadFile( const char *file);
        void print_error( lua_State *stack);
    };
};

#endif