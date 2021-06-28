#include "lobby.h"

using namespace engine;
using namespace game;

lobby::lobby() {
    p_font = NULL;
}

lobby::~lobby() {
    
}

void lobby::init( engine::font *font) {
    p_font = font;
}

void lobby::draw( engine::graphic_draw *graphic) {
    if( !p_font)
        return;
    p_font->draw( "{dies ist ein sehr langer test} mit äöü ß € und anderen sonderzeichen/()", vec2{ 100, 100});
}