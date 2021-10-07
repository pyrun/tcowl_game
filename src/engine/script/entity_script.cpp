#include "entity_script.h"

#include <string>
#include "../log.h"
#include "../entity.h"

using namespace engine;

#ifdef __cplusplus
extern "C"{
#endif

static int lua_print(lua_State* state) {
    int l_nargs = lua_gettop( state);
    std::string l_text;

    for (int i = 1; i <= l_nargs; i++) {
        if( lua_isstring( state, i)) {
            l_text += lua_tostring( state, i);
        }
    }

    log( log_trace, "lua_print '%s'", l_text.c_str());

    // finish
    return 0;
}

entity *entity_script_getObject( lua_State *state) {
    entity *l_obj = NULL;
    int l_id;

    if( !lua_isnumber( state, 1)) {
        log( log_warn, "entity_script_getObject call wrong argument");
        return l_obj;
    }
    if( engine::used_entity_handler == NULL) {
        log( log_warn, "entity_script_getObject entity_handler is NULL");
        return l_obj;
    }

    l_id = lua_tointeger( state, 1);
    l_obj = engine::used_entity_handler->get( l_id);
    return l_obj;
}

static int lua_isAlive( lua_State *state) {
    entity *l_obj;
    int l_id;
    bool l_alive = true;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;
    // TODO Death festlegen
    lua_pushboolean( state, l_alive);
    return 1;
}

static int lua_getVelocity( lua_State *state) {
    entity *l_obj;
    int l_id;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;
    
    lua_pushnumber( state, l_obj->velocity.x);
    lua_pushnumber( state, l_obj->velocity.y);

    return 2;
}

static int lua_setAnimation( lua_State *state) {
    entity *l_obj;
    action *l_action_obj;
    int l_id;
    std::string l_action;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;
    
    if( !lua_isstring( state, 2)) {
        log( log_warn, "lua_setAnimation call wrong argument");
        return 0;
    }

    l_action = lua_tostring( state, 2);

    l_action_obj = l_obj->objtype->getAction( l_action);
    if( l_action_obj)
        l_obj->action = l_action_obj->id;

    return 0;
}

#ifdef __cplusplus
}
#endif

void engine::script::entity_lib( lua_State *L) {
    lua_pushcfunction(L, lua_print);
    lua_setglobal(L, "print");

    lua_pushcfunction(L, lua_isAlive);
    lua_setglobal(L, "isAlive");
    
    lua_pushcfunction(L, lua_getVelocity);
    lua_setglobal(L, "getVelocity");

    lua_pushcfunction(L, lua_setAnimation);
    lua_setglobal(L, "setAnimation");
}