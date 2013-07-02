/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vSceneViewer/vSceneViewerPCH.h>
#include <Vision/Tools/vSceneViewer/GUI/GUISystem.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>

VModule &GUIModule = VGUIManager::GUIModule();
V_IMPLEMENT_SERIAL( SceneViewerMainMenu, VDialog, 0, &GUIModule );

SceneViewerMainMenu::SceneViewerMainMenu()
{
  m_pCamera = 0;
}

SceneViewerMainMenu::~SceneViewerMainMenu()
{
  m_bButtonBHit=false;
  m_bButtonXHit=false;
}

void SceneViewerMainMenu::OnItemClicked(VMenuEventDataObject* pEvent)
{
  VDialog::OnItemClicked(pEvent);

  if (pEvent->m_pItem == m_pLoadButton)
  {
    m_bLoadNewScene = true;
  }
  else if(pEvent->m_pItem == m_pContinueButton)
  {
    ToggleView(m_pCamera);
  }
}

void SceneViewerMainMenu::OnValueChanged(VItemValueChangedEvent *pEvent)
{
  // On mobile we select to load (thus a value change happens)
  #ifdef _VISION_MOBILE
  if(pEvent->m_pItem == m_pSceneList)
  {
    m_bLoadNewScene = true;
  }
  #endif
}

void SceneViewerMainMenu::ToggleView(VisBaseEntity_cl * pCamera)
{
  // Toggle visibility state
  SetVisible(!IsVisible());

  if (IsVisible())
  {
    GetContext()->ShowDialog(this);
    GetContext()->SetActivate(true);
  }
  else
  {
    GetContext()->CloseDialog(this);
    // Deactivate context if there are no more open dialogs.
    if (GetContext()->m_OpenDialogs.Count() == 0)
      GetContext()->SetActivate(false);
  }

  m_pCamera = pCamera;
  m_pCamera->SetThinkFunctionStatus(!IsVisible());
}

void SceneViewerMainMenu::OnInitDialog()
{
  SetVisible(true);
  m_bLoadNewScene = false;

  m_pSceneList = (VListControl *)Items().FindItem(VGUIManager::GetID("SCENE_LIST"));
  VASSERT(m_pSceneList);

  m_pLoadButton = static_cast<VPushButton*>(Items().FindItem(VGUIManager::GetID("LOAD_SCENE_BUTTON")));
  m_pContinueButton = static_cast<VPushButton*>(Items().FindItem(VGUIManager::GetID("CONTINUE_BUTTON")));
  // Don't assert if buttons not available - not needed on all platforms

  // On mobile we select to load -> thus no preselection should happen
#ifdef _VISION_MOBILE
  bool autoSelect = m_pSceneList->Items().Count() == 1; // if only one choice, just take it
#else
  bool autoSelect = true;
#endif
  if (autoSelect)
      m_pSceneList->SetSelectionIndex(0);
}

void SceneViewerMainMenu::OnTick(float dtime)
{
  VDialog::OnTick(dtime);

  if(VisSampleApp::GetInputMap()->GetTrigger(SCENE_LOAD_NEW_SCENE) || (m_pSceneList->Items().Count() == 1) )
  {
    m_bLoadNewScene = true;
#ifdef _VISION_MOBILE
    if (m_pSceneList->Items().Count() == 1)
        m_pSceneList->SetSelectionIndex(0);
#endif
  }

#if defined(WIN32) && ( !defined(_VISION_WINRT) || defined(_VISION_METRO) )

#elif defined(_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_APOLLO))
  if (VInputManagerXenon::GetPad(0).GetRawControlValue(CT_PAD_DOWN))

#elif defined(_VISION_PS3)
  if (VInputManagerPS3::GetPad(0).GetRawControlValue(CT_PAD_DOWN))

#elif defined(_VISION_PSP2)
  if (VInputManagerPSP2::GetPad(0).GetRawControlValue(CT_PAD_DOWN))

#elif defined(_VISION_WIIU)
  if (VInputManagerWiiU::GetDRC(V_DRC_FIRST).GetRawControlValue(CT_PAD_DOWN))
  
#elif defined(_VISION_MOBILE)
  if(false)
#endif
  {
    if(!m_bButtonBHit)
    {
      m_bButtonBHit = true;
      int selected = m_pSceneList->GetSelectionIndex();
      if (++selected >= m_pSceneList->Items().Count())
      {
        selected = 0;
      }
      m_pSceneList->SetSelectionIndex(selected);
      m_pSceneList->EnsureVisible(m_pSceneList->GetSelectedItem());
    }
  }
#if !defined(WIN32) || ( defined(_VISION_WINRT) && !defined(_VISION_METRO) )
  else
  {
    m_bButtonBHit = false;
  }
#endif

#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))


#elif defined(_VISION_XENON) ||  (defined(_VISION_WINRT) && !defined(_VISION_APOLLO)) 
  if (VInputManagerXenon::GetPad(0).GetRawControlValue(CT_PAD_UP))

#elif defined(_VISION_PS3)
  if (VInputManagerPS3::GetPad(0).GetRawControlValue(CT_PAD_UP))

#elif defined(_VISION_PSP2)
  if (VInputManagerPSP2::GetPad(0).GetRawControlValue(CT_PAD_UP))

#elif defined(_VISION_WIIU)
  if (VInputManagerWiiU::GetDRC(V_DRC_FIRST).GetRawControlValue(CT_PAD_UP))

#elif defined(_VISION_MOBILE)
  if(false)
#endif
  {
    if(!m_bButtonXHit)
    {
      m_bButtonXHit = true;
      int selected = m_pSceneList->GetSelectionIndex();
      if (--selected < 0 )
      {
        selected = m_pSceneList->Items().Count()-1;
      }
      m_pSceneList->SetSelectionIndex(selected);
      m_pSceneList->EnsureVisible(m_pSceneList->GetSelectedItem());
    }
  }
#if !defined(WIN32) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  else
  {
    m_bButtonXHit = false;
  }

  if (VisSampleApp::GetInputMap()->GetTrigger(SCENE_TOGGLE_MENU))
  {
    if(m_pCamera)
      ToggleView(m_pCamera);
  }

#endif

  if (VisSampleApp::GetInputMap()->GetTrigger(SCENE_TOGGLE_MENU))
  {
    if(m_pCamera)
      ToggleView(m_pCamera);
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
