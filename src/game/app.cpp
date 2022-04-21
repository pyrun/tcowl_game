#include "app.h"

#include <math.h>
#include "../engine/vec.h"
#include "../engine/helper_json.h"

using namespace game;
using json = nlohmann::json;

app::app() {
    p_network = NULL;
}

app::~app() {

}

void app::begin() {
    // settings
    p_config.load( APP_CONFIG_FILE);
    p_font_setting = { .size{ p_config.getVec2("font-size", engine::vec2{ 7, 9}) } };
    p_graphic.setTitle( p_config.get<std::string>( "windows-title-name", "The Commemoration of White Light"));
    p_framerate_cap = p_config.get<float>("framerate-cap", 16.f);

    // statup
    p_graphic.init();
    p_input.init();
    p_input.add( new engine::input_keyboard);
    p_font.init( &p_graphic, p_font_setting);
    p_entity.init( &p_types);

    // draw order
    if( !p_argv.get<bool>("hide_world", false))
        p_graphic.addObject( &p_world);
    if( !p_argv.get<bool>("hide_entity", false))
        p_graphic.addObject( &p_entity);

    // loads packs
    std::string *l_name = helper::json::getNumberArray<std::string>( p_argv.getJson(), "packs", 0xFF);
    uint32_t l_index = 0;
    while( l_name &&
           !l_name[l_index].empty()) {
        std::string l_folder = l_name[l_index];
        l_index++;
        // load files
        p_types.loadFolder( &p_graphic, l_folder +"/"+ "entity");
        p_tileset.loadFolder( &p_graphic, l_folder +"/"+ "tile");
        p_bioms.loadFolder( l_folder +"/"+ "biom");
    }

    // first world init
    p_world.begin( &p_graphic, &p_tileset, &p_bioms);

    // network
    app_config_network l_net_type = p_argv.get<app_config_network>("network_type", app_config_network_offline);
    if( l_net_type == app_config_network_offline ||
        l_net_type == app_config_network_server) {
        int16_t l_id = p_entity.createObject(1);
        p_entity.setPosition( l_id, { 100, 100});

        l_id = p_entity.createObject(1);
        p_entity.setPosition( l_id, { 100, 100});

        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 50, 102});

        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 160, 50});
        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 180, 30});
        l_id = p_entity.createObject(3);
        p_entity.setPosition( l_id, { 120, 100});

        l_id = p_entity.createObject(4);
        p_entity.setPosition( l_id, { 150, 50});

        l_id = p_entity.createObject(10);
        p_entity.setPosition( l_id, { 50, 50});

        l_id = p_entity.createObject(5);
        p_entity.setPosition( l_id, { 100, 150});

        p_entity.deleteObject( 0);
    }
    if( l_net_type == app_config_network_server)
        p_network = new network::server();
    if( l_net_type == app_config_network_client)
        p_network = new network::client_connection();

    if( p_network) {
        p_network->begin();
        p_network->addSync( &p_entity);
    }

    p_physics_lastime = SDL_GetTicks();
    p_animation_lasttime = SDL_GetTicks();

    // player
    p_player.begin( &p_font, p_input.get(0), &p_entity);
    p_graphic.addObject( &p_player);

    // Update Once
    update();


    std::vector<uint8_t> l_data = p_world.getRawData();

    /*for( size_t i = 0; i < l_data.size(); i+=5) {
        printf("%02x ", l_data[i] );
        printf("%02x\n", l_data[i+1] );
    }*/
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
    if( p_input.get(0)->edgeDetection( engine::input_key_edge_detection_down, engine::input_buttons::input_buttons_special_0)) {
        if( p_graphic.checkObject(p_entity.getPhysicHub()))
            p_graphic.delObject( p_entity.getPhysicHub());
        else
            p_graphic.addObject( p_entity.getPhysicHub());
    }
    if( p_input.get(0)->edgeDetection( engine::input_key_edge_detection_down, engine::input_buttons::input_buttons_special_1)) {
        if( p_graphic.checkObject(&p_world))
            p_graphic.delObject(&p_world);
        else
            p_graphic.addObject(&p_world);
    }
    
    if( p_input.get(0)->edgeDetection( engine::input_key_edge_detection_down, engine::input_buttons::input_buttons_special_7)) {
        p_graphic.reload();
    }

    // Update
    p_player.update();
    if( p_network)
        p_network->update();
    p_world.update();
    
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
    p_config.save( APP_CONFIG_FILE);
    if( p_network)
        p_network->close();
}