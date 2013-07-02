/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

V_IMPLEMENT_SERIAL(VisMobileExitDialog, VDialog, 0, &VGUIManager::GUIModule());

void VisMobileExitDialog::OnInitDialog()
{
  // center the GUI
  SetPosition(
    Vision::Video.GetXRes()*0.5f - GetSize().x*0.5f, 
    Vision::Video.GetYRes()*0.5f - GetSize().y*0.5f);
  
  m_iExitId = VGUIManager::GetID("ID_EXIT");
  m_iCancelId = VGUIManager::GetID("ID_CANCEL"); 

  m_bExitTriggered = false;
}

void VisMobileExitDialog::OnItemClicked(VMenuEventDataObject *pEvent)
{
  VDialog::OnItemClicked(pEvent);
  int iID = pEvent->m_pItem->GetID();
  
  if (iID == m_iExitId)
  {
    m_bExitTriggered = true;
  }
  else if (iID == m_iCancelId)
  {
    CloseDialog();
  }
}

void VisMobileExitDialog::OnSetFocus(bool bStatus)
{
  // Refuse to unfocus if dialog is visible.
  if (bStatus || !IsVisible())
    VDialog::OnSetFocus(bStatus);
}

void VisMobileExitDialog::CloseDialog()
{
  GetContext()->CloseDialog(this);
  SetVisible(false);
  if (GetContext()->m_OpenDialogs.Count() == 0)
    GetContext()->SetActivate(false);

  m_bExitTriggered = false;
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
