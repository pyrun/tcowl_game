#include "app.h"

#include <math.h>
#include "../engine/vec.h"

using namespace game;
using json = nlohmann::json;

app::app() {

}

app::~app() {

}

void app::begin( bool server) {
    // settings
    p_config.load();
    p_font_setting = { .size{ p_config.getVec2("font-size", engine::vec2{ 7, 9}) } };
    p_graphic.setTitle( p_config.getString( "windows-title-name", "The Commemoration of White Light"));
    p_serverorclient = server;
    p_framerate_cap = p_config.get<float>("framerate-cap");

    // statup
    p_graphic.init();
    p_input.init();
    p_font.init( &p_graphic, p_font_setting);
    p_entity.init( &p_types);

    // draw order
    p_graphic.addObject( &p_world);
    p_graphic.addObject( &p_entity);
    p_graphic.addObject( &p_lobby);

    // load files
    p_types.loadFolder( &p_graphic, "entity");
    p_world.begin( &p_graphic);

    p_lobby.init( &p_font, p_input.getInputMap(), &p_entity);

    // network
    if( p_serverorclient) {
        p_server.begin();
        p_server.addSync( &p_entity);

        int16_t l_id = p_entity.createObject(1);
        p_entity.setPosition( l_id, { 100, 100});

        l_id = p_entity.createObject(1);
        p_entity.setPosition( l_id, { 100, 100});

        
        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 140, 102});

        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 160, 100});
        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 180, 100});
        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 120, 100});

        l_id = p_entity.createObject(4);
        p_entity.setPosition( l_id, { 150, 50});

        l_id = p_entity.createObject(10);
        p_entity.setPosition( l_id, { 50, 50});

        p_entity.deleteObject( 0);

        /*int16_t l_id = p_entity.createObject(2);
        p_entity.get( l_id)->position = { 100, 100};

        l_id = p_entity.createObject(1);
        p_entity.get( l_id)->position = { 100, 40};
        l_id = p_entity.createObject(1);
        p_entity.get( l_id)->position = { 150, 50};
        for( int32_t i = 0, n = 0; i < 10; i++) {
            if( (i%32) == 0)
                n++;
            int16_t l_test = p_entity.createObject(1);
            if( l_test > 0)
                p_entity.get( l_test)->position = engine::vec2{ 10*(i%32), 10*n} + engine::vec2{ 30, 30};
        }*/
    } else {
        p_client.begin();
        p_client.addSync( &p_entity);
    }

    p_physics_lastime = SDL_GetTicks();
    p_animation_lasttime = SDL_GetTicks();
}

bool app::update() {
    uint64_t l_startPerf = SDL_GetPerformanceCounter();
    uint64_t l_endPerf, l_framePerf;

    // check events
    p_input.update();
    if( p_input.getEvents()->quit)
        return false;
    if( p_input.getEvents()->windows_size_change) {
        p_input.getEvents()->windows_size_change = false;
        p_graphic.init();
    }
    if( p_input.getInputMap()->verbose_level[0])
        p_graphic.addObject( p_entity.getPhysicHub());
    else
        p_graphic.delObject( p_entity.getPhysicHub());

    if( p_serverorclient)
        p_server.update();
    else
        p_client.update();
    
    // physics
    uint32_t l_physics_current = SDL_GetTicks();
    float l_delta = (l_physics_current - p_physics_lastime) / 1000.0f;
    p_entity.update( l_delta, &p_world);
    p_physics_lastime = l_physics_current;

    // rendering
    p_graphic.update( SDL_GetTicks()-p_animation_lasttime);
    p_animation_lasttime = SDL_GetTicks();

    // fps cap
    l_endPerf = SDL_GetPerformanceCounter();
    l_framePerf = l_endPerf - l_startPerf;

    // calculate delay
    float l_elapsedMS = (l_framePerf) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    float l_delay = floor(p_framerate_cap - l_elapsedMS);
    if( l_delay < 1)
        l_delay = 1;
    SDL_Delay( l_delay);
    return true;
}

void app::close() {
    p_config.save();
    p_server.close();
}