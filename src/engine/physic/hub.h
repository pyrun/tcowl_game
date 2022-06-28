#ifndef HUB_H
#define HUB_H

#include "body.h"
#include <vector>
#include "../graphic.h"

#define ENGINE_PHYSIC_TIME 0.005f

namespace physic {
    class hub : public engine::graphic_object{
        public:
            hub();
            ~hub();

            bool testAABBAABB( const engine::fvec2 &rectPos1, const engine::fvec2 &rectSize1, const engine::fvec2 &rectPos2, const engine::fvec2 &rectSize2);
            engine::fvec2 sweptAABB( engine::fvec2 pos1, engine::fvec2 rect1, engine::fvec2 pos2, engine::fvec2 rect2, engine::fvec2 vel, engine::vec2 *normal);

            void update( float dt);
            void calcPhysic( float dt);
            void reload( engine::graphic_draw *graphic) override { }
            void draw( engine::graphic_draw *graphic) override;

            void add( body* body) { p_bodys.push_back( body); }
            void del( body* body);

            void setDebugLevel( uint8_t level) { p_debug_level = level; }
        private:
            std::vector<body*> p_bodys;
            uint8_t p_debug_level; // Zero = disabled
            float p_dt;
    };
}


#endif