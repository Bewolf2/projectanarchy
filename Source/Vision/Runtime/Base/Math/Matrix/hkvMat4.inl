/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATRIX4_INL
#define VBASE_HKVMATH_HKVMATRIX4_INL

#include <Vision/Runtime/Base/Math/Helper/hkvEulerUtil.h>

#define VMATRIX_COPYCOLUMN(var, prefix) \
  const float prefix##_x = var[0]; const float prefix##_y = var[1]; const float prefix##_z = var[2]; const float prefix##_w = var[3];


HKV_FORCE_INLINE hkvMat4::hkvMat4 (float c0r0, float c1r0, float c2r0, float c3r0,
                                        float c0r1, float c1r1, float c2r1, float c3r1,
                                        float c0r2, float c1r2, float c2r2, float c3r2,
                                        float c0r3, float c1r3, float c2r3, float c3r3, 
                                        Layout layout)
{
  set (c0r0, c1r0, c2r0, c3r0,
       c0r1, c1r1, c2r1, c3r1,
       c0r2, c1r2, c2r2, c3r2,
       c0r3, c1r3, c2r3, c3r3,
       layout);
}

hkvMat4::hkvMat4 (const hkvMat3& m3x3, const hkvVec3& vTranslation)
{
  set (m3x3, vTranslation);
}

HKV_FORCE_INLINE void hkvMat4::setIdentity ()
{
  m_Column[0][0] = 1; m_Column[1][0] = 0; m_Column[2][0] = 0; m_Column[3][0] = 0;
  m_Column[0][1] = 0; m_Column[1][1] = 1; m_Column[2][1] = 0; m_Column[3][1] = 0;
  m_Column[0][2] = 0; m_Column[1][2] = 0; m_Column[2][2] = 1; m_Column[3][2] = 0;
  m_Column[0][3] = 0; m_Column[1][3] = 0; m_Column[2][3] = 0; m_Column[3][3] = 1;
}

HKV_FORCE_INLINE void hkvMat4::setZero ()
{
  // manually unrolled loop to be on the safe side
  m_ElementsCM[0] = 0; m_ElementsCM[1] = 0; m_ElementsCM[2] = 0; m_ElementsCM[3] = 0;
  m_ElementsCM[4] = 0; m_ElementsCM[5] = 0; m_ElementsCM[6] = 0; m_ElementsCM[7] = 0;
  m_ElementsCM[8] = 0; m_ElementsCM[9] = 0; m_ElementsCM[10]= 0; m_ElementsCM[11]= 0;
  m_ElementsCM[12]= 0; m_ElementsCM[13]= 0; m_ElementsCM[14]= 0; m_ElementsCM[15]= 0;
}

HKV_FORCE_INLINE bool hkvMat4::isZero (float fEpsilon) const
{
  // should not use a negative epsilon
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
  {
    for (hkUint32 i = 0; i < 16; ++i)
    {
      if (m_ElementsCM[i] != 0.0f)
        return false;
    }
  }
  else
  {
    // only really used in unit-tests, no need for maximum performance
    for (hkUint32 i = 0; i < 16; ++i)
    {
      if (!hkvMath::isZero (m_ElementsCM[i], fEpsilon))
        return false;
    }
  }

  return true;
}

HKV_FORCE_INLINE bool hkvMat4::isIdentity (float fEpsilon) const
{
  // should not use a negative epsilon
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
  {
    return
      (m_Column[0][0] == 1) && (m_Column[1][0] == 0) && (m_Column[2][0] == 0) && (m_Column[3][0] == 0) &&
      (m_Column[0][1] == 0) && (m_Column[1][1] == 1) && (m_Column[2][1] == 0) && (m_Column[3][1] == 0) &&
      (m_Column[0][2] == 0) && (m_Column[1][2] == 0) && (m_Column[2][2] == 1) && (m_Column[3][2] == 0) &&
      (m_Column[0][3] == 0) && (m_Column[1][3] == 0) && (m_Column[2][3] == 0) && (m_Column[3][3] == 1);
  }

  // only really used in unit-tests, no need for maximum performance
  return
    hkvMath::isFloatEqual (m_Column[0][0], 1, fEpsilon) && hkvMath::isFloatEqual (m_Column[1][0], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[2][0], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[3][0], 0, fEpsilon) &&
    hkvMath::isFloatEqual (m_Column[0][1], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[1][1], 1, fEpsilon) && hkvMath::isFloatEqual (m_Column[2][1], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[3][1], 0, fEpsilon) &&
    hkvMath::isFloatEqual (m_Column[0][2], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[1][2], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[2][2], 1, fEpsilon) && hkvMath::isFloatEqual (m_Column[3][2], 0, fEpsilon) &&
    hkvMath::isFloatEqual (m_Column[0][3], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[1][3], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[2][3], 0, fEpsilon) && hkvMath::isFloatEqual (m_Column[3][3], 1, fEpsilon);
}

