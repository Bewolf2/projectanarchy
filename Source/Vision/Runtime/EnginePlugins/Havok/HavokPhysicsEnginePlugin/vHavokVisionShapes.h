/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokVisionShapes.h

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKVISIONSHAPES_H_INCLUDED
#define VHAVOKVISIONSHAPES_H_INCLUDED

#include <Common/Base/Reflection/hkTypeInfo.h>
#include <Common/Base/Container/BitField/hkBitField.h>
#include <Physics2012/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics2012/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldCollection.h>
#include <Physics2012/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldBvTreeShape.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Important notes:
//
// In order to get custom Vision shape classes working with the Havok reflection system currently following 
// things have be taken into consideration:
// - Havok parser currently can't parse .hpp files.
// - LLVM assumes that parsed header files do not include any platform-specific headers. Since Vision headers
//   do include platform-specific headers (e.g. for windows), headers that contain reflected custom classes 
//   can't in general can't include Vision-specific header files, i.e. can't use Vision data types.
// - All reflected Vision class types must be registered to the Havok reflection system after the base system
//   has been initialized (see vHavokPhysicsModule.cpp, Line 670).
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// -------------------------------------------------------------------------- //
// hkvConvexVerticesShape                                                
// -------------------------------------------------------------------------- //

extern const hkClass hkvConvexVerticesShapeClass;
extern const hkTypeInfo hkvConvexVerticesShapeTypeInfo;

/// \brief
///   Custom shape class that extends the hkpConvexVerticesShape.
class hkvConvexVerticesShape: public hkpConvexVerticesShape
{   
  // +version(0)

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SHAPE);
  HK_DECLARE_REFLECTION();

  /// \brief
  ///   Constructor
  hkvConvexVerticesShape(hkInt64 iFileTime, const hkStridedVertices& vertices, const hkpConvexVerticesShape::BuildConfig& config=hkpConvexVerticesShape::BuildConfig())
    : hkpConvexVerticesShape(vertices, config), m_iFileTime(iFileTime)
  {
  }

  /// \brief
  ///   Serialization constructor
  hkvConvexVerticesShape(hkFinishLoadedObjectFlag flag)
    : hkpConvexVerticesShape(flag)
  {
  }

  /// \brief
  ///   Returns system time at which corresponding .vcolmesh had been exported.
  HK_FORCE_INLINE hkInt64 GetFileTime() const
  {
    return m_iFileTime;
  }

  /// \brief
  ///   hkReferencedObject implementation.
  virtual const hkClass* getClassType() const HK_OVERRIDE { return &hkvConvexVerticesShapeClass; }

private:
  hkInt64 m_iFileTime; // system time at which corresponding .vcolmesh had been exported

};


// -------------------------------------------------------------------------- //
// hkvBvCompressedMeshShape                                                
// -------------------------------------------------------------------------- //

extern const hkClass hkvMeshMaterialClass;
extern const hkTypeInfo hkvMeshMaterialTypeInfo;

typedef hkArray<class hkvMeshMaterial> hkvMeshMaterialCache;

/// \brief
///  	 Add material properties to static meshes
class hkvMeshMaterial
{
  // +version(0)

public:
  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkvMeshMaterial);
  HK_DECLARE_REFLECTION();

  hkvMeshMaterial()
    : m_fFriction(0.5f), m_fRestitution(0.4f), m_iOrignalSubMeshIndex(0) 
  {
  }

  hkvMeshMaterial(hkFinishLoadedObjectFlag flag)
    : m_userData(flag)
  {
  }

  float m_fFriction;
  float m_fRestitution; 
  hkStringPtr m_userData;
  int m_iOrignalSubMeshIndex;

};

extern const hkClass hkvBvCompressedMeshShapeClass;
extern const hkTypeInfo hkvBvCompressedMeshShapeTypeInfo;

/// \brief
///   Custom shape class that extends the hkpBvCompressedMeshShape.
class hkvBvCompressedMeshShape: public hkpBvCompressedMeshShape
{   
  // +version(0)

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SHAPE);
  HK_DECLARE_REFLECTION();

  /// \brief
  ///   Constructor
  hkvBvCompressedMeshShape(const hkpBvCompressedMeshShapeCinfo& cInfo, hkvMeshMaterialCache &materials, hkInt64 iFileTime)
    : hkpBvCompressedMeshShape(cInfo), m_iFileTime(iFileTime)
  {
    m_materials = materials; 
  }

  /// \brief
  ///   Constructor without material information
  hkvBvCompressedMeshShape(const hkpBvCompressedMeshShapeCinfo& cInfo, hkInt64 iFileTime)
    : hkpBvCompressedMeshShape(cInfo), m_iFileTime(iFileTime)
  {
  }

  /// \brief
  ///   Serialization constructor
  hkvBvCompressedMeshShape(hkFinishLoadedObjectFlag flag)
    : hkpBvCompressedMeshShape(flag), m_materials(flag)
  {
  }

  /// \brief
  ///   Apply materials to user data of hkpBvCompressedMeshShape.
  inline void SetupMaterials()
  {
    m_userData = (m_materials.getSize()) >0  ? (hkUlong)(&m_materials) : HK_NULL;
  }

  /// \brief
  ///   Returns system time at which corresponding .vcolmesh had been exported.
  HK_FORCE_INLINE hkInt64 GetFileTime() const
  {
    return m_iFileTime;
  }

  /// \brief
  ///   hkReferencedObject implementation.
  virtual const hkClass* getClassType() const HK_OVERRIDE { return &hkvBvCompressedMeshShapeClass; }

