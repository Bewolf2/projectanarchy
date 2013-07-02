/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/CharacterLOD/CharacterLODPCH.h>
#include <Vision/Samples/Engine/CharacterLOD/GUI/MenuSystem.hpp>
#include <Vision/Samples/Engine/CharacterLOD/LODObject.h>


VModule &GUIModule = VGUIManager::GUIModule();
V_IMPLEMENT_SERIAL( AnimationMainMenu, VDialog, 0, &GUIModule );
V_IMPLEMENT_SERIAL( VDialogSubCtrl, VDlgControlBase, 0, &GUIModule );

#define ADD_MODE(val,name,desc) \
  pItem = m_pLODSelectionList->AddItem(name,-1,val);\
  pItem->SetTooltipText(desc);

void AnimationMainMenu::OnInitDialog()
{
  // position the GUI to the left side of the screen
  SetPosition(Vision::Video.GetXRes() - GetSize().x,0);
  SetSize(GetSize().x, (float)Vision::Video.GetYRes());

  // find the entities in the scene
  for (int i=0;i<ENTITY_COUNT;i++)
  {
    char szKey[64];
    sprintf(szKey,"Soldier_%i",i);
    m_pSoldier[i] = (LODObject *)Vision::Game.SearchEntity(szKey);
    VASSERT(m_pSoldier[i]);
  }
  // sub menues
  m_pSubMenu = (VDialogSubCtrl *)Items().FindItem(VGUIManager::GetID("SUBMENU"));
  m_spSubMenuDialog = GetMenuManager()->CreateDialogInstance("CharacterLOD\\Dialogs\\SubMenu.xml", GetContext(), this);
  VASSERT(m_pSubMenu && m_spSubMenuDialog);

  // skeletal anim
  m_pLODSelectionList = (VListControl *)m_spSubMenuDialog->Items().FindItem(VGUIManager::GetID("LOD_SELECTION"));
  m_pCheckBoxStopPlay   = (VCheckBox *)m_spSubMenuDialog->Items().FindItem(VGUIManager::GetID("STOP_PLAY"));
  VASSERT(m_pLODSelectionList && m_pCheckBoxStopPlay);
  
  // fill the list control (put the enum in the data member):
  VListControlItem *pItem;
  ADD_MODE(LOD_HIGH,     "High Quality",     "Use characters with high polygon count");
  ADD_MODE(LOD_MEDIUM,   "Medium Quality",   "Use characters with medium polygon count");
  ADD_MODE(LOD_LOW,      "Low Quality",      "Use characters with low polygon count");
  ADD_MODE(LOD_ULTRALOW, "UltraLow Quality", "Use characters with ultra low polygon count");
  ADD_MODE(LOD_AUTO,     "Use AutoLOD",          "Choose LOD-Level by distance");

  m_pSubMenu->SetDialog(m_spSubMenuDialog);

  // set the initial mode
  SetLODLevel(LOD_AUTO);
}



void AnimationMainMenu::OnValueChanged(VItemValueChangedEvent *pEvent)
{
  VDialog::OnValueChanged(pEvent);

  // start single skeletal anim
  if (pEvent->m_pItem==m_pCheckBoxStopPlay)
  {
    for (int i=0;i<ENTITY_COUNT;i++)
      m_pSoldier[i]->SetPlayAnimation(pEvent->AsBool());
    return;
  }


  //set lod level
  if (pEvent->m_pItem==m_pLODSelectionList)
  {
    SetLODLevel((LODLevel_e)pEvent->AsListItem()->GetData());
    return;
  }

}

// switch the skeletal playing mode
void AnimationMainMenu::SetLODLevel(LODLevel_e eMode)
{
  //select the list item in the control
  m_pLODSelectionList->SetSelection(m_pLODSelectionList->Items().FindItemByDataValue(eMode));

  //update the entities
  for (int i=0;i<ENTITY_COUNT;i++)
    m_pSoldier[i]->m_LOD.SetLODLevel(eMode);

}

/////////////////////////////////////////////////////////////////////////////////

VWindowBase* VDialogSubCtrl::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  if (!VDlgControlBase::TestMouseOver(user, vAbsMouse))
    return NULL;
  if (!m_spDialog)
    return NULL;

  VWindowBase* pDlgItem = m_spDialog->TestMouseOver(user, vAbsMouse);
  if (pDlgItem==m_spDialog)
    return this; // do not return the dialog, because it would drag around
  return pDlgItem;
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
