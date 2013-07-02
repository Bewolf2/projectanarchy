#ifndef VLUA_APIDOC

%nodefaultctor VStringInputMap;
%nodefaultdtor VStringInputMap;

class VStringInputMap
{
public: 
  inline float GetTrigger(const char *szTriggerName);
};

VSWIG_CREATE_CONCAT(VStringInputMap, 128, "%s", self->GetObjectKey())
VSWIG_CREATE_TOSTRING(VStringInputMap, "%s: [%d triggers %d alternatives]", self->GetObjectKey(), self->GetNumOfTriggers(), self->GetNumOfAlternatives())


//implement MapTrigger native becuase we have to parse tables as input parameters,
//which is almost impossible with SWIG
%native(VStringInputMap_MapTrigger) int VStringInputMap_MapTrigger(lua_State *L);
%{
  SWIGINTERN int VStringInputMap_MapTrigger(lua_State *L)
  {
    IS_MEMBER_OF(VStringInputMap) //this will move this function to the method table of the specified class

    if(Vision::Editor.IsInEditor() && Vision::Editor.GetMode()<(int)VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME)
    {
      Vision::Error.Warning("MapTrigger will only work when using the 'Play the Game' mode!");
      lua_pushnumber(L, (lua_Number)-1);     //error
      return 1;
    }
    
    SWIG_CONVERT_POINTER(L, 1, VStringInputMap, pSelf)

    //param #2: name of the trigger
    if(!lua_isstring(L,2)) luaL_error(L, "Expected a string value as parameter 2 for VStringInputMap_MapTrigger");
    const char * szTriggerName = lua_tostring(L, 2); 

    //the name of the device, e.g. "PAD1", "MOUSE", etc..
    VString sDeviceType;
    bool bNewTouchArea = false;
    float pAreaAndPriority[] = { 0,0,0,0, -1000};

    //The third parameter can be a string to specify the device or a table for touch devices.
    //In case of touch devices the table defines the touch area
    if(lua_isstring(L,3))
    {
      sDeviceType = lua_tostring(L, 3); 
    }
    else if(lua_istable(L,3))
    {
      //for touch areas...

      VLuaTableIterator tableIter(L, 3);

      int iCounter = 0;

      sDeviceType = "TOUCH"; //default device

      while(tableIter.HasNext())
      {
        VString sTableElem;
        VScriptMember * pVar = tableIter.Next();

        if( strcmp(pVar->GetType(), "string")==0 )
        {
          bool bNew = strcmp(pVar->GetValue(), "new")==0;
          
          //boolean toggle to use of a new touch area
          if(bNew)
          {
            bNewTouchArea = bNew;
          }        
          else
          {
            //if another string is specified we will use it as the device name, e.g. "TOUCH1", "BACK", etc...
            sDeviceType = pVar->GetValue();
          }
        }
        else
        {
          //parse the specified float value: x1, y1, x2, y2, priority
          if(sscanf(pVar->GetValue(), "%f", &pAreaAndPriority[iCounter])!=1)
          {
            Vision::Error.Warning("[Lua] MapTrigger: Could not interpret number: %s", pVar->GetValue());
          }
            
          iCounter++;
                  
          //max allowed is 4 coordinates + 1x priority
          if(iCounter>5)
          {
            Vision::Error.Warning("[Lua] MapTrigger: Ignoring additional parameters in touch area table!");
            break;
          }
        }
      }

      //we need 4 coordinates for the touch area
      if(iCounter<4)
      {
        luaL_error(L, "[Lua] MapTrigger: Cannot create touch area with only %d values!", iCounter);
      }

    }
    else
    {
      luaL_error(L, "Expected an input device number or a table for touch areas as parameter 3 in VStringInputMap_MapTrigger");
    }

    //the control to map
    if(!lua_isstring(L,4)) luaL_error(L, "Expected a string value as parameter 4 for VStringInputMap_MapTrigger");
    int iControl = VInputManagerBase::GetUniqueControlIdentifier(lua_tostring(L, 4)); 
    if(iControl==-1) luaL_error(L, "Your specified control in parameter 4 does not exist");

    int iCurrentIndex = 5;

    int iPositiveControl = -1;
    //the second control to map (in case of an axis)
    if(lua_isstring(L,iCurrentIndex))
    {
      iPositiveControl = VInputManagerBase::GetUniqueControlIdentifier(lua_tostring(L, iCurrentIndex));
      if(iPositiveControl==-1) luaL_error(L, "Your specified control in parameter 5 does not exist");
      iCurrentIndex++;
    }

    VInputOptions options;
    int iOptTriggerIndex = -1;
    //optional table containing the input options
    if(lua_istable(L,iCurrentIndex))
    {
      VLuaTableIterator tableIter(L, iCurrentIndex);

      while(tableIter.HasNext())
      {
        VString sTableElem;
        VScriptMember * pVar = tableIter.Next();
        VString sName = pVar->GetName();
        sName.ToLower();
        VString sValue = pVar->GetValue();
        sValue.ToLower();

        if(sName=="deadzone")
        {
          float fValue;
          if(sscanf(pVar->GetValue(), "%f", &fValue)==1)
            options.m_fDeadZone = fValue;
          else
            Vision::Error.Warning("[Lua] MapTrigger: Could not interpret number: %s", pVar->GetValue());
        }
        else if(sName=="timescaled")
        {
          options.m_bTimeScaled = (sValue == "true");
        }
        else if(sName=="once")
        {
          options.m_bOnce = (sValue == "true");
        }
        else if(sName=="onceperframe")
        {
          options.m_bOncePerFrame = (sValue == "true");
          if(options.m_bOncePerFrame)
            options.m_bOnce = true;
        }
        else if(sName=="onpress")
        {
          bool bSet = (sValue == "true");

          if(bSet)
            options.m_eOnceMode = (VOnceMode)((int)options.m_eOnceMode | (int)ONCE_ON_PRESS);
          else
            options.m_eOnceMode = (VOnceMode)((int)options.m_eOnceMode & ~(int)ONCE_ON_PRESS);
        }
        else if(sName=="onrelease")
        {
          bool bSet = (sValue == "true");

          if(bSet)
            options.m_eOnceMode = (VOnceMode)((int)options.m_eOnceMode | (int)ONCE_ON_RELEASE);
          else
            options.m_eOnceMode = (VOnceMode)((int)options.m_eOnceMode & ~(int)ONCE_ON_RELEASE);
        }
        else if(sName=="holdtime")
        {
          float fValue;
          if(sscanf(pVar->GetValue(), "%f", &fValue)==1)
            options.m_fHoldTime = fValue;
          else
            Vision::Error.Warning("[Lua] MapTrigger: Could not interpret number: %s", pVar->GetValue());
        }
        else if(sName=="sensitivity")
        {
          float fValue;
          if(sscanf(pVar->GetValue(), "%f", &fValue)==1)
            options.m_fSensitivity = fValue;
          else
            Vision::Error.Warning("[Lua] MapTrigger: Could not interpret number: %s", pVar->GetValue());
        }
        else if(sName=="alternative")
        {
          int iValue;
          if(sscanf(pVar->GetValue(), "%d", &iValue)==1)
            options.m_iAlternative = iValue;
          else
            Vision::Error.Warning("[Lua] MapTrigger: Could not interpret number: %s", pVar->GetValue());
        }
        else if(sName=="usetriggerindex")
        {
          int iValue;
          if(sscanf(pVar->GetValue(), "%d", &iValue)==1)
            iOptTriggerIndex = iValue;
          else
            Vision::Error.Warning("[Lua] MapTrigger: Could not interpret number: %s", pVar->GetValue());
        }      
      }
      iCurrentIndex++;
    }

    int iResult = VScriptInput_wrapper::MapTrigger(
        pSelf,
        szTriggerName,
        sDeviceType,
        iControl, //negative axis control
        options,
        iOptTriggerIndex,
        VRectanglef(pAreaAndPriority[0],pAreaAndPriority[1],pAreaAndPriority[2],pAreaAndPriority[3]),
        pAreaAndPriority[4],
        bNewTouchArea,
        iPositiveControl);
        
    //clean up the stack
    lua_pop(L, iCurrentIndex-1);

    lua_pushnumber(L, (lua_Number)iResult);     //the mapping result needs to be on the stack as return value

    return 1; //one item on the stack
  }
%}

