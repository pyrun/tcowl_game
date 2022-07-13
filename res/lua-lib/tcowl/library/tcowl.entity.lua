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
---Get input buttons
---
---@param id? number # object id
---@param x? number # pos x
---@param y? number # pos y
---@param state? string # unavailable, available or taken
---@return boolean attack # button attack
function tcowl.entity.setInventoryState( id, x, y, state) end
