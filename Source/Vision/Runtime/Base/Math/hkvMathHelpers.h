/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATHHELPERS_H
#define VBASE_HKVMATH_HKVMATHHELPERS_H

/// \brief Class containing various helper functions.
class hkvMathHelpers
{
public:
  /// \brief Computes the horizontal view angle from the given vertical view angle and aspect ratio.
  HKV_FORCE_INLINE static float getFovX(float fFovY, float fAspectRatio);

  /// \brief Computes the vertical view angle from the given horizontal view angle and aspect ratio.
  HKV_FORCE_INLINE static float getFovY(float fFovX, float fAspectRatio);

  /// \brief If either of the supplied view angles is zero, it is replaced by a view angle matching the given aspect ratio.
  HKV_FORCE_INLINE static void adjustFovsForAspectRatio(float& fFovX, float& fFovY, float fDisplayAspectRatio);
};

#include <Vision/Runtime/Base/Math/hkvMathHelpers.inl>

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
