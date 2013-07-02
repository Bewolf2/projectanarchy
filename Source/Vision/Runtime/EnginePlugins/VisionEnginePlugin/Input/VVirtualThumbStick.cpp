/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

#ifdef SUPPORTS_MULTITOUCH

VVirtualThumbStick::VVirtualThumbStick(const VRectanglef& validArea, 
  float fRelativeInitialX, float fRelativeInitialY,
  const char* szCirclePath, const char* szRingPath) 
  : m_iRingWidth(0), m_iRingHeight(0)
  , m_iCircleWidth(0), m_iCircleHeight(0)
  , m_iCircleCenterX(0), m_iCircleCenterY(0)
  , m_fXValue(0.0f), m_fYValue(0.0f)
  , m_fRelativeInitialX(fRelativeInitialX), m_fRelativeInitialY(fRelativeInitialY)
  , m_spCircleMask(NULL)
  , m_spRingMask(NULL)
  , m_fTimeDiff(0.0f)
  , m_bActive(false)
  , m_spTouchArea(NULL)
  , m_validArea()
  , m_iLastTouchPointIndex(-1)
{
  if (szCirclePath == NULL)
    szCirclePath = "GUI/circle.dds"; 
  m_spCircleMask = new VisScreenMask_cl(szCirclePath);
  m_spCircleMask->SetDepthWrite(false);
  m_spCircleMask->SetTransparency(VIS_TRANSP_ALPHA);
    
  if (szRingPath == NULL)
    szRingPath = "GUI/ring.dds";
  m_spRingMask = new VisScreenMask_cl(szRingPath);
  m_spRingMask->SetDepthWrite(false);
  m_spRingMask->SetTransparency(VIS_TRANSP_ALPHA);

  // Create touch input
  SetValidArea(validArea);
  
  // Make active
  Show();
}

VVirtualThumbStick::~VVirtualThumbStick()
{
  // Deinit
  Hide();
}

void VVirtualThumbStick::SetValidArea(const VRectanglef& validArea)
{
  VRectanglef finalValidArea = validArea;
  if (!finalValidArea.IsValid())
  {
    // default area: square (width is 50% of screen height)
    const float fScreenWidth = static_cast<float>(Vision::Video.GetXRes());
    const float fScreenHeight = static_cast<float>(Vision::Video.GetYRes());
    const float fSquareWidth = hkvMath::Min(fScreenHeight, fScreenWidth) * 0.5f;

    finalValidArea.Set(
      hkvVec2(0.0f, fScreenHeight - fSquareWidth), 
      hkvVec2(fSquareWidth, fScreenHeight));
  }

  // create /resize touch area
  if (m_spTouchArea == NULL)
  {
    IVMultiTouchInput& inputDevice = static_cast<IVMultiTouchInput&>(
      VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN));
    VASSERT(&inputDevice != &VInputManager::s_NoInputDevice);

    m_spTouchArea = new VTouchArea(inputDevice, finalValidArea, -1500.0f);
  }
  else
  {
    m_spTouchArea->SetArea(finalValidArea);
  }

  m_validArea = finalValidArea;

  Reset();
}

void VVirtualThumbStick::Reset()
{
  m_fXValue = 0.0f;
  m_fYValue = 0.0f;
  m_iLastTouchPointIndex = -1;

  // (Re)position thumb stick
  const VRectanglef finalValidArea = m_spTouchArea->GetArea();

  // Outer ring mask
  const float fInitialX = finalValidArea.m_vMin.x + finalValidArea.GetSizeX() * m_fRelativeInitialX;
  const float fInitialY = finalValidArea.m_vMin.y + finalValidArea.GetSizeY() * m_fRelativeInitialY;

  m_spRingMask->GetTextureSize(m_iRingWidth, m_iRingHeight);
  m_spRingMask->SetPos(
    fInitialX - static_cast<float>(m_iRingWidth / 2), 
    fInitialY - static_cast<float>(m_iRingHeight / 2));  

  // Inner circle mask
  m_iCircleCenterX = static_cast<int>(fInitialX);
  m_iCircleCenterY = static_cast<int>(fInitialY);

  m_spCircleMask->GetTextureSize(m_iCircleWidth, m_iCircleHeight);
  m_spCircleMask->SetPos(
    fInitialX - static_cast<float>(m_iCircleWidth / 2), 
    fInitialY - static_cast<float>(m_iCircleHeight / 2));
}
  
