-- new script file
function Init(self)
    -- swing values
	self.swingX = 0.0 
	self.swingY = 5.0
	self.swingZ = 10.0
	self.positiveX = true
	self.positiveY = true
	self.positiveZ = true
	
end

function OnCreate(self)
  Init(self)

	-- original orientation
	self.oldOri = self:GetOrientation()
end

function OnSerialize(self, ar)
  if (ar:IsLoading()) then
    Init(self)
    self.oldOri = ar:Read()
  else
    ar:Write(self.oldOri)
  end
end

function OnThink(self)
	local t = VisionTimer.GetTimeDelta() * 1
	
	-- X
	if (self.positiveX == true) then
		self.swingX = self.swingX + t
		if (self.swingX > 1) then
			self.positiveX = false
		end		
	else
		self.swingX = self.swingX - t		
		if (self.swingX < -1) then
			self.postiveX = true
		end			
	end
	
	-- Y
	if (self.positiveY == true) then
		self.swingY = self.swingY + (t*2)
		if (self.swingY > 1) then
			self.positiveY = false
		end		
	else
		self.swingY = self.swingY - (t*2)		
		if (self.swingY < -1) then
			self.positiveY = true
		end			
	end
	
	-- Z
	if (self.positiveZ == true) then
		self.swingZ = self.swingZ + t
		if (self.swingZ > 1) then
			self.positiveZ = false
		end		
	else
		self.swingZ = self.swingZ - t		
		if (self.swingZ < -1) then
			self.positiveZ = true
		end			
	end

	local newOri = vector3(math.sin(self.swingX)+self.oldOri.x, math.sin(self.swingY)+self.oldOri.y, math.sin(self.swingZ)+self.oldOri.z)
	self:SetOrientation(newOri)
end


