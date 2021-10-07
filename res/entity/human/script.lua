--package.path = package.path .. ";entity/player/human/?.lua"
--package.path = package.path .. ";entity/?.lua"
--require( "entity_lib")

local velocity_window = 0.1

function update( id)
	local l_velX, l_velY

	if isAlive( id) == false then
		do return end
	end

	l_velX, l_velY = getVelocity( id)

	if math.abs(l_velX) > math.abs(l_velY) then
		if l_velX < -velocity_window then
			setAnimation( id, "Walk4")
			-- Links
		end
		if l_velX > velocity_window then
			setAnimation( id, "Walk2")
			-- Rechts
		end
	else
		if l_velY < -velocity_window then
			setAnimation( id, "Walk3")
			-- Runter
		end
		if l_velY > velocity_window then
			setAnimation( id, "Walk1")
			-- Hoch
		end
	end
end