local velocity_window = 0.1
local walk_speed = 8.0

function Intilisation( id)
	for x=3,6 do
		for y=1,3 do
			tcowl.entity.setInventoryState( id, x, y, "available")
		end
	end
	tcowl.entity.setParameter( id, "strength", 2);
	tcowl.entity.setParameter( id, "skill", 1);
	tcowl.entity.setParameter( id, "magic", 1);
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
			tcowl.entity.setAnimation( id, "WalkLeft")
			-- Links
		end
		if l_velX > velocity_window then
			tcowl.entity.setAnimation( id, "WalkRight")
			-- Rechts
		end
	else
		if l_velY < -velocity_window then
			tcowl.entity.setAnimation( id, "WalkUp")
			-- Runter
		end
		if l_velY > velocity_window then
			tcowl.entity.setAnimation( id, "WalkDown")
			-- Hoch
		end
	end
end