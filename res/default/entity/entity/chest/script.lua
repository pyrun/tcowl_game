function Intilisation( id)
	for x=0,2 do
		for y=0,2 do
			tcowl.entity.setInventoryState( id, x, y, "available")
		end
	end
	tcowl.entity.addInventoryItem( id, 0, 1, "sword")
	tcowl.entity.addInventoryItem( id, 2, 0, "magic_wood")
end

function Update( id)
	local entitys = {}
	entitys = tcowl.entity.find( id, -10, -10, 20, 20)

	for index, value in ipairs(entitys) do
		tcowl.entity.addInventoryItem( value, 5, 1, "bread")
	end
end

function OpenInventory( id)
	tcowl.entity.setAnimation( id, "Open")
	return true
end

function CloseInventory( id)
	tcowl.entity.setAnimation( id, "Close")
	return true
end
