/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTextControl.hpp

#ifndef VTEXTCONTROL_HPP_INCLUDED
#define VTEXTCONTROL_HPP_INCLUDED


/// \brief
///   Represents a text input control
class VTextControl : public VDlgControlBase

#ifdef SUPPORTS_SOFTKEYBOARD
, public IVSoftKeyboardRecipient
#endif

{
public:
  GUI_IMPEXP VTextControl();

  // new functions
  GUI_IMPEXP void SetCursorPos(int iCharacterPos, bool bEnsureVisible=true);

  /// \brief
  ///   Sets the character to display the text as 
  GUI_IMPEXP void SetPasswordChar(int iChar);

  GUI_IMPEXP virtual void OnSetFocus(bool bStatus);
  
#ifdef SUPPORTS_SOFTKEYBOARD
  
  GUI_IMPEXP virtual void EnterText(const char* pUTF8Text);
  
  GUI_IMPEXP virtual bool HasText();

  GUI_IMPEXP virtual const char* GetUTF8Text() const;

  GUI_IMPEXP virtual const char* GetDescription() const;

  GUI_IMPEXP virtual bool IsPassword() const;
  
  GUI_IMPEXP virtual void OnSpecialKey(unsigned int uiKey); 
#endif
  
protected:
  // serialization
  V_DECLARE_SERIAL_DLLEXP( VTextControl, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  // renderable
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnTick(float dtime) HKV_OVERRIDE;

  //dialog overrides
  GUI_IMPEXP virtual void OnKeyPressed(int iKey, int iKeyModifier) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnClick(VMenuEventDataObject *pEvent) HKV_OVERRIDE;
  GUI_IMPEXP virtual const char *GetText() const HKV_OVERRIDE {return m_sCurrentText;}
  GUI_IMPEXP virtual void SetText(const char *szText) HKV_OVERRIDE;
  GUI_IMPEXP void SendTextChangedEvent();


  VDialogFrame m_Frame;
  VString m_sValidChars;
  int m_iMaxChars;

  // text
  VString m_sCurrentText;
  VTextStates m_Text;
  hkvVec2 m_vTextOfs; ///< the offset for the text to be printed
  float m_fTextOfs; ///< horizontal text offset in pixels

  // cursor
  float m_fCursorPhase;
  int m_iCursorPos; ///< character index after which cursor is located

  int m_iPasswordChar;
  VString m_sPasswordString;
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
