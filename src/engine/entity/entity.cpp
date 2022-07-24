#include "entity.hpp"
#include "inventory.hpp"

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
        if( strcmp( p_types->get(i)->name.c_str(), name.c_str()) == 0)
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

    // Step 1 - object
    l_entity = p_entity[l_index] = new entity;
    p_draw_order.push_back( l_entity);
    l_entity->index = l_index;

    l_entity->objtype = objtype;
    l_entity->objtypeid = objtype->id;

    l_entity->body = new physic::body;
    l_entity->body->shape = objtype->shape;
    p_hub.add( l_entity->body);

    l_entity->action = 0;

    if( objtype->inventory_size.x || objtype->inventory_size.y) {
        l_entity->inventory = new inventory_grid( objtype->inventory_size.x, objtype->inventory_size.y);
    } else {
        l_entity->inventory = nullptr;
    }

    l_entity->change = true;
    l_entity->input = nullptr;

    log( log_trace, "entity_handler::createObject %d", l_index);
    p_amount++;

    // Step 2 - Script
    l_entity->lua_state = nullptr;
    loadScriptFile( l_entity);
    
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
    l_file = entity->objtype->src_path +  ENGINE_TYPE_FILE_SCRIPT;

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

entity *entity_handler::get( int16_t index) {
    return index>=0&&index<ENGINE_ENTITY_MAX_AMOUNT?p_entity[ (uint16_t)index]:nullptr;
}

void entity_handler::setPosition( int16_t index, fvec2 pos) {
    entity *l_entity;
    l_entity = get(index);
    if( l_entity == nullptr)
        return;
    if( l_entity->body == nullptr)
        return;
    l_entity->body->position = pos;
}

