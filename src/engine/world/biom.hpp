#ifndef BIOM_HPP
#define BIOM_HPP

#include <string>
#include <engine/script.hpp>
#include <engine/graphic/image.hpp>

namespace engine {
    struct biom {
        uint16_t id;
        std::string name;

        std::string script_file;
        lua_State *lua = nullptr;

        std::string battle_bg_file;
        image battle_bg;
    };
};

#endif