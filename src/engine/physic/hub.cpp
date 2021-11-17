#include "hub.h"

#include "../helper.h"

#include <iostream>
#include <limits>

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

// SweptAABB(Box b1, Box b2, float& normalx, float& normaly)
float hub::sweptAABB(   const engine::fvec2 &pos1, const engine::fvec2 &rect1,
                        const engine::fvec2 &pos2, const engine::fvec2 &rect2,
                        engine::fvec2 vel,
                        engine::vec2 *normal) {
    engine::fvec2 l_invEntry;
    engine::fvec2 l_invExit;

    // find the distance between the objects on the near and far sides for both x and y 
    if ( vel.x > 0.0f) {
        l_invEntry.x = pos2.x - (pos1.x + rect1.x);  
        l_invExit.x = (pos2.x + rect2.x) - pos1.x;
    } else {
        l_invEntry.x = (pos2.x + rect2.x) - pos1.x;  
        l_invExit.x = pos2.x - (pos1.x + rect1.x);  
    } 
    if ( vel.y > 0.0f)  { 
        l_invEntry.y = pos2.y - (pos1.y + rect1.y);  
        l_invExit.y = (pos2.y + rect2.y) - pos1.y;  
    } else {
        l_invEntry.y = (pos2.y + rect2.y) - pos1.y;  
        l_invExit.y = pos2.y - (pos1.y + rect1.y);  
    }

    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
    engine::fvec2 l_entry;
    engine::fvec2 l_exit;

    if( vel.x == 0.0f) {
        l_entry.x = -std::numeric_limits<float>::infinity(); 
        l_exit.x = std::numeric_limits<float>::infinity(); 
    } else {
        l_entry.x = l_invEntry.x / vel.x; 
        l_exit.x = l_invExit.x / vel.x; 
    }

    if( vel.y == 0.0f)  {
        l_entry.y = -std::numeric_limits<float>::infinity(); 
        l_exit.y = std::numeric_limits<float>::infinity(); 
    } else {
        l_entry.y = l_invEntry.y / vel.y; 
        l_exit.y = l_invExit.y / vel.y; 
    }

    // find the earliest/latest times of collisionfloat 
    float l_entryTime = std::max( l_entry.x, l_entry.y); 
    float l_exitTime = std::min( l_exit.x, l_exit.y);

    // if there was no collision
    if ( l_entryTime > l_exitTime || l_entry.x < 0.0f && l_entry.y < 0.0f || l_entry.x > 1.0f || l_entry.y > 1.0f) {
        normal->x = 0;
        normal->y = 0;
        return 1.0f; 
    } else {// if there was a collision 
        // calculate normal of collided surface
        if( l_entry.x > l_entry.y) {
            if ( l_invEntry.x < 0.0f) {
                normal->x = 1; 
                normal->y = 0; 
            } else {
                normal->x = -1; 
                normal->y = 0;
            }
        } else {
            if ( l_invEntry.y < 0.0f) {
                normal->x = 0; 
                normal->y = 1; 
            } else {
                normal->x = 0; 
                normal->y = -1; 
            }
        }
    }
    return l_entryTime; 
}

void hub::update( float dt) {
    float l_friction = 0.25f; //todo woher kommt es? was beeinflusst es
    engine::vec2 l_normal;

    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        float l_collisiontime = 1.0f;
        engine::vec2 l_face = { 0, 0};
        body* l_body = p_bodys[i];

        l_body->setCollied( false);

        // check collision annex aabb
        if( l_body->getShape()) {
            for( uint32_t n = 0; n < p_bodys.size(); n++) {
                if( testAABBAABB(   l_body->getPosition() + l_body->getShape()->getOffset() + (l_body->getVelocity() * dt), l_body->getShape()->getAABB(),
                                    p_bodys[n]->getPosition() + p_bodys[n]->getShape()->getOffset(), p_bodys[n]->getShape()->getAABB()) &&
                                    i != n) {// nicht gegen sich selbst
                    l_body->setCollied( true);
                    float l_collisiontime2 = sweptAABB(    l_body->getPosition() + l_body->getShape()->getOffset(), l_body->getShape()->getAABB(),
                                                        p_bodys[n]->getPosition() + p_bodys[n]->getShape()->getOffset(), p_bodys[n]->getShape()->getAABB(),
                                                        (l_body->getVelocity() * dt),
                                                        &l_normal);
                    
                    // Prüfen ob relavant
                    if( l_collisiontime2 < l_collisiontime) {
                            l_collisiontime  = l_collisiontime2;
                        
                        // wert behandeln
                        l_face.x = l_normal.x?l_normal.x:l_face.x;
                        l_face.y = l_normal.y?l_normal.y:l_face.y;
                    }
                }
            }
        }

        // velocity
        l_body->addPosition( l_body->getVelocity() * dt * l_collisiontime);

        // Auf face reagieren
        l_body->setVelocity( {  l_face.x?0.f:l_body->getVelocity().x,
                                l_face.y?0.f:l_body->getVelocity().y} );

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