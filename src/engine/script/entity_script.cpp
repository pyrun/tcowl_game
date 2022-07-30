#include "entity_script.hpp"

#include <string>

#include <engine/log.hpp>
#include <engine/entity/entity.hpp>

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

    lua_pushnumber( state, l_obj->body->velocity.x);
    lua_pushnumber( state, l_obj->body->velocity.y);
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

    engine::fvec2 l_vel = { (float)lua_tonumber( state, 2), (float)lua_tonumber( state, 3)};

    if( l_vel.x == 0 &&
        l_vel.y == 0)
        return 0;

    l_obj->change = true;
    l_obj->body->velocity = l_obj->body->velocity + l_vel;

    return 0;
}

static int lua_getPosition( lua_State *state) {
    int l_id;
    entity *l_obj;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    lua_pushnumber( state, l_obj->body->position.x);
    lua_pushnumber( state, l_obj->body->position.y);
    return 2;
}

static int lua_setAnimation( lua_State *state) {
    entity *l_obj;
    action *l_action;
    int l_id;
    std::string l_action_name;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;
    
    if( !lua_isstring( state, 2)) {
        log( log_warn, "lua_setAnimation call wrong argument");
        return 0;
    }

    l_action_name = lua_tostring( state, 2);
    
    for( action &action: l_obj->objtype->actions) 
        if( action.name == l_action_name)
            l_action = &action;
    if( l_action->id != l_obj->action) {
        l_obj->change = true;
        l_obj->action = l_action->id;
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

    lua_pushnumber( state, l_obj->input->buttons[ input_buttons::input_buttons_use]);
    return 1;
}

static int lua_setInventoryState( lua_State *state) {
    entity *l_obj;
    std::string l_state_name;
    vec2 l_pos;
    inventory_grid_state l_state = inventory_grid_state_unavailable;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    if( l_obj->inventory == nullptr)
        return 0;
    
    if( !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isstring( state, 4)) {
        log( log_warn, "lua_setInventoryState call wrong argument");
        return 0;
    }

    l_pos.x = lua_tonumber( state, 2);
    l_pos.y = lua_tonumber( state, 3);
    l_state_name = lua_tostring( state, 4);

    // TODO check if inventory_grid_state_unavailable or inventory_grid_state_taken
    if( l_state_name == "available")
        l_state = inventory_grid_state_available;
    if(l_obj->inventory) {
        l_obj->inventory->setState( l_pos, l_state);
    } else {
        lua_pushboolean( state, false);
        return 1;
    }


    // TODO check if happen
    lua_pushboolean( state, true);
    return 1;
}

static int lua_addInventoryItem( lua_State *state) {
    entity *l_obj;
    std::string l_state_name;
    vec2 l_pos;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    if( l_obj->inventory == nullptr)
        return 0;
    
    if( !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isstring( state, 4)) {
        log( log_warn, "lua_addInventoryItem call wrong argument");
        return 0;
    }

    l_pos.x = lua_tonumber( state, 2);
    l_pos.y = lua_tonumber( state, 3);
    l_state_name = lua_tostring( state, 4);

    engine::type *l_type = engine::used_entity_handler->getTypeByName(l_state_name);

    if( l_type == nullptr) {
        log( log_warn, "lua_addInventoryItem type not found %s", l_state_name);
        return 0;
    }

    engine::inventory_entry l_entry;
    l_entry.objtype = l_type;
    l_entry.pos = l_pos;

    engine::inventory_entry *l_item = l_obj->inventory->add( &l_entry);
    
    l_obj->change = true;

    // TODO check if happen
    lua_pushboolean( state, l_item==nullptr?false:true);
    return 1;
}

static int lua_find( lua_State *state) {
    entity *l_obj;
    int l_id;

    l_obj = entity_script_getObject( state);
    if( !l_obj ||
        l_obj->body == nullptr)
        return 0;
    
    if( !lua_isnumber( state, 2) || !lua_isnumber( state, 3) ||
        !lua_isnumber( state, 4) || !lua_isnumber( state, 5)) {
        log( log_warn, "lua_find call wrong argument");
        return 0;
    }

    vec2 l_pos;
    vec2 l_rect;

    l_pos.x = lua_tonumber( state, 2);
    l_pos.y = lua_tonumber( state, 3);
    l_rect.x = lua_tonumber( state, 4);
    l_rect.y = lua_tonumber( state, 5);

    std::vector<engine::entity*> l_objects = engine::used_entity_handler->find( l_obj->body->position.toVec() + l_pos, l_rect);
    
    lua_newtable( state);
    uint32_t i = 1;
    for( entity* const & l_entity: l_objects) {
        lua_pushnumber( state, i++); // push key 
        lua_pushnumber( state, l_entity->index); // value
        lua_settable( state, -3);
    }
    return 1;
}

static int lua_setParameter( lua_State *state) {
    entity *l_obj;
    int l_id;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    if( !lua_isstring( state, 2) || !lua_isnumber( state, 3)) {
        log( log_warn, "lua_setParameter call wrong argument");
        return 0;
    }

    // search for parameter
    bool l_search = false;
    for( entity_parameter &l_parameter:l_obj->parameter) {
        if( l_parameter.name == std::string(lua_tostring( state, 2))) {
            l_search = true;
            l_parameter.value = lua_tonumber( state, 3);
            break;
        }
    }
    // create new parameter
    if( l_search == false) {
        entity_parameter l_parameter;
        l_parameter.name = lua_tostring( state, 2);
        l_parameter.value = lua_tonumber( state, 3);
        l_obj->parameter.push_back( l_parameter);
    }
    return 0;
}

static int lua_getParameter( lua_State *state) {
    entity *l_obj;
    int l_id;

    l_obj = entity_script_getObject( state);
    if( !l_obj)
        return 0;

    if( !lua_isstring( state, 2)) {
        log( log_warn, "lua_getParameter call wrong argument");
        return 0;
    }

    // search for parameter
    bool l_search = false;
    for( entity_parameter &l_parameter:l_obj->parameter) {
        if( l_parameter.name == std::string(lua_tostring( state, 2))) {
            lua_pushnumber( state, l_parameter.value);
            return 1;
        }
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

static const struct luaL_Reg entity_lib_funcs[] = {
    {"isAlive", lua_isAlive},
    {"getVelocity", lua_getVelocity},
    {"setAnimation", lua_setAnimation},
    {"doVelocity", lua_doVelocity},
    {"getPosition", lua_getPosition},
    {"isInputPresent", lua_isInputPresent},
    {"getInputAxies", lua_getInputAxies},
    {"getInputButtons", lua_getInputButtons},
    {"setInventoryState", lua_setInventoryState},
    {"addInventoryItem", lua_addInventoryItem},
    {"find", lua_find},
    {"setParameter", lua_setParameter},
    {"getParameter", lua_getParameter},
    {NULL, NULL}
    };

LUALIB_API int engine::script::entity_lib( lua_State *L) {
    lua_getglobal(L, "tcowl");
    luaL_newlib( L, entity_lib_funcs);
    lua_setfield(L, -2, "entity");

    return 1;
}