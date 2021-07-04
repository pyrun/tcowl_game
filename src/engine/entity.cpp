#include "entity.h"

using namespace engine;

entity_handler::entity_handler() {

}

entity_handler::~entity_handler() {

}

void entity_handler::draw( engine::graphic_draw *graphic) {
    
    /*static vec2 pos = { 100, 100};
    if( p_input->x < 0)
        pos.x -= 1;
    if( p_input->x > 0)
        pos.x += 1;
    if( p_input->y < 0)
        pos.y -= 1;
    if( p_input->y > 0)
        pos.y += 1;
    
    p_font->print( { 100, 50}, "test %d %d %d amount %d", p_input->x, p_input->y, graphic->getDisplacedElements(), p_type->getAmount());

    p_font->draw( "{dies ist ein sehr langer test} mit ��� ? und anderen sonderzeichen/()", pos);*/
}

int32_t entity_handler::createObject( std::string name) {
    /*for( uint32_t i = 0; i < p_type.size(); i++)
        if( strcmp( p_type[i].getName(), name.c_str()) == 0)
            return createObject( p_type[i].getId());*/
    return -1;
}
int32_t entity_handler::createObject( uint32_t id) {
    /*type *l_type = NULL;
    for( uint32_t i = 0; i < p_type.size(); i++)
        if( p_type[i].getId() == id)
            l_type = &p_type[i];
    if( l_type == NULL)*/
        return -1;
    
}