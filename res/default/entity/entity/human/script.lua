local velocity_window = 0.1
local walk_speed = 10.0

function Intilisation( id)
	for x=3,6 do
		for y=1,3 do
			tcowl.entity.setInventoryState( id, x, y, "available")
		end
	end
	
end

function Update( id)
	local l_velX, l_velY

	if tcowl.entity.isAlive( id) == false then
		do return end
	end

	if tcowl.entity.isInputPresent( id) then
		local l_inputX, l_inputY
		l_inputX, l_inputY = tcowl.entity.getInputAxies( id)

		tcowl.entity.doVelocity( id, walk_speed*l_inputX, walk_speed*l_inputY)
	end

	l_velX, l_velY = tcowl.entity.getVelocity( id)

	if math.abs(l_velX) > math.abs(l_velY) then
		if l_velX < -velocity_window then
			tcowl.entity.setAnimation( id, "Walk4")
			-- Links
		end
		if l_velX > velocity_window then
			tcowl.entity.setAnimation( id, "Walk2")
			-- Rechts
		end
	else
		if l_velY < -velocity_window then
			tcowl.entity.setAnimation( id, "Walk3")
			-- Runter
		end
		if l_velY > velocity_window then
			tcowl.entity.setAnimation( id, "Walk1")
			-- Hoch
		end
	end
end