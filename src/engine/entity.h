#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "graphic.h"
#include "type.h"
#include "world.h"
#include "script.h"
#include "../network/network.h"

#include <string>

#define ENGINE_ENTITY_MAX_AMOUNT 1024

namespace engine {
    struct entity {
        uint16_t index;
        
        uint16_t objtypeid;
        type *objtype;

        fvec2 position;
        fvec2 velocity;

        uint8_t action;

        uint32_t animation_tick;
        uint32_t animation_time;
        
        lua_State *lua_state;

        // used for network sync
        // only sync if this true, set be false after sync
        // todo state of change, like only pos, only action, destroyed
        bool change;
    };

    class entity_handler : public engine::graphic_object, public network::synchronisation {
        public:
            entity_handler();
            ~entity_handler();

            void init( type_handler *types);

            int16_t createObject( uint16_t objid); // -> objtype
            int16_t createObject( std::string name); // -> objtype
            int16_t createObject( type *objtype, int32_t index = -1);
            bool deleteObject( uint32_t index);
            bool loadScriptFile( entity *entity);

            entity *get( int16_t index) { return p_entity[ (uint16_t)index]; }

            uint32_t outNetworkData( entity *obj, uint8_t *dataDist);
            void inNetworkData( uint8_t *dataDist);

            void update( float dt, world *world);

            void draw( engine::graphic_draw *graphic) override;
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

    extern entity_handler *used_entity_handler;
};

#endif