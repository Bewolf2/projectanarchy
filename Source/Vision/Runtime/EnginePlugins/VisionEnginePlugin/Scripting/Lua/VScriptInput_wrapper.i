
#ifndef VLUA_APIDOC

//global constants

%immutable;
// Generic
static const int KEY_UNKNOWN = VGLK_UNKNOWN;

// Control keys
static const int KEY_LSHIFT = VGLK_LSHIFT;
static const int KEY_RSHIFT = VGLK_RSHIFT;
static const int KEY_LCTRL = VGLK_LCTRL;
static const int KEY_RCTRL = VGLK_RCTRL;

static const int KEY_LALT = VGLK_LALT;
static const int KEY_RALT  = VGLK_RALT;
static const int KEY_ALTGR = VGLK_ALTGR;

static const int KEY_BACKSPACE = VGLK_BACKSP;
static const int KEY_TAB = VGLK_TAB;

static const int KEY_LWIN = VGLK_LWIN;
static const int KEY_RWIN = VGLK_RWIN;
static const int KEY_APPS = VGLK_APPS;
static const int KEY_ENTER = VGLK_ENTER;
static const int KEY_LCOM = VGLK_LCOM;
static const int KEY_RCOM = VGLK_RCOM;
static const int KEY_MAC = VGLK_MAC;
static const int KEY_LMETA = VGLK_LMETA;
static const int KEY_RMETA = VGLK_RMETA;

// Non-numerical edit and cursor movement keys
static const int KEY_INS = VGLK_INS;
static const int KEY_DEL = VGLK_DEL;
static const int KEY_HOME = VGLK_HOME;
static const int KEY_END = VGLK_END;
static const int KEY_PAGEUP = VGLK_PGUP;
static const int KEY_PAGEDOWN = VGLK_PGDN;

static const int KEY_ESC = VGLK_ESC;

static const int KEY_UP = VGLK_UP;
static const int KEY_DOWN = VGLK_DOWN;
static const int KEY_LEFT = VGLK_LEFT;
static const int KEY_RIGHT = VGLK_RIGHT;

static const int KEY_SPACE = VGLK_SPACE;

//The following keys might not be available on all keyboard layouts:
// (They work on US and German layouts)
static const int KEY_COMMA = VGLK_COMMA;
static const int KEY_PERIOD = VGLK_PERIOD;
static const int KEY_MINUS = VGLK_MINUS;

static const int KEY_GRAVE = VGLK_GRAVE;

// Standard number codes comply with ASCII codes
static const int KEY_0 = VGLK_0;
static const int KEY_1 = VGLK_1;
static const int KEY_2 = VGLK_2;
static const int KEY_3 = VGLK_3;
static const int KEY_4 = VGLK_4;
static const int KEY_5 = VGLK_5;
static const int KEY_6 = VGLK_6;
static const int KEY_7 = VGLK_7;
static const int KEY_8 = VGLK_8;
static const int KEY_9 = VGLK_9;


// Special keys
static const int KEY_CAPS = VGLK_CAPS;
static const int KEY_SCROLL = VGLK_SCROLL;
static const int KEY_NUM = VGLK_NUM;
static const int KEY_PRINTSCREEN = VGLK_PRSCR;
static const int KEY_PAUSE = VGLK_PAUSE;

// Standard letter codes comply with ASCII codes
static const int KEY_A = VGLK_A;
static const int KEY_B = VGLK_B;
static const int KEY_C = VGLK_C;
static const int KEY_D = VGLK_D;
static const int KEY_E = VGLK_E;
static const int KEY_F = VGLK_F;
static const int KEY_G = VGLK_G;
static const int KEY_H = VGLK_H;
static const int KEY_I = VGLK_I;
static const int KEY_J = VGLK_J;
static const int KEY_K = VGLK_K;
static const int KEY_L = VGLK_L;
static const int KEY_M = VGLK_M;
static const int KEY_N = VGLK_N;
static const int KEY_O = VGLK_O;
static const int KEY_P = VGLK_P;
static const int KEY_Q = VGLK_Q;
static const int KEY_R = VGLK_R;
static const int KEY_S = VGLK_S;
static const int KEY_T = VGLK_T;
static const int KEY_U = VGLK_U;
static const int KEY_V = VGLK_V;
static const int KEY_W = VGLK_W;
static const int KEY_X = VGLK_X;
static const int KEY_Y = VGLK_Y;
static const int KEY_Z = VGLK_Z;

