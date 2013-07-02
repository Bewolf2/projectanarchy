 function OnThink(self)
  -- rotate object over time
  local t = VisionTimer.GetTimeDelta()*50
  
  
  local newPos = vector3(t,0,0)

  self:IncOrientation(newPos)
 end