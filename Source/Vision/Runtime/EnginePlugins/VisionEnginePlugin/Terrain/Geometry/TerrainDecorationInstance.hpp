/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainDecorationInstance.hpp

#ifndef TERRAINDECORATIONINSTANCE_HPP_INCLUDED
#define TERRAINDECORATIONINSTANCE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Math/LargePosition.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationModel.hpp>

#define DECORATIONINSTANCE_VERSION_20       20
#define DECORATIONINSTANCE_VERSION_21       21 // added color
#define DECORATIONINSTANCE_CURRENT_VERSION  DECORATIONINSTANCE_VERSION_21

/// \brief
///   A very lightweight object representation for instanced decoration objects (e.g. painted vegetation), using a model pointer and a transformation.
class VTerrainDecorationInstance
{
public:
  /// \brief
	///  Constructor
	inline VTerrainDecorationInstance()
  {
    m_iSortingKey = 0;
    m_fScaling = 1.0f;
    m_InstanceColor = V_RGBA_WHITE;
    m_pOwnerSector = NULL;
  }

  /// \brief
	///  Internal function that generates a sorting key
	///
	/// \param iSubKey
	///  lower 16bit of the key, i.e. distance
	///
	TERRAIN_IMPEXP void ComputeSortingKey(unsigned short iSubKey);

  /// \brief
	///  Extracts position vector for this instance
	///
	/// \param destPos
	///  target vector
	///
	inline void GetPosition(hkvVec3& destPos) const
  {
    destPos = m_vPosition;
  }

	inline void GetRenderPosition (hkvVec3& destPos, const hkvVec3& vOffset) const
  {
    destPos = m_vPosition + vOffset;
  }


  /// \brief
	///  Increases position by certain amount
	///
	/// \param vDelta
	///  delta to move
	inline void IncPosition (const hkvVec3& vDelta) ///< editing only
  {
    m_vPosition += vDelta;
  }

  /// \brief
	///  Sets full transformation
	///
	/// \param vPos
  ///  Position vector (world space)
	///
	/// \param fYaw
  ///  yaw roation (degrees)
	///
	/// \param fScale
	///  Uniform scaling factor
	///
	inline void SetTransformation(const hkvVec3& vPos, float fYaw, float fScale)
  {
    hkvMat3 rotation (hkvNoInitialization);
    rotation.setFromEulerAngles (0, 0, fYaw);
    rotation *= fScale;

    SetTransformation (vPos,rotation,fScale);
  }

  /// \brief
  ///  Builds a full 4x4 transform amtrix
  hkvMat4 GetTransformation() const
  {
    hkvMat4 transform(m_Orientation,m_vPosition);
    return transform;
  }

  /// \brief
	///  Sets full transformation
	///
	/// \param vPos
  ///  Position vector (world space)
	///
	/// \param rotation
	///  Rotation matrix
	///
	/// \param fScale
  ///  Scaling (also when encoded into matrix already)
	///
	TERRAIN_IMPEXP void SetTransformation(const hkvVec3& vPos, const hkvMat3 &rotation, float fScale);

  /// \brief
	///  Gets the square distance of this instance to the passed position
	///
	/// \param vPos
  ///  (Camera) Position vector to test
	///
	/// \returns
  ///  Squared distance (xy direction only)
	///
	inline float GetDistanceSqr(const hkvVec3& vPos) const
  {
    hkvVec3 vThisPos = m_vPosition;
    vThisPos-=vPos;
    return vThisPos.x*vThisPos.x + vThisPos.y*vThisPos.y; ///< no height dependency
  }

	inline float GetRenderDistanceSqr(const hkvVec3& vPos, const hkvVec3& vOffset) const
  {
    hkvVec3 vThisPos(hkvNoInitialization);
    GetRenderPosition(vThisPos, vOffset);
    vThisPos-=vPos;
    return vThisPos.x*vThisPos.x + vThisPos.y*vThisPos.y + vThisPos.z*vThisPos.z;
  }

  inline float GetRenderDistanceSqrXY(const hkvVec3& vPos, const hkvVec3& vOffset) const
  {
    hkvVec3 vThisPos(hkvNoInitialization);
    GetRenderPosition(vThisPos, vOffset);
    vThisPos-=vPos;
    return vThisPos.x*vThisPos.x + vThisPos.y*vThisPos.y; ///< no height dependency
  }

  /// \brief
	///  Calculates the absolute bounding box for this instance
	///
	/// \param destBBox
	///  Destination box
	///
	inline void GetBoundingBox(hkvAlignedBBox &destBBox) const
  {
    m_spModel->EnsureLoaded();
    VASSERT(m_spModel->m_LocalBBox.isValid());
    destBBox.setInvalid();

    //destBBox.Inflate(m_spModel->m_LocalBBox,m_Orientation,m_vPosition);
    {
      hkvAlignedBBox temp = m_spModel->m_LocalBBox;
      hkvMat4 mTransform (m_Orientation, m_vPosition);
      temp.transformFromOrigin (mTransform);
      destBBox.expandToInclude (temp);
    }

  }

