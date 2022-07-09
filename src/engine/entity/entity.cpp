#include "entity.hpp"

#include <string.h>
#include <engine/log.hpp>
#include <engine/timer.hpp>
#include <engine/helper.hpp>
#include <engine/network_ids.hpp>

#include <cstdint>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace engine;

entity_handler *engine::used_entity_handler = nullptr;

entity_handler::entity_handler() {
    p_draw_order.reserve( ENGINE_ENTITY_MAX_AMOUNT);
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        p_entity[i] = nullptr;
    }
    p_types = nullptr;
    p_amount = 0;
}

entity_handler::~entity_handler() {

}

void entity_handler::init( type_handler *types) {
    p_types = types;
}

int16_t entity_handler::createObject( std::string name) {
    if( p_types == nullptr)
        return -1;
    for( uint32_t i = 0; i < p_types->getAmount(); i++)
        if( strcmp( p_types->get(i)->getName(), name.c_str()) == 0)
            return createObject( p_types->get(i));
    return -1;
}

int16_t entity_handler::createObject( int16_t objid) {
    if( p_types == nullptr)
        return -1;
    type *l_type = p_types->getById( objid);
    if( l_type == nullptr)
        return -1;
    return createObject( l_type);
}

int16_t entity_handler::createObject( type *objtype, int32_t index) {
    if( p_types == nullptr)
        return -1;
    if( objtype == nullptr)
        return -1;
    
    entity *l_entity = nullptr;
    bool l_found = false;
    uint16_t l_index = 0;

    if( index < 0) {
        for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
            if( p_entity[i] == nullptr) {
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

    l_entity->body = new physic::body;
    l_entity->body->linkShape( objtype->getShape());
    p_hub.add( l_entity->body);

    l_entity->action = 0;

    l_entity->lua_state = nullptr;
    loadScriptFile( l_entity);

    l_entity->inventory = nullptr;

    l_entity->change = true;
    l_entity->input = nullptr;
    
    log( log_trace, "entity_handler::createObject %d", l_index);

    p_amount++;
    
    return l_index;
}

bool entity_handler::deleteObject( uint32_t index) {
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        if( p_entity[i] != nullptr && p_entity[i]->index == index) {
            entity *l_entity = p_entity[i];

            for( uint32_t n = 0; n < p_draw_order.size(); n++) {
                if( p_draw_order[n] == l_entity) {
                    p_draw_order.erase( p_draw_order.begin()+n);
                    break;
                }
            }
            log( log_trace, "entity_handler::deleteObject %d", index);
            p_hub.del( l_entity->body);
            delete p_entity[i];
            p_entity[i] = nullptr;
            p_amount--;
            return true;
        }
    }
    return false;
}

bool entity_handler::loadScriptFile( entity *entity) {
    std::string l_file;

    // check for null
    if( entity == nullptr)
        return false;
    if( entity->objtype == nullptr) {
        log( log_error, "entity_handler::loadScriptFile type not found!\n");
        return false;
    }
    l_file = entity->objtype->getFolderPath() +  ENGINE_TYPE_FILE_SCRIPT;

    // Delete existing script beforehand if it is existing
    script::free( entity->lua_state);

    // try load script
    entity->lua_state = script::loadFile( l_file.c_str());

    // run once
    engine::used_entity_handler = this;
    script::run( entity->lua_state);
    script::function( "Intilisation", entity->lua_state, entity->index);
    engine::used_entity_handler = nullptr;
    
    return true;
}

bool entity_handler::bindInput( entity *entity, input_map *input_obj) {
    if( !entity)
        return false;
    entity->input = input_obj;
    return true;
}

void entity_handler::setPosition( int16_t index, fvec2 pos) {
    entity *l_entity;
    l_entity = get(index);
    if( l_entity == nullptr)
        return;
    if( l_entity->body == nullptr)
        return;
    l_entity->body->setPosition( pos);
}

uint32_t entity_handler::outNetworkData( entity *obj, uint8_t *dataDist) {
    uint32_t l_offset = 0;

    helper::int16toUint8x2( obj->index, dataDist + l_offset); l_offset +=2;
    helper::int16toUint8x2( obj->objtypeid, dataDist + l_offset); l_offset +=2;

    helper::floatToUint8x4( obj->body->getPosition().x, dataDist + l_offset); l_offset +=4;
    helper::floatToUint8x4( obj->body->getPosition().y, dataDist + l_offset); l_offset +=4;

    helper::floatToUint8x4( obj->body->getVelocity().x, dataDist + l_offset); l_offset +=4;
    helper::floatToUint8x4( obj->body->getVelocity().y, dataDist + l_offset); l_offset +=4;

    dataDist[l_offset] = obj->action; l_offset +=1;

    return l_offset;
}

void entity_handler::inNetworkData( uint8_t *dataDist) {
    uint32_t l_offset = 0;
    int16_t l_index;
    int16_t l_type_id;

    helper::uint8x2toInt16( dataDist + l_offset, &l_index); l_offset +=2;
    helper::uint8x2toInt16( dataDist + l_offset, &l_type_id); l_offset +=2;
    
    if( p_types->getById( l_type_id) == nullptr)
        return;
    entity *l_entity = get(l_index);
    if( l_entity == nullptr) {
        createObject( p_types->getById( l_type_id), l_index);
        l_entity = get( l_index);
    }
    l_entity->index = l_index;
    l_entity->objtype = p_types->getById( l_type_id);
    l_entity->objtypeid = l_type_id;

    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->getPositionPtr()->x); l_offset +=4;
    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->getPositionPtr()->y); l_offset +=4;

    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->getVelocityPtr()->x); l_offset +=4;
    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->getVelocityPtr()->y); l_offset +=4;

    l_entity->action = dataDist[l_offset]; l_offset +=1;
}

