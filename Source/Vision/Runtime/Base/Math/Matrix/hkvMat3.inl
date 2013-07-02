/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATRIX3_INL
#define VBASE_HKVMATH_HKVMATRIX3_INL

#include <Vision/Runtime/Base/Math/Helper/hkvEulerUtil.h>

#define HKVMATRIX3_COPYCOLUMN(var, prefix) \
  const float prefix##_x = var[0]; const float prefix##_y = var[1]; const float prefix##_z = var[2];

HKV_FORCE_INLINE hkvMat3::hkvMat3 (float c0r0, float c1r0, float c2r0,
                                        float c0r1, float c1r1, float c2r1,
                                        float c0r2, float c1r2, float c2r2,
                                        Layout layout)
{
  set (c0r0, c1r0, c2r0,
       c0r1, c1r1, c2r1,
       c0r2, c1r2, c2r2,
       layout);
}

HKV_FORCE_INLINE void hkvMat3::setIdentity ()
{
  m_Column[0][0] = 1; m_Column[1][0] = 0; m_Column[2][0] = 0;
  m_Column[0][1] = 0; m_Column[1][1] = 1; m_Column[2][1] = 0;
  m_Column[0][2] = 0; m_Column[1][2] = 0; m_Column[2][2] = 1;
}

HKV_FORCE_INLINE void hkvMat3::setZero ()
{
  // manually unrolled loop to be on the safe side
  m_ElementsCM[0] = 0; m_ElementsCM[1] = 0; m_ElementsCM[2] = 0;
  m_ElementsCM[3] = 0; m_ElementsCM[4] = 0; m_ElementsCM[5] = 0;
  m_ElementsCM[6] = 0; m_ElementsCM[7] = 0; m_ElementsCM[8]= 0;
}

HKV_FORCE_INLINE bool hkvMat3::isZero (float fEpsilon) const
{
  // should not use a negative epsilon
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
  {
    for (hkInt32 i = 0; i < 9; ++i)
    {
      if (m_ElementsCM[i] != 0.0f)
        return false;
    }
  }
  else
  {
    // only really used in unit-tests, no need for maximum performance
    for (hkInt32 i = 0; i < 9; ++i)
    {
      if (!hkvMath::isZero (m_ElementsCM[i], fEpsilon))
        return false;
    }
  }

  return true;
}

HKV_FORCE_INLINE bool hkvMat3::isIdentity (float fEpsilon) const
{
  // should not use a negative epsilon
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
  {
    return
      (m_Column[0][0] == 1) && (m_Column[1][0] == 0) && (m_Column[2][0] == 0) &&
      (m_Column[0][1] == 0) && (m_Column[1][1] == 1) && (m_Column[2][1] == 0) &&
      (m_Column[0][2] == 0) && (m_Column[1][2] == 0) && (m_Column[2][2] == 1);
  }

  // only really used in unit-tests, no need for maximum performance
  return
    hkvMath::isFloatEqual (m_Column[0][0], 1, fEpsilon) && hkvMath::isFloatEqual (m_Column[1][0], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[2][0], 0, fEpsilon) &&
    hkvMath::isFloatEqual (m_Column[0][1], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[1][1], 1, fEpsilon) && hkvMath::isFloatEqual (m_Column[2][1], 0, fEpsilon) &&
    hkvMath::isFloatEqual (m_Column[0][2], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[1][2], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[2][2], 1, fEpsilon);
}

HKV_FORCE_INLINE void hkvMat3::set (float c0r0, float c1r0, float c2r0,
                                      float c0r1, float c1r1, float c2r1,
                                      float c0r2, float c1r2, float c2r2,
                                      Layout layout)
{
  if (layout == RowMajor)
  {
    m_Column[0][0] = c0r0; m_Column[1][0] = c1r0; m_Column[2][0] = c2r0;
    m_Column[0][1] = c0r1; m_Column[1][1] = c1r1; m_Column[2][1] = c2r1;
    m_Column[0][2] = c0r2; m_Column[1][2] = c1r2; m_Column[2][2] = c2r2;
  }
  else
  {
    m_Column[0][0] = c0r0; m_Column[0][1] = c1r0; m_Column[0][2] = c2r0;
    m_Column[1][0] = c0r1; m_Column[1][1] = c1r1; m_Column[1][2] = c2r1;
    m_Column[2][0] = c0r2; m_Column[2][1] = c1r2; m_Column[2][2] = c2r2;
  }
}

