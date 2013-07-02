/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVEULERUTIL_H
#define VBASE_HKVMATH_HKVEULERUTIL_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/hkvEulerMode.h>

/// \brief
///   This is a helper class to convert Euler angles to and from matrices and quaternions.
///   It works with every mode known to man.
///   The code was taken from "Graphics Gems IV", "Euler Angle Conversion" by Ken Shoemake, page 222.
class hkvEulerUtil
{
public:

  ///
  /// @name Euler Angles to Rotation Matrices
  /// @{
  ///

  /// \brief Creates a hkvMat3 rotation matrix from Euler Angles which are given in radians.
  VBASE_IMPEXP    static hkvMat3 ConvertEulerToMat3_Rad (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  /// \brief Creates a hkvMat3 rotation matrix from Euler Angles which are given in degree.
  HKV_FORCE_INLINE static hkvMat3 ConvertEulerToMat3_Deg (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);


  /// \brief Creates a hkvMat4 rotation matrix from Euler Angles which are given in radians.
  HKV_FORCE_INLINE static hkvMat4 ConvertEulerToMat4_Rad (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  /// \brief Creates a hkvMat4 rotation matrix from Euler Angles which are given in degree.
  HKV_FORCE_INLINE static hkvMat4 ConvertEulerToMat4_Deg (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  ///
  /// @}
  ///

  ///
  /// @name Euler Angles to Quaternions
  /// @{
  ///

  /// \brief Creates a hkvQuat from Euler Angles which are given in radians.
  VBASE_IMPEXP    static hkvQuat ConvertEulerToQuaternion_Rad (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  /// \brief Creates a hkvQuat from Euler Angles which are given in degree.
  HKV_FORCE_INLINE static hkvQuat ConvertEulerToQuaternion_Deg (float fRotation1, float fRotation2, float fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  ///
  /// @}
  ///

  ///
  /// @name Rotation Matrices to Euler Angles
  /// @{
  ///

  /// \brief Extracts Euler Angles from a hkvMat3 and returns them in radians.
  VBASE_IMPEXP    static void ConvertMat3ToEuler_Rad (const hkvMat3& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  /// \brief Extracts Euler Angles from a hkvMat3 and returns them in degree.
  HKV_FORCE_INLINE static void ConvertMat3ToEuler_Deg (const hkvMat3& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  /// \brief Extracts Euler Angles from a hkvMat4 and returns them in radians.
  HKV_FORCE_INLINE static void ConvertMat4ToEuler_Rad (const hkvMat4& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  /// \brief Extracts Euler Angles from a hkvMat4 and returns them in degree.
  HKV_FORCE_INLINE static void ConvertMat4ToEuler_Deg (const hkvMat4& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  ///
  /// @}
  ///

  ///
  /// @name Quaternions to Euler Angles
  /// @{
  ///

    /// \brief Extracts Euler Angles from a hkvQuat and returns them in radians.
  VBASE_IMPEXP    static void ConvertQuaternionToEuler_Rad (const hkvQuat& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  /// \brief Extracts Euler Angles from a hkvQuat and returns them in degree.
  HKV_FORCE_INLINE static void ConvertQuaternionToEuler_Deg (const hkvQuat& m, float& out_fRotation1, float& out_fRotation2, float& out_fRotation3, hkvEulerMode::Enum EulerMode = hkvEulerMode::VisionDefault);

  ///
  /// @}
  ///
};

#include <Vision/Runtime/Base/Math/Helper/hkvEulerUtil.inl>

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
