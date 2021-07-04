#ifndef ENGINE_TYPE_H
#define ENGINE_TYPE_H

#include "vec.h"
#include "graphic.h"
#include "image.h"

#include <string>

#define ENGINE_TYPE_NAME_MAX_LENGTH 64

namespace engine {
    struct action {
        uint32_t id; // zuordnung
        std::string name;
        vec2 postion; // Wo im Bild das erste anfängt
        vec2 size; // Größe w x h
        uint32_t ticks_for_next_image; // Nach wie vielen Ticks weiter gesprungen wird
        uint32_t quantity; // Anzahl der Bilder
    };

    class type {
        public:
            type();
            ~type();

            void update();
            void draw();

            void setId( uint32_t id) { p_id = id; }
            uint32_t getId() { return p_id; }
            void setName( const char *name) { snprintf( p_name, ENGINE_TYPE_NAME_MAX_LENGTH, name); }
            char *getName() { return p_name; }

            void addAction( action annimation) { p_action.push_back(annimation); }
            uint32_t getAmountActions() { return p_action.size(); }
            image *getImage() { return &p_image; }
        private: 
            std::vector<action> p_action;
            char p_name[ENGINE_TYPE_NAME_MAX_LENGTH];
            uint32_t p_id;
            image p_image;
    };

    class type_handler{
        public:
            type_handler();
            ~type_handler();

            void loadtype( graphic *graphic, std::string file);
            
            uint32_t getAmount() { return (uint32_t)p_type.size(); }
        private:
            type *createtype();
            bool removetype( type *target);

            std::vector<type> p_type;
    };
};

#endif