/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/GUIHud_VisionGUI.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUI.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUIDialog.h>

V_IMPLEMENT_SERIAL(RPG_GUIHud, VDialog, 0, &g_RPGPluginModule);

RPG_GUIHud::RPG_GUIHud()
  : VDialog()
{
}

void RPG_GUIHud::OnInitDialog()
{
  VDialog::OnInitDialog();
}

void RPG_GUIHud::OnSetFocus(bool focus)
{
  VDialog::OnSetFocus(focus);
}

void RPG_GUIHud::OnTick(float deltaTime)
{
  VDialog::OnTick(deltaTime);
}

void RPG_GUIHud::OnKeyPressed(int iKey, int iKeyModifier)
{
  VDialog::OnKeyPressed(iKey, iKeyModifier);
}

void RPG_GUIHud::OnItemClicked(VMenuEventDataObject* menuEvent)
{
  VDialog::OnItemClicked(menuEvent);

  Vision::Message.Add(1,"RPG_GUIHud::OnItemClicked()");

  if (menuEvent->m_pItem->GetID() == VGUIManager::GetID("BUTTON_RANGEDATTACK"))
  {
    Vision::Message.Add(1,"BUTTON_RANGEDATTACK");
    return;
  }
  else if (menuEvent->m_pItem->GetID() == VGUIManager::GetID("BUTTON_POWERATTACK"))
  {
    Vision::Message.Add(1,"BUTTON_POWERATTACK");
    return;
  }
  else if (menuEvent->m_pItem->GetID() == VGUIManager::GetID("BUTTON_AOEATTACK"))
  {
    Vision::Message.Add(1,"BUTTON_AOEATTACK");
    return;
  }
  else
  {
    Vision::Error.Warning("Unrecognized control ID clicked.");
    return;
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
