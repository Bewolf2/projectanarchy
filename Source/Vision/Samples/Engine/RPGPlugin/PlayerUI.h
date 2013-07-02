/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_PLAYER_UI_H__
#define RPG_PLUGIN_PLAYER_UI_H__

#include <Vision/Samples/Engine/RPGPlugin/PlayerUIDialog.h>

//#define RPG_GUI_USE_SCALEFORM

class RPG_PlayerControllerComponent;
typedef VSmartPtr<RPG_PlayerControllerComponent> RPG_PlayerControllerComponentPtr;

class RPG_PlayerUI
{
public:
  void SetController(RPG_PlayerControllerComponent *controller);

  RPG_PlayerControllerComponent *GetController() const
  {
    return m_controller;
  }

  PlayerUIDialog *GetDialog() const { return m_dialog; }

public:
  static RPG_PlayerUI s_instance;

private:
  VGUIMainContextPtr m_GUIContext;

  VSmartPtr<PlayerUIDialog> m_dialog;

  RPG_PlayerControllerComponentPtr m_controller;
};

// Base dialog
class RPG_PlayerUIDialog : public VDialog
{
public:
  RPG_PlayerUIDialog();

  void SetController(RPG_PlayerControllerComponent *playerController);

protected:
  // VDialog
  void OnActivate() HKV_OVERRIDE;

  void OnTick(float deltaTime) HKV_OVERRIDE;

protected:
  RPG_PlayerControllerComponentPtr m_playerController;

protected:
  VImageControl *m_healthBar;
  float m_healthBarWidth;

  VImageControl *m_manaBar;
  float m_manaBarWidth;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_PlayerUIDialog, RPG_PLUGIN_IMPEXP);
};

// Mouse dialog
class RPG_PlayerUIDialog_Mouse : public RPG_PlayerUIDialog
{
public:
  RPG_PlayerUIDialog_Mouse();

protected:
  // VDialog
  void OnActivate() HKV_OVERRIDE;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_PlayerUIDialog_Mouse, RPG_PLUGIN_IMPEXP);
};

// Touch dialog
class RPG_PlayerUIDialog_Touch : public RPG_PlayerUIDialog
{
public:
  RPG_PlayerUIDialog_Touch();

protected:
  // VDialog
  void OnActivate() HKV_OVERRIDE;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_PlayerUIDialog_Touch, RPG_PLUGIN_IMPEXP);
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
