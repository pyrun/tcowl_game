#ifndef PHYSIC_BODY_HPP
#define PHYSIC_BODY_HPP

#include "shape.hpp"

namespace physic {
    struct body {
            engine::fvec2 position = { 0, 0};
            engine::fvec2 velocity = { 0, 0};

            bool collision = false;

            shape *shape = nullptr; // TODO free shape res. after use
            float density = 0.0f;
    };
}


#endif