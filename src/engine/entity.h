#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "vec.h"
#include "graphic.h"

#include <string>

#define ENGINE_ENTITY_NAME_MAX_LENGTH 64

namespace engine {
    struct action {
        uint32_t id; // zuordnung
        vec2 postion; // Wo im Bild das erste anfängt
        vec2 size; // Größe w x h
        uint32_t ticks_for_next_image; // Nach wie vielen Ticks weiter gesprungen wird
        uint32_t quantity; // Anzahl der Bilder
    };

    class entity {
        public:
            entity();
            ~entity();

            void update();
            void draw();

            void setId( uint32_t id) { p_id = id; }
            uint32_t getId() { return p_id; }
            void setName( const char *name) { snprintf( p_name, ENGINE_ENTITY_NAME_MAX_LENGTH, name); }
            char *getName() { return p_name; }
        private:
            std::vector<action> p_action;
            char p_name[ENGINE_ENTITY_NAME_MAX_LENGTH];
            uint32_t p_id;
    };

    class entity_handler {
        public:
            entity_handler();
            ~entity_handler();

            void loadEntity( std::string file);
            
            uint32_t getAmount() { return (uint32_t)p_entity.size(); }
        private:
            entity *createEntity();
            bool removeEntity( entity *target);

            std::vector<entity> p_entity;
    };
};

#endif