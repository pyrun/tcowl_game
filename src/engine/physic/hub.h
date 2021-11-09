#ifndef HUB_H
#define HUB_H

#include "body.h"
#include <vector>

namespace physic {
    class hub {
        public:
            hub();
            ~hub();

            void update( float dt);

            void add( body* body) { p_bodys.push_back( body); }
            void del( body* body);
        private:
            std::vector<body*> p_bodys;
    };
}


#endif