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
    template<class type>
    class map2d {
        public:
            map2d( const uint32_t w, const  uint32_t h) {
                p_map = new type[ w*h];
                p_w = w;
                p_h = h;
            }
            ~map2d() {}

            void fill( type value) {
                for( uint32_t i = 0; i < p_w*p_h; i++)
                    p_map[i] = value;
            }

            type *get( uint32_t x, uint32_t y) {
                if( x >= p_w || y >= p_h)
                    return nullptr;
                return &p_map[y*p_h+x];
            }

            uint32_t getW() { return p_w; }
            uint32_t getH() { return p_h; }
        private:
            type *p_map;
            uint32_t p_w, p_h;
    };

    extern bool file_exists ( const char *filename);
    extern int isDirectory(const char *path);

    void int32toUint8x4( int32_t value, uint8_t *array);
    void uint8x4toInt32( uint8_t *array, int32_t *value);

    void int16toUint8x2( int16_t value, uint8_t *array);
    void uint8x2toInt16( uint8_t *array, int16_t *value);

    void floatToUint8x4(float float_variable, uint8_t *array);
    void uint8x4toFloat(uint8_t *array, float *float_variable);

    float lerp(float a, float b, float f);
};

#endif