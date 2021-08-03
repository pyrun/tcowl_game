#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

namespace helper {
    extern bool file_exists ( const char *filename);
    extern int isDirectory(const char *path);

    void int32toUint8x4( int32_t value, uint8_t *array);
    void uint8x4toInt32( uint8_t *array, int32_t *value);

    void int16toUint8x2( int16_t value, uint8_t *array);
    void uint8x2toInt16( uint8_t *array, int16_t *value);

    float lerp(float a, float b, float f);
};

#endif