/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVEULERUTIL_INL
#define VBASE_HKVMATH_HKVEULERUTIL_INL

#include <Vision/Runtime/Base/Math/Quaternion/hkvQuat.h>

HKV_FORCE_INLINE hkvMat3 hkvEulerUtil::ConvertEulerToMat3_Deg (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode)
{
  return ConvertEulerToMat3_Rad (hkvMath::Deg2Rad (fRotation1), hkvMath::Deg2Rad (fRotation2), hkvMath::Deg2Rad (fRotation3), EulerMode);
}

HKV_FORCE_INLINE hkvMat4 hkvEulerUtil::ConvertEulerToMat4_Rad (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode)
{
  return hkvMat4 (ConvertEulerToMat3_Rad (fRotation1, fRotation2, fRotation3, EulerMode), hkvVec3 (0.0f));
}

HKV_FORCE_INLINE hkvMat4 hkvEulerUtil::ConvertEulerToMat4_Deg (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode)
{
  return hkvMat4 (ConvertEulerToMat3_Rad (hkvMath::Deg2Rad (fRotation1), hkvMath::Deg2Rad (fRotation2), hkvMath::Deg2Rad (fRotation3), EulerMode), hkvVec3 (0.0f));
}


HKV_FORCE_INLINE void hkvEulerUtil::ConvertQuaternionToEuler_Deg (const hkvQuat& q, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode)
{
  ConvertQuaternionToEuler_Rad (q, out_fRotation1, out_fRotation2, out_fRotation3, EulerMode);

  out_fRotation1 = hkvMath::Rad2Deg (out_fRotation1);
  out_fRotation2 = hkvMath::Rad2Deg (out_fRotation2);
  out_fRotation3 = hkvMath::Rad2Deg (out_fRotation3);
}

HKV_FORCE_INLINE void hkvEulerUtil::ConvertMat3ToEuler_Deg (const hkvMat3& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode)
{
  ConvertMat3ToEuler_Rad (m, out_fRotation1, out_fRotation2, out_fRotation3, EulerMode);

  out_fRotation1 = hkvMath::Rad2Deg (out_fRotation1);
  out_fRotation2 = hkvMath::Rad2Deg (out_fRotation2);
  out_fRotation3 = hkvMath::Rad2Deg (out_fRotation3);
}

HKV_FORCE_INLINE void hkvEulerUtil::ConvertMat4ToEuler_Rad (const hkvMat4& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode)
{
  ConvertMat3ToEuler_Rad (m.getRotationalPart (), out_fRotation1, out_fRotation2, out_fRotation3, EulerMode);
}

HKV_FORCE_INLINE void hkvEulerUtil::ConvertMat4ToEuler_Deg (const hkvMat4& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode)
{
  ConvertMat3ToEuler_Rad (m.getRotationalPart (), out_fRotation1, out_fRotation2, out_fRotation3, EulerMode);

  out_fRotation1 = hkvMath::Rad2Deg (out_fRotation1);
  out_fRotation2 = hkvMath::Rad2Deg (out_fRotation2);
  out_fRotation3 = hkvMath::Rad2Deg (out_fRotation3);
}

HKV_FORCE_INLINE hkvQuat hkvEulerUtil::ConvertEulerToQuaternion_Deg (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode)
{
  return ConvertEulerToQuaternion_Rad (hkvMath::Deg2Rad (fRotation1), hkvMath::Deg2Rad (fRotation2), hkvMath::Deg2Rad (fRotation3), EulerMode);
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
