#ifndef ENGINE_TYPE_H
#define ENGINE_TYPE_H

#include "vec.h"
#include "graphic.h"
#include "image.h"

#include <string>

#define ENGINE_TYPE_NAME_MAX_LENGTH 64
#define ENGINE_TYPE_FILE_DESCRIPTION "definition.json"
#define ENGINE_TYPE_FILE_SCRIPT "script.lua"

namespace engine {
    struct action {
        uint32_t id; // zuordnung
        std::string name;
        vec2 postion; // Wo im Bild das erste anfängt
        vec2 size; // Größe w x h
        uint32_t ticks_for_next_image; // Nach wie vielen Ticks weiter gesprungen wird
        uint32_t length; // Anzahl der Bilder
        bool bind_velocity; // Anpassen der Animationsgeschwindigkeit an die Geschwindigkeit
    };

    class type {
        public:
            type();
            ~type();

            void setId( uint16_t id) { p_id = id; }
            uint16_t getId() { return p_id; }
            void setName( const char *name) { snprintf( p_name, ENGINE_TYPE_NAME_MAX_LENGTH, name); }
            char *getName() { return p_name; }
            void setFolderPath( std::string folder) { p_folder = folder; }
            std::string getFolderPath() { return p_folder; }

            void addAction( action annimation) { p_action.push_back(annimation); }
            uint32_t getAmountActions() { return p_action.size(); }
            image *getImage() { return &p_image; }
            action *getAction( uint32_t index) { return &p_action[index]; }
            action *getAction( std::string name);

            void setDepthSortingOffset( vec2 value) { p_depth_sorting_offset = value; }
            vec2 getDepthSortingOffset() { return p_depth_sorting_offset; }
        private: 
            std::vector<action> p_action;
            std::string p_folder;
            char p_name[ENGINE_TYPE_NAME_MAX_LENGTH];
            uint16_t p_id;
            image p_image;
            vec2 p_depth_sorting_offset; // offset beim z-depth sorting
    };

    class type_handler{
        public:
            type_handler();
            ~type_handler();

            bool loadFolder( graphic *graphic, std::string path);
            void loadtype( graphic *graphic, std::string file);
            
            uint32_t getAmount() { return (uint32_t)p_type.size(); }
            type *get( uint32_t index) { return &p_type.at( index); }
            type *getById( uint16_t id);
        private:
            type *createtype();
            bool removetype( type *target);

            std::vector<type> p_type;
    };
};

#endif