#include "entity.h"

#include "string.h"
#include "log.h"
#include "timer.h"
#include "helper.h"
#include <algorithm>

using namespace engine;

entity_handler::entity_handler() {
    p_draw_order.reserve( ENGINE_ENTITY_MAX_AMOUNT);
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        p_entity[i] = NULL;
    }
    p_types = NULL;
    p_amount = 0;
}

entity_handler::~entity_handler() {

}

void entity_handler::init( type_handler *types) {
    p_types = types;
}

int16_t entity_handler::createObject( std::string name) {
    if( p_types == NULL)
        return -1;
    for( uint32_t i = 0; i < p_types->getAmount(); i++)
        if( strcmp( p_types->get(i)->getName(), name.c_str()) == 0)
            return createObject( p_types->get(i));
    return -1;
}

int16_t entity_handler::createObject( uint16_t objid) {
    if( p_types == NULL)
        return -1;
    type *l_type = p_types->getById( objid);
    if( l_type == NULL)
        return -1;
    return createObject( l_type);
}

int16_t entity_handler::createObject( type *objtype, int32_t index) {
    if( p_types == NULL)
        return -1;
    if( objtype == NULL)
        return -1;
    
    entity *l_entity = NULL;
    bool l_found = false;
    uint16_t l_index = 0;

    if( index < 0) {
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
    } else {
        l_index = (uint32_t)index;
    }

    l_entity = p_entity[l_index] = new entity;
    p_draw_order.push_back( l_entity);
    l_entity->index = l_index;

    l_entity->objtype = objtype;
    l_entity->objtypeid = objtype->getId();

    l_entity->position = { 0, 0};
    l_entity->velocity = { 0, 0};
    l_entity->action = 0;

    l_entity->change = true;
    
    log( log_trace, "entity_handler::createObject created");

    p_amount++;
    
    return l_index;
}
bool entity_handler::deleteObject( uint32_t index) {
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        if( p_entity[i] != NULL && p_entity[i]->index == index) {

            for( uint32_t n = 0; n < p_draw_order.size(); n++) {
                if( p_draw_order[n] == p_entity[i]) {
                    p_draw_order.erase( p_draw_order.begin()+n);
                    break;
                }
            }
            delete p_entity[i];
            p_entity[i] = NULL;
            p_amount--;
            return true;
        }
    }
    return false;
}

uint32_t entity_handler::outNetworkData( entity *obj, uint8_t *dataDist) {
    uint32_t l_offset = 0;

    helper::int16toUint8x2( obj->index, dataDist + l_offset); l_offset +=2;
    helper::int16toUint8x2( obj->objtypeid, dataDist + l_offset); l_offset +=2;

    helper::int32toUint8x4( obj->position.x, dataDist + l_offset); l_offset +=4;
    helper::int32toUint8x4( obj->position.y, dataDist + l_offset); l_offset +=4;

    helper::int32toUint8x4( obj->velocity.x, dataDist + l_offset); l_offset +=4;
    helper::int32toUint8x4( obj->velocity.y, dataDist + l_offset); l_offset +=4;

    return l_offset;
}

void entity_handler::inNetworkData( uint8_t *dataDist) {
    uint32_t l_offset = 0;
    int16_t l_index;
    int16_t l_type_id;

    helper::uint8x2toInt16( dataDist + l_offset, &l_index); l_offset +=2;
    helper::uint8x2toInt16( dataDist + l_offset, &l_type_id); l_offset +=2;
    
    if( p_types->getById( l_type_id) == NULL)
        return;
    entity *l_entity = get(l_index);
    if( l_entity == NULL) {
        createObject( p_types->getById( l_type_id), l_index);
        l_entity = get( l_index);
    }
    l_entity->index = l_index;
    l_entity->objtype = p_types->getById( l_type_id);
    l_entity->objtypeid = l_type_id;

    helper::uint8x4toInt32( dataDist + l_offset, &l_entity->position.x); l_offset +=4;
    helper::uint8x4toInt32( dataDist + l_offset, &l_entity->position.y); l_offset +=4;

    helper::uint8x4toInt32( dataDist + l_offset, &l_entity->velocity.x); l_offset +=4;
    helper::uint8x4toInt32( dataDist + l_offset, &l_entity->velocity.y); l_offset +=4;
}

void entity_handler::draw( engine::graphic_draw *graphic) {
    struct {
        bool operator()( entity *a, entity *b) const {
            return a->position.y+a->objtype->getAction(a->action)->size.y < b->position.y+b->objtype->getAction(b->action)->size.y;
        }
    } depthSorting;

    std::sort( p_draw_order.begin(), p_draw_order.end(), depthSorting);

    for( uint32_t i = 0; i < p_draw_order.size(); i++) {
        entity *l_entity = p_draw_order[i];
        drawEntity( graphic, l_entity);
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

void entity_handler::network_update( network::interface *network_interface) {
    network::packet l_packet;
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        engine::entity *l_entity = p_entity[i];
        if( l_entity == NULL ||
            l_entity->change == false)
            continue;

        l_packet.type = network::packet_type::network_type_object_data;
        l_packet.length = outNetworkData( l_entity, l_packet.data);
        l_packet.crc = network::getCRC8( l_packet);
        l_entity->change = false;

        network_interface->sendPacket( l_packet, NULL);
    }
}

bool entity_handler::newClientCallback( network::client *client, network::interface *network_interface) {
    network::packet l_packet;

    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        engine::entity *l_entity = p_entity[i];
        if( l_entity == NULL)
            continue;

        l_packet.type = network::packet_type::network_type_object_data;
        l_packet.length = outNetworkData( l_entity, l_packet.data);
        l_packet.crc = network::getCRC8( l_packet);

        network_interface->sendPacket( l_packet, client);
    }
    
    log( log_trace, "entity_handler::newClientCallback");
    return true;
}

void entity_handler::recvPacket( network::packet packet) {
    // todo check okay
    if( packet.type == network::packet_type::network_type_object_data)
        inNetworkData( packet.data);
}