/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Common/VSampleTouchMenu.hpp>

VSampleTouchButton::VSampleTouchButton(VSampleTouchMenu *pFather, float fRelPosX, float fRelPosY, float fWidth, float fHeight, int IControlIndex, bool bSingleHit)
{
    m_pFather = pFather;
    m_fRelPosX = fRelPosX;
    m_fRelPosY = fRelPosY;
    m_fWidth = fWidth;
    m_fHeight = fHeight;
    m_iControlIndex = IControlIndex;
    m_iAlternative = -1;
    m_spTouchArea = NULL;
    m_bSingleHit = bSingleHit;
}

void VSampleTouchButton::RecalculatePosition(VInputMap& InputMap)
{
    m_fPosX = m_pFather->GetPosX() + m_fRelPosX;
    m_fPosY = m_pFather->GetPosY() + m_fRelPosY;
    
    if(m_spActiveScreenMask)
    {
        m_spActiveScreenMask->SetPos(m_fPosX,m_fPosY);
    }
          
    m_spTouchArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(m_fPosX, m_fPosY, m_fPosX + m_fWidth, m_fPosY + m_fHeight), 1000.0f);
 
    if(m_bSingleHit)
      m_iAlternative = InputMap.MapTrigger(m_iControlIndex, m_spTouchArea, CT_TOUCH_ANY, VInputOptions::OncePerFrame(ONCE_ON_PRESS, m_iAlternative));
    else
      m_iAlternative = InputMap.MapTrigger(m_iControlIndex, m_spTouchArea, CT_TOUCH_ANY, VInputOptions::Alternative(m_iAlternative, false));
}

VSimpleSampleTouchButton::VSimpleSampleTouchButton(VSampleTouchMenu* pFather, float fRelPosX, float fRelPosY, const char* szIconPath, int iControlIndex, bool bSingleHit) :
VSampleTouchButton(pFather,fRelPosX,fRelPosY,0.0f,0.0f,iControlIndex, bSingleHit),
m_fGrowSpeed(20.0f),
m_fActiveSize(0.0f),
m_fTimeSinceLastToggle(100.0f)
{
    m_spIcon = new VisScreenMask_cl(szIconPath);
    m_spIcon->SetTransparency(VIS_TRANSP_ALPHA);
    m_spActiveScreenMask = m_spIcon;
    m_spIcon->GetTargetSize(m_fWidth,m_fHeight);
}

void VSimpleSampleTouchButton::Update(bool bTriggered)
{
  if(m_bSingleHit)
  {
    m_fTimeSinceLastToggle += Vision::GetTimer()->GetTimeDifference();
    if(bTriggered)
      m_fTimeSinceLastToggle = 0.0f;
    if(m_fTimeSinceLastToggle < 0.2f)
    {
      m_fActiveSize += Vision::GetTimer()->GetTimeDifference() * m_fGrowSpeed;
      if(m_fActiveSize > 5.0f)
        m_fActiveSize = 5.0f;
    }
    else
    {
      m_fActiveSize -= Vision::GetTimer()->GetTimeDifference() * m_fGrowSpeed;
      if(m_fActiveSize < 0.0f)
        m_fActiveSize = 0.0f;
    }
  }
  else 
  {
    if(bTriggered)
    {
      m_fActiveSize += Vision::GetTimer()->GetTimeDifference() * m_fGrowSpeed;
      if(m_fActiveSize > 5.0f)
        m_fActiveSize = 5.0f;
    }
    else 
    {
      m_fActiveSize -= Vision::GetTimer()->GetTimeDifference() * m_fGrowSpeed;
      if(m_fActiveSize < 0.0f)
        m_fActiveSize = 0.0f;
    }
  }
  
  m_spActiveScreenMask->SetPos(m_fPosX - m_fActiveSize, m_fPosY - m_fActiveSize);
  m_spActiveScreenMask->SetTargetSize(m_fWidth + m_fActiveSize * 2.0f,
                                      m_fHeight + m_fActiveSize * 2.0f);
}

VToggleSampleTouchButton::VToggleSampleTouchButton(VSampleTouchMenu* pFather, float fRelPosX, float fRelPosY, const char* szIconPathOn, const char* szIconPathOff, int iControlIndex) :
VSimpleSampleTouchButton(pFather,fRelPosX,fRelPosY,szIconPathOn, iControlIndex,true),
m_bLastTriggerState(false),
m_bState(true)
{
    m_spIconOff = new VisScreenMask_cl(szIconPathOff);
    m_spIconOff->SetTransparency(VIS_TRANSP_ALPHA);
    m_spIconOff->SetVisible(FALSE);
}


void VToggleSampleTouchButton::Update(bool bTriggered)
{
  bool bIsOn = false;
  if(bTriggered != m_bLastTriggerState)
  {
    m_bLastTriggerState = bTriggered;
    if(bTriggered == true){
      bIsOn = true;
      if(m_bState)
      {
        m_spIcon->SetVisible(FALSE);
        m_spIconOff->SetVisible(TRUE);
        m_spActiveScreenMask = m_spIconOff;
      }
      else
      {
        m_spIcon->SetVisible(TRUE);
        m_spIconOff->SetVisible(FALSE);
        m_spActiveScreenMask = m_spIcon;
      }
      m_bState = !m_bState;
    }
  }
  VSimpleSampleTouchButton::Update(bIsOn);
}

