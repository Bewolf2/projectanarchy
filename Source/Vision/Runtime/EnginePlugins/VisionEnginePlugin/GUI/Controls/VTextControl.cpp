/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#define CURSOR_FREQ   1.3f
#define CURSOR_WIDTH  2.f


VTextControl::VTextControl()
{
  m_Frame.SetOwner(this);
  m_iMaxChars = 100;
 // m_sValidChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  m_fCursorPhase = 0.f;
  m_iCursorPos = 0;
  m_fTextOfs = 0.f;
  m_iPasswordChar = -1;
}


V_IMPLEMENT_SERIAL( VTextControl, VDlgControlBase, 0, &g_VisionEngineModule );
void VTextControl::Serialize( VArchive &ar )
{
  char iLocalVersion = 1;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=1,"Invalid version");
    ar >> m_Frame;
    ar >> m_sValidChars;
    ar >> m_iMaxChars;
    ar >> m_sCurrentText >> m_Text >> m_vTextOfs >> m_fTextOfs;
    ar >> m_fCursorPhase >> m_iCursorPos;
    if (iLocalVersion>=1)
    {
      int iChar;
      ar >> iChar;
      SetPasswordChar(iChar); // we need the change callback
    }
  } else
  {
    ar << iLocalVersion;
    ar << m_Frame;
    ar << m_sValidChars;
    ar << m_iMaxChars;
    ar << m_sCurrentText << m_Text << m_vTextOfs << m_fTextOfs;
    ar << m_fCursorPhase << m_iCursorPos;
    ar << m_iPasswordChar;
  }
}



bool VTextControl::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  // frame
  m_Frame.Build(XMLHelper::SubNode(pNode,"frame",bWrite),szPath,bWrite);

  // text control properties
  const char *szUTF8 = XMLHelper::Exchange_String(pNode,"validchars", NULL, bWrite);
  m_sValidChars = szUTF8;

  XMLHelper::Exchange_Int(pNode,"maxchars",m_iMaxChars,bWrite);
  XMLHelper::Exchange_Int(pNode,"passwordchar",m_iPasswordChar,bWrite);

  m_vTextOfs = GetClientAreaBorder().m_vMin;

  // text
  TiXmlElement *pTextNode = XMLHelper::SubNode(pNode,"text",bWrite);
  if (pTextNode)
  {
    m_Text.Build(this,pTextNode,szPath,bWrite);
    SetText(m_Text.GetText()); // sets the cursor pos
    SetPasswordChar(m_iPasswordChar);
   
    m_vTextOfs += m_Text.GetTextOfs();
  }

  return true;
}


GUI_IMPEXP void VTextControl::SetPasswordChar(int iChar)
{
  bool bChanged = m_iPasswordChar != iChar;
  m_iPasswordChar = iChar;
  m_Text.SetTextPtr(iChar>=0 ? &m_sPasswordString : &m_sCurrentText);
  if (bChanged)
    SendTextChangedEvent();
}



void VTextControl::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VDlgControlBase::OnPaint(Graphics,parentState);

  VItemRenderInfo thisState(parentState,this,1.f);

  // render frame
  m_Frame.OnPaint(Graphics,thisState);

  // render text
  m_Text.SetTextOfs(m_vTextOfs+hkvVec2(-m_fTextOfs,0.f));
  // set clipping rectangle
  Graphics.ClippingStack.Push(GetClientRect(),true);
  m_Text.OnPaint(Graphics,thisState);

  // render blinking cursor
  if (m_fCursorPhase<(CURSOR_FREQ*0.5f) && HasFocus())
  {
    const VString &sText(m_iPasswordChar<0 ? m_sCurrentText : m_sPasswordString);

    // find out the cursor position
    VRectanglef rect;
    if (!m_Text.GetFont()->GetTextDimension(sText, rect, m_iCursorPos))
      rect.m_vMax.set(0.f,0.f);
    
    hkvVec2 p1 = GetAbsPosition() + m_Text.GetTextOfs();
    p1.x += rect.m_vMax.x;
    hkvVec2 p2 = p1 + hkvVec2(CURSOR_WIDTH,m_Text.GetFont()->GetFontHeight());
    VSimpleRenderState_t cursorState = VGUIManager::DefaultGUIRenderState();
    Graphics.Renderer.DrawSolidQuad(p1,p2,V_RGBA_WHITE,cursorState);
  }
  Graphics.ClippingStack.Pop();
}



void VTextControl::OnTick(float dtime)
{
  if (!HasFocus())
    return;

  m_fCursorPhase = hkvMath::mod (m_fCursorPhase+dtime,CURSOR_FREQ);
}



