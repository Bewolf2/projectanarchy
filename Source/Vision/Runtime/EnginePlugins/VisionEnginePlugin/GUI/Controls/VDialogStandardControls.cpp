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

///////////////////////////////////////////////////////////////////////////////////////////////////////
// VTextLabel class
///////////////////////////////////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_SERIAL( VTextLabel, VDlgControlBase, 0, &g_VisionEngineModule );
void VTextLabel::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_TextCfg;
  } else
  {
    ar << iLocalVersion;
    ar << m_TextCfg;
  }
}


void VTextLabel::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VItemRenderInfo thisState(parentState,this,1.f);
  m_TextCfg.OnPaint(Graphics,thisState);
}


bool VTextLabel::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  m_TextCfg.Build(this,XMLHelper::SubNode(pNode,"text",bWrite),szPath,bWrite);

  // size from text
  if (m_vSize.x<=0.f)
    m_vSize = m_TextCfg.m_States[VDlgControlBase::NORMAL].GetSize();

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// VImageControl class
///////////////////////////////////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_SERIAL( VImageControl, VDlgControlBase, 0, &g_VisionEngineModule );
void VImageControl::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_Image;
  } else
  {
    ar << iLocalVersion;
    ar << m_Image;
  }
}

void VImageControl::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VItemRenderInfo thisState(parentState,this,1.f);
  Graphics.ClippingStack.Push(GetBoundingBox(),true);
    m_Image.OnPaint(Graphics,thisState);
  Graphics.ClippingStack.Pop();
}

bool VImageControl::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  m_Image.Build(this,XMLHelper::SubNode(pNode,"image",bWrite),szPath,bWrite);
  return true;
}

void VImageControl::OnBuildFinished()
{
  if (m_vSize.x<0.f)
    m_vSize = m_Image.m_States[VWindowBase::NORMAL].GetSize();
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
// VPushButton class
///////////////////////////////////////////////////////////////////////////////////////////////////////


V_IMPLEMENT_SERIAL( VPushButton, VDlgControlBase, 0, &g_VisionEngineModule );
void VPushButton::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_ButtonCfg;
    ar >> m_TextCfg;
  } else
  {
    ar << iLocalVersion;
    ar << m_ButtonCfg;
    ar << m_TextCfg;
  }
}

void VPushButton::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VItemRenderInfo thisState(parentState,this,1.f);
  m_ButtonCfg.OnPaint(Graphics,thisState);
  m_TextCfg.OnPaint(Graphics,thisState);
}



bool VPushButton::Build(TiXmlElement *pNode, const char *szPath, bool bWrite) 
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  m_ButtonCfg.Build(this,XMLHelper::SubNode(pNode,"image",bWrite),szPath,bWrite);
  m_TextCfg.Build(this,XMLHelper::SubNode(pNode,"text",bWrite),szPath,bWrite);

  // size from texture
  if (m_vSize.x<=0.f)
    m_vSize = m_ButtonCfg.m_States[VDlgControlBase::NORMAL].GetSize();

  // ..or from text
  if (m_vSize.x<=0.f)
    m_vSize = m_TextCfg.m_States[VDlgControlBase::NORMAL].GetSize();

  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// VCheckBox class
///////////////////////////////////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_SERIAL( VCheckBox, VPushButton, 0, &g_VisionEngineModule );
void VCheckBox::Serialize( VArchive &ar )
{
  VPushButton::Serialize(ar);
}

void VCheckBox::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VItemRenderInfo thisState(parentState,this,1.f);

  m_ButtonCfg.OnPaint(Graphics,thisState);
  m_TextCfg.OnPaint(Graphics,thisState);
}


