#ifndef CAMERA_H
#define CAMERA_H

#include "vec.h"

namespace engine {
    enum camera_mode : uint32_t {
        camera_mode_point = 0, // always tries to reach point
        camera_mode_rect, // inside the box is not moved as long as you do not leave it
        camera_mode_rect_center, // as soon as you leave the rectangle, it is centred around the point of exit.
    };

    class camera {
        public:
            camera();
            ~camera();

            void update();

            void setMode( camera_mode mode) { p_mode = mode; }
            vec2 getPosition() { return p_position; }

            // centre of screen
            void setTarget( vec2 targetPos) { p_target = targetPos - ( p_size / vec2{ 2, 2} ); }
            void setSize( vec2 size) { p_size = size; }
            vec2 getSize() { return p_size; }
        private:
            vec2 p_position;
            vec2 p_size;

            vec2 p_target;
            camera_mode p_mode;
    };
}

#endif