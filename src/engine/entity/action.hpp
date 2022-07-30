#ifndef ENTITY_ACTION_HPP
#define ENTITY_ACTION_HPP

#include <string>
#include <vector>
#include <stdint.h>
#include <engine/vec.hpp>

namespace engine {
    struct action {
        uint32_t id; // Zuordnung
        std::string name;
        vec2 postion; // Wo im Bild das erste anfängt
        vec2 size; // Größe w x h
        uint32_t delay; // Nach wie vielen Ticks weiter gesprungen wird
        uint32_t length; // Anzahl der Bilder
        bool bind_velocity; // Anpassen der Animationsgeschwindigkeit an die Geschwindigkeit

        bool flip_vertical = false;
        bool flip_horizontal = false;
    };
}

#endif