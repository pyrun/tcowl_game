#ifndef PHYSIC_SHAPE_H
#define PHYSIC_SHAPE_H

#include "../vec.h"
#include "../graphic.h"

namespace physic {
    enum sharp_type {
        sharp_type_rect,
        sharp_type_circle,
    };

    class shape {
        public:
            shape() {}
            ~shape() {}

            virtual void draw( engine::graphic_draw *graphic, engine::fvec2 position) = 0;

            // für vorsotieren wird ein einfaches rechteck benötigt
            virtual engine::fvec2 getAABB() = 0;

            void setOffset( engine::fvec2 offset) { p_offset = offset;} 
            engine::fvec2 getOffset() { return p_offset; }

            virtual sharp_type getType() = 0;

        private:
            engine::fvec2 p_offset;
    };

    class shape_rect : public shape {
        public:
            shape_rect( engine::fvec2 rect) { p_rect = rect; }
            ~shape_rect() {}

            void draw( engine::graphic_draw *graphic, engine::fvec2 position) override;

            engine::fvec2 getAABB() override;

            sharp_type getType() override { return sharp_type_rect; }
        private:
            engine::fvec2 p_rect;
    };

    class sharp_circle : public shape {
        public:
            sharp_circle( float size) { p_size = size; }
            ~sharp_circle() {}

            void draw( engine::graphic_draw *graphic, engine::fvec2 position) override;

            engine::fvec2 getAABB() override;
            
            sharp_type getType() override { return sharp_type_circle; }
        private:
            float p_size;
    };
}


#endif