void entity_handler::update( float dt, world *world) {
    // Für Berechnungen wird diese Klasse verwendet
    engine::used_entity_handler = this;
    p_hub.update( dt);

    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        engine::entity *l_entity = p_entity[i];
        if( l_entity == nullptr)
            continue;
        script::function( "Update", l_entity->lua_state, l_entity->index);
    }

    // Freigeben
    engine::used_entity_handler = nullptr;
}

void entity_handler::reload( engine::graphic_draw *graphic) {
    p_types->reload( graphic);
}

void entity_handler::draw( engine::graphic_draw *graphic) {
    // depth sorting 
    struct {
        bool operator()( entity *a, entity *b) const {
            return a->body->getPosition().y+a->objtype->getAction(a->action)->size.y+a->objtype->getDepthSortingOffset().y
                <  b->body->getPosition().y+b->objtype->getAction(b->action)->size.y+b->objtype->getDepthSortingOffset().y;
        }
    } depthSorting;
    std::sort( p_draw_order.begin(), p_draw_order.end(), depthSorting);

    // only draw entitys which have a graphic
    for( uint32_t i = 0; i < p_draw_order.size(); i++) {
        entity *l_entity = p_draw_order[i];
        drawEntity( graphic, l_entity);
    }
}

void entity_handler::drawEntity( engine::graphic_draw *graphic, entity* obj) {
    // todo get action
    action *l_action = obj->objtype->getAction( obj->action);
    float l_time;
    float l_factor = 15.f; // TODO remove factor magic value
    uint32_t l_magical_value_ticks_mul = 10; // TODO remove this value for ticks mul

    // adjust animation speed to acceleration if wanted
    if( l_action->bind_velocity) {
        l_time = powf( obj->body->getVelocity().normalize() / l_factor, -1);
        l_time *=l_action->delay;
    } else {
        l_time = l_action->delay;
    }

    if( l_time < l_action->delay*l_magical_value_ticks_mul &&
        helper::time::check( &obj->animation_time, l_time>l_action->delay?l_time:l_action->delay)) {
        helper::time::reset( &obj->animation_time);
        obj->animation_tick++;
    }

    graphic->draw(  obj->objtype->getImage(),
                    obj->body->getPosition().toVec(),
                    l_action->size,
                    l_action->postion + vec2{ (int32_t)(obj->animation_tick%l_action->length) * l_action->size.x, 0});
}

void entity_handler::network_update( network::connection *network_interface) {
    network::packet l_packet;
    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        engine::entity *l_entity = p_entity[i];
        if( l_entity == nullptr ||
            l_entity->change == false)
            continue;

        l_packet.type = engine::network_id::network_id_object_data;
        l_packet.length = outNetworkData( l_entity, l_packet.data);
        l_packet.crc = network::getCRC8( l_packet);
        l_entity->change = false;

        network_interface->sendPacket( l_packet, nullptr);
    }
}

bool entity_handler::newClientCallback( network::client *client, network::connection *network_interface) {
    network::packet l_packet;

    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        engine::entity *l_entity = p_entity[i];
        if( l_entity == nullptr)
            continue;

        l_packet.type = engine::network_id::network_id_object_data;
        l_packet.length = outNetworkData( l_entity, l_packet.data);
        l_packet.crc = network::getCRC8( l_packet);

        network_interface->sendPacket( l_packet, client);
    }

    log( log_trace, "entity_handler::newClientCallback");
    return true;
}

void entity_handler::recvPacket( network::packet packet) {
    // todo check okay
    if( packet.type == engine::network_id::network_id_object_data)
        inNetworkData( packet.data);
}