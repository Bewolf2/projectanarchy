/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/ModelMerging/ModelMergingPCH.h>
#include <Vision/Samples/Engine/ModelMerging/GUI/MenuSystem.hpp>


VModule &GUIModule = VGUIManager::GUIModule();
V_IMPLEMENT_SERIAL( MergeModelsMainMenu, VDialog, 0, &GUIModule );

MergeModelsMainMenu::~MergeModelsMainMenu()
{
  m_pMergedModelFactory = NULL;
}

void MergeModelsMainMenu::OnInitDialog()
{
  // Position the GUI to the left side of the screen
  SetPosition(Vision::Video.GetXRes() - GetSize().x,0);
  SetSize(GetSize().x, (float)Vision::Video.GetYRes());

  m_pArmArmorCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_ARM"));
  m_pShoulderArmorCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_SHOULDER"));
  m_pLegsArmorCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_LEGS"));
  m_pKneeArmorCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_KNEE"));
  m_pAccessoireCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_ACCESSOIRE"));
  m_pBeltCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_BELT"));
  m_pClothCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_CLOTH"));
  m_pBeardCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_BEARD"));
  m_pAxeCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_AXE"));
  m_pSwordCB = (VCheckBox *)Items().FindItem(VGUIManager::GetID("CHECKBOX_SWORD"));

  VASSERT(m_pArmArmorCB);
  VASSERT(m_pShoulderArmorCB);
  VASSERT(m_pLegsArmorCB);
  VASSERT(m_pKneeArmorCB);
  VASSERT(m_pAccessoireCB);
  VASSERT(m_pBeltCB);
  VASSERT(m_pClothCB);
  VASSERT(m_pBeardCB);
  VASSERT(m_pAxeCB);
  VASSERT(m_pSwordCB);

  m_pMergedModelFactory = NULL;
}

void MergeModelsMainMenu::OnActivate()
{
  if (m_pMergedModelFactory == NULL)
    return;

  m_pMergedModelFactory->PreviewModel();

  VDialog::OnActivate();
}

void MergeModelsMainMenu::OnDeactivate()
{
  if (m_pMergedModelFactory == NULL)
    return;

  m_pMergedModelFactory->MergeModel();

  VDialog::OnDeactivate();
}

void MergeModelsMainMenu::OnValueChanged(VItemValueChangedEvent *pEvent)
{
  VDialog::OnValueChanged(pEvent);
}

void MergeModelsMainMenu::OnItemClicked(VMenuEventDataObject *pEvent)
{
  VDialog::OnItemClicked(pEvent);

  // CHECKBOX_ARM
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_ARM"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_ARM);

  // CHECKBOX_SHOULDER
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_SHOULDER"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_SHOULDER);

  // CHECKBOX_LEGS
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_LEGS"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_LEGS);

  // CHECKBOX_KNEE
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_KNEE"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_KNEE);

  // CHECKBOX_ACCESSOIRE
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_ACCESSOIRE"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_ACCESSOIRE);

  // CHECKBOX_BELT
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_BELT"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_BELT);

  // CHECKBOX_CLOTH
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_CLOTH"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_CLOTH);

  // CHECKBOX_BEARD
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_BEARD"))
    m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_BEARD);

  // CHECKBOX_AXE
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_AXE"))
  {
    if (m_pAxeCB->IsChecked())
      m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_AXE);
    m_pAxeCB->SetChecked(true);
    m_pSwordCB->SetChecked(false);
  }

  // CHECKBOX_SWORD
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CHECKBOX_SWORD"))
  {
    if (m_pSwordCB->IsChecked())
      m_pMergedModelFactory->ToggleBodyPart(BARBARIAN_AXE);
    m_pAxeCB->SetChecked(false);
    m_pSwordCB->SetChecked(true);
  }
}


void MergeModelsMainMenu::OnTick(float dtime)
{
  VDialog::OnTick(dtime);
}

void MergeModelsMainMenu::SetMergeModelFactory(MergedModelFactory_cl *factory)
{
  m_pMergedModelFactory = factory;
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
