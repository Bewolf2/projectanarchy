/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptInput_wrapper.hpp

#ifndef __VSCRIPTINPUT_WRAPPER_HPP
#define __VSCRIPTINPUT_WRAPPER_HPP

#include <Vision/Runtime/Base/VGL/VGL.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VStringInputMap.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

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

/// 
/// \brief
///   Small wrapper to unify input inside the scripting binding
/// 
class VScriptInput_wrapper
{
public:

  VScriptInput_wrapper()
  {}

  ~VScriptInput_wrapper() 
  {
#if defined(SUPPORTS_MULTITOUCH)
    V_SAFE_DELETE(s_pVirtualThumbStick);
#endif
  }

  inline bool IsKeyPressed(int iKey)
  {
    #ifdef SUPPORTS_KEYBOARD
       return Vision::Key.IsPressed(iKey)==TRUE;
    #else
       return false;
    #endif
  }

  inline bool SetKeyAsSingleHit(int iKey, bool bSingleHit = true)
  {
    #ifdef SUPPORTS_KEYBOARD
      return Vision::Key.SetSingleHit(iKey, bSingleHit)==VERR_NOERROR;
    #else
      return false;
    #endif
  }

  inline bool SetMousePosition(float fInsideViewportX, float fInsideViewportY)
  {
    #ifdef SUPPORTS_MOUSE
      Vision::Mouse.SetPosition(fInsideViewportX, fInsideViewportY);
      return true;
    #else
      return false;
    #endif
  }

  inline bool IsMouseButtonPressed(int iButton)
  {
    #ifdef SUPPORTS_MOUSE
      switch(iButton)
      {
        case BUTTON_LEFT: return Vision::Mouse.IsLeftButtonPressed() == TRUE;
        case BUTTON_MIDDLE: return Vision::Mouse.IsMiddleButtonPressed() == TRUE;
        case BUTTON_RIGHT: return Vision::Mouse.IsRightButtonPressed() == TRUE;
        default: return false;
      }
    #else
      return false;
    #endif
  }

  inline float GetMouseWheelDelta(bool bApllyTimeDiff = true)
  {
    #ifdef SUPPORTS_MOUSE
      if(bApllyTimeDiff)
      {
        VScriptResourceManager* pMan = (VScriptResourceManager*) Vision::GetScriptManager();
        VASSERT(pMan);

        // script thinking delta or simulation tick delta
        float fTimeDiff = pMan->GetThinkInterval();
        if(fTimeDiff<=0) fTimeDiff = Vision::GetTimer()->GetTimeDifference();

        return (float)Vision::Mouse.GetWheelDelta()*fTimeDiff;
      }
      else
      {
        return (float)Vision::Mouse.GetWheelDelta();
      }
    #else
      return 0;
    #endif
  }

  inline VStringInputMap * CreateMap(const char * szMapName, int iNumTriggers=32, int iNumAlternatives=4)
  {
    // Check if map already exists
    VStringInputMap* pMap = VStringInputMap::FindByKey(szMapName);
    if (pMap != NULL)
    {
      V_SAFE_DELETE(pMap);
    }

    return new VStringInputMap(szMapName, iNumTriggers, iNumAlternatives);
  }

  inline VStringInputMap * GetMap(const char * szMapName)
  {
    return VStringInputMap::FindByKey(szMapName);
  }

  inline void DestroyMap(VStringInputMap * pMap)
  {
    if (pMap != NULL)
    {
      V_SAFE_DELETE(pMap);
    }
  }

  // Virtual Thumbstick
  inline void CreateVirtualThumbStick()
  {
#if defined(SUPPORTS_MULTITOUCH)
    if (s_pVirtualThumbStick != NULL)
      return;
    s_pVirtualThumbStick = new VVirtualThumbStick();
#endif
  }

  inline void DestroyVirtualThumbStick()
  {
#if defined(SUPPORTS_MULTITOUCH)
    V_SAFE_DELETE(s_pVirtualThumbStick);
#endif
  }

  // internal helpers:
  static inline int MapTrigger( VStringInputMap *pMap, const char * szTriggerName,
                                VString & sDeviceName, int iControl,
                                const VInputOptions & options, int iOptTriggerIndex /* = -1 */,
                                const VRectanglef & area, float fPrio,
                                bool bNewTouchArea, int iOptionaAxisControl = -1)
  {
    VASSERT_MSG(pMap!=NULL, "No VStringInputMap present for mapping!");

    if(szTriggerName==NULL || szTriggerName[0]==0)
    {
      Vision::Error.Warning("[Lua] MapTrigger: Trigger name is empty, please specify a proper name.");
      return -1;
    }
    
    sDeviceName.ToLower();

    IVInputDevice* pInputDevice = NULL;
#if defined(SUPPORTS_MULTITOUCH)
    // Special case handling for the virtual thumb stick.
    if (sDeviceName == "virtualthumbstick")
    {
      pInputDevice = s_pVirtualThumbStick;
    }
    else
#endif
    {
      pInputDevice = &VInputManager::GetInputDevice(sDeviceName.AsChar());
    }

    if (pInputDevice == NULL || pInputDevice->GetModel() == INPUT_DEVICE_NONE)
    {
      Vision::Error.Warning("[Lua] MapTrigger: Could not find input device '%s' for trigger '%s'.", sDeviceName.AsChar(), szTriggerName);
      return -1;
    }

    //touch area handling
    if (sDeviceName.Search("touch") || sDeviceName.Search("back"))
    {
      IVMultiTouchInput *pTouchDevice = (IVMultiTouchInput *)pInputDevice;

      if(!bNewTouchArea)
      {
        VTouchArea **ppAreas = pTouchDevice->GetTouchAreas().GetPtrs();

        for(int i=0;i<pTouchDevice->GetTouchAreas().GetLength();i++)
        {
          if(ppAreas[i]->GetArea() == area)
          {
            return pMap->MapTrigger(szTriggerName, ppAreas[i], iControl, options, iOptTriggerIndex);
          }
        }
      }

      //we could not find a matching area -or- do not create a new one:
      VTouchArea* pArea = new VTouchArea(*pTouchDevice, area, fPrio);
      return pMap->MapTrigger(szTriggerName, pArea, iControl, options, iOptTriggerIndex);
    }   

    //mapping for non-touch devices
    if (iOptionaAxisControl >= 0)
      return pMap->MapTriggerAxis(szTriggerName, *pInputDevice, iControl, (unsigned int) iOptionaAxisControl, options, iOptTriggerIndex);
    else
      return pMap->MapTrigger(szTriggerName, *pInputDevice, iControl, options, iOptTriggerIndex);
  }

private:
  // Member variables
#if defined(SUPPORTS_MULTITOUCH)
  static VVirtualThumbStick* s_pVirtualThumbStick;
#endif
};

#endif // __VSCRIPTINPUT_WRAPPER_HPP

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
