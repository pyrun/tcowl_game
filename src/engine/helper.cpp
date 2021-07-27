#include "helper.h"

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