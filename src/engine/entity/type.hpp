#ifndef ENGINE_TYPE_HPP
#define ENGINE_TYPE_HPP

#include "action.hpp"

#include <string>
#include <engine/vec.hpp>
#include <engine/graphic/graphic.hpp>
#include <engine/graphic/image.hpp>
#include <engine/physic/shape.hpp>

#define ENGINE_TYPE_NAME_MAX_LENGTH 64
#define ENGINE_TYPE_FILE_DESCRIPTION "definition.json"
#define ENGINE_TYPE_FILE_SCRIPT "script.lua"

namespace engine {
    enum type_objecttype {
        type_objecttype_object,
        type_objecttype_item,
    };

    struct type {
        uint16_t id;
        std::string name;
        std::vector<action> actions;
        physic::shape *shape = nullptr;
        type_objecttype object_type = type_objecttype_object;

        image image;
        std::string src_path;

        std::vector<vec2> item_vertex;
        vec2 inventory_size;

        vec2 depth_sorting_offset; // offset beim z-depth sorting
        
        action *findAction( std::string name);
    };

    class type_handler {
        public:
            type_handler();
            ~type_handler();

            bool loadFolder( graphic *graphic, std::string path);
            void loadtype( graphic *graphic, std::string file);

            void reload( graphic_draw *graphic);
            
            uint32_t getAmount() { return (uint32_t)p_type.size(); }
            type *get( uint32_t index) { return &p_type.at( index); }
            type *getById( uint16_t id);
            type *getByName( std::string name);
        private:
            type *createtype();
            bool removetype( type *target);

            uint16_t p_highest_id;

            std::vector<type> p_type;
    };
};

#endif