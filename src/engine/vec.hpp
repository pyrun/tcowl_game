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

        vec2 operator-(const vec2 rhs) {
            return vec2{ x-rhs.x, y-rhs.y};
        }

        vec2 operator*(const vec2& rhs) {
            return vec2{ x*rhs.x, y*rhs.y};
        }

        vec2 operator/(const vec2& rhs) {
            return vec2{ x/rhs.x, y/rhs.y};
        }

        vec2& operator+=(const vec2& rhs){
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }

        vec2& operator/=(const vec2 rhs){
            this->x /= rhs.x;
            this->y /= rhs.y;
            return *this;
        }
        
        bool operator==(const vec2 rhs){
            return (rhs.x == this->x) && (rhs.y == this->y);
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

        fvec2 operator-(const fvec2 rhs) {
            return fvec2{ x-rhs.x, y-rhs.y};
        }

        fvec2& operator+=(const fvec2& rhs){
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }

        fvec2 operator*(const fvec2& rhs) {
            return fvec2{ x*rhs.x, y*rhs.y};
        }

        fvec2 operator/(const fvec2& rhs) {
            return fvec2{ x/rhs.x, y/rhs.y};
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

    struct fvec3 {
        float x = 0;
        float y = 0;
        float z = 0;

        fvec3 operator+(const fvec3 rhs) {
            return fvec3{ x+rhs.x, y+rhs.y, z+rhs.z};
        }

        fvec3& operator+=(const fvec3& rhs){
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            return *this;
        }

        fvec3 operator*(const float rhs) {
            return fvec3{ x*rhs, y*rhs, z*rhs};
        }
        
        fvec3& operator/=(const float rhs){
            this->x /= rhs;
            this->y /= rhs;
            this->z /= rhs;
            return *this;
        }

        vec3 toVec() {
            return vec3{ (int32_t)x, (int32_t)y, (int32_t)z};
        }

        float normalize() {
            return std::sqrt((x * x) + (y * y) + (z * z));
        }
    };

    struct fvec4 {
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;
    };
};

#endif