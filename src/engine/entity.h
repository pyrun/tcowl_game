#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "graphic.h"

#include <string>

namespace engine {
    class entity_handler : public engine::graphic_object {
        public:
            entity_handler();
            ~entity_handler();

            int32_t createObject( uint32_t id);
            int32_t createObject( std::string name);

            void draw( engine::graphic_draw *graphic);
        private:

    };
};

#endif