HKV_FORCE_INLINE void hkvMat4::set (float c0r0, float c1r0, float c2r0, float c3r0,
                                      float c0r1, float c1r1, float c2r1, float c3r1,
                                      float c0r2, float c1r2, float c2r2, float c3r2,
                                      float c0r3, float c1r3, float c2r3, float c3r3, 
                                      Layout layout)
{
  if (layout == RowMajor)
  {
    m_Column[0][0] = c0r0; m_Column[1][0] = c1r0; m_Column[2][0] = c2r0; m_Column[3][0] = c3r0;
    m_Column[0][1] = c0r1; m_Column[1][1] = c1r1; m_Column[2][1] = c2r1; m_Column[3][1] = c3r1;
    m_Column[0][2] = c0r2; m_Column[1][2] = c1r2; m_Column[2][2] = c2r2; m_Column[3][2] = c3r2;
    m_Column[0][3] = c0r3; m_Column[1][3] = c1r3; m_Column[2][3] = c2r3; m_Column[3][3] = c3r3;
  }
  else
  {
    m_Column[0][0] = c0r0; m_Column[0][1] = c1r0; m_Column[0][2] = c2r0; m_Column[0][3] = c3r0;
    m_Column[1][0] = c0r1; m_Column[1][1] = c1r1; m_Column[1][2] = c2r1; m_Column[1][3] = c3r1;
    m_Column[2][0] = c0r2; m_Column[2][1] = c1r2; m_Column[2][2] = c2r2; m_Column[2][3] = c3r2;
    m_Column[3][0] = c0r3; m_Column[3][1] = c1r3; m_Column[3][2] = c2r3; m_Column[3][3] = c3r3;
  }
}

HKV_FORCE_INLINE void hkvMat4::set (const float* pData, Layout layout)
{
  VASSERT (pData != NULL);

  if (layout == ColumnMajor)
  {
    // manually unrolled loop to be on the safe side
    m_ElementsCM[ 0] = pData[ 0]; m_ElementsCM[ 1] = pData[ 1]; m_ElementsCM[ 2] = pData[ 2]; m_ElementsCM[ 3] = pData[ 3];
    m_ElementsCM[ 4] = pData[ 4]; m_ElementsCM[ 5] = pData[ 5]; m_ElementsCM[ 6] = pData[ 6]; m_ElementsCM[ 7] = pData[ 7];
    m_ElementsCM[ 8] = pData[ 8]; m_ElementsCM[ 9] = pData[ 9]; m_ElementsCM[10] = pData[10]; m_ElementsCM[11] = pData[11];
    m_ElementsCM[12] = pData[12]; m_ElementsCM[13] = pData[13]; m_ElementsCM[14] = pData[14]; m_ElementsCM[15] = pData[15];
  }
  else
  {
    set (pData[0],  pData[1],  pData[2],  pData[3],
         pData[4],  pData[5],  pData[6],  pData[7],
         pData[8],  pData[9],  pData[10], pData[11],
         pData[12],  pData[13],  pData[14], pData[15], RowMajor);
  }
}

HKV_FORCE_INLINE void hkvMat4::set (const hkvMat3& m3x3, const hkvVec3& vTranslation)
{
  setRotationalPart (m3x3);
  setTranslation (vTranslation);
  setRow (3, hkvVec4 (0, 0, 0, 1));
}

