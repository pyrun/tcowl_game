#ifndef PHYSIC_SHAPE_H
#define PHYSIC_SHAPE_H

#include "../vec.h"

namespace physic {
    enum sharp_type {
        sharp_type_rect,
        sharp_type_circle,
    };

    class shape {
        public:
            shape();
            ~shape();
        private:
    };

    class shape_rect : public shape {
        public:
            shape_rect();
            ~shape_rect();
        private:
            engine::fvec2 p_rect;
    };

    class sharp_circle : public shape {
        public:
            sharp_circle();
            ~sharp_circle();
        private:
            float p_size;
    };
}


#endif