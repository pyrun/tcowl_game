#include "body.h"

using namespace physic;

body::body() {
    p_shape = nullptr;

    p_position = { 0, 0};
    p_velocity = { 0, 0};
}

void body::cleanup() {
    if( p_shape)
        delete p_shape;
}