/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file LargePosition.hpp

#ifndef LARGEPOSITION_HPP_INCLUDED
#define LARGEPOSITION_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>

#define TO_PARAMX(_pos) (_pos).m_iSectorX,(_pos).m_vSectorOfs.x
#define TO_PARAMY(_pos) (_pos).m_iSectorY,(_pos).m_vSectorOfs.y
#define IS_X_SMALLER(_p1,_p2) VLargePosition::IsSmallerThan(_p1.m_iSectorX,_p1.m_vSectorOfs.x, _p2.m_iSectorX,_p2.m_vSectorOfs.x)
#define IS_Y_SMALLER(_p1,_p2) VLargePosition::IsSmallerThan(_p1.m_iSectorY,_p1.m_vSectorOfs.y, _p2.m_iSectorY,_p2.m_vSectorOfs.y)
#define IS_X_LARGER(_p1,_p2) VLargePosition::IsLargerThan(_p1.m_iSectorX,_p1.m_vSectorOfs.x, _p2.m_iSectorX,_p2.m_vSectorOfs.x)
#define IS_Y_LARGER(_p1,_p2) VLargePosition::IsLargerThan(_p1.m_iSectorY,_p1.m_vSectorOfs.y, _p2.m_iSectorY,_p2.m_vSectorOfs.y)

///\brief
///Position class (x,y,z) for objects in the large outdoor game worlds
///
///It avoids large floating point values for x and y by storing the sector index and only relative xy inside the sector.
///Relative positions are always kept inside a sector. Many functions thus take a reference to VTerrainConfig to retrieve the sector spacing.
class VLargePosition
{
public:
  ///\brief
  ///Constructor
  inline VLargePosition() {m_iSectorX=m_iSectorY=0;}
  ///\brief
  ///Copy constructor
  inline VLargePosition(const VLargePosition &vOther) {m_iSectorX=vOther.m_iSectorX;m_iSectorY=vOther.m_iSectorY;m_vSectorOfs=vOther.m_vSectorOfs;}
  ///\brief
  ///Constructor that converts from world space position
  inline VLargePosition(const VTerrainConfig &config,const hkvVec3& vRenderPos) {FromRenderSpace(config,vRenderPos);}
  ///\brief
  ///Constructor
  inline VLargePosition(int iSectorX, int iSectorY, const hkvVec3& vRelPos) {m_iSectorX=iSectorX;m_iSectorY=iSectorY;m_vSectorOfs=vRelPos;}

  ///\brief
  ///Returns the world space position
  inline hkvVec3 ToRenderSpace(const VTerrainConfig &config) const;
  ///\brief
  ///Converts from world space position
  inline void FromRenderSpace(const VTerrainConfig &config,const hkvVec3& vRenderPos);
  ///\brief
  ///Returns the world space position
  inline hkvVec3 ToRenderSpace(const VTerrainConfig &config,int iOfsSectorX,int iOfsSectorY) const;

  ///\brief
  ///Ensures that the relative x any value is smaller than the sector size by wrapping into other sector indices
  inline void Validate(const VTerrainConfig &config);
  ///\brief
  ///Checks whether the position is valid, i.e. x and y are smaller than the sector size
  inline bool IsValid(const VTerrainConfig &config) const;

  ///\brief
  ///Static function to calculate the sum of two large positions
  static inline VLargePosition Sum(const VTerrainConfig &config,const VLargePosition &p1,const VLargePosition &p2);
  ///\brief
  ///Static function to calculate the sum of two positions
  static inline VLargePosition Sum(const VTerrainConfig &config,const VLargePosition &p1,const hkvVec3& p2);

  ///\brief
  ///Increments the position by delta and performs wrapping
  inline void IncPos(const VTerrainConfig &config,const hkvVec3& vDelta) {m_vSectorOfs+=vDelta;Validate(config);}

  ///\brief
  ///Debug output function
  inline void ToString(char *szBuffer) const {sprintf(szBuffer,"sector(%i,%i), pos(%.2f,%.2f,%.2f)",(int)m_iSectorX,(int)m_iSectorY,m_vSectorOfs.x,m_vSectorOfs.y,m_vSectorOfs.z);}

  V_DECLARE_SERIALX(VLargePosition,inline)
  inline void SerializeX(VArchive &ar);

  ///\brief
  /// Static indicator: true if pos1<pos2
  inline static bool IsSmallerThan(int iSector1, float fOfs1, int iSector2, float fOfs2);
  ///\brief
  /// Static indicator: true if pos1>pos2
  inline static bool IsLargerThan(int iSector1, float fOfs1, int iSector2, float fOfs2);
public:
  hkvVec3 m_vSectorOfs;      ///< relative x/y offset inside the sector. z is absolute
  short m_iSectorX,m_iSectorY; ///< x and y index of the current sector
};


