#ifndef ENTITY_INVENTORY_HPP
#define ENTITY_INVENTORY_HPP

#define ENTITY_INVENTORY_MAXSIZE_W 10
#define ENTITY_INVENTORY_MAXSIZE_H 5

namespace engine {
    struct inventory {
        int32_t map[ ENTITY_INVENTORY_MAXSIZE_W * ENTITY_INVENTORY_MAXSIZE_H];
    };
}

#endif
