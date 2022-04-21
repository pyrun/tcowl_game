#include "camera.h"
#include "helper.h"

using namespace engine;

camera::camera() {
    p_position = { 0, 0};
    setTarget( { 0, 0});
    setSize( { 0, 0});
    setMode( camera_mode::camera_mode_point);

    p_speed = 0.1f;
}

camera::~camera() {

}

void camera::update() {
    switch(p_mode) {
        case camera_mode_point: {
            //p_position = p_target;
            p_position.x = helper::lerp( p_position.x, p_target.x, p_speed);
            p_position.y = helper::lerp( p_position.y, p_target.y, p_speed);
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