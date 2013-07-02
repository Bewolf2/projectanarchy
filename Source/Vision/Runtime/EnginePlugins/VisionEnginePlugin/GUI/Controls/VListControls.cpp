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


////////////////////////////////////////////////////////////////////////////////////////////
// VListControlItem
////////////////////////////////////////////////////////////////////////////////////////////

VListControlItem::VListControlItem() 
{
  m_bCachedTextRectValid = false;
  CommonInit();
}

VListControlItem::~VListControlItem()
{
}

VListControlItem::VListControlItem(const char *szText)
{
  CommonInit();
  m_Text.SetText(szText);
}


void VListControlItem::CommonInit()
{
  m_iIndex = -1;
  m_bClipped = true;
  m_bFullyVisible = false;
  m_iData=0;

  //m_Text.SetCustomClipRectPtr(&m_BoundingBox); // use this box for additional clamping
  m_pDefaultStates = NULL;
}



bool VListControlItem::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VWindowBase::Build(pNode,szPath,bWrite))
    return false;

  XMLHelper::Exchange_Int(pNode,"data", m_iData,bWrite);
  m_Text.Build(m_pOwner,pNode,szPath,bWrite,m_pDefaultStates);

  TiXmlElement *pIconNode = XMLHelper::SubNode(pNode,"icon",bWrite);
  m_Icon.Build(m_pOwner,pIconNode,szPath,bWrite);

  return true;
}


V_IMPLEMENT_SERIAL( VListControlItem, VWindowBase, 0, &g_VisionEngineModule );
void VListControlItem::Serialize( VArchive &ar )
{
  char iLocalVersion = 1;
  VWindowBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=1,"Invalid version");
    ar >> m_iData;
    ar >> m_Text;
    if (iLocalVersion>=1)
      ar >> m_Icon;
  } else
  {
    ar << iLocalVersion;
    ar << m_iData;
    ar << m_Text;
    ar << m_Icon;
  }
}


void VListControlItem::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  if (IsClipped()) return;
  VItemRenderInfo thisState(parentState,this,1.f);
  // paint the list item's  text
  m_Text.OnPaint(Graphics,thisState);

  VListControl *pListCtrl = (VListControl *)GetParent();
  const float fIconSize = pListCtrl->m_fIconSize;

  if (fIconSize>0.f)
  {
    VImageState &state(m_Icon.m_States[GetCurrentState(thisState.bForceDisabled)]);
    if (!state.IsDefined())
      state = m_Icon.m_States[VWindowBase::NORMAL];

    hkvVec2 vPos1 = GetAbsPosition() + pListCtrl->m_vIconOfs;
    hkvVec2 vPos2 = vPos1 + hkvVec2(fIconSize,fIconSize);
    VSimpleRenderState_t iProperties = VGUIManager::DefaultGUIRenderState(state.GetTransparency());
    VColorRef iColor = parentState.iFadeColor*state.GetColor();
    const VRectanglef& texRange(state.GetTextureRange());

    Graphics.Renderer.DrawTexturedQuad(vPos1,vPos2, state.GetCurrentTexture(), texRange.m_vMin, texRange.m_vMax, iColor, iProperties);
  }
}


