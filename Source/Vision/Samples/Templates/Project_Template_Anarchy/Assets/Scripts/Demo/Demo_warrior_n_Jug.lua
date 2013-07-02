function OnCreate(self)
	-- Check which warrior is currently created and assign the jug entity to look at
	local key = self:GetKey()

	-- Another way to get the entity key is via the EntityProperties in the properties list on the right
	-- The functions to get and set these values are GetProperty and SetProperty
	-- local key = self:GetProperty("entityKey")
	
	if (key == "Warrior 1") then
		self.jugKey = "Jug 1"
	elseif (key == "Warrior 2") then
		self.jugKey = "Jug 2"
	end
end

function OnThink(self)
	-- Get the correct jug entity for the current warrior
	-- Note: we couldn't use GetEntity in the OnCreate function because the jugs may not have been created yet
	if (self.jugKey ~= nil) then
		local jug = VisionGame.GetEntity(self.jugKey)
		
		-- Calculate look-at direction
		local jugPos = jug:GetPosition() 
		local myPos = self:GetPosition()
		local viewDir = jugPos - myPos
		
		-- Set look-at direction on object
		self:SetDirection(viewDir)
		
		-- The warrior's front is his left arm so turn by 90 degrees
		self:IncOrientation(vector3(90, 0, 0))
	end
end

function OnSerialize(self, ar)
	-- The jugKey needs to be saved so that the warrior remembers which jug he has to look at after loading the scene
	if (ar:IsSaving()) then
		ar:Write(self.jugKey)
	else
		self.jugKey = ar:Read()
	end
end