void VVirtualThumbStick::Update(float fTimeDiff)
{
  if (!m_bActive)
    return;
  
  m_fTimeDiff = fTimeDiff;
  
  VColorRef color = V_RGBA_WHITE;
  int iTouchPointIndex = m_spTouchArea->GetTouchPointIndex();
  if (iTouchPointIndex >= 0)
  {
    const float fTouchPointX = m_spTouchArea->GetInputDevice().GetTouchPointValue(iTouchPointIndex, CT_TOUCH_ABS_X);
    const float fTouchPointY = m_spTouchArea->GetInputDevice().GetTouchPointValue(iTouchPointIndex, CT_TOUCH_ABS_Y);

    // If touch point was released before, reposition the thumb stick.
    if (m_iLastTouchPointIndex < 0) 
    {
      m_iCircleCenterX = static_cast<int>(fTouchPointX);
      m_iCircleCenterY = static_cast<int>(fTouchPointY);
      m_spRingMask->SetPos(
        static_cast<float>(m_iCircleCenterX - m_iRingWidth / 2), 
        static_cast<float>(m_iCircleCenterY - m_iRingHeight / 2));
    }
    
    m_fXValue = (fTouchPointX - m_iCircleCenterX) / (m_iRingWidth * 0.5f);
    m_fYValue = (fTouchPointY - m_iCircleCenterY) / (m_iRingHeight * 0.5f);
    
    float fLength = hkvMath::sqrt(m_fXValue * m_fXValue + m_fYValue * m_fYValue);
    if (fLength >= 1.0f)
    {
      m_fXValue /= fLength;
      m_fYValue /= fLength;
    }

    // Always place the touch area, so that it moves with the thumb. This does
    // prevent the virtual thumb stick to be released when the touch is outside the 
    // initial valid area and has not been released.
    float fHalfWidth, fHalfHeight;
    m_spRingMask->GetTargetSize(fHalfWidth, fHalfHeight);
    fHalfWidth *= 0.5f;
    fHalfHeight *= 0.5f;

    const VRectanglef rect(
      fTouchPointX - fHalfWidth, 
      fTouchPointY - fHalfHeight, 
      fTouchPointX + fHalfWidth, 
      fTouchPointY + fHalfHeight);
    m_spTouchArea->SetArea(rect);
  }
  else
  {
    m_fXValue = 0.0f;
    m_fYValue = 0.0f;
    color.SetRGBA(160, 160, 160, 160);

    // re-set to original area
    m_spTouchArea->SetArea(m_validArea);
  }
  m_iLastTouchPointIndex = iTouchPointIndex;
   
  // Set new circle position
  int iNewCircleX = m_iCircleCenterX + static_cast<int>(m_fXValue * (m_iCircleWidth * 0.5f));
  int iNewCircleY = m_iCircleCenterY + static_cast<int>(m_fYValue * (m_iCircleHeight * 0.5f));
  
  m_spCircleMask->SetPos(
    static_cast<float>(iNewCircleX - m_iCircleWidth / 2), 
    static_cast<float>(iNewCircleY - m_iCircleHeight / 2));

  m_spCircleMask->SetColor(color);
  m_spRingMask->SetColor(color);
}
  
void VVirtualThumbStick::Show(bool bShow)
{
  if (!bShow)
  {
    Hide();
    return;
  }

  if (m_bActive)
    return;

  m_spRingMask->SetVisible(TRUE);
  m_spCircleMask->SetVisible(TRUE);
  m_bActive = true;

  Vision::Callbacks.OnFrameUpdatePreRender += this;
}

void VVirtualThumbStick::Hide()
{
  if (!m_bActive)
    return;

  m_spRingMask->SetVisible(FALSE);
  m_spCircleMask->SetVisible(FALSE);
  m_bActive = false;
  m_fXValue = 0.0f;
  m_fYValue = 0.0f;

  Vision::Callbacks.OnFrameUpdatePreRender -= this;
}

bool VVirtualThumbStick::IsActive()
{
  return m_bActive;
}

int VVirtualThumbStick::GetRawControlValue(unsigned int uiControl)
{
  switch (uiControl)
  {
    case CT_PAD_LEFT_THUMB_STICK_UP:
    case CT_PAD_LEFT_THUMB_STICK_DOWN:
      return static_cast<int>(m_fYValue * 128);
      
    case CT_PAD_LEFT_THUMB_STICK_LEFT:
    case CT_PAD_LEFT_THUMB_STICK_RIGHT:
      return static_cast<int>(m_fXValue * 128);
  }
  
  return 0;
}

float VVirtualThumbStick::GetControlValue(unsigned int uiControl, float fDeadZone, bool bTimeScaled)
{
  float fValue = 0.0f;
  
  switch (uiControl)
  {
    case CT_PAD_LEFT_THUMB_STICK_UP:      
      if (m_fYValue <= 0.0f)
        fValue = -m_fYValue;
      break;
      
    case CT_PAD_LEFT_THUMB_STICK_DOWN:      
      if (m_fYValue >= 0.0f)
        fValue = m_fYValue;
      break;
      
    case CT_PAD_LEFT_THUMB_STICK_LEFT:      
      if (m_fXValue <= 0.0f)
        fValue = -m_fXValue;
      break;
      
    case CT_PAD_LEFT_THUMB_STICK_RIGHT:      
      if (m_fXValue >= 0.0f)
        fValue = m_fXValue;
      break;
      
    case CT_PAD_LEFT_THUMB_STICK_CHANGED:
      fValue = (m_fXValue != 0 || m_fYValue != 0) ? 1.0f : 0.0f;
      break;
  }
  
  if (fValue < fDeadZone)
    return 0.0f;
  
  const float fDeadZoneCorrection = 1.0f / (1.0f - fDeadZone);
  
  if (bTimeScaled)
    return (fValue - fDeadZone) * fDeadZoneCorrection * m_fTimeDiff;
  else
    return (fValue - fDeadZone) * fDeadZoneCorrection;
  
}

const char* VVirtualThumbStick::GetName()
{
  return "VirtualThumbStick";
}

void VVirtualThumbStick::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  // OnFrameUpdatePreRender
  if (pData->m_pSender == &Vision::Callbacks.OnFrameUpdatePreRender)
  {
    Update(Vision::GetTimer()->GetTimeDifference());
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
