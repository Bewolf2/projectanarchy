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

VDialog::VDialog()
{
  m_Frame.SetOwner(this);
  m_iDialogFlags = 0;
  m_iOrder = DIALOG_ORDER_FRONT;
  m_iDialogResult = 0;
  m_fFadeInPos = 1.f;
  m_fFadeInSpeed = 0.f;
  m_iStatus = ITEMSTATUS_VISIBLE|ITEMSTATUS_ENABLED;
  m_vClientMin.set(16.f,16.f);
  m_vClientMax.set(8192.0f, 8192.0f); // 8192.0f is enough for everyone (greetings from the past!)
  m_iBackFadeColor = VColorRef(100,100,100,255);
  m_fBackFadePos = 0.f;
  m_fBackFadeSpeed = 1.f;
  m_bFadeBack = false;
  m_eStartPos = AbsolutePos;
  m_iBackColor = VColorRef(0,0,0,0); // a=0 -> no constant color
  m_pImage = NULL;
}

VRectanglef VDialog::GetClientRect() const 
{
  return m_Frame.GetClientRect();
}

VDialog::~VDialog()
{
  V_SAFE_DELETE(m_pImage);
}


void VDialog::InitDialog(IVGUIContext *pContext, VDialogResource *pRes, VDialog *pParent, int iFlags)
{
  m_iDialogFlags = iFlags;
  m_spResource = pRes;
  m_pContext = pContext;
  m_pOwner = pParent;
}



BOOL VDialog::SaveToBinaryFile(const char *szFilename)
{
  // no file extension? Then add default binary extension
  char szBuffer[FS_MAX_PATH];
  if (VFileHelper::GetExtensionPos(szFilename)<0)
  {
    VFileHelper::AddExtension(szBuffer, szFilename, DIALOG_BINARYFILEEXT);
    szFilename = szBuffer;
  }

  IVFileOutStream *pOut = Vision::File.Create(szFilename);
  if (!pOut)
    return FALSE;

  VArchive ar(NULL, pOut, Vision::GetTypeManager());
  ar << (int)Vision::GetArchiveVersion();
  ar.WriteObject(this);
  ar.Close();
  pOut->Close();
  return TRUE;
}

VDialog* VDialog::LoadFromBinaryFile(const char *szFilename)
{
  // no file extension? Then add default binary extension
  char szBuffer[FS_MAX_PATH];
  if (VFileHelper::GetExtensionPos(szFilename)<0)
  {
    VFileHelper::AddExtension(szBuffer, szFilename, DIALOG_BINARYFILEEXT);
    szFilename = szBuffer;
  }
  IVFileInStream *pIn = Vision::File.Open(szFilename);
  if (!pIn)
    return NULL;

  VArchive ar(NULL, pIn, Vision::GetTypeManager());
  int iArchiveVersion;
  ar >> iArchiveVersion;
  ar.SetLoadingVersion(iArchiveVersion);

  VDialog* pDlg = (VDialog *)ar.ReadObject(V_RUNTIME_CLASS(VDialog));
  ar.Close();
  pIn->Close();
  return pDlg;
}



V_IMPLEMENT_SERIAL( VDialog, VWindowBase, 0, &g_VisionEngineModule );
void VDialog::Serialize( VArchive &ar )
{
  VWindowBase::Serialize(ar);
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_iDialogFlags >> (int &)m_eStartPos;
    ar >> m_fFadeInPos >> m_fFadeInSpeed >> m_iBackColor >> m_bFadeBack;
    ar >> m_iBackFadeColor >> m_fBackFadePos >> m_fBackFadeSpeed;

    V_SAFE_DELETE(m_pImage);
    bool bHasImg = false;
    ar >> bHasImg;
    if (bHasImg)
    {
      m_pImage = new VImageStates();
      ar >> *m_pImage;
    }
    ar >> m_Frame;

    // item list:
    m_Items.SerializeX(ar);
  } else
  {
    ar << iLocalVersion;
    ar << m_iDialogFlags << (int)m_eStartPos;
    ar << m_fFadeInPos << m_fFadeInSpeed << m_iBackColor << m_bFadeBack;
    ar << m_iBackFadeColor << m_fBackFadePos << m_fBackFadeSpeed;

    ar << (bool)(m_pImage!=NULL);
    if (m_pImage)
    {
      ar << *m_pImage;
    }
    ar << m_Frame;

    // item list:
    m_Items.SerializeX(ar);

  }
}


#define CHECK_TYPESTR(t)  {if (!_stricmp(szTypeStr,#t)) return t;}
VDialog::StartPositionType_e VDialog::GetStartPositionType(const char *szTypeStr)
{
  CHECK_TYPESTR(AbsolutePos)
  CHECK_TYPESTR(ScreenCenter)
  CHECK_TYPESTR(ParentPos)
  CHECK_TYPESTR(ParentCenter)
  VASSERT(FALSE);
  return AbsolutePos;
}



