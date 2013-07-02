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

  
VWindowBase::VWindowBase()
{
  m_pOwner = NULL;
  m_vSize.set(-1.f,-1.f); // undefined
  m_iStatus = ITEMSTATUS_VISIBLE|ITEMSTATUS_ENABLED;
  m_iOrder = 0;
  m_iTabOrder = -1;
  m_pContext = NULL;
  m_iID = VGUIManager::ID_STATIC;
  m_iDialogResult = 0;
  m_ClientAreaBorder.Set(0,0,0,0);
}

VWindowBase::~VWindowBase()
{
}


VGUIManager *VWindowBase::GetMenuManager() const 
{
  VASSERT(m_pContext);
  return m_pContext->GetManager();
}



#define CHECK_TYPESTR(t) {if (!_stricmp(szState,#t)) return t;}
VWindowBase::ControlState_e VWindowBase::GetState(const char *szState)
{
  if (!szState || !szState[0]) return NORMAL;
  CHECK_TYPESTR(DISABLED);
  CHECK_TYPESTR(MOUSEOVER);
  CHECK_TYPESTR(SELECTED);
  return NORMAL;
}


V_IMPLEMENT_SERIAL( VWindowBase, VisTypedEngineObject_cl, 0, &g_VisionEngineModule );
void VWindowBase::Serialize( VArchive &ar )
{
  VisTypedEngineObject_cl::Serialize(ar);
  char iLocalVersion = 0;
  const char *szScriptFile = NULL;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_pOwner;
    VMemoryTempBuffer<1024> buffer;
    m_iID = VGUIManager::GetID(buffer.ReadStringBinary(ar));
    ar >> m_iDialogResult >> m_iOrder >> m_iTabOrder >> m_iStatus;
    ar >> m_vPosition >> m_vSize >> m_ClientAreaBorder;
    ar >> m_sTooltipText;
    szScriptFile = buffer.ReadStringBinary(ar);
  } else
  {
    ar << iLocalVersion;
    ar << m_pOwner;
    ar << VGUIManager::GetIDName(m_iID);
    ar << m_iDialogResult << m_iOrder << m_iTabOrder << m_iStatus;
    ar << m_vPosition << m_vSize << m_ClientAreaBorder;
    ar << m_sTooltipText;
    ar << szScriptFile; // TODO 
  }
}



void VWindowBase::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) 
{
}


bool VWindowBase::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!IVRenderableItem::Build(pNode,szPath,bWrite))
    return false;

  // register the ID
  const char *szID = XMLHelper::Exchange_String(pNode,"ID",NULL,bWrite);
  if (szID) m_iID = VGUIManager::GetID(szID);

  // order
  XMLHelper::Exchange_Int(pNode,"order",m_iOrder,bWrite);
  XMLHelper::Exchange_Int(pNode,"taborder",m_iTabOrder,bWrite);

  // size and position
  XMLHelper::Exchange_Floats(pNode,"pos",m_vPosition.data,2,bWrite);
  XMLHelper::Exchange_Floats(pNode,"size",m_vSize.data,2,bWrite);

  // client border
  float b[4];
  int iCount = XMLHelper::Exchange_Floats(pNode,"clientborder",b,4,bWrite);
  switch (iCount)
  {
    case 0:m_ClientAreaBorder.Set(0,0,0,0);break;
    case 1:m_ClientAreaBorder.Set(b[0],b[0],b[0],b[0]);break;
    case 2:m_ClientAreaBorder.Set(b[0],b[1],b[0],b[1]);break;
    case 3:m_ClientAreaBorder.Set(b[0],b[1],b[2],b[1]);break;
    case 4:m_ClientAreaBorder.Set(b[0],b[1],b[2],b[3]);break;
  }

  // flags
  bool bEnabled = true, bVisible = true, bSelected = false;
  bool bCaching = m_spCachedBuffer!=NULL;

  XMLHelper::Exchange_Bool(pNode,"enabled", bEnabled, bWrite);    SetStatus(ITEMSTATUS_ENABLED,bEnabled);
  XMLHelper::Exchange_Bool(pNode,"visible", bVisible, bWrite);    SetStatus(ITEMSTATUS_VISIBLE,bVisible);
  XMLHelper::Exchange_Bool(pNode,"selected", bSelected, bWrite);  SetStatus(ITEMSTATUS_STAYSELECTED,bSelected);
  XMLHelper::Exchange_Bool(pNode,"usecaching", bCaching, bWrite);
  // tooltip
  const char *szUTF8 = XMLHelper::Exchange_String(pNode,"tooltiptext", NULL, bWrite);
  const char *szText = GetMenuManager()->TranslateString(this, szUTF8);
  SetTooltipText(szText);
  
  // scripting
  IVScriptManager *pScriptMan = Vision::GetScriptManager();
  TiXmlElement *pScriptNode = XMLHelper::SubNode(pNode,"script",bWrite);
  if (pScriptNode && pScriptMan)
  {
    char szPathname[FS_MAX_PATH];
    const char *szFilename = XMLHelper::Exchange_String(pScriptNode,"filename", NULL, bWrite);
    if (szFilename != NULL && szFilename[0])
    {
      VFileHelper::CombineDirAndFile(szPathname,szPath,szFilename);
      IVScriptInstance *pScriptObj = pScriptMan->CreateScriptInstanceFromFile(szPathname);
      if (pScriptObj)
        pScriptMan->SetScriptInstance(this, pScriptObj);
    }
  }

  SetUseCaching(bCaching);

  return true;
}


void VWindowBase::SetPosition(float x, float y) 
{
   if (x==m_vPosition.x && y==m_vPosition.y && (m_iStatus&ITEMSTATUS_VALIDPOSITION))
     return;
   m_iStatus |= ITEMSTATUS_VALIDPOSITION;
   m_vPosition.set(x,y);
   OnPositionChanged();
}