///\brief
/// Bounding box class with VLargePosition min/max value
class VLargeBoundingBox
{
public:
  ///\brief
  /// Constructor
  inline VLargeBoundingBox() {Reset();}
  ///\brief
  /// Constructor
  inline VLargeBoundingBox(const VLargePosition &vMin, const VLargePosition &vMax) : m_vMin(vMin), m_vMax(vMax) {}
  ///\brief
  /// Constructor
  inline VLargeBoundingBox(const VTerrainConfig &config, const hkvVec3& vMin, const hkvVec3& vMax) : m_vMin(config,vMin), m_vMax(config,vMax) {}
  ///\brief
  /// Constructor
  inline VLargeBoundingBox(const VTerrainConfig &config, const hkvAlignedBBox &localBBox) : m_vMin(config,(const hkvVec3& )localBBox.m_vMin), m_vMax(config,(const hkvVec3& )localBBox.m_vMax) {}
  ///\brief
  /// Constructor
  inline VLargeBoundingBox(const VTerrainConfig &config, const hkvAlignedBBox &localBBox, const hkvMat3 &rotation, const VLargePosition &center);

  ///\brief
  /// Converts bounding box from world space
  void FromRenderSpace(const VTerrainConfig &config,const hkvAlignedBBox &viewBBox) {m_vMin.FromRenderSpace(config,(const hkvVec3& )viewBBox.m_vMin);m_vMax.FromRenderSpace(config,(const hkvVec3& )viewBBox.m_vMax);}
  ///\brief
  /// Converts bounding box to world space
  hkvAlignedBBox ToRenderSpace(const VTerrainConfig &config) const {return hkvAlignedBBox(m_vMin.ToRenderSpace(config),m_vMax.ToRenderSpace(config));}
  ///\brief
  /// Converts bounding box to world space
  hkvAlignedBBox ToRenderSpace(const VTerrainConfig &config, int iOfsSectorX,int iOfsSectorY) const {return hkvAlignedBBox(m_vMin.ToRenderSpace(config,iOfsSectorX,iOfsSectorY),m_vMax.ToRenderSpace(config,iOfsSectorX,iOfsSectorY));}

  ///\brief
  /// Resets the bounding box so that min>max
  inline void Reset();
  ///\brief
  /// Checks whether min>=max
  inline bool IsValid() const;
  ///\brief
  /// Checks whether both min and max are valid large positions themselves
  inline bool IsValidated(const VTerrainConfig &config) const {return m_vMin.IsValid(config) && m_vMax.IsValid(config);}

  ///\brief
  /// Indicates whether a position is inside the box
  inline bool IsInside(const VLargePosition &pos) const;
  ///\brief
  /// Indicates whether two boxes overlaps
  inline bool IntersectsWith(const VLargeBoundingBox &other) const;

  ///\brief
  /// Adds a position to the box.
  inline void Add(const VTerrainConfig &config,const hkvVec3& pos);
  ///\brief
  /// Adds a position to the box.
  inline void Add(const VLargePosition &pos);
  ///\brief
  /// Adds a bounding box to this box.
  inline void Add(const VLargeBoundingBox &other) {Add(other.m_vMin);Add(other.m_vMax);}

  ///\brief
  /// Retrieves the affected sector range, clamped to the terrain's number of sectors.
  inline void GetSectorIndices_Clamped(const VTerrainConfig &config, int &x1, int &y1, int &x2, int &y2) const;

