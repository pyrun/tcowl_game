#ifndef HELPER_NOISE_HPP
#define HELPER_NOISE_HPP

#include <stdint.h>

namespace helper {
    float perlin2d( float x, float y, float freq, int32_t depth, int32_t seed = 0);
};

#endif