HKV_FORCE_INLINE void hkvMat3::set (const float* pData, Layout layout)
{
  VASSERT (pData != NULL);

  if (layout == ColumnMajor)
  {
    // manually unrolled loop to be on the safe side
    m_ElementsCM[ 0] = pData[ 0]; m_ElementsCM[ 1] = pData[ 1]; m_ElementsCM[ 2] = pData[ 2];
    m_ElementsCM[ 3] = pData[ 3]; m_ElementsCM[ 4] = pData[ 4]; m_ElementsCM[ 5] = pData[ 5];
    m_ElementsCM[ 6] = pData[ 6]; m_ElementsCM[ 7] = pData[ 7]; m_ElementsCM[ 8] = pData[ 8];
  }
  else
  {
    set (pData[0],  pData[1],  pData[2],
         pData[3],  pData[4],  pData[5],
         pData[6],  pData[7],  pData[8], RowMajor);
  }
}

HKV_FORCE_INLINE void hkvMat3::get (float* HKV_RESTRICT pData, Layout layout) const
{
  VASSERT (pData != NULL);

  if (layout == ColumnMajor)
  {
    // manually unrolled loop to be on the safe side
    pData[ 0] = m_ElementsCM[ 0]; pData[ 1] = m_ElementsCM[ 1]; pData[ 2] = m_ElementsCM[ 2];
    pData[ 3] = m_ElementsCM[ 3]; pData[ 4] = m_ElementsCM[ 4]; pData[ 5] = m_ElementsCM[ 5];
    pData[ 6] = m_ElementsCM[ 6]; pData[ 7] = m_ElementsCM[ 7]; pData[ 8] = m_ElementsCM[ 8];
  }
  else
  {
    for (hkUint32 i = 0; i < 3; ++i)
    {
      pData[i * 3 + 0] = m_Column[0][i];
      pData[i * 3 + 1] = m_Column[1][i];
      pData[i * 3 + 2] = m_Column[2][i];
    }
  }
}

HKV_FORCE_INLINE void hkvMat3::setColumn (hkUint32 iColumn, const hkvVec3& values)
{
  VASSERT (iColumn <= 2);

  m_Column[iColumn][0] = values[0];
  m_Column[iColumn][1] = values[1];
  m_Column[iColumn][2] = values[2];
}

HKV_FORCE_INLINE void hkvMat3::setRow (hkUint32 iRow, const hkvVec3& values)
{
  VASSERT (iRow <= 2);

  m_Column[0][iRow] = values[0];
  m_Column[1][iRow] = values[1];
  m_Column[2][iRow] = values[2];
}

HKV_FORCE_INLINE const hkvVec3 hkvMat3::getColumn (hkUint32 iColumn) const
{
  VASSERT (iColumn <= 2);

  return hkvVec3 (m_Column[iColumn][0], m_Column[iColumn][1], m_Column[iColumn][2]);
}

HKV_FORCE_INLINE const hkvVec3 hkvMat3::getRow (hkUint32 iRow) const
{
  VASSERT (iRow <= 2);

  return hkvVec3 (m_Column[0][iRow], m_Column[1][iRow], m_Column[2][iRow]);
}

HKV_FORCE_INLINE float hkvMat3::getElement (hkUint32 iNthElement, Layout layout) const
{
  VASSERT (iNthElement <= 8);

  if (layout == ColumnMajor)
    return m_ElementsCM[iNthElement];

  return m_Column[iNthElement % 3][iNthElement / 3];
}

HKV_FORCE_INLINE void hkvMat3::setElement (hkUint32 iNthElement, float f, Layout layout)
{
  VASSERT (iNthElement <= 8);

  if (layout == ColumnMajor)
    m_ElementsCM[iNthElement] = f;
  else
    m_Column[iNthElement % 3][iNthElement / 3] = f;
}

HKV_FORCE_INLINE void hkvMat3::setDiagonal (const hkvVec3& values)
{
  m_Column[0][0] = values[0];
  m_Column[1][1] = values[1];
  m_Column[2][2] = values[2];
}