void VToggleSampleTouchButton::SetState(bool bState)
{
  if(!bState)
  {
    m_spIcon->SetVisible(FALSE);
    m_spIconOff->SetVisible(TRUE);
    m_spActiveScreenMask = m_spIconOff;
  }
  else
  {
    m_spIcon->SetVisible(TRUE);
    m_spIconOff->SetVisible(FALSE);
    m_spActiveScreenMask = m_spIcon;
  }  
  m_bState = bState;
}

VSampleTouchMenu::VSampleTouchMenu(VInputMap& InputMap, VOrientation orientation) :
m_InputMap(InputMap),
m_Orientation(orientation),
m_pShareSpaceWith(NULL),
m_eHorizontalOrientation(VHO_LEFT)
{
  Vision::Callbacks.OnUpdateSceneBegin += this;
}

VSampleTouchMenu::~VSampleTouchMenu()
{
    Vision::Callbacks.OnUpdateSceneBegin -= this;
    for(int i=0;i<m_Children.GetSize();i++)
    {
      m_InputMap.SetMapping(m_Children[i]->m_iControlIndex, m_Children[i]->m_iAlternative, NULL);
      V_SAFE_DELETE(m_Children[i]);
    }
}

void VSampleTouchMenu::RecalculatePosition()
{
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    float fOffset = 0.0f;
    
    GetSize(fWidth,fHeight);
  
    if(m_pShareSpaceWith != NULL && m_pShareSpaceWith->m_Orientation == m_Orientation)
    {
      VASSERT(m_pShareSpaceWith->m_pShareSpaceWith == this && m_pShareSpaceWith->m_eHorizontalOrientation != m_eHorizontalOrientation);
      float fWidthOther = 0.0f;
      float fHeightOther = 0.0f;
      m_pShareSpaceWith->GetSize(fWidthOther,fHeightOther);
      if(fHeightOther > fHeight)
        fHeight = fHeightOther;
    
      fWidth += fWidthOther;
      if(m_eHorizontalOrientation == VHO_RIGHT)
        fOffset = fWidthOther;
    }
    
    switch(m_Orientation)
    {
        case VO_TOP:
            m_fPosX = (VVideo::GetVideoConfig()->ViewWidth - fWidth) / 2.f + fOffset;
            m_fPosY = 10;
            break;
        case VO_BOTTOM:
            m_fPosX = (VVideo::GetVideoConfig()->ViewWidth - fWidth) / 2.f + fOffset;
            m_fPosY = VVideo::GetVideoConfig()->ViewHeight - fHeight - 10.0f;
            break;
    }
    
    for(int i=0;i<m_Children.GetSize();i++)
        m_Children[i]->RecalculatePosition(m_InputMap);
}

VSimpleSampleTouchButton* VSampleTouchMenu::AddSimpleButton(const char* szIconPath,int iIndex, bool bSingleHit)
{
    float fWidth = 0;
    
    for(int i=0;i<m_Children.GetSize();i++)
        fWidth += m_Children[i]->m_fWidth + 10.0f;
    
    VSimpleSampleTouchButton* pButton = new VSimpleSampleTouchButton(this,fWidth,0.0f,szIconPath,iIndex,bSingleHit);
    m_Children.Append(pButton);
    
    return pButton;
}

VToggleSampleTouchButton* VSampleTouchMenu::AddToggleButton(const char* szIconPathOn, const char* szIconPathOff, int iIndex)
{
    float fWidth = 0;
    for(int i=0;i<m_Children.GetSize();i++)
        fWidth += m_Children[i]->m_fWidth + 10.0f;
    
    VToggleSampleTouchButton* pButton = new VToggleSampleTouchButton(this,fWidth,0.0f,szIconPathOn,szIconPathOff,iIndex);
    m_Children.Append(pButton);
    
    return pButton;
}

void VSampleTouchMenu::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
    for(int i=0;i<m_Children.GetSize();i++)
    {
      m_Children[i]->Update(m_InputMap.GetTrigger(m_Children[i]->m_iControlIndex) > 0.0f);
    }
}

void VSampleTouchMenu::GetSize(float& fWidth, float& fHeight)
{
  for(int i=0;i<m_Children.GetSize();i++)
  {
    fWidth += m_Children[i]->m_fWidth + 10.0f;
    if(fHeight < m_Children[i]->m_fHeight)
      fHeight = m_Children[i]->m_fHeight;
  }  
}

void VSampleTouchMenu::SetShareSpaceWith(VSampleTouchMenu *pShareSpaceWith, VHorizontalOrientation eHorizontalOrientation)
{
  if(m_pShareSpaceWith != NULL)
  {
    m_pShareSpaceWith->m_pShareSpaceWith = NULL;
  }
  m_pShareSpaceWith = pShareSpaceWith;
  m_eHorizontalOrientation = eHorizontalOrientation;
  if(m_pShareSpaceWith != NULL)
  {
    m_pShareSpaceWith->m_pShareSpaceWith = this;
    if(m_eHorizontalOrientation == VHO_LEFT)
      m_pShareSpaceWith->m_eHorizontalOrientation = VHO_RIGHT;
    else
      m_pShareSpaceWith->m_eHorizontalOrientation = VHO_LEFT;
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
