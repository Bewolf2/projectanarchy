/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VZoomDetector.hpp>
#ifdef SUPPORTS_MULTITOUCH
VZoomDetector::VZoomDetector(const VRectanglef& validArea, float fMaxDistance) : 
  m_area(validArea), m_fMaxDistance(fMaxDistance), m_fLastValue(0.0f), m_fValue(0.0f), m_bActive(true)
{
  m_fullScreen = false;
  if (!m_area.IsValid())
  {
	m_fullScreen = true;
    m_area.Set(0.f, 0.f, (float)Vision::Video.GetXRes(), (float)Vision::Video.GetYRes());
  }
  
  if (m_fMaxDistance < 0.0f)
  {
    m_fMaxDistance = m_area.GetSizeX() * m_area.GetSizeX() + m_area.GetSizeY() * m_area.GetSizeY();
  }
    
  IVMultiTouchInput& inputDevice = static_cast<IVMultiTouchInput&>(VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN));
  VASSERT(&inputDevice != &VInputManager::s_NoInputDevice);
  m_pInputDevice = &inputDevice;
}

void VZoomDetector::Reset()
{
  m_fLastValue = 0.0f;
  m_fValue = 0.0f;
}
  
void VZoomDetector::Update(float timeDiff)
{
  if (!m_bActive)
    return;
  
  if (m_pInputDevice->GetNumberOfTouchPoints() != 2)
  {
    m_fLastValue = 0.0f;
    m_fValue = 0.0f;
    return;
  }
  
  int iTouchPoints[2];
  int index = 0;
  
  for (int i = 0; i < m_pInputDevice->GetMaximumNumberOfTouchPoints() && index < 2; i++)
  {
    if (m_pInputDevice->IsActiveTouch(i) && ( m_fullScreen || m_pInputDevice->GetTouch(i).IsInArea(m_area)))
      iTouchPoints[index++] = i;
  }
  
  if (index < 2)
    return;
  
  float dx = m_pInputDevice->GetTouch(iTouchPoints[0]).fXAbsolute - m_pInputDevice->GetTouch(iTouchPoints[1]).fXAbsolute;
  float dy = m_pInputDevice->GetTouch(iTouchPoints[0]).fYAbsolute - m_pInputDevice->GetTouch(iTouchPoints[1]).fYAbsolute;
  float fDistance = dx * dx + dy * dy;
  
  if (m_fLastValue == 0.0f)
    m_fLastValue = fDistance;
  else
    m_fLastValue = m_fValue;
  
  m_fValue = fDistance;
}
  
bool VZoomDetector::IsActive()
{
  return m_bActive;
}

int VZoomDetector::GetRawControlValue(unsigned int uiControl)
{
  switch (uiControl)
  {
    case CT_TOUCH_ZOOM:
      return (int)(m_fLastValue - m_fValue);
    case CT_TOUCH_ZOOM_IN:
      return m_fLastValue > m_fValue ? (int)(m_fLastValue - m_fValue) : 0;
    case CT_TOUCH_ZOOM_OUT:
      return m_fLastValue < m_fValue ? (int)(m_fValue - m_fLastValue) : 0;
  }
  return 0;
}

float VZoomDetector::GetControlValue(unsigned int uiControl, float fDeadZone, bool bTimeScaled)
{
  return GetRawControlValue(uiControl) / m_fMaxDistance;  
}

const char* VZoomDetector::GetName()
{
  return "Zoom Detector";
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
