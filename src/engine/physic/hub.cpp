#include "hub.h"

#include "../helper.h"
#include "../log.h"

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
engine::fvec2 hub::sweptAABB(   const engine::fvec2 &pos1, const engine::fvec2 &rect1,
                        const engine::fvec2 &pos2, const engine::fvec2 &rect2,
                        engine::fvec2 velocity,
                        engine::vec2 *normal) {
    engine::fvec2 l_delta_entry;
    engine::fvec2 l_delta_exit;
    *normal = { 0, 0};

    // find the distance between the objects on the near and far sides for both x and y 
    if ( velocity.x > 0.0f) {
        l_delta_entry.x = pos2.x - (pos1.x + rect1.x);  
        l_delta_exit.x = (pos2.x + rect2.x) - pos1.x;
    } else {
        l_delta_entry.x = (pos2.x + rect2.x) - pos1.x;
        l_delta_exit.x = pos2.x - (pos1.x + rect1.x);
    } 
    if ( velocity.y > 0.0f)  { 
        l_delta_entry.y = pos2.y - (pos1.y + rect1.y);  
        l_delta_exit.y = (pos2.y + rect2.y) - pos1.y;  
    } else {
        l_delta_entry.y = (pos2.y + rect2.y - pos1.y);
        l_delta_exit.y = pos2.y - (pos1.y + rect1.y);  
    }

    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
    engine::fvec2 l_time_entry;
    engine::fvec2 l_time_exit;

    if( velocity.x == 0.00f) {
        l_time_entry.x = -std::numeric_limits<float>::infinity(); 
        l_time_exit.x = std::numeric_limits<float>::infinity(); 
    } else {
        l_time_entry.x = l_delta_entry.x / velocity.x; 
        l_time_exit.x = l_delta_exit.x / velocity.x; 
    }

    if( velocity.y == 0.00f)  {
        l_time_entry.y = -std::numeric_limits<float>::infinity(); 
        l_time_exit.y = std::numeric_limits<float>::infinity(); 
    } else {
        l_time_entry.y = l_delta_entry.y / velocity.y; 
        l_time_exit.y = l_delta_exit.y / velocity.y; 
    }

    // find the earliest/latest times of collisionfloat 
    float l_entryTime = std::max( l_time_entry.x, l_time_entry.y);
    float l_exitTime = std::min( l_time_exit.x, l_time_exit.y);

    // if there was no collision -> cancel
    if ( l_entryTime > l_exitTime || l_time_entry.x < 0.0f && l_time_entry.y < 0.0f || l_time_entry.x > 1.0f || l_time_entry.y > 1.0f) {
        return { 1.f, 1.f};
    } // else there was a collision
    
    engine::log( engine::log_debug, "hub::sweptAABB OBJ   %.2f %.2f %.2f %.2f", pos1.x, pos1.y, rect1.x, rect1.y);
    engine::log( engine::log_debug, "hub::sweptAABB OTHER %.2f %.2f %.2f %.2f", pos2.x, pos2.y, rect2.x, rect2.y);
    engine::log( engine::log_debug, "hub::sweptAABB velocity %.8f %.8f", velocity.x, velocity.y);
    engine::log( engine::log_debug, "hub::sweptAABB delta %.8f %.8f", l_delta_entry.x, l_delta_entry.y);
    engine::log( engine::log_debug, "hub::sweptAABB time %.2f %.2f", l_time_entry.x, l_time_entry.y);
    engine::log( engine::log_debug, "hub::sweptAABB entryTime %.10f", l_entryTime);
    engine::log( engine::log_debug, "hub::sweptAABB exitTime %.10f", l_exitTime);

    // calculate normal of collided surface
    engine::fvec2 *l_collide_factor = &l_delta_entry;
    if( l_entryTime < 1e-7f) // Special case if l_entryTime near zero. Happens as soon as the collision position is equal to the position of the object.
        l_collide_factor = &velocity; // This results delta is near zero and leads to calculation error
    if( l_time_entry.x > l_time_entry.y) {// check if x or y
        normal->x = (l_collide_factor->x<0.f)? -1: 1;
        return { l_entryTime, 1.f};
    } else {
        normal->y = (l_collide_factor->y<0.f)? -1: 1;
        return { 1.f, l_entryTime};
    }
}

void hub::update( float dt) {
    float l_friction = 0.25f; //todo woher kommt es? was beeinflusst es
    engine::vec2 l_normal;

    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        engine::fvec2 l_nearest_collisiontime = { 1.f, 1.f};
        l_normal = { 0, 0};
        engine::vec2 l_collision_face = { 0, 0};
        body* l_body = p_bodys[i];

        l_body->setCollied( false);

        // check collision annex aabb
        if( l_body->getShape()) {
            for( uint32_t n = 0; n < p_bodys.size(); n++) {
                physic::body *l_body_other = p_bodys[n];
                if( l_body_other->getShape() == nullptr)
                    continue;
                if( i == n) // nicht gegen sich selbst
                    continue;
                engine::fvec2 l_position_other = l_body_other->getPosition() + l_body_other->getShape()->getOffset();
                engine::fvec2 l_rect_other = l_body_other->getShape()->getAABB();

                // check collision simple
                if( testAABBAABB(   l_body->getPosition() + l_body->getShape()->getOffset() + (l_body->getVelocity() * dt), l_body->getShape()->getAABB(),
                                    l_position_other, l_rect_other)) {
                    l_body->setCollied( true);
                    // calculate collision
                    engine::fvec2 l_collisiontime = sweptAABB( l_body->getPosition() + l_body->getShape()->getOffset(), l_body->getShape()->getAABB(), // obj
                                                        l_position_other, l_rect_other, // other obj
                                                        l_body->getVelocity() * dt, // vel
                                                        &l_normal); // return normal
                    
                    // check if this collison is earlier than the previous one, if yes give them priority
                    if( l_nearest_collisiontime.x >= l_collisiontime.x)
                        l_nearest_collisiontime.x  = l_collisiontime.x;
                    if( l_nearest_collisiontime.y >= l_collisiontime.y)
                        l_nearest_collisiontime.y  = l_collisiontime.y;
                    
                    // see which side is affected and set it
                    l_collision_face.x = l_normal.x==0.f?l_collision_face.x:l_normal.x;
                    l_collision_face.y = l_normal.y==0.f?l_collision_face.y:l_normal.y;

                    engine::log( engine::log_debug, "hub::update %d %d", l_normal.x, l_normal.y);
                }
            }
        }

        // velocity
        l_body->addPosition( { l_body->getVelocity().x * dt * l_nearest_collisiontime.x, 
                               l_body->getVelocity().y * dt * l_nearest_collisiontime.y} );

        // Auf face reagieren
        l_body->setVelocity( {  l_collision_face.x?0.f:l_body->getVelocity().x,
                                l_collision_face.y?0.f:l_body->getVelocity().y} );

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