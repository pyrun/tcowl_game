#include "body.h"

using namespace physic;

body::body() {
    p_shape = nullptr;
}

void body::cleanup() {
    if( p_shape)
        delete p_shape;
}