/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/GUIManager.h>
#if (RPG_UI_SCALEFORM)
#include <Vision/Samples/Engine/RPGPlugin/GUIManager_Scaleform.h>
#endif
#include <Vision/Samples/Engine/RPGPlugin/GUIManager_VisionGUI.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUIDialog.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUI.h>

RPG_GuiManager* RPG_GuiManager::s_instance = NULL;

RPG_GuiManager::RPG_GuiManager()
  : m_parentDialog(NULL)
{
}

RPG_GuiManager* RPG_GuiManager::GetInstance()
{
  if (!s_instance)
  {
#if (RPG_UI_SCALEFORM)
    s_instance = new RPG_GuiManager_Scaleform();
#else
    s_instance = new RPG_GuiManager_VisionGUI();
#endif
  }
  return s_instance;
}

void RPG_GuiManager::DestroyInstance()
{
  if (s_instance)
  {
    V_SAFE_DELETE(s_instance);
    s_instance = NULL;
  }
  else
  {
    VASSERT_MSG(false, "Tried to destroy GUIManager instance after it has already been destroyed.");
  }
}

void RPG_GuiManager::OnActivate(PlayerUIDialog* parentDialog)
{
  VASSERT(parentDialog);
  m_parentDialog = parentDialog;
}

void RPG_GuiManager::OnDeactivate()
{
  DestroyInstance();
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