// Numeric keypad keys. Some of these keys are doubled for your leisure :)
static const int KEYPAD_SLASH = VGLK_KP_SLASH;
static const int KEYPAD_MUL = VGLK_KP_MUL;
static const int KEYPAD_MINUS = VGLK_KP_MINUS;
static const int KEYPAD_PLUS = VGLK_KP_PLUS;
static const int KEYPAD_ENTER = VGLK_KP_ENTER;
static const int KEYPAD_PERIOD = VGLK_KP_PERIOD;
static const int KEYPAD_DEL = VGLK_KP_DEL;
static const int KEYPAD_0 = VGLK_KP_0;
static const int KEYPAD_INS = VGLK_KP_INS;
static const int KEYPAD_1 = VGLK_KP_1;
static const int KEYPAD_END = VGLK_KP_END;
static const int KEYPAD_2 = VGLK_KP_2;
static const int KEYPAD_DOWN = VGLK_KP_DOWN;
static const int KEYPAD_3 = VGLK_KP_3;
static const int KEYPAD_PAGEDOWN = VGLK_KP_PGDN;
static const int KEYPAD_4 = VGLK_KP_4;
static const int KEYPAD_LEFT = VGLK_KP_LEFT;
static const int KEYPAD_5 = VGLK_KP_5;
static const int KEYPAD_6 = VGLK_KP_6;
static const int KEYPAD_RIGHT = VGLK_KP_RIGHT;
static const int KEYPAD_7 = VGLK_KP_7;
static const int KEYPAD_HOME = VGLK_KP_HOME;
static const int KEYPAD_8 = VGLK_KP_8;
static const int KEYPAD_UP = VGLK_KP_UP;
static const int KEYPAD_9 = VGLK_KP_9;
static const int KEYPAD_PGAGEUP = VGLK_KP_PGUP;

// Function keys
static const int KEY_F1 = VGLK_F1;
static const int KEY_F2 = VGLK_F2;
static const int KEY_F3 = VGLK_F3;
static const int KEY_F4 = VGLK_F4;
static const int KEY_F5 = VGLK_F5;
static const int KEY_F6 = VGLK_F6;
static const int KEY_F7 = VGLK_F7;
static const int KEY_F8 = VGLK_F8;
static const int KEY_F9 = VGLK_F9;
static const int KEY_F10 = VGLK_F10;
static const int KEY_F11 = VGLK_F11;
static const int KEY_F12 = VGLK_F12;

// Keys only available on specific layouts

//US (and English)
static const int KEY_EQUAL = VGLK_EQUAL;
static const int KEY_LSQBRK = VGLK_LSQBRK;
static const int KEY_RSQBRK = VGLK_RSQBRK;
static const int KEY_SEMICL = VGLK_SEMICL;
static const int KEY_APOSTR = VGLK_APOSTR;
static const int KEY_BACKSL = VGLK_BACKSL;
static const int KEY_SLASH = VGLK_SLASH;

//German keyboard
static const int KEY_DE_SS = VGLK_DE_SS;
static const int KEY_DE_ACCENT = VGLK_DE_ACCENT;
static const int KEY_DE_UE  = VGLK_DE_UE;
static const int KEY_DE_PLUS = VGLK_DE_PLUS;
static const int KEY_DE_OE = VGLK_DE_OE;
static const int KEY_DE_AE = VGLK_DE_AE;
static const int KEY_DE_HASH = VGLK_DE_HASH;
static const int KEY_DE_LT  = VGLK_DE_LT;
static const int KEY_DE_CIRC = VGLK_DE_CIRC;

//French keyboard
static const int KEY_FR_UGRAVE = VGLK_FR_UGRAVE;
static const int KEY_FR_MULTIPLY = VGLK_FR_MULTIPLY;
static const int KEY_FR_COMMA = VGLK_FR_COMMA;
static const int KEY_FR_SEMICL = VGLK_FR_SEMICL;
static const int KEY_FR_COLON = VGLK_FR_COLON;
static const int KEY_FR_SS = VGLK_FR_SS;
static const int KEY_FR_RBRACKET = VGLK_FR_RBRACKET;
static const int KEY_FR_EQUAL = VGLK_FR_EQUAL;
static const int KEY_FR_DOLL = VGLK_FR_DOLL;