//we are using the same code for MapTriggerAxis (two seperate functions to keep compatiblity with C++)
%native(VStringInputMap_MapTriggerAxis) int VStringInputMap_MapTriggerAxis(lua_State *L);
%{
  SWIGINTERN int VStringInputMap_MapTriggerAxis(lua_State *L)
  {
    IS_MEMBER_OF(VStringInputMap) //this will move this function to the method table of the specified class
  
    return VStringInputMap_MapTrigger(L);
  }
%}

#else

/// \brief VStringInputMap is a named input map, additionally support strings to identify your triggers.
/// 			 VStringInputMap additionally comes with with an element manager so you can use
/// 			 VStringInputMap::FindByKey to find your named input map.
/// 			 
/// \note If you are also using integer based trigger indices, please setup their mapping BEFORE string based
/// 			triggers, otherwise the named trigger will accidentally use another ones integer based index.
/// 			You can also initialize your whole map with named triggers and use the integer representation in your
/// 			C++ code and the named trigger just in your script binding. By doing that you will not lose any
/// 			performance in C++ and will be still able to access all triggers without additional code in your
/// 			scripts as well.
///
/// \see VInputMap
/// \see VisObjectKey_cl
/// \see VEnginePluginElementManager<class VStringInputMap*>
class VStringInputMap
{

public:

  /// \brief Map a trigger using a string. The method behaves like VInputMap::MapTrigger but uses a string instead
  /// 			 an int for the trigger. Please refer to the VInputMap documentation for further details.
  ///
  /// \param triggerName   The name of the trigger.
  /// \param deviceOrArea  The string representation of the input device ("MOUSE", "KEYBOARD", "PAD1", "PAD2", "PAD3", "PAD4",
  ///                      etc... please see VInputManager::GetInputDevice for supported devices on your target platform) or a touch
  ///                      area defined as a table having 4 to 7 values: {number x1, number y1, number x2, number y2[, number priority]}.
  ///                      You can add two additional string parameter to the table: "new" to create a new touch area instead of
  ///                      re-using a matching touch area and second string specifying the device, e.g. "BACK" to use the back side
  ///                      touch screen (please see VInputManager::GetInputDevice for supported touch devices).
  ///                      Please also have a look at the provided code sample!
  /// \param control       The control (key, button, touch, ...) to map. The format of these strings matches VInputControl, e.g. "CT_KB_ENTER".
  /// \param options       [\b optional] This optional table defines the behavior of the mapped control. You can define the following table elements:
  ///                       - deadzone: number - create a dead zone ranging from 0 to 1.
  ///                       - timescaled: boolean - set to true to multiply the value with the frame time delta.
  ///                       - once: boolean - set to true if the trigger is active just once when pressed (instead of ell the time).
  ///                       - onceperframe: boolean - set to true if the trigger is active just in the first frame when pressed.
  ///                       - onpress: boolean - activate the trigger when pressing the key (use with once/once per frame).
  ///                       - onrelease: boolean - activate the trigger when releasing the key (use with once/once per frame).
  ///                       - holdtime: number - the time before the trigger gets active (incompatible with one/onceperframe).
  ///                       - sensitivity: number - the sensitivity (scaling) of the control from 0 to 1, default is 1.
  ///                       - alternative: number - define an explicit alternative for the mapped control (instead of the auto assignment with -1).
  ///                       - usetriggerindex: number - explicitly define which trigger number to use for this named trigger (e.g. in order to match with the C++ implementation).
  /// \return              The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success.
  ///
  /// \par Example
  ///   \code
  ///    function OnAfterSceneLoaded(self)
  ///
  ///      -- create an input map with specific dimensions
  ///      int numTriggers = 10 --the max number of triggers we can use with this map
  ///      int numAlternatives = 5 --we are going to map 5 controls to the trigger "Jump"
  ///
  ///      self.inputMap = Input:CreateMap("Player", numTriggers, numAlternatives)
  ///      
  ///      -- map a trigger for the space bar
  ///      self.inputMap:MapTrigger("Jump", "KEYBOARD", "CT_KB_SPACE")
  ///
  ///      -- map a trigger for the pad with a dead zone of 0.3
  ///      self.inputMap:MapTrigger("Jump", "PAD1", "CT_PAD_LEFT_TRIGGER", {once = true, deadzone = 0.3})
  ///
  ///      -- map a trigger for the left mouse button
  ///      self.inputMap:MapTrigger("Jump", "MOUSE", "CT_MOUSE_LEFT_BUTTON", {once = true})
  ///
  ///      -- map a trigger to the touch area at 200,200, 400,400
  ///      self.inputMap:MapTrigger("Jump", {200,200, 400,400},  "CT_TOUCH_DOUBLE_TAP", {once = true})
  ///
  ///      -- map a trigger to the touch area at 200,200, 400,400 with priority -900, use the backside touch screen
  ///      self.inputMap:MapTrigger("Jump", {200,200, 400,400, -900, "BACK"},  "CT_TOUCH_ANY", { once = true})
  ///      
  ///    end
  ///   \endcode
  ///
  /// \see MapTriggerAxis
  /// \see VScriptInput_wrapper::CreateMap
  /// \see VInputControl
  number MapTrigger(string triggerName, mixed deviceOrArea, string control, table options = nil);

