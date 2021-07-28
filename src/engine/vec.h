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

    struct fvec2 {
        float x = 0;
        float y = 0;

        fvec2 operator+(const fvec2 rhs) {
            return fvec2{ x+rhs.x, y+rhs.y};
        }

        vec2 toVec() {
            return vec2{ (int32_t)x, (int32_t)y};
        }
    };
};

#endif