HKV_FORCE_INLINE const hkvVec3 hkvMat3::getDiagonal ()
{
  return hkvVec3 (m_Column[0][0], m_Column[1][1], m_Column[2][2]);
}

HKV_FORCE_INLINE hkvVec3 hkvMat3::getScalingFactors () const
{
  hkvVec3 v;

  v.x = hkvVec3 (m_Column[0][0], m_Column[0][1], m_Column[0][2]).getLength ();
  v.y = hkvVec3 (m_Column[1][0], m_Column[1][1], m_Column[1][2]).getLength ();
  v.z = hkvVec3 (m_Column[2][0], m_Column[2][1], m_Column[2][2]).getLength ();

  return v;
}

HKV_FORCE_INLINE hkvResult hkvMat3::setScalingFactors (const hkvVec3& vXYZ, float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  hkvVec3 tx (m_Column[0][0], m_Column[0][1], m_Column[0][2]);
  hkvVec3 ty (m_Column[1][0], m_Column[1][1], m_Column[1][2]);
  hkvVec3 tz (m_Column[2][0], m_Column[2][1], m_Column[2][2]);

  if (tx.setLength (vXYZ.x, fEpsilon) == HKV_FAILURE) return HKV_FAILURE;
  if (ty.setLength (vXYZ.y, fEpsilon) == HKV_FAILURE) return HKV_FAILURE;
  if (tz.setLength (vXYZ.z, fEpsilon) == HKV_FAILURE) return HKV_FAILURE;

  m_Column[0][0] = tx.x; m_Column[0][1] = tx.y; m_Column[0][2] = tx.z;
  m_Column[1][0] = ty.x; m_Column[1][1] = ty.y; m_Column[1][2] = ty.z;
  m_Column[2][0] = tz.x; m_Column[2][1] = tz.y; m_Column[2][2] = tz.z;

  return HKV_SUCCESS;
}

HKV_FORCE_INLINE void hkvMat3::transpose ()
{
  // swap uses one temporary, this is easier for the compiler to optimize
  // than a complete temporary copy of the matrix

  hkvMath::swap (m_ElementsCM[1], m_ElementsCM[3]);
  hkvMath::swap (m_ElementsCM[2], m_ElementsCM[6]);
  hkvMath::swap (m_ElementsCM[5], m_ElementsCM[7]);
}

HKV_FORCE_INLINE const hkvMat3 hkvMat3::getTransposed () const
{
  hkvMat3 r (hkvNoInitialization);
  r.set (m_ElementsCM, RowMajor);

  // the data is stored in column-major order, so creating a matrix with the same data, but telling it that the data
  // is in row-major order will transpose the matrix
  return r;
}

HKV_FORCE_INLINE const hkvMat3 hkvMat3::getInverse () const
{
  hkvMat3 temp (*this);
  temp.invert ();
  return temp;
}

HKV_FORCE_INLINE const hkvVec3 hkvMat3::getAxis (hkUint32 iAxisXYZ) const
{
  VASSERT (iAxisXYZ < 3);

  return hkvVec3 (m_Column[iAxisXYZ][0], m_Column[iAxisXYZ][1], m_Column[iAxisXYZ][2]);
}

HKV_FORCE_INLINE void hkvMat3::setAxis (hkUint32 iAxisXYZ, const hkvVec3& v)
{
  VASSERT (iAxisXYZ < 3);

  HKVVEC3_COPYTEMPS (v);

  m_Column[iAxisXYZ][0] = v_x;
  m_Column[iAxisXYZ][1] = v_y;
  m_Column[iAxisXYZ][2] = v_z;
}

HKV_FORCE_INLINE void hkvMat3::getAxisXYZ (hkvVec3* HKV_RESTRICT out_AxisX, hkvVec3* HKV_RESTRICT out_AxisY, hkvVec3* HKV_RESTRICT out_AxisZ) const
{
  VASSERT ((out_AxisX != NULL) && (out_AxisY != NULL) && (out_AxisZ != NULL));

  *out_AxisX = getAxis (0);
  *out_AxisY = getAxis (1);
  *out_AxisZ = getAxis (2);
}

HKV_FORCE_INLINE void hkvMat3::setAxisXYZ (const hkvVec3& AxisX, const hkvVec3& AxisY, const hkvVec3& AxisZ)
{
  setAxis (0, AxisX);
  setAxis (1, AxisY);
  setAxis (2, AxisZ);
}

