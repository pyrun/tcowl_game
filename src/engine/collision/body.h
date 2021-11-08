#ifndef PHYSIC_BODY_H
#define PHYSIC_BODY_H

#include "shape.h"

namespace physic {
    class body {
        public:
            body();
            ~body();

            void cleanup();

            void linkShape( shape* shape) { p_shape = shape; }
        private:

        shape *p_shape;
    };
}


#endif