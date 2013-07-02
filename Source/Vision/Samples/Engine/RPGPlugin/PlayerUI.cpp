/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/PlayerUI.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUIDialog.h>

#include <Vision/Samples/Engine/RPGPlugin/PlayerControllerComponent.h>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

// RPG_PlayerUI
void RPG_PlayerUI::SetController(RPG_PlayerControllerComponent *controller)
{
  if(m_controller)
  {
    if(m_GUIContext)
    {
      m_GUIContext->SetActivate(false);
      m_GUIContext = NULL;

      m_dialog = NULL;
    }
  }

  m_controller = controller;

  if(m_controller)
  {
    m_GUIContext = new VGUIMainContext(NULL);

#if 0
    RPG_PlayerUIDialog *dialog = vstatic_cast<RPG_PlayerUIDialog*>(m_GUIContext->ShowDialog("GUI\\MouseHUD.xml"));
    dialog->SetController(controller);

    m_dialog = dialog;
#else
    m_dialog = new PlayerUIDialog(m_controller);
    m_dialog->InitDialog(m_GUIContext, NULL, NULL);

    m_GUIContext->ShowDialog(m_dialog);
#endif
    m_GUIContext->SetActivate(true);
  }
}

RPG_PlayerUI RPG_PlayerUI::s_instance;

V_IMPLEMENT_SERIAL(RPG_PlayerUIDialog, VDialog, 0, &g_RPGPluginModule);

RPG_PlayerUIDialog::RPG_PlayerUIDialog()
  : VDialog()
{
}

void RPG_PlayerUIDialog::SetController(RPG_PlayerControllerComponent *playerController)
{
  m_playerController = playerController;
}

void RPG_PlayerUIDialog::OnActivate()
{
  // Position energy bar control
  VItemContainer *energyBars = vstatic_cast<VItemContainer*>(Items().FindItem(VGUIManager::GetID("ID_HUD_EnergyBars")));
  VASSERT(energyBars);
  if(energyBars)
  {
    energyBars->SetPosition((GetSize().x - energyBars->GetSize().x) / 2, energyBars->GetPosition().y);

    // Store health and mana bar image controls
    m_healthBar = vstatic_cast<VImageControl*>(energyBars->SubItems().FindItem(VGUIManager::GetID("ID_HUD_HealthBar")));
    VASSERT(m_healthBar);
    if(m_healthBar)
      m_healthBarWidth = m_healthBar->GetSize().x;

    m_manaBar = vstatic_cast<VImageControl*>(energyBars->SubItems().FindItem(VGUIManager::GetID("ID_HUD_ManaBar")));
    VASSERT(m_manaBar);
    if(m_manaBar)
      m_manaBarWidth = m_manaBar->GetSize().x;
  }
}

void RPG_PlayerUIDialog::OnTick(float deltaTime)
{
  m_healthBar->SetSize(0.5f * m_healthBarWidth, m_healthBar->GetSize().y);
  m_manaBar->SetSize(0.5f * m_manaBarWidth, m_manaBar->GetSize().y);
}

V_IMPLEMENT_SERIAL(RPG_PlayerUIDialog_Mouse, RPG_PlayerUIDialog, 0, &g_RPGPluginModule);

RPG_PlayerUIDialog_Mouse::RPG_PlayerUIDialog_Mouse()
  : RPG_PlayerUIDialog()
{
}

void RPG_PlayerUIDialog_Mouse::OnActivate()
{
  RPG_PlayerUIDialog::OnActivate();

  // Load resources (cursor)
  GetMenuManager()->LoadResourceFile("GUI\\MouseHUDResources.xml");
}

V_IMPLEMENT_SERIAL(RPG_PlayerUIDialog_Touch, RPG_PlayerUIDialog, 0, &g_RPGPluginModule);

RPG_PlayerUIDialog_Touch::RPG_PlayerUIDialog_Touch()
  : RPG_PlayerUIDialog()
{
}

void RPG_PlayerUIDialog_Touch::OnActivate()
{
  RPG_PlayerUIDialog::OnActivate();

  // Position skill button controls
  {
    VDlgControlBase *skillButtons = Items().FindItem(VGUIManager::GetID("ID_HUD_SkillButtonsLeft"));
    VASSERT(skillButtons);
    if(skillButtons)
      skillButtons->SetPosition(skillButtons->GetPosition().x, (GetSize().y - skillButtons->GetSize().y) / 2);
  }
  {
    VDlgControlBase *skillButtons = Items().FindItem(VGUIManager::GetID("ID_HUD_SkillButtonsRight"));
    VASSERT(skillButtons);
    if(skillButtons)
      skillButtons->SetPosition((GetSize().x - skillButtons->GetSize().x), (GetSize().y - skillButtons->GetSize().y) / 2);
  }
}

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
