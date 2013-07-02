/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATHELPERS_H
#define VBASE_HKVMATH_HKVMATHELPERS_H

/// \brief
///   This struct is returned if a pointer access is desired but not allowed, because the data layout has changed.
///
/// If the compiler complains about this class, it means that you are trying to get the pointer to some data,
/// which was linear in memory in previous versions of the Vision Engine, but not anymore in this version.
/// As such, it is not possible to take that pointer and operate on the data, as before.
struct ThisDataIsNonLinear_PointerAccessDenied {};


/// \brief
///   [internal] A helper class that is used by hkvMat3 and hkvMat4 to prevent people from trying to get
///   a pointer to the internal data of those classes.
///
/// This was allowed in previous versions of the Vision Engine, but since the layout of the matrix classes
/// has changed from row-major to column-major, it is not possible to access that data in the same way as before.
/// Therefore this class will return the struct 'ThisDataIsNonLinear_PointerAccessDenied' whenever such an
/// access is detected, to ease porting code that uses the old classes to the new code.
///
/// For performance reasons hkvMat_ElementFloat is only used on windows in debug builds, it is not enabled on any
/// console build or in release builds (as it is a debugging only feature).
class hkvMat_ElementFloat
{
public:
  /// \brief
  ///   Takes a float reference and stores it for later modification.
  HKV_FORCE_INLINE hkvMat_ElementFloat (float& f) : m_Float (f) {}

  /// \brief
  ///   Copy Constructor
  HKV_FORCE_INLINE hkvMat_ElementFloat (const hkvMat_ElementFloat& rhs) : m_Float ((float&) rhs.m_Float) {}

  /// \brief
  ///   Conversion operator: Allows read-access.
  HKV_FORCE_INLINE operator float () const { return m_Float; } // read-access

  /// \brief
  ///   Conversion operator: Allows write-access.
  HKV_FORCE_INLINE operator float& () { return m_Float; } // write-access

  /// \brief
  ///   Allows assigning values to the float.
  HKV_FORCE_INLINE const hkvMat_ElementFloat& operator= (float rhs) { m_Float = rhs; return *this; } // write-access

  /// \brief
  ///   Reference operator: Returns a helper struct that describes that this operation is not allowed.
  HKV_FORCE_INLINE ThisDataIsNonLinear_PointerAccessDenied operator& () const {return ThisDataIsNonLinear_PointerAccessDenied (); } // prevent taking the pointer of the float

  /// \brief
  ///   Additional function to get the value of the float.
  HKV_FORCE_INLINE float getFloat () const { return m_Float; }

private:
  hkvMat_ElementFloat ();
  float& m_Float;
};

#if defined (WIN32) && defined (HK_DEBUG_SLOW)
  #define hkvPlatformElementFloat hkvMat_ElementFloat
#else
  #define hkvPlatformElementFloat float&
#endif


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
