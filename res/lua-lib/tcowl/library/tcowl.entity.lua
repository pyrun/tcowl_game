---@meta

---
---Provides an interface to entitys.
---
---@class tcowl.entity
tcowl.entity = {}

---
---Check if object id is alive
---
---@param id? number # object id
---@return boolean value # if alive true
function tcowl.entity.isAlive( id) end

---
---Get velocity of object
---
---@param id? number # object id
---@return number x # x velocity
---@return number y # y velocity
function tcowl.entity.getVelocity( id) end

---
---Set animation of object
---
---@param id? number # object id
---@param name? string # name of the animation
---@param tick? number # tick(optional)
function tcowl.entity.setAnimation( id, name, tick) end

---
---Do/add velocity of object
---
---@param id? number # object id
---@param x? number # x velocity
---@param y? number # y velocity
function tcowl.entity.doVelocity( id, x , y) end
---
---Get position of object
---
---@param id? number # object id
---@return number x # x position
---@return number y # y position
function tcowl.entity.getPosition( id) end

---
---Check if object id has input
---
---@param id? number # object id
---@return boolean value # has input
function tcowl.entity.isInputPresent( id) end

---
---Get input axies
---
---@param id? number # object id
---@return number x # x axie -1 to 1
---@return number y # y axie -1 to 1
function tcowl.entity.getInputAxies( id) end

---
---Get input buttons
---
---@param id? number # object id
---@return boolean attack # button attack
function tcowl.entity.getInputAxies( id) end

---
---Change state of inventory tile
---
---@param id? number # object id
---@param x? number # pos x
---@param y? number # pos y
---@param state? string # unavailable, available or taken
---@return boolean happend # happend
function tcowl.entity.setInventoryState( id, x, y, state) end

---
---Add item to inventory
---
---@param id? number # object id
---@param x? number # pos x
---@param y? number # pos y
---@param objname? string # object name
---@return boolean happend # happend
function tcowl.entity.addInventoryItem( id, x, y, objname) end

---
---Search for objects in Rect Pos from object
---
---@param id? number # object id
---@param x? number # pos x
---@param y? number # pos y
---@param w? number # rect w
---@param h? number # rect h
---@return table number objects_id # ids
function tcowl.entity.find( id, x, y, w, h) end

---
---Set Parameter, will be created if not existing
---
---@param id? number # object id
---@param name? string # parameter name
---@param value? number #
function tcowl.entity.setParameter( id, name, value)  end

---
---Get Parameter
---
---@param id? number # object id
---@param name? string # parameter name
function tcowl.entity.getParameter( id, name, value)  end