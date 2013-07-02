
-- new script file

local count =0;

function OnCreate(self)
	 VisionMessage.Add("STARTED", color(255,0,0));
  -- object-local variable for keeping the elapsed time
  self.time_elapsed = 0
end

function OnThink(self)
  -- update the elapsed time variable
  self.time_elapsed = self.time_elapsed + VisionTimer.GetTimeDelta()

  if (self.time_elapsed >= 0.5) then
    -- get the current entity position
    local oldPos = self:GetPosition()

    -- create new random x and y positions
    local newX = VisionGame.GetNormalizedRandNeg() * 200
    local newY = VisionGame.GetNormalizedRandNeg() * 200

    -- create a new vector with the new x,y positions and the old z position and
    -- assign it to the entity
    local newPos = vector3(newX, newY, oldPos.z)
    self:SetPosition(newPos)

    -- reset the elapsed time variable
    self.time_elapsed = 0
  end
end

function OnSerialize(self, ar)
  -- Simply re-initialize the time_elapsed variable to 0 after loading the scene
  if (ar:IsLoading()) then
    self.time_elapsed = 0
  end
end