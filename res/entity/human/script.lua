--package.path = package.path .. ";entity/player/human/?.lua"
--package.path = package.path .. ";entity/?.lua"
-- require( "entity")

local velocity_window = 0.1
local walk_speed = 10.0

function update( id)
	local l_velX, l_velY

	if entity.isAlive( id) == false then
		do return end
	end

	if entity.isInputPresent( id) then
		local l_inputX, l_inputY
		l_inputX, l_inputY = entity.getInputAxies( id)

		entity.doVelocity( id, walk_speed*l_inputX, walk_speed*l_inputY)
	end

	l_velX, l_velY = entity.getVelocity( id)

	if math.abs(l_velX) > math.abs(l_velY) then
		if l_velX < -velocity_window then
			entity.setAnimation( id, "Walk4")
			-- Links
		end
		if l_velX > velocity_window then
			entity.setAnimation( id, "Walk2")
			-- Rechts
		end
	else
		if l_velY < -velocity_window then
			entity.setAnimation( id, "Walk3")
			-- Runter
		end
		if l_velY > velocity_window then
			entity.setAnimation( id, "Walk1")
			-- Hoch
		end
	end
end