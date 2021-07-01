#ifndef ENGINE_VEC_H
#define ENGINE_VEC_H

#include <stdint.h>

namespace engine {
    struct vec2 {
        int32_t x = 0;
        int32_t y = 0;

        vec2 operator+(const vec2 rhs) {
            return vec2{ x+rhs.x, y+rhs.y};
        }
    };

    struct vec3 {
        int32_t x;
        int32_t y;
        int32_t z;
    };
    
    struct vec4 {
        int32_t x;
        int32_t y;
        int32_t z;
        int32_t w;
    };
};

#endif