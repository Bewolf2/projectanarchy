/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMAT3HELPERS_H
#define VBASE_HKVMATH_HKVMAT3HELPERS_H

#include <Vision/Runtime/Base/Math/Helper/hkvMatHelpers.h>

/// \brief
///   [internal] Helper object to implement index swizzling for matrices.
///
/// Will recompute indices from a row-major convention to a column-major convention.
/// Used by hkvMat3.
class hkvMat3_ElementSwizzle
{
public:

  /// \brief
  ///   Index operator. Will recompute the index from a row-major convention to a column-major convention.
  HKV_FORCE_INLINE hkvPlatformElementFloat operator[] (hkUint32 idx)
  {
    VASSERT (idx < 9);
    return (rows[idx / 3 + (idx % 3) * 3]);
  }

  /// \brief
  ///   Index operator. Will recompute the index from a row-major convention to a column-major convention.
  HKV_FORCE_INLINE const hkvPlatformElementFloat operator[] (hkUint32 idx) const
  {
    VASSERT (idx < 9);
    return ((float&) rows[idx / 3 + (idx % 3) * 3]);
  }

private:
  void operator& () {}
  float rows[9];
};


/// \brief
///   [internal] Helper object to implement index swizzling for matrices.
///
/// Will recompute indices from a row-major convention to a column-major convention.
/// Used by hkvMat3.
class hkvMat3_RowSwizzle
{
public:

  /// \brief 
  ///   [internal] Stores the pointer from which the next index will be computed.
  HKV_FORCE_INLINE hkvMat3_RowSwizzle (float* p) : m_pRow (p) 
  {
  }

  /// \brief
  ///   Index operator. Will recompute the index from a row-major convention to a column-major convention.
  HKV_FORCE_INLINE hkvPlatformElementFloat operator[] (hkUint32 idx)
  {
    VASSERT (idx < 3);
    return (m_pRow[3 * idx]);
  }

  /// \brief
  ///   Index operator. Will recompute the index from a row-major convention to a column-major convention.
  HKV_FORCE_INLINE const hkvPlatformElementFloat operator[] (hkUint32 idx) const
  {
    VASSERT (idx < 3);
    return (m_pRow[3 * idx]);
  }

private:
  void operator& () {}
  float* m_pRow;
};

/// \brief
///   [internal] Helper object to implement index swizzling for matrices.
///
/// Will recompute indices from a row-major convention to a column-major convention.
/// Used by hkvMat3.
class hkvMat3_ColumnSwizzle
{
public:

  /// \brief
  ///   Index operator. Will recompute the index from a row-major convention to a column-major convention.
  HKV_FORCE_INLINE hkvMat3_RowSwizzle operator[] (hkUint32 idx)
  {
    VASSERT (idx < 3);
    return hkvMat3_RowSwizzle (&m_Columns[0][idx]);
  }

  /// \brief
  ///   Index operator. Will recompute the index from a row-major convention to a column-major convention.
  HKV_FORCE_INLINE const hkvMat3_RowSwizzle operator[] (hkUint32 idx) const
  {
    VASSERT (idx < 3);
    return hkvMat3_RowSwizzle ((float*) &m_Columns[0][idx]);
  }

private:
  void operator& () {}
  float m_Columns[3][3];
};


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
