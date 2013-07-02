/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef MENU_SYSTEM_HPP_INCLUDED
#define MENU_SYSTEM_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/CharacterLOD/LODObject.h>

class SubMenu_Skeletal;
class VDialogSubCtrl;
class LODObject;

#define ENTITY_COUNT    9

class AnimationMainMenu : public VDialog
{
public:
  VOVERRIDE void OnInitDialog();
  VOVERRIDE void OnValueChanged(VItemValueChangedEvent *pEvent);

  void SetLODLevel(LODLevel_e eMode);

protected:
  V_DECLARE_SERIAL_DLLEXP( AnimationMainMenu, DECLSPEC_DLLEXPORT )
  VOVERRIDE void Serialize( VArchive &ar ) {}

  //entity
  LODObject *m_pSoldier[ENTITY_COUNT];

  // sub menus
  VDialogSubCtrl *m_pSubMenu;
  VDialogPtr m_spSubMenuDialog;
  VListControl *m_pLODSelectionList;
  VCheckBox *m_pCheckBoxStopPlay;
};



/*
class SubMenu_Skeletal : public VDialog
{
protected:
  V_DECLARE_SERIAL_DLLEXP( SubMenu_Skeletal, GUI_IMPEXP );
  VOVERRIDE void Serialize( VArchive &ar ) {}
};


class SubMenu_Vertex : public VDialog
{
public:
  VOVERRIDE void OnItemClicked(VMenuEventDataObject *pEvent)
  {
  }

protected:
  V_DECLARE_SERIAL_DLLEXP( SubMenu_Vertex, GUI_IMPEXP );
  VOVERRIDE void Serialize( VArchive &ar ) {}
};
*/

class VDialogSubCtrl : public VDlgControlBase
{
public:

  // specific functions
  inline void SetDialog(VDialog *pDialog) {m_spDialog=pDialog;}
  inline VDialog* GetDialog() const {return m_spDialog;}

  // overridden control functions
  VOVERRIDE void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) {if (m_spDialog) m_spDialog->OnPaint(Graphics,parentState);}
  VOVERRIDE void OnTick(float dtime) {if (m_spDialog) m_spDialog->OnTick(dtime);}
  VOVERRIDE VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse);
  VOVERRIDE VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) {if (m_spDialog) return m_spDialog->GetMouseOverCursor(user);return VDlgControlBase::GetMouseOverCursor(user);}
  VOVERRIDE VTooltip* GetTooltip(VGUIUserInfo_t &user) {if (m_spDialog) return m_spDialog->GetTooltip(user);return VDlgControlBase::GetTooltip(user);}

protected:
  V_DECLARE_SERIAL_DLLEXP( VDialogSubCtrl, DECLSPEC_DLLEXPORT )
  VOVERRIDE void Serialize( VArchive &ar ) {}

  VDialogPtr m_spDialog;
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
