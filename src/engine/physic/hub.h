#ifndef HUB_H
#define HUB_H

#include "body.h"
#include <vector>
#include "../graphic.h"

namespace physic {
    class hub : public engine::graphic_object{
        public:
            hub();
            ~hub();

            void update( float dt);
            void draw( engine::graphic_draw *graphic) override;

            void add( body* body) { p_bodys.push_back( body); }
            void del( body* body);
        private:
            std::vector<body*> p_bodys;
    };
}


#endif