#ifndef GAME_NETWORK_IDS_H
#define GAME_NETWORK_IDS_H

#include "../network/network.h"

namespace engine {
    enum network_id {
        // 0...128 system intern
        network_id_object_data = 128,
        network_id_world_data,
    };
};

#endif