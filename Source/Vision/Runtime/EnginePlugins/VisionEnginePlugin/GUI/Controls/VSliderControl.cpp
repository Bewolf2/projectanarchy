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
// VSliderControl
////////////////////////////////////////////////////////////////////////////////////////////


VSliderControl::VSliderControl()
{
  m_Frame.SetStretchMode(VImageState::BORDER);
  m_fCurrentValue = m_fRangeMin = 0.f;
  m_fRangeMax = 100.f;
  m_iTickCount = 0;
  m_bVertical = false;
  m_fSliderRelWidth = 0.f; // texture size
  m_BorderSize.Set(0,0,0,0);
}


VSliderControl::~VSliderControl()
{
}


V_IMPLEMENT_SERIAL( VSliderControl, VDlgControlBase, 0, &g_VisionEngineModule );
void VSliderControl::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_fRangeMin >> m_fRangeMax >> m_fCurrentValue;
    ar >> m_iTickCount >> m_bVertical >> m_fSliderRelWidth;
    ar >> m_BorderSize;
    ar >> m_Frame;
    m_spSlider = (VSlider *)ar.ReadObject(V_RUNTIME_CLASS(VSlider));
  } else
  {
    ar << iLocalVersion;
    ar << m_fRangeMin << m_fRangeMax << m_fCurrentValue;
    ar << m_iTickCount << m_bVertical << m_fSliderRelWidth;
    ar << m_BorderSize;
    ar << m_Frame;
    ar.WriteObject(m_spSlider);
  }
}


VSlider* VSliderControl::GetSlider()
{
  if (!m_spSlider)
    m_spSlider = new VSlider(this);
  return m_spSlider;
}

void VSliderControl::OnClick(VMenuEventDataObject *pEvent)
{
  if (pEvent->m_iButtons != BUTTON_LMOUSE)
  {
    // no left button, so nothing to be done here
    return;
  }

  // calculate the new position for the center of the slider
  hkvVec2 vNewPos = pEvent->m_vMousePos - GetAbsPosition() - GetSlider()->GetSize() * 0.5f;

  // calculate the moving range
  hkvVec2 vMoveRange = GetSize() - GetSlider()->GetSize() - m_BorderSize.m_vMin - m_BorderSize.m_vMax;

  float fRelValue = 0.f;

  // align
  if (m_bVertical)
  {
    vNewPos.x=0.f; 
    if (vNewPos.y<0.f) vNewPos.y = 0.f;
      else if (vNewPos.y>vMoveRange.y) vNewPos.y = vMoveRange.y;
    if (vMoveRange.y>0.f)
      fRelValue = vNewPos.y/vMoveRange.y;
  } 
  else
  {
    vNewPos.y=0.f;
    if (vNewPos.x<0.f) vNewPos.x = 0.f;
      else if (vNewPos.x>vMoveRange.x) vNewPos.x = vMoveRange.x;
    if (vMoveRange.x>0.f)
      fRelValue = vNewPos.x/vMoveRange.x;
  }
  // map to specified range and set value
  SetValue(m_fRangeMin+fRelValue*(m_fRangeMax-m_fRangeMin), false);

}


bool VSliderControl::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  // frame
  m_Frame.Build(this, XMLHelper::SubNode(pNode,"frame",bWrite),szPath,bWrite);

  // border
  float *borderSizeData = (float*)(&m_BorderSize.m_vMin);
  XMLHelper::Exchange_Floats(pNode,"border",borderSizeData,4,bWrite);


  // slider button
  TiXmlElement *pSliderNode = XMLHelper::SubNode(pNode,"slider",bWrite);
  GetSlider()->Build(pSliderNode,szPath,bWrite);
  GetSlider()->OnBuildFinished();
  
  // range
  float fRange[2] = {m_fRangeMin, m_fRangeMax};
  XMLHelper::Exchange_Floats(pNode,"range",fRange,2,bWrite);
  m_fRangeMin = fRange[0];
  m_fRangeMax = fRange[1];
  XMLHelper::Exchange_Int(pNode,"ticks",m_iTickCount,bWrite);
  XMLHelper::Exchange_Float(pNode,"value",m_fCurrentValue,bWrite);

  // slider control properties
  XMLHelper::Exchange_Bool(pNode,"vertical",m_bVertical,bWrite);

  float fRelSize = 0.f;
  XMLHelper::Exchange_Float(pSliderNode,"relsize",fRelSize,bWrite);

  SetSliderRelSize(fRelSize);

  return true;
}


