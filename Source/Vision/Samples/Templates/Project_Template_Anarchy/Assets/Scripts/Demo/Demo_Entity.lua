-- entity script

-- this method is called when the script is first connected to its object, so that you can initialize it
function OnCreate(self)

    -- set the model, and make it twice as big
    self:SetModelFile("Models\\Warrior\\Warrior.model")
    self:SetScaling(vector3(2, 2, 2))
    self:SetTraceAccuracy(trace_accuracy.AABOX)
    self:IncOrientation(vector3(90, 0, 0))
    
    -- access a function on the user-defined entity
    VisionMessage.Add("")
    self:MyFunction()
    
    -- access a dynamic entity property
    local propType = self:GetPropertyType("myProp");
    local propVal = self:GetProperty("myProp");
    VisionMessage.Add("")
    VisionMessage.Add("The entity's property 'myProp' is of type '" .. propType .. "' and contains the value: " .. propVal)
    
    VisionMessage.Add("Setting a new property value...")
    self:SetProperty("myProp", vector3(1.2, 2.3, 3.4));
    propVal = self:GetProperty("myProp");
    VisionMessage.Add("The entity's new property value is: " .. propVal)

    -- accessing global, gamescript and scenescript variables
    VisionMessage.Add("")
    VisionMessage.Add(G.message)
    VisionMessage.Add(G.GameScript.message)
    VisionMessage.Add(G.SceneScript.message)
    
end


-- this method is called within regular intervals (use VisionTimer.GetTimeDelta() to get the time since the last call)
function OnThink(self)
  
  -- call some methods
  --self:SetCastShadows(not self:GetCastShadows())

  -- rotate entity over time
  --t = VisionTimer.GetTimeDelta()
  --self:IncOrientation(vector3(t*10, 0, 0))
  
end


-- called before a scene update
function OnUpdateSceneBegin(self)
end


-- called after a scene update
function OnUpdateSceneFinished(self)
end


coldetected = false

-- called when this object collides with something
function OnCollision(self, info)
  if (coldetected ~= true) then
  
    coldetected = true
    
    VisionMessage.Add("Warrior: ouch!");
    VisionMessage.Add("A collision happened at the following touch point: " .. info.touchPoint)
    VisionMessage.Add("The collision was caused by entity: " .. info.ownObject:GetKey())
    
    if (info.colliderObject ~= nil) then
      VisionMessage.Add("It crashed into entity: " .. info.colliderObject:GetKey())
    end
    
    if (info.ownType == collision_type.ENTITYBOX) then
      VisionMessage.Add("The collision was against our entity's bounding box")
    elseif (info.ownType == collision_type.ENTITYPOLY) then
      VisionMessage.Add("The collision was against one of our entity's polygons")
    end
  
  end
  
  return collision_reaction.STOP
end


-- called when something collides with this object
function OnTouch(self, info)

  VisionMessage.Add("Something touched us at the following touch point: " .. info.touchPoint)
  return collision_reaction.STOP

end


-- called when collisions are switched off and this object is moving through something else
function OnVirtualHit(self, info)

  VisionMessage.Add("A virtual hit happened at the following touch point: " .. info.touchPoint)
  return collision_reaction.STOP

end


-- called when this object is serialized and de-serialized (no OnDestroy/OnCreate call will be called when serializing and de-serializing)
function OnSerialize(self, ar)

  if (ar:IsLoading()) then
    VisionMessage.Add("");
    local s = ar:Read()
    VisionMessage.Add("Read back the following object: " .. s)
    local v = ar:Read()
    VisionMessage.Add("Read back the following object: " .. v)
    local tab = ar:Read()
    VisionMessage.Add("Read back the following table: " .. table.concat(tab, ","))
  else
    VisionMessage.Add("");
    VisionMessage.Add("Serializing entity: " .. self:GetKey())
    ar:Write("Hello, Lua")
    ar:Write(vector3(-1, 0, 1))
    local tab = { "a", "b", "c", 1 }
    ar:Write(tab)
  end

end


-- called after the object has been fully restored after de-serialization
function OnDeserializationCallback(self)
  VisionMessage.Add("Deserialization call on object: " .. self:GetKey())
end

