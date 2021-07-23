#ifndef HELPER_H
#define HELPER_H

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

namespace helper {
    extern bool file_exists ( const char *filename);
    extern int isDirectory(const char *path);
};

#endif