void VTextControl::OnKeyPressed(int iKey, int iKeyModifier)
{
#if defined(VISION_GUI_USE_WINDOWS_INPUT)
  int iCurrentLength = m_sCurrentText.GetLen();

  switch(iKey)
  {
    case VK_HOME:
      SetCursorPos(0);
      break;

    case VK_END:
      SetCursorPos(iCurrentLength);
      break;

    case VK_DELETE:
      if (m_iCursorPos<iCurrentLength)
      {
        m_sCurrentText.RemoveAt(m_iCursorPos);
        SendTextChangedEvent();
        SetCursorPos(m_iCursorPos);
      }
      break;

    case VK_BACK:
      if (m_iCursorPos>0)
      {
        m_sCurrentText.RemoveAt(m_iCursorPos-1);
        SendTextChangedEvent();
        SetCursorPos(m_iCursorPos-1);
      }
      break;

    case VK_LEFT:
      SetCursorPos(m_iCursorPos-1);
      break;

    case VK_RIGHT:
      SetCursorPos(m_iCursorPos+1);
      break;

    // A character was entered
    default:
      m_sCurrentText.InsertAt(m_iCursorPos, iKey);
      SendTextChangedEvent();
      SetCursorPos(m_iCursorPos + 1);

      break;
  }

#else

#if defined(SUPPORTS_KEYBOARD)
  int iCurrentLength = m_sCurrentText.GetLen();

  wchar_t iKeyChar = 0;
  bool bCapital = (iKeyModifier & KEYMODIFIER_SHIFT) > 0;

  // use this layout specific table for mapping
  const VGLKey_t *pKeyTable = VGLGetKeyTable();

  // handle some special keys:
  switch (iKey)
  {
    // back space
    case VGLK_BACKSP: 
      if (m_iCursorPos > 0)
      {
        m_sCurrentText.RemoveAt(m_iCursorPos-1);
        SendTextChangedEvent();
        SetCursorPos(m_iCursorPos-1);
      }
      return;
    // delete
    case VGLK_DEL:
    case VGLK_KP_DEL: 
      if (m_iCursorPos < iCurrentLength)
      {
        m_sCurrentText.RemoveAt(m_iCursorPos);
        SendTextChangedEvent();
        SetCursorPos(m_iCursorPos);
      }
      return;
    // cursor keys
    case VGLK_LEFT:
    case VGLK_KP_LEFT: 
      SetCursorPos(m_iCursorPos-1);
      return;
    case VGLK_RIGHT:
    case VGLK_KP_RIGHT: 
      SetCursorPos(m_iCursorPos+1);
      return;
    // home
    case VGLK_HOME:
    case VGLK_KP_HOME:
      SetCursorPos(0);
      return;
    // end
    case VGLK_END:
    case VGLK_KP_END:
      SetCursorPos(iCurrentLength);
      return;

    // for every other (valid) character, map to ASCII
    default:
      if (iKey > 0) // convert to wchar_t to get valid unicode characters (umlauts are not valid ANSI)
        iKeyChar = static_cast<wchar_t>(bCapital ? pKeyTable[iKey].m_chUpper : pKeyTable[iKey].m_chLower);
  }
  if(!iKeyChar)
    return;

  // convert character to UTF8
  char utf8[4];
  int iUTF8CharSize = VString::ConvertWCharToUTF8(iKeyChar, utf8);
  VASSERT(iUTF8CharSize >= 1);
  utf8[iUTF8CharSize] = '\0';

  // insert a character
  if (!m_sValidChars.IsEmpty() && m_sValidChars.Find(utf8, true) < 0)
    return;
  if (m_iMaxChars > 0 && iCurrentLength >= m_iMaxChars)
    return;

  m_sCurrentText.InsertAt(m_iCursorPos, utf8);
  SendTextChangedEvent();
  SetCursorPos(m_iCursorPos+1);

#endif // SUPPORTS_KEYBOARD
#endif // VISION_GUI_USE_WINDOWS_INPUT
}


void VTextControl::SetText(const char *szText)
{
  if (m_sCurrentText!=szText)
  {
    m_sCurrentText=szText;
    SetCursorPos(m_sCurrentText.GetLen(), false); // set cursor to the end
    SendTextChangedEvent();
  }
}


void VTextControl::SendTextChangedEvent()
{
  VItemValueChangedEvent data(this, VItemValueChangedEvent::VALUE_STRING, 
    (void *)m_sCurrentText.GetSafeStr(), false);
  SendValueChangedEvent(&data);

  if (m_iPasswordChar>=0)
  {
    // convert password character to UTF8
    const wchar_t wcPasswordChar = static_cast<wchar_t>(m_iPasswordChar);

    int iLen = m_sCurrentText.GetLen();
    m_sPasswordString = m_sCurrentText; // allocate correct length
    for (int i=0; i<iLen; i++)
      m_sPasswordString[i] = wcPasswordChar;
  }
}