HKV_FORCE_INLINE void hkvMat4::get (float* HKV_RESTRICT pData, Layout layout) const
{
  VASSERT (pData != NULL);

  if (layout == ColumnMajor)
  {
    // manually unrolled loop to be on the safe side
    pData[ 0] = m_ElementsCM[ 0]; pData[ 1] = m_ElementsCM[ 1]; pData[ 2] = m_ElementsCM[ 2]; pData[ 3] = m_ElementsCM[ 3];
    pData[ 4] = m_ElementsCM[ 4]; pData[ 5] = m_ElementsCM[ 5]; pData[ 6] = m_ElementsCM[ 6]; pData[ 7] = m_ElementsCM[ 7];
    pData[ 8] = m_ElementsCM[ 8]; pData[ 9] = m_ElementsCM[ 9]; pData[10] = m_ElementsCM[10]; pData[11] = m_ElementsCM[11];
    pData[12] = m_ElementsCM[12]; pData[13] = m_ElementsCM[13]; pData[14] = m_ElementsCM[14]; pData[15] = m_ElementsCM[15];
  }
  else
  {
    for (hkUint32 i = 0; i < 4; ++i)
    {
      pData[i * 4 + 0] = m_Column[0][i];
      pData[i * 4 + 1] = m_Column[1][i];
      pData[i * 4 + 2] = m_Column[2][i];
      pData[i * 4 + 3] = m_Column[3][i];
    }
  }
}

HKV_FORCE_INLINE void hkvMat4::setColumn (hkUint32 iColumn, const hkvVec4& values)
{
  VASSERT (iColumn <= 3);

  m_Column[iColumn][0] = values[0];
  m_Column[iColumn][1] = values[1];
  m_Column[iColumn][2] = values[2];
  m_Column[iColumn][3] = values[3];
}

HKV_FORCE_INLINE void hkvMat4::setRow (hkUint32 iRow, const hkvVec4& values)
{
  VASSERT (iRow <= 3);

  m_Column[0][iRow] = values[0];
  m_Column[1][iRow] = values[1];
  m_Column[2][iRow] = values[2];
  m_Column[3][iRow] = values[3];
}

HKV_FORCE_INLINE const hkvVec4 hkvMat4::getColumn (hkUint32 iColumn) const
{
  VASSERT (iColumn <= 3);

  return hkvVec4 (m_Column[iColumn][0], m_Column[iColumn][1], m_Column[iColumn][2], m_Column[iColumn][3]);
}

HKV_FORCE_INLINE const hkvVec4 hkvMat4::getRow (hkUint32 iRow) const
{
  VASSERT (iRow <= 3);

  return hkvVec4 (m_Column[0][iRow], m_Column[1][iRow], m_Column[2][iRow], m_Column[3][iRow]);
}

HKV_FORCE_INLINE float hkvMat4::getElement (hkUint32 iNthElement, Layout layout) const
{
  VASSERT (iNthElement <= 15);

  if (layout == ColumnMajor)
    return m_ElementsCM[iNthElement];

  return m_Column[iNthElement % 4][iNthElement / 4]; // since iNthElement is an unsigned int this should be automatically converted to (>> 3, & 3)
}

HKV_FORCE_INLINE void hkvMat4::setElement (hkUint32 iNthElement, float f, Layout layout)
{
  VASSERT (iNthElement <= 15);

  if (layout == ColumnMajor)
    m_ElementsCM[iNthElement] = f;
  else
    m_Column[iNthElement % 4][iNthElement / 4] = f; // since iNthElement is an unsigned int this should be automatically converted to (>> 3, & 3)
}

HKV_FORCE_INLINE void hkvMat4::setDiagonal (const hkvVec4& values)
{
  m_Column[0][0] = values[0];
  m_Column[1][1] = values[1];
  m_Column[2][2] = values[2];
  m_Column[3][3] = values[3];
}

HKV_FORCE_INLINE const hkvVec4 hkvMat4::getDiagonal () const
{
  return hkvVec4 (m_Column[0][0], m_Column[1][1], m_Column[2][2], m_Column[3][3]);
}

HKV_FORCE_INLINE hkvVec3 hkvMat4::getScalingFactors () const
{
  hkvVec3 v;

  v.x = hkvVec3 (m_Column[0][0], m_Column[0][1], m_Column[0][2]).getLength ();
  v.y = hkvVec3 (m_Column[1][0], m_Column[1][1], m_Column[1][2]).getLength ();
  v.z = hkvVec3 (m_Column[2][0], m_Column[2][1], m_Column[2][2]).getLength ();

  return v;
}