void VSliderControl::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VSlider *pSlider = GetSlider();
  // update slider position and size on screen
  if (m_fSliderRelWidth>0.f)
  {
    hkvVec2 vNewSize = pSlider->GetUnscaledSize();
    if (m_bVertical)
      vNewSize.y = hkvMath::Max(GetSize().y*m_fSliderRelWidth, pSlider->GetUnscaledSize().y); 
    else
      vNewSize.x = hkvMath::Max(GetSize().x*m_fSliderRelWidth, pSlider->GetUnscaledSize().x);
    pSlider->SetSize(vNewSize.x,vNewSize.y);
  }

  // evaluate movable range pixels
  hkvVec2 vMoveRange = GetSize() - pSlider->GetSize() - m_BorderSize.m_vMin - m_BorderSize.m_vMax;
  VASSERT(vMoveRange.x>=0.f && vMoveRange.y>=0.f);

  // set position
  hkvVec2 vNewPos = m_BorderSize.m_vMin;
  float fRelPos = (m_fCurrentValue-m_fRangeMin) / (m_fRangeMax-m_fRangeMin);
  if (m_bVertical)
    vNewPos.y += vMoveRange.y*fRelPos;
  else
    vNewPos.x += vMoveRange.x*fRelPos;
  pSlider->SetPosition(vNewPos.x,vNewPos.y);

  // do the rendering
  VDlgControlBase::OnPaint(Graphics,parentState);

  VItemRenderInfo thisState(parentState,this,1.f);
  m_Frame.OnPaint(Graphics,thisState);

  pSlider->SetEnabled(IsEnabled());
  pSlider->OnPaint(Graphics,thisState);
}



VWindowBase* VSliderControl::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  if (!VDlgControlBase::TestMouseOver(user, vAbsMouse)) // outside bbox
    return NULL;

  // hit slider?
  VSlider *pSlider = GetSlider();
  if (pSlider->TestMouseOver(user, vAbsMouse))
    return pSlider;

  return this;
}


void VSliderControl::OnKeyPressed(int iKey, int iKeyModifier)
{
  VDlgControlBase::OnKeyPressed(iKey, iKeyModifier);
  float fStep = GetSingleSliderStep();
  if (fStep<=0.f)
    return;
  if (m_bVertical)
  {
    if (iKey==VGLK_UP) SetValue(GetValue()-fStep); else
    if (iKey==VGLK_DOWN) SetValue(GetValue()+fStep);
  }
  else
  {
    if (iKey==VGLK_LEFT) SetValue(GetValue()-fStep); else
    if (iKey==VGLK_RIGHT) SetValue(GetValue()+fStep);
  }
}


void VSliderControl::SetSliderRange(float fMin, float fMax, int iTicks)
{
  m_fRangeMin = fMin;
  m_fRangeMax = fMax;
  m_iTickCount = iTicks;
  SetValue(GetValue(),false); // check value again
}