HKV_FORCE_INLINE void hkvMat3::setScalingMatrix (const hkvVec3& vScaleXYZ)
{
  set (vScaleXYZ.x, 0, 0, 
       0, vScaleXYZ.y, 0,
       0, 0, vScaleXYZ.z);
}

HKV_FORCE_INLINE bool hkvMat3::isIdentical (const hkvMat3& rhs) const
{
  // mostly used for unit-tests

  for (hkInt32 i = 0; i < 9; ++i)
  {
    if (m_ElementsCM[i] != rhs.m_ElementsCM[i])
      return false;
  }

  return true;
}

HKV_FORCE_INLINE bool hkvMat3::isEqual (const hkvMat3& rhs, float fEpsilon) const
{
  // should not use a negative epsilon
  VASSERT (fEpsilon >= 0.0f);

  // mostly used for unit-tests

  for (hkInt32 i = 0; i < 9; ++i)
  {
    if (!hkvMath::isFloatEqual (m_ElementsCM[i], rhs.m_ElementsCM[i], fEpsilon))
      return false;
  }

  return true;
}

HKV_FORCE_INLINE bool hkvMat3::isValid () const
{
  for (hkInt32 i = 0; i < 9; ++i)
    if (!hkvMath::isFiniteNumber (m_ElementsCM[i]))
      return false;

  return true;
}

HKV_FORCE_INLINE void hkvMat3::operator+= (const hkvMat3& rhs)
{
  // partially unrolled loop
  for (hkInt32 i = 0; i < 3; ++i)
  {
    HKVMATRIX3_COPYCOLUMN (rhs.m_Column[i], rhs);

    m_Column[i][0] += rhs_x;
    m_Column[i][1] += rhs_y;
    m_Column[i][2] += rhs_z;
  }
}

HKV_FORCE_INLINE void hkvMat3::operator-= (const hkvMat3& rhs)
{
  // partially unrolled loop
  for (hkInt32 i = 0; i < 3; ++i)
  {
    HKVMATRIX3_COPYCOLUMN (rhs.m_Column[i], rhs);

    m_Column[i][0] -= rhs_x;
    m_Column[i][1] -= rhs_y;
    m_Column[i][2] -= rhs_z;
  }
}

HKV_FORCE_INLINE void hkvMat3::operator*= (float f)
{
  for (hkInt32 i = 0; i < 9; ++i)
    m_ElementsCM[i] *= f;
}

HKV_FORCE_INLINE void hkvMat3::operator/= (float f0)
{
  const float f = 1.0f / f0;

  operator*= (f);
}

HKV_FORCE_INLINE const hkvVec3 hkvMat3::transformDirection (const hkvVec3& v) const
{
  HKVVEC3_COPYTEMPS (v);

  const float rx = m_Column[0][0] * v_x + m_Column[1][0] * v_y + m_Column[2][0] * v_z;
  const float ry = m_Column[0][1] * v_x + m_Column[1][1] * v_y + m_Column[2][1] * v_z;
  const float rz = m_Column[0][2] * v_x + m_Column[1][2] * v_y + m_Column[2][2] * v_z;

  return hkvVec3 (rx, ry, rz);
}

HKV_FORCE_INLINE const hkvMat3 hkvMat3::multiplyReverse (const hkvMat3& rhs) const
{
  return rhs.multiply (*this);
}

HKV_FORCE_INLINE hkvResult hkvMat3::normalize (float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  hkvVec3 vx, vy, vz;
  getAxisXYZ (&vx, &vy, &vz);

  if (vx.normalizeIfNotZero (fEpsilon) == HKV_FAILURE) return HKV_FAILURE;
  if (vy.normalizeIfNotZero (fEpsilon) == HKV_FAILURE) return HKV_FAILURE;
  if (vz.normalizeIfNotZero (fEpsilon) == HKV_FAILURE) return HKV_FAILURE;

  // only modify the matrix, if the normalization was a success
  setAxisXYZ (vx, vy, vz);

  return HKV_SUCCESS;
}