bool VCheckBox::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  m_ButtonCfg.Build(this,XMLHelper::SubNode(pNode,"image",bWrite),szPath,bWrite);
  m_TextCfg.Build(this,XMLHelper::SubNode(pNode,"text",bWrite),szPath,bWrite);

  m_ButtonCfg.SetStretchMode(VImageState::TEXTURE_SIZE);

  // get the size of the control
  if (m_vSize.x<=0.f)
  {
    m_vSize = m_ButtonCfg.m_States[VDlgControlBase::NORMAL].GetSize();
    float fBoxWidth = m_vSize.x;
    hkvVec2 vTextSize = m_TextCfg.m_States[VDlgControlBase::NORMAL].GetSize();
    m_vSize.x += vTextSize.x + fBoxWidth;
    m_vSize.y = hkvMath::Max(m_vSize.y,vTextSize.y);

    // offset the text by the size of the checkbox
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
    {
      hkvVec2 vNewOfs = m_TextCfg.m_States[i].GetTextOfs();
      vNewOfs.x += fBoxWidth;
      m_TextCfg.m_States[i].SetTextOfs(vNewOfs);
    }
  }

  // initial checked status (actually same as selected)
  bool bChecked = false;
  XMLHelper::Exchange_Bool(pNode,"checked",bChecked,bWrite);
  SetChecked(bChecked);

  return true;
}



void VCheckBox::SetChecked(bool bStatus)
{
  if (bStatus==IsChecked())
    return;
  SetSelected(bStatus);

  // send changed event
  VItemValueChangedEvent data(this,VItemValueChangedEvent::VALUE_BOOL,&bStatus, false);
  SendValueChangedEvent(&data);
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// Dialog Title bar (can move dialog)
/////////////////////////////////////////////////////////////////////////////////////////////////


VDialogTitleBar::VDialogTitleBar()
{
  m_bDragParent = false;
  m_iOrder = 1;
}

V_IMPLEMENT_SERIAL( VDialogTitleBar, VDlgControlBase, 0, &g_VisionEngineModule );
void VDialogTitleBar::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_TextCfg;
  } else
  {
    ar << iLocalVersion;
    ar << m_TextCfg;
  }
}


void VDialogTitleBar::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VItemRenderInfo thisState(parentState,this,1.f);
  m_TextCfg.OnPaint(Graphics,thisState);
}


bool VDialogTitleBar::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  // title bar text
  m_TextCfg.Build(this,XMLHelper::SubNode(pNode,"text",bWrite),szPath,bWrite);

  return true;
}

  
void VDialogTitleBar::AdjustBar()
{
  VASSERT(m_pOwner);
  hkvVec2 v = m_pOwner->GetSize();
  const VDialogFrame *pFrame = &((VDialog *)m_pOwner)->GetFrame(); // the owner must be a dialog!
  SetSize(v.x-pFrame->GetSizeX(),pFrame->m_fBorderTop);
  SetPosition(pFrame->m_fBorderLeft,0);
}


void VDialogTitleBar::OnActivate()
{
  VDlgControlBase::OnActivate();
  AdjustBar();
}

void VDialogTitleBar::OnParentChanged(int iFlags)
{
  VDlgControlBase::OnParentChanged(iFlags);
  AdjustBar();
}


void VDialogTitleBar::OnDragBegin(const hkvVec2 &vMousePos, int iButtonMask)
{
  VDlgControlBase::OnDragBegin(vMousePos,iButtonMask);
  // drag the owner
  VASSERT(m_pOwner);
  m_bDragParent = (iButtonMask==BUTTON_LMOUSE); // only drag with left button
  if (m_bDragParent)
    m_pOwner->OnDragBegin(vMousePos,iButtonMask);
}

void VDialogTitleBar::OnDragging(const hkvVec2 &vMouseDelta)
{
  VDlgControlBase::OnDragging(vMouseDelta);
  // drag the owner
  if (m_bDragParent)
    m_pOwner->OnDragging(vMouseDelta);
}

void VDialogTitleBar::OnDragEnd(VWindowBase *pOver) 
{
  VDlgControlBase::OnDragEnd(pOver);
  // drag the owner
  if (m_bDragParent)
    m_pOwner->OnDragEnd(pOver);
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// Dialog Resize control
/////////////////////////////////////////////////////////////////////////////////////////////////


VDialogResizeCtrl::VDialogResizeCtrl()
{
  m_iOrder = 2;
}

V_IMPLEMENT_SERIAL( VDialogResizeCtrl, VDlgControlBase, 0, &g_VisionEngineModule );
void VDialogResizeCtrl::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_vDragStart >> m_vDragPos >> m_vStartSize;
  } else
  {
    ar << iLocalVersion;
    ar << m_vDragStart << m_vDragPos << m_vStartSize;
  }
}