void VDialog::OnActivate()
{
  m_Items.SortByPriority();
  m_Items.OnActivate(true);
}

void VDialog::OnDeactivate()
{
  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
    m_spMouseOverItem[i] = NULL;
  m_Items.OnActivate(false);
}

void VDialog::Refresh()
{
  m_Items.Refresh();
}

void VDialog::OnDragging(const hkvVec2 &vMouseDelta)
{
  VWindowBase::OnDragging(vMouseDelta);
  hkvVec2 vNewPos = GetPosition() + vMouseDelta;
  SetPosition(vNewPos.x,vNewPos.y);
}


void VDialog::OnPositionChanged()
{
  VWindowBase::OnPositionChanged();

  if (IsFullscreen())
  {
    VRectanglef v = GetContext()->GetClientRect();
    m_vPosition = v.m_vMin;
    m_vSize = v.GetSize();
  }
  else
    m_Items.OnParentChanged(PARENTCHANGED_POSITION);
}

void VDialog::OnSizeChanged()
{
  VWindowBase::OnSizeChanged();

  if (IsFullscreen())
  {
    VRectanglef v = GetContext()->GetClientRect();
    m_vPosition = v.m_vMin;
    m_vSize = v.GetSize();
  } else
  {
    VRectanglef range(m_vClientMin, m_vClientMax);
    range += hkvVec2(m_Frame.GetSizeX(),m_Frame.GetSizeY());
    range.Clamp(m_vSize);
    m_Items.OnParentChanged(PARENTCHANGED_SIZE);
  }
}



VCursor* VDialog::GetCurrentCursor(VGUIUserInfo_t &user)
{
  int i = user.m_iID;
  if (m_spMouseOverItem[i])
    return m_spMouseOverItem[i]->GetMouseOverCursor(user);
  return NULL;
}


void VDialog::BringToFront()
{
  m_iOrder = m_pContext->m_OpenDialogs.GetMinOrder()-1;
  m_pContext->m_OpenDialogs.SortByOrder();
}

void VDialog::BringToBack()
{
  m_iOrder = m_pContext->m_OpenDialogs.GetMaxOrder()+1;
  m_pContext->m_OpenDialogs.SortByOrder();
}



void VDialog::OnSetFocus(bool bStatus)
{
  VWindowBase::OnSetFocus(bStatus);

  if (bStatus)
  {
    BringToFront();
  }
  else
  {
    RemoveStatus(ITEMSTATUS_HASFOCUS);
  }
}

VWindowBase* VDialog::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  int i = user.m_iID;
  if (m_spMouseOverItem[i])
    return m_spMouseOverItem[i];
  return VWindowBase::TestMouseOver(user, vAbsMouse);
}



void VDialog::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  if (!IsVisible())
    return;

  if (m_spCachedBuffer)
    if (m_spCachedBuffer->UpdateCache(this, Graphics, parentState))
      return; // finished rendering

  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState();

  VItemRenderInfo thisState(parentState, this, m_fFadeInPos);

  // render multiplicative screen quad
  if (m_bFadeBack)
  {
    VRectanglef screenrect = GetContext()->GetClientRect();
    VSimpleRenderState_t multState = VGUIManager::DefaultGUIRenderState(VIS_TRANSP_MULTIPLICATIVE);
    VColorRef iColor;
    iColor.Lerp(V_RGBA_WHITE,m_iBackFadeColor,m_fBackFadePos);
    Graphics.Renderer.DrawSolidQuad(screenrect.m_vMin,screenrect.m_vMax,iColor,multState);
  }

  // constant background color
  if (m_iBackColor.a>0)
  {
    VRectanglef rect = GetClientRect();
    Graphics.Renderer.DrawSolidQuad(rect.m_vMin,rect.m_vMax,m_iBackColor,state);
  }

  // render dialog frame and background image
  if (m_pImage) m_pImage->OnPaint(Graphics,thisState);
  m_Frame.OnPaint(Graphics,thisState);

  // render all items in it and clip against the window rectangle (clipping against GetClientRect would cause the close button to disappear)
  Graphics.ClippingStack.Push(GetBoundingBox(),true);
    m_Items.RenderAll(Graphics,thisState);
  Graphics.ClippingStack.Pop();
}


void VDialog::OnTick(float dtime)
{
  m_fFadeInPos += m_fFadeInSpeed*dtime;
  if (m_fFadeInPos>1.f)
    m_fFadeInPos = 1.f;

  m_fBackFadePos += m_fBackFadeSpeed*dtime;

  m_Items.OnTickFunction(dtime);

  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
  {
    VGUIUserInfo_t *pUser = GetContext()->GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
    if (!pUser) continue;
    m_spMouseOverItem[i] = m_Items.GetMouseOverItem(*pUser);
  }
}