void VListControlItem::OnMeasureItem(const VRectanglef &area, hkvVec2 &vCurrentPos)
{
  // Update the item's position. Make sure the item is positioned on pixel boundaries,
  // or the text will be sampled incorrectly.
  m_vPosition.x = floor(vCurrentPos.x +0.5f);
  m_vPosition.y = floor(vCurrentPos.y +0.5f);

  VListControl *pListCtrl = (VListControl *)GetParent();

  // Center text vertically next to the icon
  const float fIconSize = pListCtrl->GetIconSize();
  if(fIconSize > 0.0f)
  {
    const float fFontHeight = m_Text.GetFont() != NULL ? m_Text.GetFont()->GetFontHeight() : 0.0f;
    hkvVec2 vOffset = hkvVec2(fIconSize + 4.0f, (fIconSize * 0.5f) - (fFontHeight * 0.5f));
    vOffset += pListCtrl->GetIconRelOfs();

    vOffset.x = floor(vOffset.x +0.5f);
    vOffset.y = floor(vOffset.y +0.5f);

    m_Text.SetTextOfs(vOffset);
  }

  if (!m_bCachedTextRectValid)
  {
    VTextState &state(m_Text.m_States[GetCurrentState()]);

    if (state.GetTextWrap()) // automatic wrap
    {
      state.Paint(NULL,GetParent(),V_RGBA_WHITE); // refresh height
      m_vSize.y = state.GetHeight();
    }
    else
    {
      VRectanglef textRect(false);
      state.GetFont()->GetTextDimension(GetText(),textRect);
      m_vSize.y = textRect.GetSizeY();
    }
    m_bCachedTextRectValid = true;
  }
  m_vSize.x = area.GetSizeX(); // full width for correct centering

  m_vSize.y = hkvMath::Max(pListCtrl->m_fIconSize+4.f, m_vSize.y);

  vCurrentPos.y += m_vSize.y;

  m_bClipped = (m_vPosition.y > area.m_vMax.y) || (m_vPosition.y+m_vSize.y<area.m_vMin.y);
  m_bFullyVisible = (m_vPosition.y >= area.m_vMin.y) && (m_vPosition.y+m_vSize.y<=area.m_vMax.y);
}


////////////////////////////////////////////////////////////////////////////////////////////
// VListControlItemCollection
////////////////////////////////////////////////////////////////////////////////////////////


void VListControlItemCollection::Add(VListControlItem *pItem, int iInsertBefore, bool bSetDefaultStates)
{
  pItem->SetParent(m_pOwner);
  if (bSetDefaultStates)
  {
    pItem->m_pDefaultStates = &m_DefaultProperties;
    pItem->m_Text = m_DefaultProperties;
    pItem->m_Text.FinishSetup();
  }
  VRefCountedCollection<VListControlItem>::Insert(pItem,iInsertBefore);
}


bool VListControlItemCollection::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!pNode)
    return false;

  // store the item default properties in the m_DefaultProperties member:
  m_DefaultProperties.Build(m_pOwner,XMLHelper::SubNode(pNode,"defaults",bWrite),szPath,bWrite,NULL);

  // iterate through all "listitem" elements
  for (TiXmlElement *pItemNode=pNode->FirstChildElement("listitem"); pItemNode; pItemNode=pItemNode->NextSiblingElement("listitem") )
  {
    VListControlItem *pItem = new VListControlItem();
    pItem->SetParent(m_pOwner);
    pItem->m_pDefaultStates = &m_DefaultProperties;
    pItem->Build(pItemNode,szPath,bWrite);
    pItem->OnBuildFinished();
    Add(pItem,-1,false);
  }

  return true;
}

VListControlItem *VListControlItemCollection::FindItemByDataValue(int iValue) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->GetData()==iValue)
      return GetAt(i);
  return NULL;
}

VListControlItem *VListControlItemCollection::FindItemByUserData(const void *pData) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->GetUserData()==pData)
      return GetAt(i);
  return NULL;
}


void VListControlItemCollection::MeasureItems(const VRectanglef &area, hkvVec2 &vStartPos)
{
  for (int i=0;i<Count();i++)
  {
    GetAt(i)->m_iIndex = i;
    GetAt(i)->OnMeasureItem(area,vStartPos);
  }
}

hkvVec2 VListControlItemCollection::GetSize() const
{
  hkvVec2 vSize;
  for (int i=0;i<Count();i++)
  {
    hkvVec2 vItemSize = GetAt(i)->GetSize();
    vSize.x = hkvMath::Max(vSize.x,vItemSize.x);
    VASSERT(vItemSize.y>0.f); // not initialized!!
    vSize.y += vItemSize.y;
  }
  return vSize;
}

  
void VListControlItemCollection::PaintAll(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  for (int i=0;i<Count();i++)
    GetAt(i)->OnPaint(Graphics,parentState);
}

  
VListControlItem *VListControlItemCollection::GetItemAt(VGUIUserInfo_t &user, const hkvVec2 &vAbsPos) const
{
  for (int i=0;i<Count();i++)
  {
    VListControlItem *pItem = GetAt(i);
    if (pItem->IsVisible() && !pItem->IsClipped() && pItem->TestMouseOver(user, vAbsPos))
      return pItem;
  }
  return NULL;
}


