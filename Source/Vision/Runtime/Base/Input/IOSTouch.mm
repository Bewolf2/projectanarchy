/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#include <Vision/Runtime/Base/BasePCH.h>
#import <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>

// Helper functions called by the Objective-C layer to set the internal values
void IOSMotionSetAcceleration(float fAccelerationX, float fAccelerationY, float fAccelerationZ)
{
  if (VInputManagerIOS::IsInitialized())
  {
    ((VMotionInputIOS&)VInputManagerIOS::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR)).SetAcceleration(fAccelerationX, fAccelerationY, fAccelerationZ);
  }
}

void IOSTouchAddNewTouch(void* pTouchObject, int iXAbsolute, int iYAbsolute)
{
  if (VInputManagerIOS::IsInitialized())
  {
    ((VMultiTouchInputIOS&)VInputManagerIOS::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN)).AddNewTouch(pTouchObject, iXAbsolute, iYAbsolute);
    //printf("NEW %p (%d, %d)\n", pTouchObject, iXAbsolute, iYAbsolute);
  }
}

void IOSTouchRemoveTouch(void* pTouchObject)
{
  if (VInputManagerIOS::IsInitialized())
  {
    ((VMultiTouchInputIOS&)VInputManagerIOS::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN)).RemoveTouch(pTouchObject);
  }
}

void IOSTouchUpdateTouch(void* pTouchObject, int iXAbsolute, int iYAbsolute, int iXPrevLocAbsolute, int iYPrevLocAbsolute)
{
  if (VInputManagerIOS::IsInitialized())
  {
    ((VMultiTouchInputIOS&)VInputManagerIOS::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN)).UpdateTouch(pTouchObject, iXAbsolute, iYAbsolute, iXPrevLocAbsolute, iYPrevLocAbsolute);
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