void VTextControl::SetCursorPos(int iCharacterPos, bool bEnsureVisible)
{
  // int iOldPos = m_iCursorPos;
  // clamp to valid range
  const VString &sText(m_iPasswordChar < 0 ? m_sCurrentText : m_sPasswordString);

  if (iCharacterPos < 0) iCharacterPos = 0; 
  else if (iCharacterPos > sText.GetLen())
    iCharacterPos = sText.GetLen();

  m_iCursorPos = iCharacterPos;
  //if (iOldPos!=m_iCursorPos) 
    m_fCursorPhase = 0.f; // reset the cursor phase

  if (bEnsureVisible)
  {
    hkvVec2 vSize = GetClientRect().GetSize();
    float fCursorPos = -m_fTextOfs;
    VRectanglef textRect;
    if (m_Text.GetFont()->GetTextDimension(sText,textRect,m_iCursorPos))
      fCursorPos += textRect.m_vMax.x;

    if (fCursorPos <= 0.f || fCursorPos >= vSize.x) // recenter the cursor if cursor pos is out of range
    {
      float fDiff = fCursorPos - vSize.x*0.5f;
      m_fTextOfs += fDiff;
      if (m_fTextOfs < 0.f)
        m_fTextOfs=0.f;
    }
  }
}


void VTextControl::OnClick(VMenuEventDataObject *pEvent)
{
  if (pEvent->m_iButtons != BUTTON_LMOUSE)
  {
    // no left button, so nothing to be done here
    return;
  }

  VDlgControlBase::OnClick(pEvent);

  const VString &sText(m_iPasswordChar < 0 ? m_sCurrentText : m_sPasswordString);

  // relative position inside text
  hkvVec2 vRelPos = pEvent->m_vMousePos - GetAbsPosition() - m_Text.GetTextOfs();

  // find character at mouse click position
  SetCursorPos(m_Text.GetFont()->GetCharacterIndexAtPos(sText, vRelPos.x), true);
}

void VTextControl::OnSetFocus(bool bStatus)
{
  VDlgControlBase::OnSetFocus(bStatus);
  
#ifdef SUPPORTS_SOFTKEYBOARD
  
  if(bStatus)
  {
    VInputManager::GetSoftkeyboardAdapter().SetCurrentRecipient(this);
    VInputManager::GetSoftkeyboardAdapter().Show();
  }
  else
  {
    VInputManager::GetSoftkeyboardAdapter().Hide();
    VInputManager::GetSoftkeyboardAdapter().SetCurrentRecipient(NULL);
  }
  
#endif
}

#ifdef SUPPORTS_SOFTKEYBOARD

void VTextControl::EnterText(const char* pUTF8Text)
{
  if(pUTF8Text == NULL)
    return;
  
  // insert a character
  if (!m_sValidChars.IsEmpty() && m_sValidChars.Find(pUTF8Text, true) < 0)
    return;
  if (m_iMaxChars > 0 && m_sCurrentText.GetLen() >= m_iMaxChars)
    return;
  
  m_sCurrentText.InsertAt(m_iCursorPos, pUTF8Text);
  SendTextChangedEvent();
  SetCursorPos(m_iCursorPos+1);  
}

bool VTextControl::HasText()
{
  return m_sCurrentText.GetSize() > 0;
}

const char* VTextControl::GetUTF8Text() const
{
  return m_sCurrentText.GetSafeStr();
}

const char* VTextControl::GetDescription() const
{
  if (m_sTooltipText.IsEmpty())
    return "";

  return m_sTooltipText.GetSafeStr();
}

bool VTextControl::IsPassword() const
{
  return (m_iPasswordChar >= 0);
}

void VTextControl::OnSpecialKey(unsigned int uiKey)
{
  int iCurrentLength = m_sCurrentText.GetLen();
  
  switch(uiKey)
  {
      // back space
    case VGLK_BACKSP: 
      if (m_iCursorPos > 0)
      {
        m_sCurrentText.RemoveAt(m_iCursorPos-1);
        SendTextChangedEvent();
        SetCursorPos(m_iCursorPos-1);
      }
      return;
      // delete
    case VGLK_DEL:
      if (m_iCursorPos < iCurrentLength)
      {
        m_sCurrentText.RemoveAt(m_iCursorPos);
        SendTextChangedEvent();
        SetCursorPos(m_iCursorPos);
      }
      return;
      // cursor keys
    case VGLK_LEFT:
      SetCursorPos(m_iCursorPos-1);
      return;
    case VGLK_RIGHT:
      SetCursorPos(m_iCursorPos+1);
      return;
      // home
    case VGLK_HOME:
      SetCursorPos(0);
      return;
      // end
    case VGLK_END:
      SetCursorPos(iCurrentLength);
      return;      
  }
}

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
