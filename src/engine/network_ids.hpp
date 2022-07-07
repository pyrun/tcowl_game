#ifndef GAME_NETWORK_IDS_HPP
#define GAME_NETWORK_IDS_HPP

#include <network/network.hpp>

namespace engine {
    enum network_id {
        // 0...128 system intern
        network_id_object_data = 128,
        network_id_world_data,
    };
};

#endif