private:
  hkInt64 m_iFileTime; // system time at which corresponding .vcolmesh had been exported
  hkvMeshMaterialCache m_materials;

};


// -------------------------------------------------------------------------- //
// hkvSampledHeightFieldShape                                                
// -------------------------------------------------------------------------- //

class VTerrainSector;

extern const hkClass hkvSampledHeightFieldShapeClass;
extern const hkTypeInfo hkvSampledHeightFieldShapeTypeInfo;

/// \brief
///   Custom shape class that extends hkpSampledHeightFieldShape:
///
/// \note 
///   For this to work on SPU you will need to set the two function ptrs instead of the virtual 
///   functions and of course dma in the VTerrainSector.
class hkvSampledHeightFieldShape: public hkpSampledHeightFieldShape
{
  // +version(0)

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SHAPE);
  HK_DECLARE_REFLECTION();

  /// \brief
  ///   Constructor
  hkvSampledHeightFieldShape(const hkpSampledHeightFieldBaseCinfo &ci, const VTerrainSector *pSector);

  /// \brief
  ///   Serialization constructor
  hkvSampledHeightFieldShape(hkFinishLoadedObjectFlag flag);

  /// \brief
  ///   hkpSampledHeightFieldShape interface implementation.
  virtual hkReal getHeightAtImpl(int x, int z) const HK_OVERRIDE;

  /// \brief
  ///   hkpSampledHeightFieldShape interface implementation.
  virtual hkBool getTriangleFlipImpl() const HK_OVERRIDE
  {	
    return false;
  }

  /// \brief
  ///   hkpHeightFieldShape interface implementation.
  virtual void collideSpheres(const CollideSpheresInput &input,SphereCollisionOutput *pOutputArray) const HK_OVERRIDE
  {
    hkSampledHeightFieldShape_collideSpheres(*this, input, pOutputArray);
  }

  /// \brief
  ///   hkReferencedObject implementation.
  virtual const hkClass* getClassType() const HK_OVERRIDE { return &hkvSampledHeightFieldShapeClass; }

private:
  const VTerrainSector *m_pSector; // +nosave 

  int m_iTerrainIndex;
  int m_iSectorIndexX;
  int m_iSectorIndexY;

};


// -------------------------------------------------------------------------- //
// hkvTriSampledHeightFieldCollection                                                
// -------------------------------------------------------------------------- //
extern const hkClass hkvTriSampledHeightFieldCollectionClass;
extern const hkTypeInfo hkvTriSampledHeightFieldCollectionTypeInfo;

/// \brief
///   This class adds terrain sector hole support to default class hkpTriSampledHeightFieldCollection.
class hkvTriSampledHeightFieldCollection : public hkpTriSampledHeightFieldCollection
{
  // +version(0)

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SHAPE);
  HK_DECLARE_REFLECTION();

  /// \brief
  ///   Constructor
  hkvTriSampledHeightFieldCollection(const VTerrainSector *pSector, const hkpSampledHeightFieldShape *pShape);

  /// \brief
  ///   Serialization constructor
  hkvTriSampledHeightFieldCollection(hkFinishLoadedObjectFlag flag)
    : hkpTriSampledHeightFieldCollection(flag), m_tileHoleMask(flag)
  {
  }

  /// \brief
  ///   hkpShapeContainer implementation.
  virtual hkUint32 getCollisionFilterInfo(hkpShapeKey key) const HK_OVERRIDE;

  /// \brief
  ///   hkReferencedObject implementation.
  virtual const hkClass* getClassType() const HK_OVERRIDE { return &hkvTriSampledHeightFieldCollectionClass; }

private:
  // two masks that represent hole and non-hole material
  hkUint32 m_iCollide; 
  hkUint32 m_iNonCollide;

  // parameters that map incoming x/z value to bitmask index
  int m_iStrideX; 
  int m_iTileSamplesX;
  int m_iTileSamplesY; 
  
  // the bitmask itself (16*16 pre-allocated bits because this is the default setting for tile count)
  hkBitField m_tileHoleMask;

};


// -------------------------------------------------------------------------- //
// hkvTriSampledHeightFieldBvTreeShape                                                
// -------------------------------------------------------------------------- //

extern const hkClass hkvTriSampledHeightFieldBvTreeShapeClass;
extern const hkTypeInfo hkvTriSampledHeightFieldBvTreeShapeTypeInfo;

/// \brief
///   Custom shape class for proper raycasts against holes.
class hkvTriSampledHeightFieldBvTreeShape : public hkpTriSampledHeightFieldBvTreeShape
{
  // +version(0)

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SHAPE);
  HK_DECLARE_REFLECTION();

  /// \brief
  ///   Constructor
  hkvTriSampledHeightFieldBvTreeShape(const hkpTriSampledHeightFieldCollection *pCollection) 
    : hkpTriSampledHeightFieldBvTreeShape(pCollection)
  {
  }

  /// \brief
  ///   Serialization constructor
  hkvTriSampledHeightFieldBvTreeShape(hkFinishLoadedObjectFlag flag)
    : hkpTriSampledHeightFieldBvTreeShape(flag)
  {
  }

  /// \brief
  ///   hkpShape interface implementation.
  virtual void castRayWithCollector(const hkpShapeRayCastInput& input, const hkpCdBody& cdBody, hkpRayHitCollector& collector) const HK_OVERRIDE;

  /// \brief
  ///   hkReferencedObject implementation.
  virtual const hkClass* getClassType() const HK_OVERRIDE { return &hkvTriSampledHeightFieldBvTreeShapeClass; }

};

#endif // VHAVOKVISIONSHAPES_H_INCLUDED

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
