

-- Useful to position Particle FX such as rain , snow at the camera location.

function OnCreate(self)
	
	self.heightOffset = 750;
	
end

function OnThink(self)

  -- update Particle Position according to Camera Position
  local cam = VisionCamera.GetMainCamera()
  
  local camPos = cam:GetPosition()
  
  -- height offset
  camPos.z = camPos.z + self.heightOffset;
  
  self:SetPosition(camPos)
    
end