uint32_t entity_handler::outNetworkData( entity *obj, uint8_t *dataDist) {
    uint32_t l_offset = 0;

    helper::int16toUint8x2( obj->index, dataDist + l_offset); l_offset +=2;
    helper::int16toUint8x2( obj->objtypeid, dataDist + l_offset); l_offset +=2;

    helper::floatToUint8x4( obj->body->position.x, dataDist + l_offset); l_offset +=4;
    helper::floatToUint8x4( obj->body->position.y, dataDist + l_offset); l_offset +=4;

    helper::floatToUint8x4( obj->body->velocity.x, dataDist + l_offset); l_offset +=4;
    helper::floatToUint8x4( obj->body->velocity.y, dataDist + l_offset); l_offset +=4;

    dataDist[l_offset] = obj->action; l_offset +=1;
    dataDist[l_offset] = obj->animation_tick; l_offset +=1;


    // inventory def
    if( obj->inventory == nullptr) {
        dataDist[l_offset] = 0; l_offset +=1;
    } else {
        uint8_t l_size = obj->inventory->getGrid()->getH() * obj->inventory->getGrid()->getW();
        dataDist[l_offset] = l_size; l_offset +=1;
        for( uint8_t i = 0; i < l_size; i++) {
            engine::inventory_grid_state *l_tileg_grid_state = &obj->inventory->getGrid()->getArray()[i];
            dataDist[l_offset] = (uint8_t)*l_tileg_grid_state; l_offset +=1;
        }
    }
    // inventory items
    if( obj->inventory == nullptr) {
        dataDist[l_offset] = 0; l_offset +=1;
    } else {
        dataDist[l_offset] = (uint8_t)obj->inventory->getList()->size(); l_offset +=1;
        for( int32_t i = 0; i < obj->inventory->getList()->size(); i++) {
            inventory_entry *l_item = &obj->inventory->getList()->at(i);
            dataDist[l_offset] = l_item->angle; l_offset +=1;
            helper::int32toUint8x4( l_item->pos.x, dataDist + l_offset); l_offset +=4;
            helper::int32toUint8x4( l_item->pos.y, dataDist + l_offset); l_offset +=4;
            helper::int16toUint8x2( l_item->objtype->id, dataDist + l_offset); l_offset +=2;
        }
    }
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

    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->position.x); l_offset +=4;
    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->position.y); l_offset +=4;

    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->velocity.x); l_offset +=4;
    helper::uint8x4toFloat( dataDist + l_offset, &l_entity->body->velocity.y); l_offset +=4;

    l_entity->action = dataDist[l_offset]; l_offset +=1;
    l_entity->animation_tick = dataDist[l_offset]; l_offset +=1;
    helper::time::reset( &l_entity->animation_time);

    // inventory def
    uint8_t l_length = dataDist[l_offset]; l_offset +=1;
    for( uint8_t i = 0; i < l_length; i++) {
        engine::inventory_grid_state *l_tileg_grid_state = &l_entity->inventory->getGrid()->getArray()[i];
        engine::inventory_grid_state l_tile_grid_state_recv = (engine::inventory_grid_state)dataDist[l_offset]; l_offset +=1;
        if( l_tile_grid_state_recv == engine::inventory_grid_state::inventory_grid_state_unavailable)
            *l_tileg_grid_state = l_tile_grid_state_recv;
        else if( *l_tileg_grid_state == engine::inventory_grid_state::inventory_grid_state_unavailable)
            *l_tileg_grid_state = engine::inventory_grid_state::inventory_grid_state_available;
    }
    // inventory items
    l_length = dataDist[l_offset]; l_offset +=1;
    if( l_entity->inventory != nullptr &&
        l_length == 0 &&
        l_entity->inventory->getList()->size()) {
        l_entity->inventory->clear();
    }
    if( l_length && l_length != l_entity->inventory->getList()->size()) { // check size match
        // TODO: better solution
        l_entity->inventory->clear();
    }
    for( uint8_t i = 0; i < l_length; i++) {
        int16_t l_id;
        engine::inventory_entry l_item;
        l_item.angle = (inventory_angle)dataDist[l_offset]; l_offset +=1;
        helper::uint8x4toInt32( dataDist + l_offset, &l_item.pos.x); l_offset +=4;
        helper::uint8x4toInt32( dataDist + l_offset, &l_item.pos.y); l_offset +=4;
        helper::uint8x2toInt16( dataDist + l_offset, &l_id); l_offset +=2;
        l_item.objtype = p_types->getById( l_id);

        if( i >= (uint8_t)l_entity->inventory->getList()->size()) {
            l_entity->inventory->add( &l_item);
        } else {
            engine::inventory_entry *l_list_item = &l_entity->inventory->getList()->at(i);
            if( !(l_item.pos == l_list_item->pos) ||
                l_item.angle != l_list_item->angle ||
                l_item.objtype->id != l_list_item->objtype->id) {
                l_entity->inventory->del( l_list_item);
                l_entity->inventory->add( &l_item);
            }
        }
    }
    l_entity->change = false;
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
            return a->body->position.y+a->objtype->actions[a->action].size.y+a->objtype->depth_sorting_offset.y
                <  b->body->position.y+b->objtype->actions[b->action].size.y+b->objtype->depth_sorting_offset.y;
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
    action *l_action = &obj->objtype->actions[ obj->action];
    float l_time;
    float l_factor = 15.f; // TODO remove factor magic value
    uint32_t l_magical_value_ticks_mul = 10; // TODO remove this value for ticks mul

    // adjust animation speed to acceleration if wanted
    if( l_action->bind_velocity) {
        l_time = powf( obj->body->velocity.normalize() / l_factor, -1);
        l_time *=l_action->delay;
    } else {
        l_time = l_action->delay;
    }

    if( l_time < l_action->delay*l_magical_value_ticks_mul &&
        helper::time::check( &obj->animation_time, l_time>l_action->delay?l_time:l_action->delay)) {
        helper::time::reset( &obj->animation_time);
        obj->animation_tick++;
    }

    graphic->draw(  &obj->objtype->image,
                    obj->body->position.toVec(),
                    l_action->size,
                    l_action->postion + vec2{ (int32_t)(obj->animation_tick%l_action->length) * l_action->size.x, 0},
                    0.0,
                    nullptr,
                    l_action->flip_horizontal?graphic_flip_horizontal:graphic_flip_none | l_action->flip_vertical?graphic_flip_vertical:graphic_flip_none);
}

std::vector<entity*> entity_handler::find( vec2 pos, vec2 rect) {
    std::vector<entity*> l_array;

    for( uint32_t i = 0; i < ENGINE_ENTITY_MAX_AMOUNT; i++) {
        engine::entity *l_entity = p_entity[i];
        if( l_entity == nullptr ||
            l_entity->body == nullptr ||
            l_entity->body->shape == nullptr)
            continue;
        // use physic hub for check
        if( p_hub.testAABBAABB( fvec2{ (float)pos.x, (float)pos.y},
            fvec2{ (float)rect.x, (float)rect.y},
            l_entity->body->position+l_entity->body->shape->getOffset(),
            l_entity->body->shape->getAABB())) {
            // check if its new
            bool l_new = true;
            for( entity* const & l_entity_hit: l_array)
                if( l_entity_hit == l_entity)
                    l_new = false;
            if( l_new)
                l_array.push_back( l_entity);

        }
    }
    return l_array;
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