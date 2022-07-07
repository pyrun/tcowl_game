#ifndef PHYSIC_SHAPE_H
#define PHYSIC_SHAPE_H

#include <engine/graphic/graphic.hpp>
#include <engine/vec.hpp>

namespace physic {
    enum shape_type {
        shape_type_rect,
        shape_type_circle,
        shape_type_line,
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

            virtual shape_type getType() = 0;

        private:
            engine::fvec2 p_offset;
    };

    class shape_rect : public shape {
        public:
            shape_rect( engine::fvec2 rect) { p_rect = rect; }
            ~shape_rect() {}

            void draw( engine::graphic_draw *graphic, engine::fvec2 position) override;

            engine::fvec2 getAABB() override;

            shape_type getType() override { return shape_type_rect; }
        private:
            engine::fvec2 p_rect;
    };

    class shape_circle : public shape {
        public:
            shape_circle( float size) { p_size = size; }
            ~shape_circle() {}

            void draw( engine::graphic_draw *graphic, engine::fvec2 position) override;

            engine::fvec2 getAABB() override;
            
            shape_type getType() override { return shape_type_circle; }
        private:
            float p_size;
    };

    class shape_line : public shape {
        public:
            shape_line( engine::fvec2 offsetTarget) { p_offsetpoint = offsetTarget; }
            ~shape_line() {}

            void draw( engine::graphic_draw *graphic, engine::fvec2 position) override;

            engine::fvec2 getAABB() override;
            
            shape_type getType() override { return shape_type_line; }
        private:
            engine::fvec2 p_offsetpoint; // offset from position
    };
}


#endif