-- script for the ScriptMenu.xml dialog

-- called when an item in the dialog is clicked
function OnItemClicked(self, uiItem, buttons, mousePosX, mousePosY)
  
  -- upon a left button click
  if (buttons[0] == true) then
  
    -- check which UI control triggered this message
    if (uiItem:GetID() == VGUIManager.GetID("WARRIOR_WALK")) then
  
      local entity = VisionGame.GetObject("the one");
      if (entity ~= nil) then
        -- start the "Walk" animation one time when the user presses this button
        local res = entity:StartSkeletalAnimation("Walk", false, 1);
        if (res) then
          VisionMessage.Add("Warrior: I am walking");
        else
          VisionMessage.Add("Could not start the warrior animation!");
        end
      end
  
    elseif (uiItem:GetID() == VGUIManager.GetID("WARRIOR_VISIBLE")) then
  
      local entity = VisionGame.GetObject("the one");
      local visible = bitmask();
      if (entity ~= nil) then
        -- make the warrior visible or invisible
        if (entity:IsVisible() == true) then
          VisionMessage.Add("Warrior: I am invisible");
          visible[0] = false  -- set to invisible in context 0
          uiItem:SetText("Warrior Command: Make Visible")
        else
          VisionMessage.Add("Warrior: they can see me");
          visible[0] = true  -- set to visible in context 1
          uiItem:SetText("Warrior Command: Make Invisible")
        end
        
        entity:SetVisibleBitmask(visible)
      end
  
    elseif  (uiItem:GetID() == VGUIManager.GetID("LIGHT_SWITCH")) then
  
      local light = VisionGame.GetObject("lux");
      if (light ~= nil) then
        -- change the radius of the light source and update the menu item's text
        if (light:GetRadius() > 100) then
          VisionMessage.Add("Dimming the light");
          light:SetRadius(100)
          uiItem:SetText("Brighten Light")
        else
          VisionMessage.Add("Brightening the light");
          light:SetRadius(500)
          uiItem:SetText("Dim Light")
        end
      end
  
    end
  
  end
  
end

