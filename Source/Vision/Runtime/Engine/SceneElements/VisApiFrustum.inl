/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiFrustum.inl


inline unsigned int VisFrustum_cl::GetNumPlanes() const
{
  return m_iPlaneCounter;
}

inline bool VisFrustum_cl::Overlaps(const hkvVec3& point) const
{
  for (unsigned int i=0; i < m_iPlaneCounter; i++)
  {
    hkvPlane& plane = GETPLANE(i);
    float r = (point.x * plane.m_vNormal.x) + (point.y * plane.m_vNormal.y) + (point.z * plane.m_vNormal.z) + plane.m_fNegDist;
    if (r>0)
      return false;
  }
  return true;
}

#ifndef SPU

inline bool VisFrustum_cl::Overlaps(const hkvBoundingSphere& sphere) const
{
  for (unsigned int i=0; i < m_iPlaneCounter; i++)
  {
    hkvPlane& plane = GETPLANE(i);
    float r=sphere.m_vCenter[0]*plane.m_vNormal.x+sphere.m_vCenter[1]*plane.m_vNormal.y+sphere.m_vCenter[2]*plane.m_vNormal.z+plane.m_fNegDist;
    r -= sphere.m_fRadius;
    if (r>0)
      return false;
  }
  return true;
}

#endif

inline bool VisFrustum_cl::Overlaps(const hkvAlignedBBox &box) const
{
  unsigned int i;
  for (i=0; i<m_iPlaneCounter; i++) {
    hkvPlane& plane = GETPLANE(i);
    //hkvPlane& plane = m_vPlanes[i];
    float a = plane.m_vNormal.x;
    float b = plane.m_vNormal.y;
    float c = plane.m_vNormal.z;
    float d = plane.m_fNegDist;
    float r;
    r=box.m_vMin.x*a+box.m_vMin.y*b+box.m_vMin.z*c+d;
    if (r<=0) continue;
    r=box.m_vMin.x*a+box.m_vMin.y*b+box.m_vMax.z*c+d;
    if (r<=0) continue;
    r=box.m_vMin.x*a+box.m_vMax.y*b+box.m_vMin.z*c+d;
    if (r<=0) continue;
    r=box.m_vMin.x*a+box.m_vMax.y*b+box.m_vMax.z*c+d;
    if (r<=0) continue;
    r=box.m_vMax.x*a+box.m_vMin.y*b+box.m_vMin.z*c+d;
    if (r<=0) continue;
    r=box.m_vMax.x*a+box.m_vMin.y*b+box.m_vMax.z*c+d;
    if (r<=0) continue;
    r=box.m_vMax.x*a+box.m_vMax.y*b+box.m_vMin.z*c+d;
    if (r<=0) continue;
    r=box.m_vMax.x*a+box.m_vMax.y*b+box.m_vMax.z*c+d;
    if (r<=0) continue;
    return false;
  }
  return true;
}

// If a plane flag is not set, it is considered impossible that the box lies "outside" of this plane
// TODO: OPTIMIZE!
inline bool VisFrustum_cl::Overlaps(const hkvAlignedBBox &box, int iPlaneFlags) const
{
  if (iPlaneFlags == 0)
    return true;
  unsigned int i;
  for (i=0; i<m_iPlaneCounter; i++) {
    hkvPlane& plane = GETPLANE(i);
    if ( !(iPlaneFlags & (1<<i))) continue;
    float a = plane.m_vNormal.x;
    float b = plane.m_vNormal.y;
    float c = plane.m_vNormal.z;
    float d = plane.m_fNegDist;
    float r;
    r=box.m_vMin.x*a+box.m_vMin.y*b+box.m_vMin.z*c+d;
    if (r<0) continue;
    r=box.m_vMin.x*a+box.m_vMin.y*b+box.m_vMax.z*c+d;
    if (r<0) continue;
    r=box.m_vMin.x*a+box.m_vMax.y*b+box.m_vMin.z*c+d;
    if (r<0) continue;
    r=box.m_vMin.x*a+box.m_vMax.y*b+box.m_vMax.z*c+d;
    if (r<0) continue;
    r=box.m_vMax.x*a+box.m_vMin.y*b+box.m_vMin.z*c+d;
    if (r<0) continue;
    r=box.m_vMax.x*a+box.m_vMin.y*b+box.m_vMax.z*c+d;
    if (r<0) continue;
    r=box.m_vMax.x*a+box.m_vMax.y*b+box.m_vMin.z*c+d;
    if (r<0) continue;
    r=box.m_vMax.x*a+box.m_vMax.y*b+box.m_vMax.z*c+d;
    if (r<0) continue;
    return false;
  }
  return true;
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
