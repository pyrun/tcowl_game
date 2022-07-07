#ifndef CAMERA_H
#define CAMERA_H

#include <engine/vec.hpp>

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
            fvec2 getPosition() { return p_position; }

            void setTarget( fvec2 targetPos);
            void setSize( fvec2 size) { p_size = size; }
            fvec2 getSize() { return p_size; }

            void setBorder( fvec2 border) { p_border = border; }
        private:
            fvec2 p_position;
            fvec2 p_size;
            fvec2 p_border;

            fvec2 p_target;
            camera_mode p_mode;

            float p_speed;
    };
}

#endif