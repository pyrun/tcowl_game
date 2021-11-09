#include "hub.h"

#include "../helper.h"

using namespace physic;

hub::hub() {

}

hub::~hub() {

}

void hub::update( float dt) {
    float l_friction = 0.25f; //todo woher kommt es? was beeinflusst es

    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        body* l_body = p_bodys[i];

        // velocity
        l_body->addPosition( l_body->getVelocity() * dt);

        // friction
        l_body->setVelocity( {  helper::lerp( l_body->getVelocity().x, 0, l_friction),
                                helper::lerp( l_body->getVelocity().y, 0, l_friction)});
    }
}

void hub::del( body* body) {
    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        if( p_bodys[i] == body) {
            p_bodys.erase( p_bodys.begin()+i);
            return;
        }
    }
}