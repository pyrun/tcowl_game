#include "body.h"

using namespace physic;

body::body() {
    p_collision = false;

    p_position = { 0, 0};
    p_velocity = { 0, 0};
    
    p_shape = nullptr;
}

void body::cleanup() {
    if( p_shape)
        delete p_shape;
}