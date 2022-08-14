#include "hub.hpp"

#include <iostream>
#include <limits>

#include <engine/helper.hpp>
#include <engine/log.hpp>


using namespace engine;
using namespace physic;

bool physic::testAABBAABB(  const engine::fvec2 &rectPos1, const engine::fvec2 &rectSize1,
                    const engine::fvec2 &rectPos2, const engine::fvec2 &rectSize2) {
    float l_left = rectPos2.x - ( rectPos1.x + rectSize1.x);
    float l_top = (rectPos2.y + rectSize2.y) - rectPos1.y;
    float l_right = (rectPos2.x + rectSize2.x) - rectPos1.x;
    float l_bottom = rectPos2.y - ( rectPos1.y + rectSize1.y);

    return !(l_left > 0|| l_right < 0 || l_top < 0 || l_bottom > 0);
};

// SweptAABB(Box b1, Box b2, float& normalx, float& normaly)
engine::fvec2 physic::sweptAABB(   engine::fvec2 pos1, engine::fvec2 rect1,
                        engine::fvec2 pos2, engine::fvec2 rect2,
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
        l_delta_entry.y = (pos2.y + rect2.y) - pos1.y;
        l_delta_exit.y = pos2.y - (pos1.y + rect1.y);  
    }

    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
    engine::fvec2 l_time_entry;
    engine::fvec2 l_time_exit;

    if( velocity.x == 0.f) {
        l_time_entry.x = -std::numeric_limits<float>::infinity(); 
        l_time_exit.x = std::numeric_limits<float>::infinity(); 
    } else {
        l_time_entry.x = l_delta_entry.x / velocity.x; 
        l_time_exit.x = l_delta_exit.x / velocity.x; 
    }

    if( velocity.y == 0.f)  {
        l_time_entry.y = -std::numeric_limits<float>::infinity(); 
        l_time_exit.y = std::numeric_limits<float>::infinity(); 
    } else {
        l_time_entry.y = l_delta_entry.y / velocity.y; 
        l_time_exit.y = l_delta_exit.y / velocity.y; 
    }

    // find the earliest/latest times of collisionfloat 
    float l_entryTime = std::max( l_time_entry.x, l_time_entry.y);
    float l_exitTime = std::min( l_time_exit.x, l_time_exit.y);

    if( l_entryTime > l_exitTime ||
        l_time_entry.x > 1.0f||
        l_time_entry.y > 1.0f)
        return { 1.f, 1.f};
    if( l_time_entry.x < 0.0f && l_time_entry.y < 0.0f)
        return { 1.f, 1.f};
    if( l_time_entry.x < 0.0f)
        if (pos1.x+rect1.x <= pos2.x || pos1.x >= pos2.x+rect2.x)
            return { 1.f, 1.f};
    if( l_time_entry.y < 0.0f)
        if (pos1.y+rect1.y <= pos2.y || pos1.y >= pos2.y+rect2.y)
            return { 1.f, 1.f};
    
    /*if( p_debug_level > 0) { TODO add Debug
        engine::log( engine::log_debug, "hub::sweptAABB pos1,rect1 %3.2f %3.2f %3.2f %3.2f", pos1.x, pos1.y, rect1.x, rect1.y);
        engine::log( engine::log_debug, "hub::sweptAABB pos2,rect2 %3.2f %3.2f %3.2f %3.2f", pos2.x, pos2.y, rect2.x, rect2.y);
        engine::log( engine::log_debug, "hub::sweptAABB velocity %.16f %.16f", velocity.x, velocity.y);
        engine::log( engine::log_debug, "hub::sweptAABB l_delta_entry %.8f %.8f", l_delta_entry.x, l_delta_entry.y);
        engine::log( engine::log_debug, "hub::sweptAABB l_time_entry %.2f %.2f", l_time_entry.x, l_time_entry.y);
        engine::log( engine::log_debug, "hub::sweptAABB l_entryTime %.10f", l_entryTime);
        engine::log( engine::log_debug, "hub::sweptAABB l_exitTime %.10f", l_exitTime);
    }*/

    // calculate normal of collided surface
    if( l_time_entry.x > l_time_entry.y) {// check if x or y
        normal->x = (velocity.x<0.f)? -1: 1;
        if( normal->x == -1) // Avoid glitch with nagative numbers - origin not fully understood
            l_entryTime -= std::nextafter(1.0f, 0.0f);
        return { l_entryTime, 1.f};
    } else {
        normal->y = (velocity.y<0.f)? -1: 1;
        if( normal->y == -1) // Avoid glitch with nagative numbers - origin not fully understood
            l_entryTime -= std::nextafter(1.0f, 0.0f);
        return { 1.f, l_entryTime};
    }
}