HKV_FORCE_INLINE bool operator== (const hkvMat3& lhs, const hkvMat3& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvMat3& lhs, const hkvMat3& rhs)
{
  return !lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE const hkvMat3 operator+ (const hkvMat3& lhs, const hkvMat3& rhs)
{
  hkvMat3 temp (hkvNoInitialization);

  // partially unrolled loop, looks stupid but gives the compiler more room for optimization
  for (hkInt32 i = 0; i < 3; ++i)
  {
    HKVMATRIX3_COPYCOLUMN (lhs.m_Column[i], lhs);
    HKVMATRIX3_COPYCOLUMN (rhs.m_Column[i], rhs);
    const float rx = lhs_x + rhs_x;
    const float ry = lhs_y + rhs_y;
    const float rz = lhs_z + rhs_z;

    temp.m_Column[i][0] = rx;
    temp.m_Column[i][1] = ry;
    temp.m_Column[i][2] = rz;
  }
  
  return (temp);
}

HKV_FORCE_INLINE const hkvMat3 operator- (const hkvMat3& lhs, const hkvMat3& rhs)
{
  hkvMat3 temp (hkvNoInitialization);

  // partially unrolled loop, looks stupid but gives the compiler more room for optimization
  for (hkInt32 i = 0; i < 3; ++i)
  {
    HKVMATRIX3_COPYCOLUMN (lhs.m_Column[i], lhs);
    HKVMATRIX3_COPYCOLUMN (rhs.m_Column[i], rhs);
    const float rx = lhs_x - rhs_x;
    const float ry = lhs_y - rhs_y;
    const float rz = lhs_z - rhs_z;

    temp.m_Column[i][0] = rx;
    temp.m_Column[i][1] = ry;
    temp.m_Column[i][2] = rz;
  }

  return (temp);
}

HKV_FORCE_INLINE const hkvMat3 operator* (const hkvMat3& lhs, float f)
{
  hkvMat3 temp (hkvNoInitialization);

  // partially unrolled loop, looks stupid but gives the compiler more room for optimization
  for (hkInt32 i = 0; i < 3; ++i)
  {
    HKVMATRIX3_COPYCOLUMN (lhs.m_Column[i], lhs);

    const float rx = lhs_x * f;
    const float ry = lhs_y * f;
    const float rz = lhs_z * f;

    temp.m_Column[i][0] = rx;
    temp.m_Column[i][1] = ry;
    temp.m_Column[i][2] = rz;
  }

  return (temp);
}

HKV_FORCE_INLINE const hkvMat3 operator* (float f, const hkvMat3& rhs)
{
  // same as "rhs * f"
  return operator* (rhs, f);
}

HKV_FORCE_INLINE const hkvMat3 operator/ (const hkvMat3& lhs, float f0)
{
  const float f = 1.0f / f0;

  // same as "rhs * f"
  return operator* (lhs, f);

}

HKV_FORCE_INLINE const hkvVec3 operator* (const hkvMat3& lhs, const hkvVec3& rhs)
{
  return lhs.transformDirection (rhs);
}

#ifdef HKVMATH_ENABLE_NEW_OPERATORS
  HKV_FORCE_INLINE const hkvMat3 operator* (const hkvMat3& lhs, const hkvMat3& rhs)
  {
    return lhs.multiply (rhs);
  }
#endif

HKV_FORCE_INLINE void hkvMat3::setFromEulerAngles (float fRoll, float fPitch, float fYaw)
{
  *this = hkvEulerUtil::ConvertEulerToMat3_Deg (fRoll, fPitch, fYaw, hkvEulerMode::VisionDefault);
}

HKV_FORCE_INLINE void hkvMat3::getAsEulerAngles (float& out_fRoll, float& out_fPitch, float& out_fYaw) const
{
  hkvEulerUtil::ConvertMat3ToEuler_Deg (*this, out_fRoll, out_fPitch, out_fYaw, hkvEulerMode::VisionDefault);
}

HKV_FORCE_INLINE void hkvMat3::setFromQuaternion (const hkvQuat& q)
{
  *this = q.getAsMat3 ();
}

HKV_FORCE_INLINE const hkvQuat hkvMat3::getAsQuaternion () const
{
  hkvQuat q (hkvNoInitialization);
  q.setFromMat3 (*this);
  return q;
}

HKV_FORCE_INLINE const hkvMat3_AsArray hkvMat3::getAsArray (Layout layout) const
{
  hkvMat3_AsArray r;
  get (r.data, layout);
  return r;
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