//mouse
static const int BUTTON_LEFT = 0;
static const int BUTTON_MIDDLE = 1;
static const int BUTTON_RIGHT = 2;

%mutable;

class VScriptInput_wrapper
{
public:
  VScriptInput_wrapper();
  ~VScriptInput_wrapper();
  
  bool IsKeyPressed(int lKey);

  bool SetKeyAsSingleHit(int lKey, bool bSingleHit = true);

  bool SetMousePosition(float fInsideViewportX, float fInsideViewportY);
  
  bool IsMouseButtonPressed(int iButton);

  float GetMouseWheelDelta(bool bApplyTimeDiff=true);
  
  VStringInputMap * CreateMap(const char * szMapName, int iNumTriggers=32, int iNumAlternatives=4);

  VStringInputMap * GetMap(const char * szMapName);

  void DestroyMap(VStringInputMap * pMap);

  void CreateVirtualThumbStick();

  void DestroyVirtualThumbStick();
};

  
//implement GetMousePosition native because we would like to return two values at once
%native(VScriptInput_wrapper_GetMousePosition) int VScriptInput_wrapper_GetMousePosition(lua_State *L);
%{
  SWIGINTERN int VScriptInput_wrapper_GetMousePosition(lua_State *L)
  {
    IS_MEMBER_OF(VScriptInput_wrapper) //this will move this function to the method table of the specified class
        
    #if defined(WIN32) && defined(SUPPORTS_MOUSE)
      int x, y, w, h;
      Vision::Contexts.GetMainRenderContext()->GetViewport(x, y, w, h);
      int iMouseX = VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_RAW_CURSOR_X);
      int iMouseY = VInputManager::GetMouse().GetRawControlValue(CT_MOUSE_RAW_CURSOR_Y);
      
      if(iMouseX<0)
        lua_pushnumber(L, (lua_Number)0);
      else if(iMouseX>w)
        lua_pushnumber(L, (lua_Number)w);
      else
        lua_pushnumber(L, (lua_Number)iMouseX);
      
      if(iMouseY<0)
        lua_pushnumber(L, (lua_Number)0);
      else if(iMouseY>h)
        lua_pushnumber(L, (lua_Number)h);
      else
        lua_pushnumber(L, (lua_Number)iMouseY);
    #else
      lua_pushnumber(L, (lua_Number)0);
      lua_pushnumber(L, (lua_Number)0);
    #endif
    
    return 2; //in every case we leave two values at the stack
  }
%}

//implement GetMouseDelta as well as native
%native(VScriptInput_wrapper_GetMouseDelta) int VScriptInput_wrapper_GetMouseDelta(lua_State *L);
%{
  SWIGINTERN int VScriptInput_wrapper_GetMouseDelta(lua_State *L)
  {
    IS_MEMBER_OF(VScriptInput_wrapper) //this will move this function to the method table of the specified class
    
    #ifdef SUPPORTS_MOUSE
      DECLARE_ARGS_OK
    
      GET_OPT_ARG(2, bool, bApllyTimeDiff, true)
    
      if(ARGS_OK && bApllyTimeDiff)
      {
        VScriptResourceManager* pMan = (VScriptResourceManager*) Vision::GetScriptManager();
        VASSERT(pMan);

        // script thinking delta or simulation tick delta
        float fTimeDiff = pMan->GetThinkInterval();
        if(fTimeDiff<=0) fTimeDiff = Vision::GetTimer()->GetTimeDifference();

        lua_pushnumber(L, (lua_Number)(Vision::Mouse.GetDeltaX()*fTimeDiff));
        lua_pushnumber(L, (lua_Number)(Vision::Mouse.GetDeltaY()*fTimeDiff));
      }
      else
      {
        lua_pushnumber(L, (lua_Number)Vision::Mouse.GetDeltaX());
        lua_pushnumber(L, (lua_Number)Vision::Mouse.GetDeltaY());
      }
    #else
      lua_pushnumber(L, (lua_Number)0);
      lua_pushnumber(L, (lua_Number)0);
    #endif
    
    return 2; //in every case we leave two values at the stack
  }
%}

