---@meta

---
---Provides an interface to world.
---
---@class tcowl.tcowl
tcowl.world = {}

---
---Gets the current size of the world
---
---@return number x # Size x
---@return number y # Size y
function tcowl.world.getSize() end

---
---Set Tile in the current world
---
---@param id? number # Tile Id
---@param x? number # The x coordinate
---@param y? number # The y coordinate
function tcowl.world.setTile( id , x, y) end