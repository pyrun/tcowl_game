#ifndef ENGINE_WORLD_H
#define ENGINE_WORLD_H

#include <vector>

#include "../image.h"
#include "../graphic.h"

#include "room.h"

namespace engine {
    class world : public engine::graphic_object {
        public:
            world();
            ~world();

            void begin( graphic *graphic, uint32_t seed = 0x0);

            void createRoom( vec2 position);

            void draw( engine::graphic_draw *graphic) override;
        private:
            image p_tileset;
            uint32_t p_seed;

            graphic *p_graphic;

            std::vector<room*> p_rooms;
    };
};
#endif