void VWindowBase::SetSize(float dx, float dy) 
{
  if (dx==m_vSize.x && dy==m_vSize.y && (m_iStatus&ITEMSTATUS_VALIDSIZE))
    return;
   m_iStatus |= ITEMSTATUS_VALIDSIZE;
  m_vSize.set(dx,dy);
  OnSizeChanged();
}


void VWindowBase::SetStatus(int iMask, bool bSet) 
{
  int iNewStatus = m_iStatus;
  if (bSet) iNewStatus |= iMask; else iNewStatus &= (~iMask);
  if (m_iStatus!=iNewStatus)
  {
    int iOldStatus = m_iStatus;
    m_iStatus = iNewStatus;
    OnStatusFlagsChanged(iOldStatus,iNewStatus);
    InvalidateCache();
  }
}


void VWindowBase::RemoveStatus(int iMask) 
{
  if ((m_iStatus&iMask)==0)
    return;
  m_iStatus &= (~iMask); 
  InvalidateCache();
}


VWindowBase::ControlState_e VWindowBase::GetCurrentState(bool bForceDisabled) const
{
  if (!IsEnabled() || bForceDisabled) return DISABLED;
  if (IsSelected()) return SELECTED;
  if (IsMouseOver()) return MOUSEOVER;
  return NORMAL;
}


hkvVec2 VWindowBase::GetAbsPosition() const
{
  hkvVec2 v=m_vPosition;
  if (m_pOwner) 
    v+=m_pOwner->GetAbsPosition();
  return v;
}


VRectanglef VWindowBase::GetBoundingBox() const 
{
  hkvVec2 v = GetAbsPosition();
  return VRectanglef(v,v+m_vSize);
}


VRectanglef VWindowBase::GetClientRect() const 
{
  VRectanglef rect = GetBoundingBox();
  // make rectangle smaller by its border
  rect.m_vMin += m_ClientAreaBorder.m_vMin;
  rect.m_vMax -= m_ClientAreaBorder.m_vMax;
  return rect;
}


VWindowBase* VWindowBase::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  if (GetBoundingBox().IsInside(vAbsMouse))
    return this;
  return NULL;
}


void VWindowBase::SendValueChangedEvent(VItemValueChangedEvent *pEvent)
{
  OnValueChanged(pEvent);
  if (m_pOwner)
    m_pOwner->SendValueChangedEvent(pEvent);
  else if (GetContext())
    GetContext()->OnItemValueChanged.TriggerCallbacks(pEvent);
}


VTooltip* VWindowBase::GetTooltip(VGUIUserInfo_t &user) 
{
  if (m_sTooltipText.IsEmpty())
    return NULL;
  // create a default instance
  VTooltip *pTooltip = GetContext()->CreateTooltip(m_sTooltipText);
  return pTooltip;
}


void VWindowBase::InvalidateCache()
{
  if (m_spCachedBuffer)
    m_spCachedBuffer->Invalidate();
  if (m_pOwner)
    m_pOwner->InvalidateCache();
}


void VWindowBase::SetUseCaching(bool bState)
{
  if (bState)
  {
    if (!m_spCachedBuffer)
      m_spCachedBuffer = new VGUICommandBuffer();
  } 
  else
  {
    m_spCachedBuffer = NULL;
  }
}


void VWindowBase::OnClick(VMenuEventDataObject *pEvent)
{
  int iButtons = ((pEvent->m_iButtons&1)<<1)|((pEvent->m_iButtons&2)>>1); //exchange bit 0 and 1
  TriggerScriptEvent("OnClick", "*oiff", pEvent->m_pItem, iButtons, pEvent->m_vMousePos.x, pEvent->m_vMousePos.y);
}

void VWindowBase::OnDoubleClick(VMenuEventDataObject *pEvent)
{
  int iButtons = ((pEvent->m_iButtons&1)<<1)|((pEvent->m_iButtons&2)>>1); //exchange bit 0 and 1
  TriggerScriptEvent("OnDoubleClick", "*oiff", pEvent->m_pItem, iButtons, pEvent->m_vMousePos.x, pEvent->m_vMousePos.y);
}

void VWindowBase::OnPointerDown(VMenuEventDataObject *pEvent)
{
  int iButtons = ((pEvent->m_iButtons&1)<<1)|((pEvent->m_iButtons&2)>>1); //exchange bit 0 and 1
  TriggerScriptEvent("OnPointerDown", "*oiff", pEvent->m_pItem, iButtons, pEvent->m_vMousePos.x, pEvent->m_vMousePos.y);
}

void VWindowBase::OnPointerUp(VMenuEventDataObject *pEvent)
{
  int iButtons = ((pEvent->m_iButtons&1)<<1)|((pEvent->m_iButtons&2)>>1); //exchange bit 0 and 1
  TriggerScriptEvent("OnPointerUp", "*oiff", pEvent->m_pItem, iButtons, pEvent->m_vMousePos.x, pEvent->m_vMousePos.y);
}

void VWindowBase::OnMouseEnter(VGUIUserInfo_t &user)
{
  SetStatus(ITEMSTATUS_MOUSEOVER_USER0<<user.m_iID); 
  TriggerScriptEvent("OnMouseEnter");
  //Vision::Message.Add("Enter : %i",user.m_iID);
}

void VWindowBase::OnMouseLeave(VGUIUserInfo_t &user) 
{
  RemoveStatus((ITEMSTATUS_MOUSEOVER_USER0<<user.m_iID)|ITEMSTATUS_SELECTED); 
  TriggerScriptEvent("OnMouseLeave");
  //Vision::Message.Add("Leave : %i",user.m_iID);
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
