#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "graphic.h"
#include "type.h"
#include "../network/network.h"

#include <string>

#define ENGINE_ENTITY_MAX_AMOUNT 1024

namespace engine {
    struct entity {
        uint32_t id;
        
        uint32_t objtypeid;
        type *objtype;

        vec2 position;
        vec2 velocity;

        uint8_t action;

        uint32_t animation_tick;
        uint32_t animation_time;
    };

    class entity_handler : public engine::graphic_object, public network::synchronisation {
        public:
            entity_handler();
            ~entity_handler();

            void init( type_handler *types);

            int32_t createObject( uint32_t id); // -> objtype
            int32_t createObject( std::string name); // -> objtype
            int32_t createObject( type *objtype);
            bool deleteObject( uint32_t id);

            entity *get( int32_t index) { return p_entity[ (uint32_t)index]; }

            uint32_t outNetworkData( entity *obj, uint8_t *dataDist);
            void inNetworkData( uint8_t *dataDist);

            void draw( engine::graphic_draw *graphic);
            void drawEntity( engine::graphic_draw *graphic, entity* obj);

            uint32_t getAmount() { return p_amount; }
            // network
            void network_update( network::interface *network_interface);
            bool newClientCallback( network::client *client, network::interface *network_interface);
            void recvPacket( network::packet packet);
        private:
            type_handler *p_types;
            entity *p_entity[ENGINE_ENTITY_MAX_AMOUNT];
            std::vector<entity*> p_draw_order;
            uint32_t p_amount;
    };
};

#endif