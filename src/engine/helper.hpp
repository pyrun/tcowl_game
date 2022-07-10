#ifndef ENGINE_HELPER_HPP
#define ENGINE_HELPER_HPP

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

            type *get( int32_t x, int32_t y) {
                if( x >= p_w || y >= p_h ||
                    x < 0 || y < 0)
                    return nullptr;
                return &p_map[y*p_w+x];
            }

            bool set( uint32_t x, uint32_t y, type value) {
                type *l_tile = get( x, y);
                if( l_tile) {
                    *l_tile = value;
                    return true;
                } else {
                    return false;
                }
            }

            uint32_t getW() { return p_w; }
            uint32_t getH() { return p_h; }
        private:
            type *p_map;
            uint32_t p_w, p_h;
    };

    extern bool file_exists ( const char *filename);
    extern int isDirectory(const char *path);

    extern void int32toUint8x4( int32_t value, uint8_t *array);
    extern void uint8x4toInt32( uint8_t *array, int32_t *value);

    extern void int16toUint8x2( int16_t value, uint8_t *array);
    extern void uint8x2toInt16( uint8_t *array, int16_t *value);

    extern void floatToUint8x4(float float_variable, uint8_t *array);
    extern void uint8x4toFloat(uint8_t *array, float *float_variable);

    extern float lerp(float a, float b, float f);
};

#endif