#else

/// \brief Here you can find input related functions. The global instance in Lua is named \b Input.
/// \par Example
///   \code
///  function OnAfterSceneLoaded(self)
///    Input:SetKeyAsSingleHit(Vision.KEY_SPACE)
///    Debug:PrintLine("Press SPACE to receive the current mouse position.")
///  end
///
///  function OnThink(self)
///    -- input will only work in the game and in 'Play the Game' mode, since
///    -- it is the only mode which grabs the input control from windows.
///    if Input:IsKeyPressed(Vision.KEY_SPACE) then
///      local x,y = Input:GetMousePosition()
///      Debug:PrintLine("The mouse is currently at "..x..", "..y..".")
///    end
///  end
///   \endcode
class VScriptInput_wrapper {
public:

  /// @name Input Map Functions
  /// @{
  
  /// \brief Create a named input map with named triggers. If a map with your selected name already exists it will
  ///        be deleted! You can use Input:GetMap to access an existing map.
  /// 			 
  /// \note If you are also using integer based trigger indices for the same map (e.g. C++ code), please setup
  /// 			their mapping before string based triggers.
  /// 			You can also initialize your native C++ map with named triggers and use the integer representation in
  /// 			your C++ code and the named trigger in your script binding. By doing that you will not lose any
  /// 			performance in C++ and can still access all triggers without additional code in your scripts as well.
  /// 
  /// \note Named input maps created via VScriptInput_wrapper::CreateMap will not automatically be destroyed when
  ///       the current scene is unloaded. Use VScriptInput_wrapper::DestroyMap to delete such a map if restoring 
  ///       the application's initial input behavior is desired.
  ///
  /// \param mapName         The unique name of the string input map.
  /// \param numTriggers     [\b optional] The number of mappable trigger, adjust this value to your needs, default
  /// 												value is 32, VStringInputMap::MapTrigger will return -1 if you exceed this limit.
  /// \param numAlternatives [\b optional] The number of mappable alternatives, adjust this value to your needs,
  /// 											 default value is 4, VStringInputMap::MapTrigger will return -1 if you exceed this limit.
  /// \returns               The newly created or already existing map (in case the name is already in use).
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///
  ///       self.playerInputMap = Input:CreateMap("PlayerInputMap")
  ///
  ///       self.playerInputMap:MapTrigger("Jump", "KEYBOARD", "CT_KB_SPACE",           { once = true })
  ///       self.playerInputMap:MapTrigger("Jump", "MOUSE",    "CT_MOUSE_RIGHT_BUTTON", { once = true })
  ///     end  
  ///   \endcode
  /// \see GetMap
  /// \see VStringInputMap::MapTrigger
  /// \see VStringInputMap::GetTrigger
  VStringInputMap CreateMap(string mapName, number numTriggers = 32, number numAlternatives = 4);
  
  /// \brief          Get an already existing map (created in C++ or Lua script)
  /// \param mapName  The name of the map to find.
  /// \returns        The map or nil if it does not exist.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       self.playerInputMap = Input:GetMap("PlayerInputMap")
  ///     end  
  ///
  ///     function OnThink(self)
  ///       if self.map:GetTrigger("Jump")>0 then
  ///         self:Jump()
  ///       end
  ///     end
  ///   \endcode
  /// \see CreateMap
  /// \see GetMap
  /// \see VStringInputMap::MapTrigger
  /// \see VStringInputMap::GetTrigger
  VStringInputMap GetMap(string mapName);
 
  /// \brief Deletes the given input map.
  /// 			 
  /// \note Named input maps created via VScriptInput_wrapper::CreateMap will not automatically be destroyed when
  ///       the current scene is unloaded. Use VScriptInput_wrapper::DestroyMap to delete such a map if restoring 
  ///       the application's initial input behavior is desired.
  ///
  /// \param inputMap        The string input map to be destroyed.
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneUnloaded(self)
  ///
  ///       Input:DestroyMap(self.playerInputMap)
  ///
  ///     end  
  ///   \endcode
  /// \see CreateMap
  void DestroyMap(VStringInputMap inputMap);

  /// @}
  /// @ name Virtual Thumbstick Functions (Only if touch is available)
  /// @{

