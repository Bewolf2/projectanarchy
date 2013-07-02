/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VMONITORCONTEXT_HPP_INCLUDED
#define VMONITORCONTEXT_HPP_INCLUDED

enum GUI_DEMO_CONTROL {
  GUI_DEMO_ACTION_1 = VISION_INPUT_CONTROL_LAST_ELEMENT+1,
  GUI_DEMO_ACTION_2,
  GUI_DEMO_ACTION_3,
  GUI_DEMO_EXIT
};


class VMonitorGUIContext;
typedef VSmartPtr<VMonitorGUIContext> VMonitorGUIContextPtr;

//////////////////////////////////////////////////////////////////////////////////////////////////
// Class MonitorRenderLoop:
// Implement a custom render loop attached to the monitor GUI context: This render loop
// isn't doing much: It just clears the screen and triggers a callback for rendering the GUI
//////////////////////////////////////////////////////////////////////////////////////////////////
class MonitorRenderLoop : public IVisRenderLoop_cl
{
public:
  MonitorRenderLoop(IVGUIContext* pGUIContext) : m_pGUIContext(pGUIContext)
  {
  }

  virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE
  {
    // Clear the screen
    Vision::RenderLoopHelper.ClearScreen();

    // send render callback
    VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_GUI);
    Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);

    // or as an alternative, trigger rendering directly
    // m_pGUIContext->OnRender();
  }

private:
  IVGUIContext* m_pGUIContext;
};



//////////////////////////////////////////////////////////////////////////////////////////////////
// Class VMonitorGUIContext:
// This class represents a GUI context on a texture used in the game (e.g. monitor)
// It is attached to an entity, replaces the texture of a specified material by the GUI texture
// and implements mouse cursor position by tracing the entity
//////////////////////////////////////////////////////////////////////////////////////////////////
class VMonitorGUIContext : public IVGUIContext
{
public:
  // constructor that takes an entity that it attaches to along with the surface name that it uses for picking and displaying
  VMonitorGUIContext(VGUIManager *pManager, VisSurface_cl *pScreenSrf);
  virtual ~VMonitorGUIContext();

  inline VisRenderContext_cl *GetRenderContext() const {return m_spRenderContext;}

  void TestPicking();
protected:
  // overridden GUI context functions
  VOVERRIDE void UpdateCursorPos(VGUIUserInfo_t &user);
  VOVERRIDE int GetButtonMask(VGUIUserInfo_t &user);
  VOVERRIDE void OnTickFunction(float fTimeDelta);
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);
private:
  VisRenderContextPtr m_spRenderContext;
  hkvVec2 m_vMouseTracePos, m_vTraceDelta;
  bool m_bPicked;
  float m_fTraceRange;
  VisSurface_cl *m_pGUISurface;
};

#endif

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
