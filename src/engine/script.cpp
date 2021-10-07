#include "script.h"

#include "helper.h"
#include "log.h"
#include "type.h"

#include "script/entity_script.h"

void engine::script::run( lua_State *state) {
    int32_t l_return;
    if( state == NULL)
        return;
    
    l_return = lua_pcall( state, 0, 0, 0);
    if( l_return != 0) {
        log( log_error, "engine::script::run LUA_ERROR: %s", lua_tostring( state, -1));
    }
}

void engine::script::function( std::string function, lua_State *state, int16_t number) {
    if( state == NULL)
        return;

    // name the function
    lua_getglobal( state, function.c_str());
    if( !lua_isfunction( state, -1)) {
        lua_pop( state,1);
        return;
    }
    lua_pushnumber( state, number);

    // call the function
    if( lua_pcall( state, 1, 0, 0)) {
        log( log_error, "engine::script::function %s", lua_tostring( state, -1));
    }
}

void engine::script::free( lua_State *state) {
    if( state != NULL) {
        lua_close( state);
        state = NULL;
    }
}

lua_State *engine::script::loadFile( const char *file) {
    lua_State *l_state = NULL;

    // check if file exist
    if( helper::file_exists( file) == false) {
        log( log_warn, "engine::lua::loadFile file '%s' dont found", ENGINE_TYPE_FILE_SCRIPT);
        return l_state;
    }

    l_state = luaL_newstate();
    if( l_state == NULL) {
        log( log_warn, "engine::lua::loadFile cannot create new lua state" );
        return l_state;
    }

    // standard libraries
    luaL_openlibs( l_state);

    // add libs
    script::entity_lib( l_state);

    // load the file and call its once
    uint32_t l_result = luaL_loadfile( l_state, file);
    if( l_result != LUA_OK ) {
        // print error
        print_error( l_state);
        script::free( l_state);
        return l_state;
    }
    return l_state;
}

void engine::script::print_error( lua_State *stack) {
    // The error message is on top of the stack.
    // Fetch it, print it and then pop it off the stack.
    const char* l_message = lua_tostring( stack, -1);
    
    //puts(message);

    lua_pop( stack, 1);
}