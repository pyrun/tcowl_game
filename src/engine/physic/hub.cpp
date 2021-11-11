#include "hub.h"

#include "../helper.h"

using namespace physic;

hub::hub() {

}

hub::~hub() {

}

bool hub::testAABBAABB( const engine::fvec2 &rectPos1, const engine::fvec2 &rectSize1,
                        const engine::fvec2 &rectPos2, const engine::fvec2 &rectSize2) {
    if( rectPos1.x < rectPos2.x + rectSize2.x &&
        rectPos1.x + rectSize1.x > rectPos2.x &&
        rectPos1.y < rectPos2.y + rectSize2.y &&
        rectPos1.y + rectSize1.y > rectPos2.y)
        return true;
    return false;
};

void hub::update( float dt) {
    float l_friction = 0.25f; //todo woher kommt es? was beeinflusst es

    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        body* l_body = p_bodys[i];

        l_body->setCollied( false);

        // check collision annex aabb
        if( l_body->getShape()) {
            for( uint32_t n = 0; n < p_bodys.size(); n++) {
                if( testAABBAABB(   l_body->getPosition() + l_body->getShape()->getOffset(), l_body->getShape()->getAABB(),
                                    p_bodys[n]->getPosition() + p_bodys[n]->getShape()->getOffset(), p_bodys[n]->getShape()->getAABB()) &&
                    i != n)
                    l_body->setCollied( true);
            }
        }

        // velocity
        l_body->addPosition( l_body->getVelocity() * dt);

        // friction
        l_body->setVelocity( {  helper::lerp( l_body->getVelocity().x, 0, l_friction),
                                helper::lerp( l_body->getVelocity().y, 0, l_friction)});
    }
}

void hub::draw( engine::graphic_draw *graphic) {
    // Render pipeline
    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        body* l_body = p_bodys[i];
        graphic->setDrawColor( l_body->getCollied()?255:0, 255, 0, 255);
        if( l_body->getShape())
            l_body->getShape()->draw( graphic, l_body->getPosition());
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