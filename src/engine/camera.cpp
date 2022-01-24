#include "camera.h"

using namespace engine;

camera::camera() {
    p_position = { 0, 0};
    setTarget( { 0, 0});
    setSize( { 0, 0});
    setMode( camera_mode::camera_mode_point);
}

camera::~camera() {

}

void camera::update() {
    switch(p_mode) {
        case camera_mode_point: {
            p_position = p_target;
        } break;
        case camera_mode_rect: {
            p_position = p_target;
        } break;
        case camera_mode_rect_center: { 
            p_position = p_target;
        } break;
        default: {
        } break;
    }
}