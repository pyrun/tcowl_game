#include "camera.hpp"

#include <limits>

#include <engine/helper.hpp>

using namespace engine;

camera::camera() {
    p_position = { 0, 0};
    setTarget( { 0, 0});
    setSize( { 0, 0});
    setMode( camera_mode::camera_mode_point);

    p_speed = 0.1f;

    p_border = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
}

camera::~camera() {

}


void camera::setTarget( fvec2 targetPos) {
    p_target = targetPos - fvec2{ (float)(p_size.x/2), (float)(p_size.y/2)};
    if( p_target.x < 0)
        p_target.x = 0;
    if( p_target.y < 0)
        p_target.y = 0;
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

    if( p_position.x < 0)
        p_position.x = 0;
    if( p_position.y < 0)
        p_position.y = 0;
    if( p_border.x + p_size.x < p_position.x)
        p_position.x = p_border.x + p_size.x;
    if( p_border.y + p_size.y < p_position.y)
        p_position.y = p_border.y + p_size.y;
}