	inline void GetRenderBoundingBox(hkvAlignedBBox &destBBox, const hkvVec3& vOffset) const
  {
    VASSERT(m_spModel->m_LocalBBox.isValid());
    destBBox.setInvalid();

    //destBBox.Inflate(m_spModel->m_LocalBBox,m_Orientation,m_vPosition);
    {
      hkvAlignedBBox temp = m_spModel->m_LocalBBox;
      hkvMat4 mTransform (m_Orientation, m_vPosition);
      temp.transformFromOrigin (mTransform);
      destBBox.expandToInclude (temp);
    }

    destBBox.m_vMin += vOffset;
    destBBox.m_vMax += vOffset;
  }

  /// \brief
  ///  Returns the scaling of the model
  inline float GetScaling() const
  {
    return m_fScaling;
  }


  /// \brief
	///  Assignment operator
	///
	/// \param other
	///  source object
	///
	/// \returns
	///  Reference to 'this'
	inline VTerrainDecorationInstance& operator = (const VTerrainDecorationInstance &other)
  {
    m_spModel = other.m_spModel;
    m_Orientation = other.m_Orientation;
    m_vPosition = other.m_vPosition;
    m_fScaling = other.m_fScaling;
    m_InstanceColor = other.m_InstanceColor;
    m_iSortingKey = other.m_iSortingKey;
    m_pOwnerSector = other.m_pOwnerSector;
    return *this;
  }

  /// \brief
  ///  Gets the decoration model referenced by this instance
  inline IVTerrainDecorationModelPtr GetModel() { return m_spModel; }


  /// \brief
	///  Serializes this instance to binary file
	///
	/// \param ar
	///  Binary archive
	///
	/// \param iLocalVersion
	///  Sector file's local file version
  ///
  /// \param bSerializeModel
  ///  If true, this instance serializes its own model pointer
	TERRAIN_IMPEXP void SerializeX(VArchive &ar, int iLocalVersion, bool bSerializeModel=true);

  hkvMat3 m_Orientation;          ///< (Scaled) orientation of object
  hkvVec3 m_vPosition;            ///< Position of object
  float m_fScaling;               ///< Additional scaling value
  VColorRef m_InstanceColor;      ///< Tint color for this instance 
  IVTerrainDecorationModelPtr m_spModel;  ///< Decoration model
  VTerrainSector *m_pOwnerSector;         ///< owner sector

  // per instance temp values:
  int m_iSortingKey;    ///< Temporary value

  /// \brief
	///  Static sorting function for qsort to sort decoration by sorting key
	///
	/// \param elem1
	///  Pointer to decoration instance
	///
	/// \param elem2
	///  Pointer to decoration instance
	///
	/// \returns
	///  Sorting result
	TERRAIN_IMPEXP static int SortObjects( const void *elem1, const void *elem2);

  /// \brief
	///  Static sorting function for qsort to sort decoration by pointer
	///
	/// \param elem1
	///  Pointer to decoration instance
	///
	/// \param elem2
	///  Pointer to decoration instance
	///
	/// \returns
	///  Sorting result
  TERRAIN_IMPEXP static int SortObjectPtrs( const void *elem1, const void *elem2);
};


/// \brief
///  Allocates a block of VTerrainDecorationInstance instances and implements the IVisDecorationGroup_cl interface to access the objects
class VTerrainDecorationInstanceCollection : public IVisDecorationGroup_cl
{
public:
  /// \brief
	///  Constructor
  inline VTerrainDecorationInstanceCollection() : IVisDecorationGroup_cl()
  {
    m_iCount = 0;
    m_pInstances = NULL;
  }

  /// \brief
	///  Destructor
  virtual ~VTerrainDecorationInstanceCollection()
  {
    V_SAFE_DELETE_ARRAY(m_pInstances);
  }

  /// \brief
	///  Allocates iCount decoration instances in this list
	///
	/// \param pOwner
	///  Initializes each instance's m_pOwnerSector member
	///
	/// \param iCount
	///  Number of objects to allocate
	///
	inline void Allocate(VTerrainSector *pOwner, int iCount)
  {
    if (m_iCount==iCount) return;
    V_SAFE_DELETE_ARRAY(m_pInstances);
    m_iCount = iCount;
    if (iCount<1)
      return;

    m_pInstances = new VTerrainDecorationInstance[m_iCount];
    for (int i=0;i<iCount;i++)
      m_pInstances[i].m_pOwnerSector = pOwner;
  }

  /// \brief
	///  Releases allocated instances
	///
	inline void Free()
  {
    m_iCount = 0;
    V_SAFE_DELETE_ARRAY(m_pInstances);
  }

  /// \brief
  ///  Implements IVisDecorationGroup_cl
  TERRAIN_IMPEXP virtual int GetDecorationObjectCount() const
  {
    return m_iCount;
  }

  /// \brief
  ///  Implements IVisDecorationGroup_cl
  TERRAIN_IMPEXP virtual VTerrainDecorationInstance* GetDecorationObject(int iIndex) const
  {
    return &m_pInstances[iIndex];
  }


  int m_iCount; ///< Number of allocated instances
  VTerrainDecorationInstance *m_pInstances; ///< List of instances
};

#ifndef _VISION_DOC

inline void VModelInstanceData_t::Set(const VTerrainDecorationInstance &srcinst)
{
  Set(srcinst.m_Orientation, srcinst.m_vPosition, srcinst.m_InstanceColor);
}

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