/*
void VDialogResizeCtrl::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  hkvVec2 v1 = m_pOwner->GetPosition() + GetPosition();
  hkvVec2 v2 = v1 + m_vSize;

  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState();
  Graphics.Renderer.DrawSolidQuad(v1,v2,IsStatusSet(ITEMSTATUS_MOUSEOVER) ? V_RGBA_RED:V_RGBA_GREEN, state);
}
*/
  
void VDialogResizeCtrl::AdjustPosition()
{
  // align it to corner
  VASSERT(m_pOwner);
  const VDialogFrame *pFrame = &((VDialog *)m_pOwner)->GetFrame(); // the owner must be a dialog!
  SetSize(pFrame->m_fBorderRight,pFrame->m_fBorderBottom);
  hkvVec2 v = m_pOwner->GetSize();
  SetPosition(v.x-m_vSize.x, v.y-m_vSize.y);
}


void VDialogResizeCtrl::OnActivate()
{
  VDlgControlBase::OnActivate();
  AdjustPosition();
}

void VDialogResizeCtrl::OnParentChanged(int iFlags)
{
  VDlgControlBase::OnParentChanged(iFlags);
  AdjustPosition();
}


void VDialogResizeCtrl::OnDragBegin(const hkvVec2 &vMousePos, int iButtonMask)
{
  m_vDragPos = m_vDragStart = vMousePos;
  m_vStartSize = m_pOwner->GetSize();
  VDlgControlBase::OnDragBegin(vMousePos,iButtonMask);
}

void VDialogResizeCtrl::OnDragging(const hkvVec2 &vMouseDelta)
{
  VDlgControlBase::OnDragging(vMouseDelta);
  VASSERT(m_pOwner);
  m_vDragPos += vMouseDelta;

  hkvVec2 vNewSize = m_vStartSize + m_vDragPos - m_vDragStart;
  m_pOwner->SetSize(vNewSize.x,vNewSize.y);
}

void VDialogResizeCtrl::OnDragEnd(VWindowBase *pOver) 
{
  VDlgControlBase::OnDragEnd(pOver);
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// Dialog Close button
/////////////////////////////////////////////////////////////////////////////////////////////////


VDialogCloseButton::VDialogCloseButton()
{
  m_iOrder = 3;
  m_iID = m_iDialogResult = VGUIManager::ID_CANCEL;
}

V_IMPLEMENT_SERIAL( VDialogCloseButton, VPushButton, 0, &g_VisionEngineModule );
void VDialogCloseButton::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VPushButton::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_vOfs;
  } else
  {
    ar << iLocalVersion;
    ar << m_vOfs;
  }
}



bool VDialogCloseButton::Build(TiXmlElement *pNode, const char *szPath, bool bWrite) 
{
  if (!VPushButton::Build(pNode,szPath,bWrite))
    return false;
  
  // relative offset to corner
  XMLHelper::Exchange_Floats(pNode,"ofs",m_vOfs.data,2,bWrite);

  return true;
}

void VDialogCloseButton::AdjustPosition()
{
  // align it to corner
  VASSERT(m_pOwner);
  const VDialogFrame *pFrame = &((VDialog *)m_pOwner)->GetFrame(); // the owner must be a dialog!
  hkvVec2 vArea(pFrame->m_fBorderRight,pFrame->m_fBorderTop);
  hkvVec2 vOfs = m_vOfs + (vArea-m_vSize)*0.5f;
  hkvVec2 v = m_pOwner->GetSize();
  SetPosition(v.x-vArea.x+vOfs.x, vOfs.y);
}


void VDialogCloseButton::OnActivate()
{
  VPushButton::OnActivate();
  AdjustPosition();
}

void VDialogCloseButton::OnParentChanged(int iFlags)
{
  VPushButton::OnParentChanged(iFlags);
  AdjustPosition();
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
