/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


V_IMPLEMENT_SERIAL(VisProfilingMenu, VDialog, 0, &VGUIManager::GUIModule());

void VisProfilingMenu::OnInitDialog()
{
  // position the GUI to the left side of the screen
  SetPosition(Vision::Video.GetXRes() - GetSize().x, 0);
  
  m_iFpsID = VGUIManager::GetID("PMID_FRAMERATE");
  m_iEntityID = VGUIManager::GetID("PMID_ENTITYDEBUG");
  m_iPolyCountID = VGUIManager::GetID("PMID_POLYCOUNT");
  m_iPrevPageID = VGUIManager::GetID("PMID_PREVPAGE");
  m_iNextPageID = VGUIManager::GetID("PMID_NEXTPAGE");
  m_iResetMaxID = VGUIManager::GetID("PMID_RESETMAX");
  m_iReloadResourcesID = VGUIManager::GetID("PMID_RELOADRESOURCES");
  m_iHideID = VGUIManager::GetID("PMID_HIDE");
}

void VisProfilingMenu::OnItemClicked(VMenuEventDataObject *pEvent)
{
  VDialog::OnItemClicked(pEvent);
  int iID = pEvent->m_pItem->GetID();
  
  if (iID == m_iFpsID)
  {
    static bool show = false;
    show = !show;
    static_cast<VisSampleApp*>(Vision::GetApplication())->SetShowFrameRate(show);
  }
  else if (iID == m_iEntityID)
  {
    static int iFlags = 0; //no flags on
    iFlags <<=1 ; //move on to next flag
    if (iFlags==0)
      iFlags = 1;
    if (iFlags >= DEBUGRENDERFLAG_ALL)
      iFlags = 0; //start at the beginning again without any flag
    
    if (iFlags & DEBUGRENDERFLAG_VISION_VERSION)
      Vision::Message.Add("Display vision version");
    else if (iFlags & DEBUGRENDERFLAG_OBJECT_TRIANGLECOUNT)
      Vision::Message.Add("Display object triangle count");
    else if (iFlags & DEBUGRENDERFLAG_OBJECT_VISBBOX)
      Vision::Message.Add("Display object visibility bounding box");
    else if (iFlags & DEBUGRENDERFLAG_OBJECT_VISIBILITYZONES)
      Vision::Message.Add("Display object vis area assignment");
    else if (iFlags & DEBUGRENDERFLAG_LIGHT_INFLUENCEBOX)
      Vision::Message.Add("Display light influence bounding box");
    else if (iFlags & DEBUGRENDERFLAG_TRACELINES)
      Vision::Message.Add("Display trace lines");
    else if (iFlags & DEBUGRENDERFLAG_VISIBILITYOBJECTS)
      Vision::Message.Add("Display visibility objects");
    else if (iFlags & DEBUGRENDERFLAG_PORTALS)
      Vision::Message.Add("Display portals");
    else if (iFlags & DEBUGRENDERFLAG_VISIBILITYZONES)
      Vision::Message.Add("Display visibility zones");
    else if (iFlags & DEBUGRENDERFLAG_SCRIPTSTATISTICS)
    {
      if (Vision::GetScriptManager()!=NULL)
        Vision::Message.Add("Scripting statistics");
      else
        Vision::Message.Add("Scripting statistics (no script manager installed)");
    }
    else if (iFlags & DEBUGRENDERFLAG_OBJECTRENDERORDER)
      Vision::Message.Add("Display object render order");
    else if (iFlags & DEBUGRENDERFLAG_THREADWORKLOAD)
      Vision::Message.Add("Display thread workload");
    else if (iFlags & DEBUGRENDERFLAG_ZONES)
      Vision::Message.Add("Display streaming zones");
    else
      Vision::Message.Add("");
    
    Vision::Profiling.SetDebugRenderFlags(iFlags); 
  }
  else if (iID == m_iPolyCountID)
  {
    Vision::Profiling.ToggleObjectTriangleCounts();
    Vision::Profiling.ToggleOverallPolygons();
  }
  else if (iID == m_iPrevPageID)
  {
    Vision::Profiling.PreviousPage();
  }
  else if (iID == m_iNextPageID)
  {
    Vision::Profiling.NextPage();
  }
  else if (iID == m_iResetMaxID)
  {
    Vision::Profiling.ResetProfilingMaxValues();
  }
  else if (iID == m_iReloadResourcesID)
  {
    //special shader assignment treatment
    Vision::Shaders.GetShaderFXLibManager().ResetCompiledEffectCaches();

    int iCount = Vision::ResourceSystem.ReloadModifiedResourceFiles(NULL, TRUE, TRUE);
    
    // Reassign all shaders
    Vision::Shaders.ReloadAllShaderAssignmentFiles();

    Vision::Message.Add(1, "%d resources were outdated and have been reloaded",iCount);
  }
  else if (iID == m_iHideID)
  {
    GetContext()->CloseDialog(this);
    SetVisible(false);
    if (GetContext()->m_OpenDialogs.Count() == 0)
      GetContext()->SetActivate(false);
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
