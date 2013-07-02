/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDialogStandardControls.hpp

#ifndef VDIALOGSTANDARDCONTROLS_HPP_INCLUDED
#define VDIALOGSTANDARDCONTROLS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDlgControlBase.hpp>

class VDialogTitleBar;
typedef VSmartPtr<VDialogTitleBar> VDialogTitleBarPtr;


/// \brief
///   Simple Control for showing a text label
class VTextLabel : public VDlgControlBase
{
public:
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;

  // item overrides
  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) HKV_OVERRIDE {return m_TextCfg.m_States[GetCurrentState()].GetCursor();}
  GUI_IMPEXP virtual void SetText(const char *szString) HKV_OVERRIDE {m_TextCfg.SetText(szString);}
  GUI_IMPEXP virtual const char *GetText() const HKV_OVERRIDE {return m_TextCfg.GetText();}

  /// \brief Returns the VTextStates object for this label
  GUI_IMPEXP VTextStates& Text() {return m_TextCfg;}

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VTextLabel, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

protected:
  VTextStates m_TextCfg; ///< text
};


/// \brief
///   Simple Control for showing an image texture
class VImageControl : public VDlgControlBase
{
public:
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnBuildFinished() HKV_OVERRIDE;
  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) HKV_OVERRIDE {return m_Image.m_States[GetCurrentState()].GetCursor();}

  /// \brief Returns the VImageStates object for this image.
  GUI_IMPEXP VImageStates& Image() {return m_Image;}

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VImageControl, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

  VImageStates m_Image;  ///< image
};


/// \brief
///   Simple push button class with image 4 states. Optionally with text on image.
class VPushButton : public VDlgControlBase
{
public:
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;

  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) HKV_OVERRIDE
  {
    // use cursor from text
    VCursor *pCursor=m_TextCfg.m_States[GetCurrentState()].GetCursor();
    if (pCursor) return pCursor;
    //..or from image
    return m_ButtonCfg.m_States[GetCurrentState()].GetCursor();
  }

  GUI_IMPEXP virtual void SetText(const char *szString) HKV_OVERRIDE {m_TextCfg.SetText(szString);}
  GUI_IMPEXP virtual const char *GetText() const HKV_OVERRIDE {return m_TextCfg.GetText();}

  /// \brief Returns the VImageStates object for the button
  GUI_IMPEXP VImageStates& Image() {return m_ButtonCfg;}

  /// \brief Returns the VTextStates object for the button
  GUI_IMPEXP VTextStates& Text() {return m_TextCfg;}

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VPushButton, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

protected:
  VImageStates m_ButtonCfg;  ///< 4 states
  VTextStates m_TextCfg;     ///< optional text
};


/// \brief
///   Uses the Push button for checkbox (with the text next to the checkbox)
/// 
/// The selected state is used for rendering the checked status of the check box.
class VCheckBox : public VPushButton
{
public:
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;

  /// \brief Checked status : actually same as selected.
  inline bool IsChecked() const {return IsStatusSet(ITEMSTATUS_STAYSELECTED);}

  /// \brief Sets the checkbox to checked or unchecked.
  GUI_IMPEXP void SetChecked(bool bStatus);

  // toggle check status
  GUI_IMPEXP virtual void OnClick(VMenuEventDataObject *pEvent) HKV_OVERRIDE
  {
    VPushButton::OnClick(pEvent);
    SetChecked(!IsChecked());
  }

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VCheckBox, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );
};


/// \brief
///   Class for handling a dialog title bar (including dragging). Gathers position from owner
///   dialog.
class VDialogTitleBar : public VDlgControlBase
{
public:
  VDialogTitleBar();
  virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;

  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnParentChanged(int iFlags) HKV_OVERRIDE;

  virtual void OnDragBegin(const hkvVec2 &vMousePos, int iButtonMask) HKV_OVERRIDE;
  virtual void OnDragging(const hkvVec2 &vMouseDelta) HKV_OVERRIDE;
  virtual void OnDragEnd(VWindowBase *pOver) HKV_OVERRIDE;

  virtual void SetText(const char *szString) HKV_OVERRIDE {m_TextCfg.SetText(szString);}
  virtual const char *GetText() const HKV_OVERRIDE
  {
    return m_TextCfg.GetText();
  }

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VDialogTitleBar, GUI_IMPEXP_DATA );
  virtual void Serialize( VArchive &ar );

private:
  void AdjustBar();
  VTextStates m_TextCfg;  ///< title bar text

  // temp data:
  bool m_bDragParent;
};


/// \brief
///   Invisible item in the dialog's corner that can resize a dialog. Gathers position from owner
///   dialog.
class VDialogResizeCtrl : public VDlgControlBase
{
public:
  VDialogResizeCtrl();

  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnParentChanged(int iFlags) HKV_OVERRIDE;

  virtual void OnDragBegin(const hkvVec2 &vMousePos, int iButtonMask) HKV_OVERRIDE;
  virtual void OnDragging(const hkvVec2 &vMouseDelta) HKV_OVERRIDE;
  virtual void OnDragEnd(VWindowBase *pOver) HKV_OVERRIDE;

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VDialogResizeCtrl, GUI_IMPEXP_DATA );
  virtual void Serialize( VArchive &ar );

private:
  void AdjustPosition();

  // temp values:
  hkvVec2 m_vDragStart, m_vDragPos, m_vStartSize;
};


/// \brief
///   PushButton that sets the dialog result to "CANCEL" if pressed. Gathers position from owner
///   dialog.
class VDialogCloseButton : public VPushButton
{
public:
  VDialogCloseButton();
  virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;

  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnParentChanged(int iFlags) HKV_OVERRIDE;

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VDialogCloseButton, GUI_IMPEXP_DATA );
  virtual void Serialize( VArchive &ar );

private:
  hkvVec2 m_vOfs;
  void AdjustPosition();
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
