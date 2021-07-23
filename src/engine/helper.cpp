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