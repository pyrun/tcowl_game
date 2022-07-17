local rooms = {}

function AddRoom( x, y, w, h)
    table.insert( rooms, { x = x, y = y, w = w, h = h})
end

function CheckSpaceRoom( x, y, w, h, space)
    -- Check world border
    local world_size_x, world_size_y = tcowl.world.getSize()
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

function Intilisation( try)
    print("Biom grass script")

    local world_size_x, world_size_y = tcowl.world.getSize()

    local room_min_length = 4
    local room_max_length = 20

    for i = 1, 2024 do
        local room = {}
        room.x = math.random(0,world_size_x)
        room.y = math.random(0,world_size_y)
        room.w = math.random( room_min_length, room_max_length)
        room.h = math.random( room_min_length, room_max_length)

        local result = CheckSpaceRoom( room.x, room.y, room.w, room.h, 1)
        if result then
            AddRoom( room.x, room.y, room.w, room.h)
        end
    end
end

function Generation( w, h)

    for x = 0, w-1 do
        for y = 0, h-1 do
            tcowl.world.setTile( "earth", x, y)
        end
    end



    --for i, room in pairs(rooms) do
    --    if (room.x <= x and room.y <= y and room.x+room.w > x and room.y+room.h > y) then
    --        tcowl.world.setTile( "tree", x, y-1)
    --        tcowl.world.setTile( "grass", x, y)
    --    end
    --end
end