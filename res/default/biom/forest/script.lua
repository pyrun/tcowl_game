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
    print("Forest grass script")

    local world_size_x, world_size_y = tcowl.world.getSize()

    local room_min_length = 4
    local room_max_length = 7

    AddRoom( 16, 16, 5, 5)
    local last_room = rooms[#rooms]


    for i = 1, 500 do
        local room = {}
        local dir = math.random(1,4)
        local last_room = rooms[math.random(#rooms)]
        local result

        room.w = math.random( room_min_length, room_max_length)
        room.h = math.random( room_min_length, room_max_length)

        if dir == 1 then
            room.x = last_room.w+last_room.x+1
            room.y = last_room.y
            result = CheckSpaceRoom( room.x, room.y, room.w, room.h, 1)
            if result then
                tcowl.world.setTile( "grass", room.x-1, room.y+math.random(math.min(room.h, last_room.h)-1))
            end
        elseif dir == 2 then
            room.x = last_room.x
            room.y = last_room.h+last_room.y+1
            result = CheckSpaceRoom( room.x, room.y, room.w, room.h, 1)
            if result then
                tcowl.world.setTile( "grass", room.x+math.random(math.min(room.w, last_room.w))-1, room.y-1)
            end
        elseif dir == 3 then
            room.x = last_room.x - 1 - room.w
            room.y = last_room.y
            result = CheckSpaceRoom( room.x, room.y, room.w, room.h, 1)
            if result then
                tcowl.world.setTile( "grass", room.x+room.w, room.y+math.random(math.min(room.h, last_room.h)-1))
            end
        elseif dir == 4 then
            room.x = last_room.x
            room.y = last_room.y - 1 - room.h
            result = CheckSpaceRoom( room.x, room.y, room.w, room.h, 1)
            if result then
             tcowl.world.setTile( "grass", room.x+math.random(math.min(room.w, last_room.w))-1, room.y+room.h)
            end
        end

        if result then
            AddRoom( room.x, room.y, room.w, room.h)
        end
    end
end

function Generation( w, h)

    for x = 0, w-1 do
        for y = 0, h-1 do
            for i, room in pairs(rooms) do
                if (room.x <= x and room.y <= y and room.x+room.w > x and room.y+room.h > y) then
                    tcowl.world.setTile( "grass", x, y)
                end
            end
        end
    end
end