  /// \brief Map a trigger axis using a string. The method behaves like VInputMap::MapTriggerAxis but uses a string instead
  /// 			 an int for the trigger. Please refer to the VInputMap documentation for further details.
  ///
  /// \param triggerName     The name of the trigger.
  /// \param device          The string representation of the input device ("MOUSE", "KEYBOARD", "PAD1", "PAD2", "PAD3", "PAD4",
  ///                        etc... please see VInputManager::GetInputDevice for supported devices on your target platform). Touch areas are not supported.
  /// \param controlNegative The control (key, button, thumbstick, ...) to map as negative value. The format of these strings
  ///                        matches VInputControl, e.g. "CT_PAD_DOWN".
  /// \param controlPositive The control (key, button, thumbstick, ...) to map as positive value. The format of these strings
  ///                        matches VInputControl, e.g. "CT_PAD_UP".
  /// \param options         [\b optional] This optional table defines the behavior of the mapped control. You can define the following table elements:
  ///                         - deadzone: number - create a dead zone ranging from 0 to 1.
  ///                         - timescaled: boolean - set to true to multiply the value with the frame time delta.
  ///                         - once: boolean - set to true if the trigger is active just once when pressed (instead of ell the time).
  ///                         - onceperframe: boolean - set to true if the trigger is active just in the first frame when pressed.
  ///                         - onpress: boolean - activate the trigger when pressing the key (use with once/once per frame).
  ///                         - onrelease: boolean - activate the trigger when releasing the key (use with once/once per frame).
  ///                         - holdtime: number - the time before the trigger gets active (incompatible with one/onceperframe).
  ///                         - sensitivity: number - the sensitivity (scaling) of the control from 0 to 1, default is 1.
  ///                         - alternative: number - define an explicit alternative for the mapped control (instead of the auto assignment with -1).
  ///                         - usetriggerindex: number - explicitly define which trigger number to use for this named trigger (e.g. in order to match with the C++ implementation).
  /// \return                The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success.
  /// 				
  /// \par Example
  ///   \code
  ///    function OnAfterSceneLoaded(self)
  ///      -- create an input map with default dimensions
  ///      self.map = Input:CreateMap("InputSample")
  ///
  ///      self.map:MapTriggerAxis("CursorX", "PAD1", "CT_PAD_RIGHT_THUMB_STICK_LEFT", "CT_PAD_RIGHT_THUMB_STICK_RIGHT", { timescaled = true} )
  ///      self.map:MapTriggerAxis("CursorY", "PAD1", "CT_PAD_RIGHT_THUMB_STICK_UP", "CT_PAD_RIGHT_THUMB_STICK_DOWN", { timescaled = true} )    
  ///    end
  ///
  ///    function OnThink(self)
  ///      -- change the cursor object's position according to the mouse position
  ///      self:IncPosition(self.map:GetTrigger("CursorX")*400, self.map:GetTrigger("CursorY")*400, 0)
  ///    end
  ///   \endcode
  ///
  /// \see MapTrigger
  /// \see VScriptInput_wrapper::CreateMap
  number MapTriggerAxis(string triggerName, number device, number controlNegative, number controlPositive, table options = nil);

  /// \brief Get a trigger value, where the trigger is identified by a string. The method behaves like VInputMap::GetTrigger but
  /// 			 performs a lookup of the trigger (to get the internally assigned integer) and calls VInputMap::GetTrigger afterwards.
  /// 			 So this operation is slightly more expensive because of the additional lookup.
  ///
  /// \param triggerName The name of the trigger.
  ///
  /// \return The current value of the trigger, which usually ranges from 0 to 1 (or -1 to 1 for axes).
  /// \par Example
  ///   \code
  ///    function OnAfterSceneLoaded(self)
  ///      -- create an input map with default dimensions
  ///      self.map = Input:CreateMap("InputSample")
  ///
  ///      local width, height = Screen:GetViewportSize()
  ///
  ///      self.map:MapTrigger("X", {0,0, width, height}, "CT_TOUCH_ABS_DELTA_X", { sensitivity = 150.0f / Screen:GetDeviceDpi() } )
  ///      self.map:MapTrigger("Y", {0,0, width, height}, "CT_TOUCH_ABS_DELTA_Y", { sensitivity = 150.0f / Screen:GetDeviceDpi() } )
  ///    end
  ///
  ///    function OnThink(self)
  ///      -- change the cursor object's position according to the mouse position
  ///      local x = self.map:GetTrigger("X")
  ///      local y = self.map:GetTrigger("Y")
  ///
  ///      self:IncPosition(x*400, y*400, 0)
  ///    end
  ///   \endcode
  ///
  /// \see MapTrigger
  /// \see VScriptInput_wrapper::CreateMap
  number GetTrigger(string triggerName);

};

#endif