// focus
void VDialog::SetFocusItem(VWindowBase *pNewItem)
{
  if (m_spFocusItem)
  {
    m_spFocusItem->OnSetFocus(false);
  }
  m_spFocusItem = pNewItem;
  if (m_spFocusItem)
  {
    m_spFocusItem->OnSetFocus(true);
  }
}


void VDialog::OnItemClicked(VMenuEventDataObject *pEvent)
{
  // set the dialog result
  VASSERT(pEvent->m_pItem);
  SetFocusItem(pEvent->m_pItem);
  pEvent->m_pItem->OnClick(pEvent);
  SetDialogResult(pEvent->m_pItem->GetDialogResult());

  int iButtons = ((pEvent->m_iButtons&1)<<1)|((pEvent->m_iButtons&2)>>1); //exchange bit 0 and 1
  TriggerScriptEvent("OnItemClicked", "*oiff", pEvent->m_pItem, iButtons , pEvent->m_vMousePos.x, pEvent->m_vMousePos.y);

}

void VDialog::OnItemDoubleClicked(VMenuEventDataObject *pEvent)
{
  VASSERT(pEvent->m_pItem);
  pEvent->m_pItem->OnDoubleClick(pEvent);

  int iButtons = ((pEvent->m_iButtons&1)<<1)|((pEvent->m_iButtons&2)>>1); //exchange bit 0 and 1
  TriggerScriptEvent("OnItemDoubleClicked", "*oiff", pEvent->m_pItem, iButtons, pEvent->m_vMousePos.x, pEvent->m_vMousePos.y);

}


void VDialog::OnValueChanged(VItemValueChangedEvent *pEvent)
{
}


void VDialog::FillEvent(VMenuEventDataObject *pEvent)
{
  if (!IsVisible() || !IsEnabled()) // nothing should happen in these cases
    return;

  // item clicked?
  int i = pEvent->m_User.m_iID;
  if (m_spMouseOverItem[i])
  {
    pEvent->m_pDialog = this;
    pEvent->m_bProcessed = true;
    pEvent->m_pItem = m_spMouseOverItem[i];
    return;
  }

  // just inside dialog bbox? Then the focus will be set
  if (TestMouseOver(pEvent->m_User, pEvent->m_vMousePos))
  {
    pEvent->m_pDialog = this;
    pEvent->m_bProcessed = true;
    pEvent->m_pItem = NULL;
  }
}


void VDialog::AddControl(VDlgControlBase *pItem)
{
  pItem->SetParent(this);
  m_Items.Add(pItem);
}


void VDialog::ResumeLayout()
{
  VRectanglef bbox;
  for (int i=0;i<m_Items.Count();i++)
  {
    VDlgControlBase *pItem = m_Items.GetAt(i);
    bbox.Add(pItem->GetBoundingBox());
  }
  if (bbox.IsValid())
  {
    //SetPosition(bbox.m_vMin.x,bbox.m_vMin.y);
    SetSize(bbox.m_vMax.x,bbox.m_vMax.y);
  }
}

hkvVec2 VDialog::GetStartPosition() const
{
  switch (m_eStartPos)
  {
    case AbsolutePos:
      return GetPosition();
    case ScreenCenter:
    {
      IVGUIContext *pContext = GetContext();
      VRectanglef rect = pContext->GetClientRect();
      hkvVec2 center = (rect.GetSize() - GetSize())*0.5f + GetPosition();
      if (m_pOwner) 
        center -= m_pOwner->GetAbsPosition();
      return center;
    }
    case ParentPos:
    {
      if (!m_pOwner) return GetPosition();
      return GetPosition() + m_pOwner->GetAbsPosition();
    }
    case ParentCenter:
    {
      if (!m_pOwner) return GetPosition();
      hkvVec2 OwnerSize = m_pOwner->GetSize();
      return ((OwnerSize - GetSize())*0.5f);
    }
  }
  return hkvVec2(0,0);
}