void VSliderControl::SetSliderRelSize(float fVal)
{
  if (fVal>1.f) fVal=1.f;
  m_fSliderRelWidth=fVal;
  GetSlider()->SetBorderMode(m_fSliderRelWidth>0.f);
}


  
void VSliderControl::SetValue(float fValue, bool bChanging)
{
  // clamp and quantize new value
  float fMin = hkvMath::Min(m_fRangeMin,m_fRangeMax);
  float fMax = hkvMath::Max(m_fRangeMin,m_fRangeMax);
  if (fValue<fMin) fValue = fMin;
  if (fValue>fMax) fValue = fMax;
  if (m_iTickCount>1)
  {
    float fRange = fMax-fMin;
    float fTicks = (float)m_iTickCount-1.f;
    fValue -= hkvMath::mod (fValue-fMin, fRange/fTicks); // quantize
  }

  if (fValue==m_fCurrentValue) 
    return;

  m_fCurrentValue = fValue;

  // send changed event
  VItemValueChangedEvent data(this,VItemValueChangedEvent::VALUE_FLOAT,&m_fCurrentValue, bChanging);
  SendValueChangedEvent(&data);
  InvalidateCache();
}



void VSliderControl::SetSliderPos(const hkvVec2 &vRelMousePos, bool bChanging)
{
  hkvVec2 vNewPos = vRelMousePos;
  hkvVec2 vMoveRange = GetSize() - GetSlider()->GetSize() - m_BorderSize.m_vMin - m_BorderSize.m_vMax;

  float fRelValue = 0.f;

  // align
  if (m_bVertical)
  {
    vNewPos.x=0.f; 
    if (vNewPos.y<0.f) vNewPos.y = 0.f;
      else if (vNewPos.y>vMoveRange.y) vNewPos.y = vMoveRange.y;
    if (vMoveRange.y>0.f)
      fRelValue = vNewPos.y/vMoveRange.y;
  } 
  else
  {
    vNewPos.y=0.f;
    if (vNewPos.x<0.f) vNewPos.x = 0.f;
      else if (vNewPos.x>vMoveRange.x) vNewPos.x = vMoveRange.x;
    if (vMoveRange.x>0.f)
      fRelValue = vNewPos.x/vMoveRange.x;
  }
  // map to specified range and set value
  SetValue(m_fRangeMin+fRelValue*(m_fRangeMax-m_fRangeMin), bChanging);
  InvalidateCache();
}



////////////////////////////////////////////////////////////////////////////////////////////
// VSlider
////////////////////////////////////////////////////////////////////////////////////////////


void VSlider::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VWindowBase::OnPaint(Graphics,parentState);

  VItemRenderInfo thisState(parentState,this,1.f);
  m_Image.OnPaint(Graphics,thisState);
}

bool VSlider::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VWindowBase::Build(pNode,szPath,bWrite))
    return false;

  m_Image.Build(this,pNode,szPath,bWrite);

  return true;
}

void VSlider::OnBuildFinished()
{
  // use size from texture (if not defined as a slider property)
  if (m_vSize.x<=0.f)
  {
    m_vSize = m_Image.m_States[VWindowBase::NORMAL].GetSize();
    m_vUnscaledSize = m_vSize;
  }
}

V_IMPLEMENT_SERIAL( VSlider, VWindowBase, 0, &g_VisionEngineModule );
void VSlider::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VWindowBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_pSliderCtrl;
    ar >> m_Image;
    ar >> m_vUnscaledSize << m_vDragStart << m_vDragPos;
    SetParent(m_pSliderCtrl);
  } else
  {
    ar << iLocalVersion;
    ar << m_pSliderCtrl;
    ar << m_Image;
    ar << m_vUnscaledSize << m_vDragStart << m_vDragPos;
  }
}



void VSlider::OnDragging(const hkvVec2 &vMouseDelta)
{
  m_vDragPos += vMouseDelta;
  m_pSliderCtrl->SetSliderPos(m_vDragPos-m_vDragStart, true);
}

void VSlider::OnDragEnd(VWindowBase *pOver)
{
  m_pSliderCtrl->SetSliderPos(m_vDragPos-m_vDragStart, false);

  // mark the end of dragging:
  float fValue = m_pSliderCtrl->GetValue();
  VItemValueChangedEvent data(m_pSliderCtrl,VItemValueChangedEvent::VALUE_FLOAT,&fValue, false);
  m_pSliderCtrl->SendValueChangedEvent(&data);
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
