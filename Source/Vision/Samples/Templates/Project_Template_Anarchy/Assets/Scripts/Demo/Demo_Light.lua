-- light source script

-- called when the script is attached to the light source
function OnCreate(self)
  -- object variable
  self.myvar = true;

  -- set the position of the light source in world space
 -- local pos = vector3(50, -300, 60)
 -- VisionMessage.Add("Setting light source position: "..pos)
  --self:SetPosition(pos)

  -- linear interpolation of colors
  local col1 = color(255,255,0,255)
  local col2 = color(255,0,255,255)
  local col3 = color(0,255,255,255)
  VisionMessage.Add("Color Lerp: "..col3)
  
  -- set the color of the light source
  VisionMessage.Add("Setting light source color: ("..col3[0]..", "..col3[1]..", "..col3[2]..", "..col3[3]..")")
  --self:SetColor("0,255,255,255")

  -- call some functions on the light source
  --self:SetMultiplier("2");
  --self:SetFlag_CastShadows(true, true);
 -- self:SetRadius(500);
  
end

-- method
function OnThink(self)

  -- call these functions once only
  if (self.myvar == true) then
    VisionMessage.Add("Light Source: Testing object local variable. This should only print once");
    self.myvar = false;
    
    -- examine the light source's bounding box
   -- local bboxLight = self:GetBoundingBox(true)
   -- VisionMessage.Add("Bounding box of the light source: "..bboxLight)
   -- VisionMessage.Add("Max extent of the bounding box: "..bboxLight:GetMaxExtent())
    --local bbox = boundingbox(vector3(1000,1000,1000), vector3(1200,1200,1200))
   -- VisionMessage.Add("Min distance to other bounding box: "..bboxLight:GetMinDist(bbox))
    
    -- find an object (can be either an entity or a light source)
    -- note: this must be done here since we don't know whether the entity already exists in the OnCreate function
    local entity = VisionGame.GetObject("Ball_small")
    if (entity ~= nil) then
      -- attach the light source as a child to entity "the one"
      VisionMessage.Add("*********************")
      self:AttachToParent(entity)
    end
  end
  
  --self:Trigger()
end

