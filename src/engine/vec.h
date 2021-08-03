#ifndef ENGINE_VEC_H
#define ENGINE_VEC_H

#include <stdint.h>
#include <math.h>

namespace engine {
    struct vec2 {
        int32_t x = 0;
        int32_t y = 0;

        vec2 operator+(const vec2 rhs) {
            return vec2{ x+rhs.x, y+rhs.y};
        }

        vec2& operator+=(const vec2& rhs){
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
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

        fvec2& operator+=(const fvec2& rhs){
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }

        fvec2 operator*(const float rhs) {
            return fvec2{ x*rhs, y*rhs};
        }
        
        fvec2& operator/=(const float rhs){
            this->x /= rhs;
            this->y /= rhs;
            return *this;
        }

        vec2 toVec() {
            return vec2{ (int32_t)x, (int32_t)y};
        }

        float normalize() {
            return std::sqrt(x*x + y*y);
        }
    };
};

#endif