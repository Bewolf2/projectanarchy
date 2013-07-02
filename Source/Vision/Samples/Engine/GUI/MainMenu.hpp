/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

///////////////////////////////////////////////////////////////////////////////
// Sub dialog class
///////////////////////////////////////////////////////////////////////////////

class SubDialog : public VDialog
{
public:
  // get the item click event
  VOVERRIDE void OnItemClicked(VMenuEventDataObject *pEvent);

protected:
  V_DECLARE_SERIAL_DLLEXP( SubDialog, DECLSPEC_DLLEXPORT )
  VOVERRIDE void Serialize( VArchive &ar ) {}
};

///////////////////////////////////////////////////////////////////////////////
// Main menu dialog class
///////////////////////////////////////////////////////////////////////////////

class MyMainMenu : public VDialog
{
public:
  MyMainMenu() : VDialog(), m_pExitDialog(NULL) {}

  // overridden function to prompt a dialog before actually closing
  VOVERRIDE void SetDialogResult(int iResult);

  // get the item click event
  VOVERRIDE void OnItemClicked(VMenuEventDataObject *pEvent);

  // override this function to get the ESC key
  VOVERRIDE void OnKeyPressed(int iKey, int iKeyModifier)
  {
    VDialog::OnKeyPressed(iKey,iKeyModifier);
    if (iKey==VGLK_ESC)
      SetDialogResult(VGUIManager::ID_OK); // if we use ID_CANCEL here, it pops up the request dialog, so we use OK
  }

  VOVERRIDE void OnTick(float dtime);

protected:
  V_DECLARE_SERIAL_DLLEXP( MyMainMenu, DECLSPEC_DLLEXPORT )
  VOVERRIDE void Serialize( VArchive &ar ) {}

  VDialog* m_pExitDialog;
};

///////////////////////////////////////////////////////////////////////////////
// List control dialog class
///////////////////////////////////////////////////////////////////////////////

class ListControlDialog : public SubDialog
{
public:
  VOVERRIDE void OnItemClicked(VMenuEventDataObject *pEvent);

protected:
  V_DECLARE_SERIAL_DLLEXP( ListControlDialog, DECLSPEC_DLLEXPORT )
  VOVERRIDE void Serialize( VArchive &ar ) {}
};

///////////////////////////////////////////////////////////////////////////////
// Slider control dialog class
///////////////////////////////////////////////////////////////////////////////

class SliderDialog : public SubDialog
{
public:
  VOVERRIDE void OnValueChanged(VItemValueChangedEvent *pEvent);
  VOVERRIDE void OnInitDialog();

  VOVERRIDE void OnActivate()    {Vision::Video.SaveGamma(Vision::Video.GetCurrentConfig()->m_iAdapter);}
  VOVERRIDE void OnDeactivate()  {Vision::Video.RestoreGamma(Vision::Video.GetCurrentConfig()->m_iAdapter);}

protected:
  V_DECLARE_SERIAL_DLLEXP( SliderDialog, DECLSPEC_DLLEXPORT )
  VOVERRIDE void Serialize( VArchive &ar ) {}

  VSliderControl *m_pImageScrollH;
  VSliderControl *m_pImageScrollV;
  VImageControl *m_pImageCtrl;

  VSliderControl *m_pSliderGamma;
};

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
