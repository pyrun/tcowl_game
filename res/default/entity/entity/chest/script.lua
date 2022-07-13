function Intilisation( id)
	for x=0,2 do
		for y=0,2 do
			tcowl.entity.setInventoryState( id, x, y, "available")
		end
	end
	tcowl.entity.addInventoryItem( id, 0, 1, "bread")
end

function Update( id)
	local l_hasItems = false

	if l_hasItems == false then
		tcowl.entity.setAnimation( id, "Open")
	end
end