  // serialization
  V_DECLARE_SERIALX(VLargeBoundingBox,inline)
  inline void SerializeX(VArchive &ar);
public:
  VLargePosition m_vMin,m_vMax;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// VLargePosition inlines
///////////////////////////////////////////////////////////////////////////////////////////////

inline hkvVec3 VLargePosition::ToRenderSpace(const VTerrainConfig &config) const
{
  return hkvVec3(
    config.m_vTerrainPos.x + (float)(m_iSectorX/*-g_iCurrentSectorOfs[0]*/)*config.m_vSectorSize.x + m_vSectorOfs.x,
    config.m_vTerrainPos.y + (float)(m_iSectorY/*-g_iCurrentSectorOfs[1]*/)*config.m_vSectorSize.y + m_vSectorOfs.y, 
    config.m_vTerrainPos.z + m_vSectorOfs.z);
}

inline hkvVec3 VLargePosition::ToRenderSpace(const VTerrainConfig &config,int iOfsSectorX,int iOfsSectorY) const
{
  return hkvVec3(
    config.m_vTerrainPos.x + (float)(m_iSectorX-iOfsSectorX)*config.m_vSectorSize.x + m_vSectorOfs.x,
    config.m_vTerrainPos.y + (float)(m_iSectorY-iOfsSectorY)*config.m_vSectorSize.y + m_vSectorOfs.y, 
    config.m_vTerrainPos.z + m_vSectorOfs.z);
}


inline void VLargePosition::FromRenderSpace(const VTerrainConfig &config,const hkvVec3& vRenderPos)
{
  m_iSectorX = 0;//g_iCurrentSectorOfs[0];
  m_iSectorY = 0;//g_iCurrentSectorOfs[1];
  m_vSectorOfs = vRenderPos-config.m_vTerrainPos;
  Validate(config);
}

inline void VLargePosition::Validate(const VTerrainConfig &config)
{
  if (m_vSectorOfs.x<0.f) 
  {
    // NB: We use div config.m_vSectorSize rather than mul config.m_vInvSectorSize because of rounding issues [#]
    float fRes = -m_vSectorOfs.x / config.m_vSectorSize.x;
    int iDelta = (int)fRes + 1;
    m_iSectorX -= iDelta;
    m_vSectorOfs.x+=config.m_vSectorSize.x*(float)iDelta;
  } 
  else if (m_vSectorOfs.x>=config.m_vInvSectorSize.x)
  {
    float fRes = m_vSectorOfs.x / config.m_vSectorSize.x;
    int iDelta = (int)fRes;
    m_iSectorX += iDelta;
    m_vSectorOfs.x-=config.m_vSectorSize.x*(float)iDelta;
  }
  if (m_vSectorOfs.y<0.f) 
  {
    float fRes = -m_vSectorOfs.y / config.m_vSectorSize.y;
    int iDelta = (int)fRes + 1;
    m_iSectorY -= iDelta;
    m_vSectorOfs.y+=config.m_vSectorSize.y*(float)iDelta;
  } 
  else if (m_vSectorOfs.y>=config.m_vInvSectorSize.y)
  {
    float fRes = m_vSectorOfs.y / config.m_vSectorSize.y;
    int iDelta = (int)fRes;
    m_iSectorY += iDelta;
    m_vSectorOfs.y-=config.m_vSectorSize.y*(float)iDelta;
  }
}

inline VLargePosition VLargePosition::Sum(const VTerrainConfig &config,const VLargePosition &p1,const VLargePosition &p2)
{
  VLargePosition newPos(p1.m_iSectorX+p2.m_iSectorX,p1.m_iSectorY+p2.m_iSectorY,p1.m_vSectorOfs+p2.m_vSectorOfs);
  newPos.Validate(config);
  return newPos;
}

inline VLargePosition VLargePosition::Sum(const VTerrainConfig &config,const VLargePosition &p1,const hkvVec3& p2)
{
  VLargePosition newPos(p1.m_iSectorX,p1.m_iSectorY,p1.m_vSectorOfs+p2);
  newPos.Validate(config);
  return newPos;
}
 
inline bool VLargePosition::IsValid(const VTerrainConfig &config) const
{
  return (m_vSectorOfs.x>=0.f && m_vSectorOfs.x<=config.m_vSectorSize.x
    && m_vSectorOfs.y>=0.f && m_vSectorOfs.y<=config.m_vSectorSize.y);
}


V_IMPLEMENT_SERIALX( VLargePosition);
void VLargePosition::SerializeX(VArchive &ar)
{
  if (ar.IsLoading())
  {
    ar >> m_iSectorX >> m_iSectorY >> m_vSectorOfs.x >> m_vSectorOfs.y >> m_vSectorOfs.z;
  } else
  {
    ar << m_iSectorX << m_iSectorY << m_vSectorOfs.x << m_vSectorOfs.y << m_vSectorOfs.z;
  }
}

// true if pos1<pos2
inline bool VLargePosition::IsSmallerThan(int iSector1, float fOfs1, int iSector2, float fOfs2)
{
  if (iSector1<iSector2) return true;
  if (iSector1>iSector2) return false;
  return (fOfs1<fOfs2);
}

// true if pos1>pos2
inline bool VLargePosition::IsLargerThan(int iSector1, float fOfs1, int iSector2, float fOfs2)
{
  if (iSector1>iSector2) return true;
  if (iSector1<iSector2) return false;
  return (fOfs1>fOfs2);
}



///////////////////////////////////////////////////////////////////////////////////////////////
// VLargeBoundingBox inlines
///////////////////////////////////////////////////////////////////////////////////////////////

inline VLargeBoundingBox::VLargeBoundingBox(const VTerrainConfig &config, const hkvAlignedBBox &localBBox, const hkvMat3 &rotation, const VLargePosition &center)
{
  Reset();
  hkvVec3 vCorner[8];
  localBBox.getCorners(vCorner);

  // add the 8 corners to the bounding box so the oriented box is completely inside the new box
  for (int i=0;i<8;i++)
  {
    vCorner[i] = rotation.transformDirection (vCorner[i]);
    Add(VLargePosition::Sum(config,center,VLargePosition (config, vCorner[i])) );
  }
}

inline bool VLargeBoundingBox::IsValid() const
{
  if (IS_X_LARGER(m_vMin,m_vMax)) return false;
  if (IS_Y_LARGER(m_vMin,m_vMax)) return false;
  if (m_vMin.m_vSectorOfs.z>m_vMax.m_vSectorOfs.z) return false;
  return true;
}

inline bool VLargeBoundingBox::IsInside(const VLargePosition &pos) const
{
//  VASSERT(pos.IsValid());
  if (IS_X_SMALLER(pos,m_vMin)) return false;
  if (IS_Y_SMALLER(pos,m_vMin)) return false;
  if (IS_X_LARGER(pos,m_vMax)) return false;
  if (IS_Y_LARGER(pos,m_vMax)) return false;
  if (pos.m_vSectorOfs.z<m_vMin.m_vSectorOfs.z || pos.m_vSectorOfs.z>m_vMax.m_vSectorOfs.z) return false;
  return true;
}

inline bool VLargeBoundingBox::IntersectsWith(const VLargeBoundingBox &other) const
{
  if (IS_X_SMALLER(other.m_vMax,m_vMin)) return false;
  if (IS_Y_SMALLER(other.m_vMax,m_vMin)) return false;
  if (IS_X_LARGER(other.m_vMin,m_vMax)) return false;
  if (IS_Y_LARGER(other.m_vMin,m_vMax)) return false;
  return true;
}

inline void VLargeBoundingBox::Add(const VLargePosition &pos)
{
  if (IS_X_SMALLER(pos,m_vMin)) {m_vMin.m_vSectorOfs.x=pos.m_vSectorOfs.x;m_vMin.m_iSectorX=pos.m_iSectorX;}
  if (IS_Y_SMALLER(pos,m_vMin)) {m_vMin.m_vSectorOfs.y=pos.m_vSectorOfs.y;m_vMin.m_iSectorY=pos.m_iSectorY;}
  if (pos.m_vSectorOfs.z<m_vMin.m_vSectorOfs.z) {m_vMin.m_vSectorOfs.z=pos.m_vSectorOfs.z;}
  if (IS_X_LARGER(pos,m_vMax)) {m_vMax.m_vSectorOfs.x=pos.m_vSectorOfs.x;m_vMax.m_iSectorX=pos.m_iSectorX;}
  if (IS_Y_LARGER(pos,m_vMax)) {m_vMax.m_vSectorOfs.y=pos.m_vSectorOfs.y;m_vMax.m_iSectorY=pos.m_iSectorY;}
  if (pos.m_vSectorOfs.z>m_vMax.m_vSectorOfs.z) {m_vMax.m_vSectorOfs.z=pos.m_vSectorOfs.z;}
}

inline void VLargeBoundingBox::Add(const VTerrainConfig &config, const hkvVec3& pos)
{
  VLargePosition largePos(config, pos);
  Add(largePos);
}

inline void VLargeBoundingBox::GetSectorIndices_Clamped(const VTerrainConfig &config, int &x1, int &y1, int &x2, int &y2) const
{
  x1 = m_vMin.m_iSectorX;
  y1 = m_vMin.m_iSectorY;
  x2 = m_vMax.m_iSectorX;
  y2 = m_vMax.m_iSectorY;
  config.ClampSectorIndices(x1,y1);
  config.ClampSectorIndices(x2,y2);
}

inline void VLargeBoundingBox::Reset()
{
  m_vMin.m_iSectorX = m_vMin.m_iSectorY = 32000;
  m_vMax.m_iSectorX = m_vMax.m_iSectorY = -32000;
  m_vMin.m_vSectorOfs.set(0,0,1000000000.f);
  m_vMax.m_vSectorOfs.set(0,0,-1000000000.f);

}

inline void VLargeBoundingBox::SerializeX(VArchive &ar)
{
  if (ar.IsLoading())
  {
    ar >> m_vMin >> m_vMax;
  } else
  {
    ar << m_vMin << m_vMax;
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
