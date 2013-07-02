/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef GUI_SYSTEM_HPP_INCLUDED
#define GUI_SYSTEM_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

class SceneViewerMainMenu;
typedef VSmartPtr<SceneViewerMainMenu> SceneViewerMainMenuPtr;

class SceneViewerMainMenu : public VDialog
{
public:
  SceneViewerMainMenu();
  ~SceneViewerMainMenu();
  VOVERRIDE void OnInitDialog();
  VOVERRIDE void OnTick(float dtime);

  VOVERRIDE void OnItemClicked(VMenuEventDataObject *pEvent);
  VOVERRIDE void OnValueChanged(VItemValueChangedEvent *pEvent);

  void ToggleView(VisBaseEntity_cl * pCamera);
  bool IsLoadNewScene() {return m_bLoadNewScene;}
  void ResetLoadNewScene() {m_bLoadNewScene=false;}
  const char *GetScenePath() const {return ( m_pSceneList->GetSelectedItem() != NULL ) ? ( m_pSceneList->GetSelectedItem()->m_sTooltipText.AsChar() ) : NULL; }

protected:
  V_DECLARE_SERIAL_DLLEXP( SceneViewerMainMenu, GUI_IMPEXP )
  VOVERRIDE void Serialize( VArchive &ar ) {}

  bool m_bButtonBHit;
  bool m_bButtonXHit;

  bool m_bLoadNewScene;
  VListControl *m_pSceneList;
  VPushButton* m_pLoadButton;
  VPushButton* m_pContinueButton;

  VisBaseEntity_cl* m_pCamera;
};

#endif // GUI_SYSTEM_HPP_INCLUDED

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