void VListControlItemCollection::SerializeX( VArchive &ar )
{
  VRefCountedCollection<VListControlItem>::SerializeX(ar);
 
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_pOwner;
    ar >> m_DefaultProperties;
  } else
  {
    ar << iLocalVersion;
    ar << m_pOwner;
    ar << m_DefaultProperties;
  } 
}



////////////////////////////////////////////////////////////////////////////////////////////
// VListControl
////////////////////////////////////////////////////////////////////////////////////////////


VListControl::VListControl()
{
  m_fCurrentScrollPos = 0.f;
  m_iBackgroundCol = VColorRef(0,0,0,255);
  m_bCollectionChanged = true;
  m_bAllowSelection = true;
  m_Items.m_pOwner = this;
  m_bFromScrollbar = true;
  m_fIconSize = 0.0f;
  m_vIconOfs.set(0.f,0.f);
}


V_IMPLEMENT_SERIAL( VListControl, VDlgControlBase, 0, &g_VisionEngineModule );
void VListControl::Serialize( VArchive &ar )
{
  char iLocalVersion = 1;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=1,"Invalid version");
    ar >> m_bAllowSelection >> m_iBackgroundCol;
    if (iLocalVersion>=1)
      ar >> m_fIconSize >> m_vIconOfs;
    m_Items.SerializeX(ar);
    m_spVScrollbar = (VSliderControl *)ar.ReadObject(V_RUNTIME_CLASS(VSliderControl));
  } else
  {
    ar << iLocalVersion;
    ar << m_bAllowSelection << m_iBackgroundCol;
    ar << m_fIconSize << m_vIconOfs;
    m_Items.SerializeX(ar);
    ar.WriteObject(m_spVScrollbar);
  }
}



bool VListControl::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  // background color
  XMLHelper::Exchange_Color(XMLHelper::SubNode(pNode,"background",bWrite),"color",m_iBackgroundCol,bWrite);

  // selection
  XMLHelper::Exchange_Bool(pNode,"allowselection",m_bAllowSelection,bWrite);

  // icons
  XMLHelper::Exchange_Float(pNode,"iconsize", m_fIconSize, bWrite);
  XMLHelper::Exchange_Floats(pNode,"iconofs", m_vIconOfs.data, 2, bWrite);

  // load items
  m_Items.Build(XMLHelper::SubNode(pNode,"listitems",bWrite), szPath, bWrite);

  // scroll bars
  TiXmlElement *pScrollV = XMLHelper::SubNode(pNode,"vscrollbar",bWrite);
  if (pScrollV)
  {
    m_spVScrollbar = new VSliderControl();
    m_spVScrollbar->SetParent(this);
    m_spVScrollbar->SetVertical(true);
    m_spVScrollbar->Build(pScrollV,szPath,bWrite);
    m_spVScrollbar->OnBuildFinished();
  }
  m_bCollectionChanged = true;

  return true;
}


void VListControl::AddItem(VListControlItem *pItem, int iInsertBefore, bool bSetDefaultStates)
{
  m_Items.Add(pItem,iInsertBefore,bSetDefaultStates);
  m_bCollectionChanged = true;
}


VListControlItem* VListControl::AddItem(const char *szName, int iInsertBefore, int iData, void *pUserData)
{
  VListControlItem *pItem = new VListControlItem();
  pItem->m_iData = iData;
  pItem->SetUserData(pUserData);
  AddItem(pItem,iInsertBefore,true);
  pItem->SetText(szName);
  return pItem;
}



void VListControl::RemoveItem(VListControlItem *pItem)
{
  if (!pItem)
    return;

  m_Items.Remove(pItem);
  m_bCollectionChanged = true;

  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
  {
    VGUIUserInfo_t *pUser = GetContext()->GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
    if (!pUser) continue;
    if (pItem==m_spMouseOverItem[i]) SetMouseOverItem(*pUser,NULL);
    if (pItem==m_spSelectedItem[i]) SetSelection(NULL, pUser);
  }
}

