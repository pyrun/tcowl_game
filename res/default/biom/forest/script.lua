rooms = {}

function addRoom( x, y, w, h)
    table.insert( rooms, { x = x, y = y, w = w, h = h})
end

function checkSpaceRoom( x, y, w, h, space)
    -- Check world border
    world_size_x, world_size_y = world.getSize()
    if space > x or space > y or world_size_x - space < x+w or world_size_y - space < y+h then
        return false
    end

    -- Check whether room is colled with other rooms
    for i, room in pairs(rooms) do
        if ((x >= room.x + room.w + space) or
            (x + w <= room.x - space) or
            (y >= room.y + room.h + space) or
            (y + h <= room.y - space)) then
        else
            return false
        end
    end
    return true
end

function intilisation( try)
    print("Biom grass script")

    world_size_x, world_size_y = world.getSize()

    room_min_length = 2
    room_max_length = 10

    for i = 1, 5000 do
        room = {}
        room.x = math.random(0,world_size_x)
        room.y = math.random(0,world_size_y)
        room.w = math.random( room_min_length, room_max_length)
        room.h = math.random( room_min_length, room_max_length)

        result = checkSpaceRoom( room.x, room.y, room.w, room.h, 1)
        if result then
            addRoom( room.x, room.y, room.w, room.h)
        end
    end
end

function set( x, y, value)
    world.setTile( 2, x, y)

    for i, room in pairs(rooms) do
        if (room.x <= x and room.y <= y and room.x+room.w > x and room.y+room.h > y) then
            world.setTile( 1, x, y)
        end
    end
end