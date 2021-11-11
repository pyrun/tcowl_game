#ifndef PHYSIC_BODY_H
#define PHYSIC_BODY_H

#include "shape.h"

namespace physic {
    class body {
        public:
            body();
            ~body();

            void cleanup();

            void linkShape( shape* shape) { p_shape = shape; }
            void setDensity( float density) { p_density = density; }

            void addPosition( engine::fvec2 value) { p_position += value; }
            void setPosition( engine::fvec2 value) { p_position = value; }

            void addVelocity( engine::fvec2 value) { p_velocity += value; }
            void setVelocity( engine::fvec2 value) { p_velocity = value; }

            engine::fvec2 getPosition() { return p_position; }
            engine::fvec2 getVelocity() { return p_velocity; }
            
            engine::fvec2 *getPositionPtr() { return &p_position; }
            engine::fvec2 *getVelocityPtr() { return &p_velocity; }

            bool getCollied() { return p_collision; }
            void setCollied( bool set) { p_collision = set; }

            shape *getShape() { return p_shape; }
        private:
            engine::fvec2 p_position;
            engine::fvec2 p_velocity;

            bool p_collision;

            shape *p_shape;
            float p_density;
    };
}


#endif