void VListControl::RemoveItemAt(int iIndex)
{
  RemoveItem(m_Items.GetAtSafe(iIndex));
}

void VListControl::Reset()
{
  m_Items.Clear();
  m_bCollectionChanged = true;

  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
  {
    VGUIUserInfo_t *pUser = GetContext()->GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
    if (!pUser) continue;
    SetMouseOverItem(*pUser,NULL);
    SetSelection(NULL, pUser);
  }
}



void VListControl::SetScrollPosition(float fPos)
{
  if (m_fCurrentScrollPos==fPos) return;
  m_fCurrentScrollPos = fPos;
  m_bCollectionChanged = true;
  m_bFromScrollbar = false; // ignore scroll bar position next update; instead, set scrollbar value
}


void VListControl::EnsureVisible(VListControlItem *pItem)
{
  VASSERT(m_Items.Contains(pItem));
  hkvVec2 vScrollPos(0.f, -m_fCurrentScrollPos);
  m_Items.MeasureItems(VRectanglef(0,0,m_vSize.x,m_vSize.y),vScrollPos);

  if (pItem->IsFullyVisible())
    return;

  float fDiff;
  // upper bound outside ?
  fDiff = pItem->GetPosition().y;
  if (fDiff<0.f)
  {
    SetScrollPosition(m_fCurrentScrollPos+fDiff);
    return;
  }

  fDiff += pItem->GetSize().y - GetSize().y;
  VASSERT(fDiff>0.f) // since we know it is outside
  SetScrollPosition(m_fCurrentScrollPos+fDiff);
}



void VListControl::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VDlgControlBase::OnPaint(Graphics,parentState);

  // background:
  if (m_iBackgroundCol.a>0)
  {
    VRectanglef bbox = GetBoundingBox();
    VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState();
    Graphics.Renderer.DrawSolidQuad(bbox.m_vMin,bbox.m_vMax,m_iBackgroundCol,state);
  }

  // items
  if (m_Items.Count()>0)
  {
    VItemRenderInfo thisInfo(parentState,this,1.f);
    hkvVec2 vScrollPos(0.f, -m_fCurrentScrollPos);
    
    if (m_spVScrollbar)
    {
      if (m_bCollectionChanged)
      {
        m_Items.MeasureItems(VRectanglef(0,0,m_vSize.x,m_vSize.y),vScrollPos); // otherwise size isn't always correct
        m_bCollectionChanged = false;
        hkvVec2 vItemsSize = m_Items.GetSize();
        float fRange = vItemsSize.y - GetSize().y;
        m_spVScrollbar->SetVisible(fRange>0.f);
        if (fRange>0.f)
        {
          m_spVScrollbar->SetSliderRange(0.f,fRange, 0);
          m_spVScrollbar->SetSliderRelSize(GetSize().y/vItemsSize.y);
        }
      }
      if (m_spVScrollbar->IsVisible())
      {
        if (m_bFromScrollbar)
          m_fCurrentScrollPos = m_spVScrollbar->GetValue();
        else
          m_spVScrollbar->SetValue(m_fCurrentScrollPos);
      }

      m_bFromScrollbar = true;
      vScrollPos.y = -m_fCurrentScrollPos;
    }

    // pass the relative clipping rectangle to measureitems
    m_Items.MeasureItems(VRectanglef(0,0,m_vSize.x,m_vSize.y),vScrollPos);
    Graphics.ClippingStack.Push(GetBoundingBox(),true);
      m_Items.PaintAll(Graphics,thisInfo);
    Graphics.ClippingStack.Pop();
  }
  else if (m_spVScrollbar)
    m_spVScrollbar->SetVisible(false);

// paint scroll bar
  if (m_spVScrollbar && m_spVScrollbar->IsVisible())
    m_spVScrollbar->OnPaint(Graphics,parentState);

}