hub::hub() {
    setDebugLevel(0);
    p_dt = 0;
}

hub::~hub() {

}

void hub::update( float dt) {
    // dt normalisieren
    p_dt += dt;
    if( p_dt > ENGINE_PHYSIC_TIME) {
        float l_dt = 0;

        // Länge ermitteln
        while( l_dt < p_dt)
            l_dt += ENGINE_PHYSIC_TIME;
        
        // ausführen
        calcPhysic( l_dt);
        p_dt -= l_dt;
    }
}

void hub::calcPhysic( float dt) {
    float l_friction = 0.25f; //todo woher kommt es? was beeinflusst es
    engine::vec2 l_normal;

    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        engine::fvec2 l_nearest_collisiontime = { 1.f, 1.f};
        l_normal = { 0, 0};
        engine::vec2 l_collision_face = { 0, 0};
        body* l_body = p_bodys[i];

        l_body->collision = false;

        // check collision annex aabb
        if( l_body->shape) {
            for( uint32_t n = 0; n < p_bodys.size(); n++) {
                physic::body *l_body_other = p_bodys[n];
                if( l_body_other->shape == nullptr)
                    continue;
                if( i == n) // nicht gegen sich selbst
                    continue;
                engine::fvec2 l_position_other = l_body_other->position + l_body_other->shape->getOffset();
                engine::fvec2 l_rect_other = l_body_other->shape->getAABB();

                engine::fvec2 l_vel_dt = l_body->velocity * dt;

                if( l_body->velocity.x == 0.f && l_body->velocity.y == 0.f)
                    continue;

                // check collision simple
                if( testAABBAABB(
                    l_body->position + l_body->shape->getOffset() + engine::fvec2{ l_vel_dt.x<0.f?l_vel_dt.x:0.f, l_vel_dt.y<0.f?l_vel_dt.y:0.f },
                    l_body->shape->getAABB() + engine::fvec2{ fabs( l_vel_dt.x), fabs( l_vel_dt.y)},
                    l_position_other,
                    l_rect_other
                )) {
                    // calculate collision
                    engine::fvec2 l_collisiontime = sweptAABB( l_body->position + l_body->shape->getOffset(), // pos
                                                        l_body->shape->getAABB(), // size
                                                        l_position_other, // pos obj
                                                        l_rect_other, // size obj
                                                        l_body->velocity * dt, // vel
                                                        &l_normal); // return normal
                    
                    // check if this collison is earlier than the previous one, if yes give them priority
                    bool l_collision = false;


                    if( l_nearest_collisiontime.x > l_collisiontime.x) {
                        l_nearest_collisiontime.x  = l_collisiontime.x;
                        l_collision = true;
                    }
                    if( l_nearest_collisiontime.y > l_collisiontime.y) {
                        l_nearest_collisiontime.y  = l_collisiontime.y;
                        l_collision = true;
                    }
                    
                    // see which side is affected and set it
                    if( l_collision) {
                        l_body->collision = true;
                        l_collision_face = l_normal;

                        if( p_debug_level)
                            engine::log( engine::log_debug, "hub::update %d %d %d with %d, l_nearest_collisiontime %1.4f %1.4f", l_normal.x, l_normal.y, i, n, l_nearest_collisiontime.x, l_nearest_collisiontime.y);
                    }
                }
            }
        }

        // velocity
        l_body->position = l_body->position + fvec2{l_body->velocity.x * dt * l_nearest_collisiontime.x, l_body->velocity.y * dt * l_nearest_collisiontime.y};

        // Auf face reagieren
        l_body->velocity = { l_collision_face.x?0.f:l_body->velocity.x, l_collision_face.y?0.f:l_body->velocity.y};

        // friction
        l_body->velocity = { helper::lerp( l_body->velocity.x, 0, l_friction), helper::lerp( l_body->velocity.y, 0, l_friction)};
        
        // if value is too low set zero
        if( fabs( l_body->velocity.x) < 0.01f)
            l_body->velocity.x = 0.f;
        if( fabs( l_body->velocity.y) < 0.01f)
            l_body->velocity.y = 0.f;
    }
}

void hub::draw( engine::graphic_draw *graphic) {
    // Render pipeline
    for( uint32_t i = 0; i < p_bodys.size(); i++) {
        body* l_body = p_bodys[i];
        graphic->setDrawColor( l_body->collision?255:0, 255, 0, 255);
        if( l_body->shape)
            l_body->shape->draw( graphic, l_body->position);
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