HKV_FORCE_INLINE hkvResult hkvMat4::setScalingFactors (const hkvVec3& vXYZ, float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
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

HKV_FORCE_INLINE void hkvMat4::transpose ()
{
  // swap uses one temporary, this is easier for the compiler to optimize
  // than a complete temporary copy of the matrix

  hkvMath::swap (m_ElementsCM[ 1], m_ElementsCM[ 4]);
  hkvMath::swap (m_ElementsCM[ 8], m_ElementsCM[ 2]);
  hkvMath::swap (m_ElementsCM[ 3], m_ElementsCM[12]);
  hkvMath::swap (m_ElementsCM[ 6], m_ElementsCM[ 9]);
  hkvMath::swap (m_ElementsCM[ 7], m_ElementsCM[13]);
  hkvMath::swap (m_ElementsCM[11], m_ElementsCM[14]);
}

HKV_FORCE_INLINE const hkvMat4 hkvMat4::getTransposed () const
{
  hkvMat4 r (hkvNoInitialization);
  r.set (m_ElementsCM, RowMajor);

  // the data is stored in column-major order, so creating a matrix with the same data, but telling it that the data
  // is in row-major order will transpose the matrix
  return r;
}

HKV_FORCE_INLINE const hkvMat4 hkvMat4::getInverse () const
{
  hkvMat4 temp (*this);
  temp.invert ();
  return temp;
}

HKV_FORCE_INLINE const hkvVec3 hkvMat4::getAxis (hkUint32 iAxisXYZ) const
{
  VASSERT (iAxisXYZ < 3);

  return hkvVec3 (m_Column[iAxisXYZ][0], m_Column[iAxisXYZ][1], m_Column[iAxisXYZ][2]);
}

HKV_FORCE_INLINE void hkvMat4::setAxis (hkUint32 iAxisXYZ, const hkvVec3& v)
{
  VASSERT (iAxisXYZ < 3);

  HKVVEC3_COPYTEMPS (v);

  m_Column[iAxisXYZ][0] = v_x;
  m_Column[iAxisXYZ][1] = v_y;
  m_Column[iAxisXYZ][2] = v_z;
}

HKV_FORCE_INLINE void hkvMat4::getAxisXYZ (hkvVec3* HKV_RESTRICT out_AxisX, hkvVec3* HKV_RESTRICT out_AxisY, hkvVec3* HKV_RESTRICT out_AxisZ) const
{
  VASSERT ((out_AxisX != NULL) && (out_AxisY != NULL) && (out_AxisZ != NULL));

  *out_AxisX = getAxis (0);
  *out_AxisY = getAxis (1);
  *out_AxisZ = getAxis (2);
}

HKV_FORCE_INLINE void hkvMat4::setAxisXYZ (const hkvVec3& AxisX, const hkvVec3& AxisY, const hkvVec3& AxisZ)
{
  setAxis (0, AxisX);
  setAxis (1, AxisY);
  setAxis (2, AxisZ);
}

HKV_FORCE_INLINE const hkvVec3 hkvMat4::getTranslation () const
{
  return hkvVec3 (m_Column[3][0], m_Column[3][1], m_Column[3][2]);
}

HKV_FORCE_INLINE void hkvMat4::setTranslation (const hkvVec3& vTranslation)
{
  HKVVEC3_COPYTEMPS (vTranslation);

  m_Column[3][0] = vTranslation_x;
  m_Column[3][1] = vTranslation_y;
  m_Column[3][2] = vTranslation_z;
}

HKV_FORCE_INLINE void hkvMat4::setTranslationMatrix (const hkvVec3& vTranslation)
{
  setIdentity ();
  setTranslation (vTranslation);
}

HKV_FORCE_INLINE void hkvMat4::setScalingMatrix (const hkvVec3& vScaleXYZ)
{
  set (vScaleXYZ.x, 0, 0, 0, 
       0, vScaleXYZ.y, 0, 0,
       0, 0, vScaleXYZ.z, 0,
       0, 0, 0, 1);
}

HKV_FORCE_INLINE bool hkvMat4::isIdentical (const hkvMat4& rhs) const
{
  // mostly used for unit-tests

  for (hkUint32 i = 0; i < 16; ++i)
  {
    if (m_ElementsCM[i] != rhs.m_ElementsCM[i])
      return false;
  }

  return true;
}

HKV_FORCE_INLINE bool hkvMat4::isEqual (const hkvMat4& rhs, float fEpsilon) const
{
  // should not use a negative epsilon
  VASSERT (fEpsilon >= 0.0f);

  // mostly used for unit-tests

  for (hkUint32 i = 0; i < 16; ++i)
  {
    if (!hkvMath::isFloatEqual (m_ElementsCM[i], rhs.m_ElementsCM[i], fEpsilon))
      return false;
  }

  return true;
}

HKV_FORCE_INLINE bool hkvMat4::isValid () const
{
  for (hkUint32 i = 0; i < 16; ++i)
    if (!hkvMath::isFiniteNumber (m_ElementsCM[i]))
      return false;

  return true;
}

HKV_FORCE_INLINE void hkvMat4::operator+= (const hkvMat4& rhs)
{
  // partially unrolled loop
  for (hkUint32 i = 0; i < 4; ++i)
  {
    VMATRIX_COPYCOLUMN (rhs.m_Column[i], rhs);

    m_Column[i][0] += rhs_x;
    m_Column[i][1] += rhs_y;
    m_Column[i][2] += rhs_z;
    m_Column[i][3] += rhs_w;
  }
}

HKV_FORCE_INLINE void hkvMat4::operator-= (const hkvMat4& rhs)
{
  // partially unrolled loop
  for (hkUint32 i = 0; i < 4; ++i)
  {
    VMATRIX_COPYCOLUMN (rhs.m_Column[i], rhs);

    m_Column[i][0] -= rhs_x;
    m_Column[i][1] -= rhs_y;
    m_Column[i][2] -= rhs_z;
    m_Column[i][3] -= rhs_w;
  }
}

HKV_FORCE_INLINE void hkvMat4::operator*= (float f)
{
  for (hkUint32 i = 0; i < 16; ++i)
    m_ElementsCM[i] *= f;
}

HKV_FORCE_INLINE void hkvMat4::operator/= (float f0)
{
  const float f = 1.0f / f0;

  operator*= (f);
}

HKV_FORCE_INLINE const hkvVec3 hkvMat4::transformPosition (const hkvVec3& v) const
{
  HKVVEC3_COPYTEMPS (v);

  const float rx = m_Column[0][0] * v_x + m_Column[1][0] * v_y + m_Column[2][0] * v_z + m_Column[3][0];
  const float ry = m_Column[0][1] * v_x + m_Column[1][1] * v_y + m_Column[2][1] * v_z + m_Column[3][1];
  const float rz = m_Column[0][2] * v_x + m_Column[1][2] * v_y + m_Column[2][2] * v_z + m_Column[3][2];

  return hkvVec3 (rx, ry, rz);
}

HKV_FORCE_INLINE const hkvVec3 hkvMat4::transformDirection (const hkvVec3& v) const
{
  HKVVEC3_COPYTEMPS (v);

  const float rx = m_Column[0][0] * v_x + m_Column[1][0] * v_y + m_Column[2][0] * v_z;
  const float ry = m_Column[0][1] * v_x + m_Column[1][1] * v_y + m_Column[2][1] * v_z;
  const float rz = m_Column[0][2] * v_x + m_Column[1][2] * v_y + m_Column[2][2] * v_z;

  return hkvVec3 (rx, ry, rz);
}

HKV_FORCE_INLINE const hkvMat4 hkvMat4::multiplyReverse (const hkvMat4& rhs) const
{
  return rhs.multiply (*this);
}

HKV_FORCE_INLINE bool operator== (const hkvMat4& lhs, const hkvMat4& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvMat4& lhs, const hkvMat4& rhs)
{
  return !lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE const hkvMat4 operator+ (const hkvMat4& lhs, const hkvMat4& rhs)
{
  hkvMat4 temp (hkvNoInitialization);

  // partially unrolled loop, looks stupid but gives the compiler more room for optimization
  for (hkUint32 i = 0; i < 4; ++i)
  {
    VMATRIX_COPYCOLUMN (lhs.m_Column[i], lhs);
    VMATRIX_COPYCOLUMN (rhs.m_Column[i], rhs);
    const float rx = lhs_x + rhs_x;
    const float ry = lhs_y + rhs_y;
    const float rz = lhs_z + rhs_z;
    const float rw = lhs_w + rhs_w;

    temp.m_Column[i][0] = rx;
    temp.m_Column[i][1] = ry;
    temp.m_Column[i][2] = rz;
    temp.m_Column[i][3] = rw;
  }
  
  return (temp);
}

HKV_FORCE_INLINE const hkvMat4 operator- (const hkvMat4& lhs, const hkvMat4& rhs)
{
  hkvMat4 temp (hkvNoInitialization);

  // partially unrolled loop, looks stupid but gives the compiler more room for optimization
  for (hkUint32 i = 0; i < 4; ++i)
  {
    VMATRIX_COPYCOLUMN (lhs.m_Column[i], lhs);
    VMATRIX_COPYCOLUMN (rhs.m_Column[i], rhs);
    const float rx = lhs_x - rhs_x;
    const float ry = lhs_y - rhs_y;
    const float rz = lhs_z - rhs_z;
    const float rw = lhs_w - rhs_w;

    temp.m_Column[i][0] = rx;
    temp.m_Column[i][1] = ry;
    temp.m_Column[i][2] = rz;
    temp.m_Column[i][3] = rw;
  }

  return (temp);
}

HKV_FORCE_INLINE const hkvMat4 operator* (const hkvMat4& lhs, float f)
{
  hkvMat4 temp (hkvNoInitialization);

  // partially unrolled loop, looks stupid but gives the compiler more room for optimization
  for (hkUint32 i = 0; i < 4; ++i)
  {
    VMATRIX_COPYCOLUMN (lhs.m_Column[i], lhs);

    const float rx = lhs_x * f;
    const float ry = lhs_y * f;
    const float rz = lhs_z * f;
    const float rw = lhs_w * f;

    temp.m_Column[i][0] = rx;
    temp.m_Column[i][1] = ry;
    temp.m_Column[i][2] = rz;
    temp.m_Column[i][3] = rw;
  }

  return (temp);
}

HKV_FORCE_INLINE const hkvMat4 operator* (float f, const hkvMat4& rhs)
{
  // same as "rhs * f"
  return operator* (rhs, f);
}

HKV_FORCE_INLINE const hkvMat4 operator/ (const hkvMat4& lhs, float f0)
{
  const float f = 1.0f / f0;

  // same as "rhs * f"
  return operator* (lhs, f);

}

HKV_FORCE_INLINE const hkvVec4 operator* (const hkvMat4& lhs, const hkvVec4& rhs)
{
  return lhs.transform (rhs);
}

HKV_FORCE_INLINE const hkvVec3 operator* (const hkvMat4& lhs, const hkvVec3& rhs)
{
  return lhs.transformPosition (rhs);
}

#ifdef HKVMATH_ENABLE_NEW_OPERATORS
  HKV_FORCE_INLINE const hkvMat4 operator* (const hkvMat4& lhs, const hkvMat4& rhs)
  {
    return lhs.multiply (rhs);
  }
#endif



HKV_FORCE_INLINE const hkvMat3 hkvMat4::getRotationalPart () const
{
  return hkvMat3 (m_Column[0][0], m_Column[1][0], m_Column[2][0], 
                  m_Column[0][1], m_Column[1][1], m_Column[2][1], 
                  m_Column[0][2], m_Column[1][2], m_Column[2][2], hkvMat3::RowMajor);
}

HKV_FORCE_INLINE void hkvMat4::setRotationalPart (const hkvMat3& mRot)
{
  for (hkUint32 col = 0; col < 3; ++col)
    for (hkUint32 row = 0; row < 3; ++row)
      m_Column[col][row] = mRot.m_Column[col][row];
}

HKV_FORCE_INLINE hkvResult hkvMat4::normalize (float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
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

HKV_FORCE_INLINE const hkvVec4 hkvMat4::transform (const hkvVec4& v) const
{
  HKVVEC4_COPYTEMPS (v);

  const float rx = m_Column[0][0] * v_x + m_Column[1][0] * v_y + m_Column[2][0] * v_z + m_Column[3][0] * v_w;
  const float ry = m_Column[0][1] * v_x + m_Column[1][1] * v_y + m_Column[2][1] * v_z + m_Column[3][1] * v_w;
  const float rz = m_Column[0][2] * v_x + m_Column[1][2] * v_y + m_Column[2][2] * v_z + m_Column[3][2] * v_w;
  const float rw = m_Column[0][3] * v_x + m_Column[1][3] * v_y + m_Column[2][3] * v_z + m_Column[3][3] * v_w;

  return hkvVec4 (rx, ry, rz, rw);
}

HKV_FORCE_INLINE void hkvMat4::setFromEulerAngles (float fRoll, float fPitch, float fYaw)
{
  *this = hkvEulerUtil::ConvertEulerToMat4_Deg (fRoll, fPitch, fYaw, hkvEulerMode::VisionDefault);
}

HKV_FORCE_INLINE void hkvMat4::getAsEulerAngles (float& out_fRoll, float& out_fPitch, float& out_fYaw) const
{
  hkvEulerUtil::ConvertMat4ToEuler_Deg (*this, out_fRoll, out_fPitch, out_fYaw, hkvEulerMode::VisionDefault);
}

HKV_FORCE_INLINE const hkvMat4_AsArray hkvMat4::getAsArray (Layout layout) const
{
  hkvMat4_AsArray r;
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