bool VDialog::Build(TiXmlElement *pNode, const char *szPath, bool bWrite) 
{
  m_vSize.set(100,100);
  if (!VWindowBase::Build(pNode,szPath,bWrite))
    return false;

  // additional size/position properties
  XMLHelper::Exchange_Floats(pNode,"clientMin",m_vClientMin.data,2,bWrite);
  XMLHelper::Exchange_Floats(pNode,"clientMax",m_vClientMax.data,2,bWrite);
  bool bFullscreen = false;
  XMLHelper::Exchange_Bool(pNode,"fullscreen",bFullscreen,bWrite);
  if (bFullscreen) m_iDialogFlags|=DIALOGFLAGS_FULLSCREEN;
  
  // start position
  const char *szStartPos = XMLHelper::Exchange_String(pNode,"startPosition",NULL,bWrite);
  if (szStartPos)
    m_eStartPos = GetStartPositionType(szStartPos);
  
  // build frame and background image
  TiXmlElement *pImageNode = XMLHelper::SubNode(pNode,"image",bWrite);
  if (pImageNode)
  {
    V_SAFE_DELETE(m_pImage);
    m_pImage = new VImageStates();
    m_pImage->Build(this, pImageNode, szPath, bWrite);
  }
  m_Frame.Build(XMLHelper::SubNode(pNode,"frame",bWrite), szPath, bWrite);

  // dlg fading
  TiXmlElement *pFadein = XMLHelper::SubNode(pNode,"fadein",bWrite);
  if (pFadein)
  {
    m_fFadeInPos = 0.f;
    m_fFadeInSpeed = 2.f;
    float fFadeTime = 0.f;
    XMLHelper::Exchange_Float(pFadein,"start",m_fFadeInPos,bWrite);
    XMLHelper::Exchange_Float(pFadein,"speed",m_fFadeInSpeed,bWrite);
    XMLHelper::Exchange_Float(pFadein,"time",fFadeTime,bWrite);// optional
    if (fFadeTime>0.f) 
      m_fFadeInSpeed = (1.f-m_fFadeInPos)/fFadeTime;
  }

  TiXmlElement *pFadeBack = XMLHelper::SubNode(pNode,"fadeback",bWrite);
  if (pFadeBack)
  {
    m_bFadeBack = true;
    float fFadeTime = 0.f;
    XMLHelper::Exchange_Float(pFadeBack,"start",m_fBackFadePos,bWrite);
    XMLHelper::Exchange_Float(pFadeBack,"speed",m_fBackFadeSpeed,bWrite);
    XMLHelper::Exchange_Color(pFadeBack,"color",m_iBackFadeColor,bWrite);
    XMLHelper::Exchange_Float(pFadeBack,"time",fFadeTime,bWrite);// optional
    if (fFadeTime>0.f) 
      m_fBackFadeSpeed = (1.f-m_fBackFadePos)/fFadeTime;
  }

  // constant dialog background color
  XMLHelper::Exchange_Color(pNode,"backgroundcolor",m_iBackColor,bWrite);
  

  // build all items ("control" nodes)
  m_Items.Build(this,pNode,szPath,bWrite);

  return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// VDialogCollection functions
/////////////////////////////////////////////////////////////////////////////////////////////////


int VDialogCollection::CompareDialogOrder( const void *arg1, const void *arg2)
{
  VDialog *pElem1 = *(VDialog **)arg1;
  VDialog *pElem2 = *(VDialog **)arg2;
  if (pElem1->m_iOrder>pElem2->m_iOrder) return 1;
  if (pElem1->m_iOrder<pElem2->m_iOrder) return -1;
  return 0;
}

void VDialogCollection::SortByOrder()
{
  qsort(GetPtrs(),Count(),sizeof(VDialog *),CompareDialogOrder);
}

void VDialogCollection::AssignNewOrder()
{
  for (int i=0;i<Count();i++)
    GetAt(i)->m_iOrder = i;
}

int VDialogCollection::GetMinOrder()
{
  int iMin = 0; // the first one should be 0;
  for (int i=0;i<Count();i++)
    if (i==0 || GetAt(i)->m_iOrder<iMin)
      iMin = GetAt(i)->m_iOrder;
  return iMin;
}

int VDialogCollection::GetMaxOrder()
{
  int iMax = 0; // the first one should be 0;
  for (int i=0;i<Count();i++)
    if (i==0 || GetAt(i)->m_iOrder>iMax)
      iMax = GetAt(i)->m_iOrder;
  return iMax;
}



  
void VDialogCollection::RenderAll(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  // render back to front
  int iCount = Count();
  for (int i=iCount-1;i>=0;i--)
    GetAt(i)->OnPaint(Graphics,parentState);
}

  
void VDialogCollection::OnTickFunction(float fTimeDelta)
{
  for (int i=0;i<Count();i++)
    GetAt(i)->OnTick(fTimeDelta);
}


void VDialogCollection::Clear()
{
  for (int i=0;i<Count();i++)
    GetAt(i)->OnDeactivate();
  VRefCountedCollection<VDialog>::Clear();
}

void VDialogCollection::Add(VDialog *pDialog)
{
  VRefCountedCollection<VDialog>::Add(pDialog);
  pDialog->OnActivate();
}

void VDialogCollection::Remove(VDialog *pDialog)
{
  pDialog->OnDeactivate();
  VRefCountedCollection<VDialog>::Remove(pDialog);
}


VDialog* VDialogCollection::FindDialog(int iID) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->GetID()==iID)
      return GetAt(i);
  return NULL;
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
