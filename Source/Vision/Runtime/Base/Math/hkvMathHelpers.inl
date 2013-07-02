/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATHELPERS_INL
#define VBASE_HKVMATH_HKVMATHELPERS_INL

#include <Vision/Runtime/Base/Math/hkvMathHelpers.h>

#include <Vision/Runtime/Base/Math/hkvMath.h>

float hkvMathHelpers::getFovX(float fFovY, float fAspectRatio)
{
  float fFovX = 2.0f * hkvMath::atanDeg(hkvMath::tanDeg(0.5f * fFovY) * fAspectRatio);

  VASSERT(fFovX > 0 && fFovX < 180.0);

  return fFovX;
}

float hkvMathHelpers::getFovY(float fFovX, float fAspectRatio)
{
  float fFovY = 2.0f * hkvMath::atanDeg(hkvMath::tanDeg(0.5f * fFovX) / fAspectRatio);

  VASSERT(fFovY > 0 && fFovY < 180.0);

  return fFovY;
}

void hkvMathHelpers::adjustFovsForAspectRatio(float& fFovX, float& fFovY, float fDisplayAspectRatio)
{
  VASSERT(fFovX != 0 || fFovY != 0);
  if(fFovX == 0)
  {
    fFovX = getFovX(fFovY, fDisplayAspectRatio);
  }
  else if(fFovY == 0)
  {
    fFovY = getFovY(fFovX, fDisplayAspectRatio);
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
