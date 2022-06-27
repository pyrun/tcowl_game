#include "entity_script.h"

#include <string>
#include "../log.h"
#include "../entity.h"

using namespace engine;

#ifdef __cplusplus
extern "C"{
#endif

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
    int l_id;
    entity *l_obj;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    lua_pushnumber( state, l_obj->body->getVelocity().x);
    lua_pushnumber( state, l_obj->body->getVelocity().y);
    return 2;
}

static int lua_doVelocity( lua_State *state) {
    int l_id;
    entity *l_obj;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    if( !lua_isnumber( state, 2) || !lua_isnumber( state, 3)) {
        log( log_warn, "lua_doVelocity call wrong argument");
        return 0;
    }

    l_obj->body->addVelocity( { (float)lua_tonumber( state, 2), (float)lua_tonumber( state, 3)});

    return 0;
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
    if( l_action_obj) {
        l_obj->action = l_action_obj->id;
        if( lua_isnumber( state, 3))
            l_obj->animation_tick = lua_tonumber( state, 3);
    }

    return 0;
}

static int lua_isInputPresent( lua_State *state) {
    entity *l_obj;
    int l_id;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    lua_pushboolean( state, l_obj->input!=NULL?true:false);
    return 1;
}

static int lua_getInputAxies( lua_State *state) {
    entity *l_obj;
    int l_id;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    if( !l_obj->input)
        return 0;

    lua_pushnumber( state, l_obj->input->axies.x);
    lua_pushnumber( state, l_obj->input->axies.y);
    return 2;
}

static int lua_getInputButtons( lua_State *state) {
    entity *l_obj;
    int l_id;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    if( !l_obj->input)
        return 0;

    lua_pushnumber( state, l_obj->input->buttons[ input_buttons::input_buttons_attack]);
    return 1;
}

#ifdef __cplusplus
}
#endif

static const struct luaL_Reg entity_lib_funcs[] = {
    {"isAlive", lua_isAlive},
    {"getVelocity", lua_getVelocity},
    {"setAnimation", lua_setAnimation},
    {"doVelocity", lua_doVelocity},
    {"isInputPresent", lua_isInputPresent},
    {"getInputAxies", lua_getInputAxies},
    {"getInputButtons", lua_getInputButtons},
    {NULL, NULL}
    };

LUALIB_API int engine::script::entity_lib( lua_State *L) {
    lua_getglobal(L, "tcowl");
    luaL_newlib( L, entity_lib_funcs);
    lua_setfield(L, -2, "entity");

    return 1;
}