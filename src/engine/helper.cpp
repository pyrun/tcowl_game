#include "helper.hpp"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>   // stat
#include <sys/types.h>
#include <stdbool.h>    // bool type

bool helper::file_exists( const char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

int helper::isDirectory(const char *path) {
  struct stat statbuf;
  if( stat( path, &statbuf) != 0)
      return 0;
  return S_ISDIR( statbuf.st_mode);
}
  
void helper::int32toUint8x4( int32_t value, uint8_t *array) {
  array[0] = value >> 24;
  array[1] = value >> 16;
  array[2] = value >>  8;
  array[3] = value;
}

void helper::uint8x4toInt32( uint8_t *array, int32_t *value) {
  *value  = array[0] << 24;
  *value += array[1] << 16;
  *value += array[2] <<  8;
  *value += array[3];
}

void helper::int16toUint8x2( int16_t value, uint8_t *array) {
  array[0] = value >> 8;
  array[1] = value;
}

void helper::uint8x2toInt16( uint8_t *array, int16_t *value) {
  *value = array[0] <<  8;
  *value += array[1];
}

void helper::floatToUint8x4(float float_variable, uint8_t *array) { 
  union {
    float a;
    unsigned char bytes[4];
  } value;
  value.a = float_variable;
  memcpy( array, value.bytes, 4);
}

void helper::uint8x4toFloat(uint8_t *array, float *float_variable) { 
  union {
    float a;
    unsigned char bytes[4];
  } value;
  memcpy( value.bytes, array, 4);
  *float_variable = value.a;
}

float helper::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}