#ifndef ENGINE_ENTITY_HPP
#define ENGINE_ENTITY_HPP

#include "type.hpp"
#include "inventory.hpp"

#include <engine/graphic/graphic.hpp>
#include <engine/world/world.hpp>
#include <engine/script.hpp>
#include <engine/input/input.hpp>
#include <engine/physic/hub.hpp>
#include <network/network.hpp>

#include <string>

#define ENGINE_ENTITY_MAX_AMOUNT 1024

namespace engine {
    typedef uint16_t entity_id;

    struct entity_parameter {
        std::string name;
        uint32_t value;
    };

    struct entity {
        entity_id index;

        type *objtype;
        int16_t objtypeid;
        physic::body *body;

        uint8_t action;

        uint32_t animation_tick;
        uint32_t animation_time;

        lua_State *lua_state;
        engine::input_map *input;

        inventory_grid *inventory;

        std::vector<entity_parameter> parameter;

        bool change; // synchronised with other systems, used for example in the network
    };

    class entity_handler : public engine::graphic_object, public network::synchronisation {
        public:
            entity_handler();
            ~entity_handler();

            void init( type_handler *types);

            entity_id createObject( entity_id objid); // -> objtype
            entity_id createObject( std::string name); // -> objtype
            entity_id createObject( type *objtype, int32_t index = -1);
            bool deleteObject( uint32_t index);
            bool loadScriptFile( entity *entity);
            bool bindInput( entity *entity, engine::input_map *input_obj);

            entity *get( entity_id index);
            void setPosition( entity_id index, fvec2 pos);

            uint32_t outNetworkData( entity *obj, uint8_t *dataDist);
            void inNetworkData( uint8_t *dataDist);

            void update( float dt, world *world);

            void reload( engine::graphic_draw *graphic) override;
            void draw( engine::graphic_draw *graphic) override;
            void drawEntity( engine::graphic_draw *graphic, entity* obj);

            std::vector<entity*> find( vec2 pos, vec2 rect);

            uint32_t getAmount() { return p_amount; }

            // network
            void network_update( network::connection *network_interface);
            bool newClientCallback( network::client *client, network::connection *network_interface);
            void recvPacket( network::packet packet);

            type *getType( uint16_t id) { return p_types->getById( id); }
            type *getTypeByName( std::string name) { return p_types->getByName( name); }
            const uint32_t getPriority() override { return 128; }
            physic::hub *getPhysicHub() { return &p_hub; }
        private:
            type_handler *p_types;
            entity *p_entity[ENGINE_ENTITY_MAX_AMOUNT];
            std::vector<entity*> p_draw_order;
            physic::hub p_hub;
            uint32_t p_amount;
    };

    extern entity_handler *used_entity_handler;
};

#endif