#include "entity.h"

#include "string.h"
#include "log.h"
#include "timer.h"

using namespace engine;

entity_handler::entity_handler() {
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++)
        p_entity[i] = NULL;
    p_types = NULL;
}

entity_handler::~entity_handler() {

}

void entity_handler::init( type_handler *types) {
    p_types = types;
}

int32_t entity_handler::createObject( std::string name) {
    if( p_types == NULL)
        return -1;
    for( uint32_t i = 0; i < p_types->getAmount(); i++)
        if( strcmp( p_types->get(i)->getName(), name.c_str()) == 0)
            return createObject( p_types->get(i));
    return -1;
}
int32_t entity_handler::createObject( uint32_t id) {
    if( p_types == NULL)
        return -1;
    type *l_type = p_types->getById( id);
    if( l_type == NULL)
        return -1;
    return createObject( l_type);
}
int32_t entity_handler::createObject( type *objtype) {
    if( p_types == NULL)
        return -1;
    if( objtype == NULL)
        return -1;
    
    entity *l_entity = NULL;
    bool l_found = false;
    uint32_t l_index = 0;
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        if( p_entity[i] == NULL) {
            l_found = true;
            l_index = i;
            break;
        }
    }
    if( l_found == false) {
        log( log_error, "entity_handler::createObject no space for new entity");
        return -1;
    }

    l_entity = p_entity[l_index] = new entity;
    l_entity->id = 0;

    l_entity->objtype = objtype;
    l_entity->objtypeid = objtype->getId();

    l_entity->position = { 0, 0};
    l_entity->velocity = { 0, 0};

    log( log_trace, "entity_handler::createObject created");
    
    return l_index;
}

void entity_handler::draw( engine::graphic_draw *graphic) {
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        if( p_entity[i] != NULL) {
            drawEntity( graphic, p_entity[i]);
        }
    }
}

void entity_handler::drawEntity( engine::graphic_draw *graphic, entity* obj) {
    action *l_action = obj->objtype->getAction(0);

    if( helper::time::check( &obj->animation_time, l_action->ticks_for_next_image)) {
        helper::time::reset( &obj->animation_time);
        obj->animation_tick++;
    }

    graphic->draw(  obj->objtype->getImage(),
                    obj->position,
                    l_action->size,
                    l_action->postion + vec2{ (int32_t)(obj->animation_tick%l_action->length) * l_action->size.x, 0});
}