VWindowBase* VListControl::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  if (m_spVScrollbar && m_spVScrollbar->IsVisible())
  {
    VWindowBase *pHit = m_spVScrollbar->TestMouseOver(user, vAbsMouse);
    if (pHit) return pHit;
  }

  if (!VDlgControlBase::TestMouseOver(user, vAbsMouse))
    return NULL;

  return this;
}


VTooltip* VListControl::GetTooltip(VGUIUserInfo_t &user)
{
  int i = user.m_iID;
  if (m_spMouseOverItem[i])
    return m_spMouseOverItem[i]->GetTooltip(user);
  return VDlgControlBase::GetTooltip(user);
}



void VListControl::OnSizeChanged() 
{
  VDlgControlBase::OnSizeChanged();

  for (int i=0;i<m_Items.Count();i++)
    m_Items.GetAt(i)->OnParentSizeChanged();

  AdjustScrollBars();
}

void VListControl::AdjustScrollBars()
{
  if (m_spVScrollbar)
  {
    hkvVec2 oldSize = m_spVScrollbar->GetSize();
    m_spVScrollbar->SetSize(oldSize.x,GetSize().y);
    m_spVScrollbar->SetPosition(GetSize().x,0.f);
  }
}


void VListControl::OnClick(VMenuEventDataObject *pEvent)
{
}

void VListControl::OnDoubleClick(VMenuEventDataObject *pEvent)
{
  VWindowBase::OnDoubleClick(pEvent);
}

void VListControl::OnPointerDown(VMenuEventDataObject *pEvent)
{
  VWindowBase::OnPointerDown(pEvent);

  if (pEvent->m_iButtons != BUTTON_LMOUSE)
  {
    // no left button, so nothing to be done here
    return;
  }

  int i = pEvent->m_User.m_iID;
  m_spMouseDownItem[i] = m_spMouseOverItem[i];
}

void VListControl::OnPointerUp(VMenuEventDataObject *pEvent)
{
  VWindowBase::OnPointerUp(pEvent);

  if (pEvent->m_iButtons != BUTTON_LMOUSE)
  {
    // no left button, so nothing to be done here
    return;
  }

  int i = pEvent->m_User.m_iID;
  if (m_spMouseOverItem[i] && m_spMouseOverItem[i] == m_spMouseDownItem[i])
  {
    SetSelection(m_spMouseOverItem[i], &pEvent->m_User);
    m_spMouseDownItem[i] = NULL;
  }
}

void VListControl::SetSelectionIndex(int iIndex, const VGUIUserInfo_t *pUser)
{
  SetSelection(m_Items.GetAtSafe(iIndex),pUser);
}


void VListControl::SetSelection(VListControlItem *pItem, const VGUIUserInfo_t *pUser)
{
  int i = (pUser!=NULL) ? pUser->m_iID : 0;
  if (!m_bAllowSelection || m_spSelectedItem[i]==pItem)
    return;
  if (m_spSelectedItem[i])
    m_spSelectedItem[i]->SetSelected(false);
  m_spSelectedItem[i] = pItem;
  if (m_spSelectedItem[i])
    m_spSelectedItem[i]->SetSelected(true);

  // send selection changed event here
  VItemValueChangedEvent data(this,VItemValueChangedEvent::VALUE_LISTITEM,pItem, false);
  SendValueChangedEvent(&data);
}


  
VListControlItem *VListControl::GetItemAt(VGUIUserInfo_t &user,const hkvVec2 &vAbsPos) const
{
  return m_Items.GetItemAt(user, vAbsPos);
}


void VListControl::SetMouseOverItem(VGUIUserInfo_t &user, VListControlItem *pItem)
{
  int i = user.m_iID;
  if (m_spMouseOverItem[i]==pItem) return;
  if (m_spMouseOverItem[i])
    m_spMouseOverItem[i]->OnMouseLeave(user);
  m_spMouseOverItem[i] = pItem;
  if (pItem)
    pItem->OnMouseEnter(user);
}


void VListControl::OnTick(float dtime)
{
  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
  {
    VGUIUserInfo_t *pUser = GetContext()->GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
    if (!pUser) continue;
    SetMouseOverItem(*pUser, GetItemAt(*pUser, pUser->m_vMousePos));
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