  /// \brief
  ///   Creates the single instance of a virtual thumb stick.
  ///
  /// The device name of the virtual thumb stick is "VirtualThumbStick". Pass
  /// this name in order to map CT_PAD_LEFT_THUMB_STICK_UP, CT_PAD_LEFT_THUMB_STICK_DOWN,
  /// CT_PAD_LEFT_THUMB_STICK_LEFT and CT_PAD_LEFT_THUMB_STICK_RIGHT using an input map.
  ///
  /// \sa VStringInputMap::MapTrigger
  ///
  void CreateVirtualThumbStick();

  /// \brief
  ///   Destroys the single instance of a virtual thumb stick.
  ///
  void DestroyVirtualThumbStick();
 
  /// @}
  /// @name Low Level Input Functions (PC Only)
  /// @{
  
  /// \brief Check if the specified key is pressed
  /// \param key The value representing a specific key (Vision.KEY_*)
  /// \return true if the key is pressed, otherwise false.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       Input:SetKeyAsSingleHit(Vision.KEY_V) -- setup V as a single hit key
  ///     end  
  ///
  ///     function OnThink(self)
  ///       -- toggle visibility of this entity
  ///       if Input:IsKeyPressed(Vision.KEY_V) then
  ///         self:SetVisible(not self:IsVisible())
  ///       end
  ///     end
  ///   \endcode
  /// \see SetKeyAsSingleHit 
  boolean IsKeyPressed(number key);

  /// \brief Handle a key repetition, a pressed key will report it's status only in the first frame of the change.
  /// \param key The value representing a key (Vision.KEY_*)
  /// \param singleHit (\b optional) Use true to setup this key with the "single hit" behavior, false to remove this behavior.
  /// \return true on success, otherwise false (e.g. there is no keyboard).
  /// \see IsKeyPressed
  boolean SetKeyAsSingleHit(number key, boolean singleHit = true);

  /// \brief Set the mouse position to specified 2D point.
  /// \param x X position inside the viewport.
  /// \param y Y position inside the viewport.
  /// \return true on success, otherwise false (e.g. there is no mouse).
  boolean SetMousePosition(number x, number y);
  
  /// \brief Check if a mouse button is pressed
  /// \param button The button to check, which has to be one of the following:
  ///  - BUTTON_LEFT: The left mouse button.
  ///  - BUTTON_MIDDLE: The middle mouse button.
  ///  - BUTTON_RIGHT: The right mouse button.
  /// \return true if the requested mouse button is pressed, otherwise false.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       if Input:IsMouseButtonPressed(Vision.BUTTON_LEFT) then
  ///         self:IncPosition(0,0,10*Timer:GetTimeDiff())
  ///       end
  ///     end
  ///   \endcode
  boolean IsMouseButtonPressed(number button);

  /// \brief Get the increment of the mouse wheel between the last frame and this frame.
  /// \param applyTimeDiff (\b optional) Set to true to multiply the delta value with the time difference, otherwise to false.
  /// \return The change of the mouse wheel as number.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       local delta = Input:GetMouseWheelDelta()
  ///       if delta ~= 0 then
  ///         self:IncPosition(0,0,10*delta)
  ///       end
  ///     end
  ///   \endcode
  number GetMouseWheelDelta(boolean applyTimeDiff = true);

  /// \brief Get the absolute mouse position.
  /// \return Two values representing the x and y mouse position.
  /// \par Example
  ///   \code
  ///     OnThink(self)
  ///       local width, height = Screen:GetViewportSize()
  ///       local x,y = Input:GetMousePosition()
  ///       -- move the entity absolute with the mouse motion
  ///       self:SetPosition(x-width*0.5,y-height*0.5,0)
  ///     end
  ///   \endcode
  multiple GetMousePosition();

  /// \brief Get the change of the mouse position.
  /// \param applyTimeDiff (\b optional) Set to true to multiply the delta values with the time difference, otherwise to false.
  /// \return Two values representing the difference in the x and y mouse position.
  /// \par Example
  ///   \code
  ///     OnThink(self)
  ///       local dx,dy = Input:GetMouseDelta()
  ///       -- move the entity relative with the mouse motion
  ///       self:IncPosition(dx,dy,0)
  ///     end
  ///   \endcode
  multiple GetMouseDelta(boolean applyTimeDiff = true);

  /// @}

};

#endif
