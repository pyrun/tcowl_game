#ifndef ENTITY_INVENTORY_HPP
#define ENTITY_INVENTORY_HPP

#include <vector>
#include <stdint.h>
#include <engine/vec.hpp>
#include <engine/helper.hpp>
#include <engine/entity/type.hpp>
#include <engine/graphic/graphic.hpp>

namespace engine {
    enum inventory_angle {
        inventory_angle_0 = 0,
        inventory_angle_90 = 90,
        inventory_angle_180 = 180,
        inventory_angle_270 = 270,
    };

    struct inventory_entry {
        engine::type *objtype;
        vec2 pos;
        inventory_angle angle;
    };

    enum inventory_grid_state {
        inventory_grid_state_unavailable = 0,
        inventory_grid_state_available,
        inventory_grid_state_taken,
    };

    class inventory_grid : public engine::graphic_object {
        public:
            inventory_grid( uint32_t w, uint32_t h) {
                p_grid = new helper::map2d<inventory_grid_state>( w, h);
                p_grid->fill( inventory_grid_state::inventory_grid_state_unavailable);
            }
            ~inventory_grid() {
                if(p_grid)
                    delete p_grid;
            }

            void add( vec2 pos, engine::type *objtype);
            bool del( inventory_entry *item);

            inventory_entry *onClick( vec2 pos);
            void turn( inventory_entry *item, bool clockwise);

            void setState( vec2 pos, inventory_grid_state state);
            void reload( graphic_draw *graphic) override;
            void draw( graphic_draw *graphic) override { draw( graphic, true); };
            void draw( graphic_draw *graphic, bool top);
            void drawItem( graphic_draw *graphic, vec2 pos, inventory_entry *item, vec2 centre = vec2{ 0, 0});

            std::vector<inventory_entry> *getList() { return &p_items; }
        private:
            vec2 calcDrawPos( graphic_draw *graphic, bool top);
        private:
            helper::map2d<inventory_grid_state> *p_grid;
            std::vector<inventory_entry> p_items;

            engine::vec2 p_draw_pos;
    };
}

#endif
