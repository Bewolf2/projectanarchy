/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokShapeFactory.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKSHAPEFACTORY_HPP_INCLUDED
#define VHAVOKSHAPEFACTORY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

#include<Common/Base/Container/StringMap/hkStorageStringMap.h>

#define HKVIS_MESH_SHAPE_TOLERANCE 0.1f
#define HKVIS_CONVEX_SHAPE_TOLERANCE 0.01f

typedef hkStorageStringMap<hkpShape*> vHavokShapeCache;

/// 
/// \brief
///   Factory class with static functions to create Havok shapes.
/// 
class vHavokShapeFactory
{
public:

  ///
  /// @name Init/ Deinit
  /// @{
  ///

  /// 
  /// \brief
  ///   Initializes shape factory.
  /// 
  static void Init();

  /// 
  /// \brief
  ///   Deinitializes shape factory.
  /// 
  static void Deinit();

  ///
  /// @}
  ///


  ///
  /// @name Havok Shape Creation
  /// @{
  ///

  /// 
  /// \brief
  ///   Enumeration of Havok shape creation flags
  ///
  enum VShapeCreationFlags_e
  {
    VShapeCreationFlags_CACHE_SHAPE         = V_BIT(0), ///< Allow runtime/ disc shape caching 
    VShapeCreationFlags_USE_VCOLMESH        = V_BIT(1), ///< Try using collision mesh (.vcolmesh)
    VShapeCreationFlags_SHRINK              = V_BIT(2), ///< Shrink by the convex radius
    VShapeCreationFlags_ALLOW_PERTRICOLINFO = V_BIT(3)  ///< Allow per triangle collision info for static mesh shapes
  };

  /// 
  /// \brief
  ///   Factory function to create a Havok shape based on a convex hull (hkvConvexVerticesShape).
  /// 
  /// This function takes the vertices data of the given mesh, respectively the corresponding collision mesh,
  /// and creates a convex geometry which is used for the Havok shape representation. This shape gets cached 
  /// respective to the mesh's file name and the scaling of this shape.
  ///
  /// \param pMesh
  ///   Input: Pointer to the mesh.
  ///
  /// \param vScale
  ///   Input: Scaling vector for the Havok convex shape.
  ///
  /// \param szShapeCacheId
  ///   Output: ID string of corresponding shape into the cache table (points to memory in cache table), if shape could be retrieved.
  ///
  /// \param iCreationFlags
  ///   Input: Bit mask for multiple VShapeCreationFlags_e flags.
  ///
  /// \returns
  ///   Pointer to Havok shape based on a convex hull.
  /// 
  VHAVOK_IMPEXP static hkRefNew<hkpShape> CreateConvexHullShapeFromMesh(VBaseMesh* pMesh, const hkvVec3& vScale, const char **szShapeCacheId, 
                                                                        int iCreationFlags = VShapeCreationFlags_CACHE_SHAPE | VShapeCreationFlags_USE_VCOLMESH);


  /// 
  /// \brief
  ///   Factory function to create a Havok shape based on the given mesh data (hkvBvCompressedMeshShape).
  /// 
  /// This function takes the geometry data of the given mesh, respectively the corresponding collision mesh,
  /// and creates a Havok mesh shape. This shape gets cached respective to the mesh's file name, scaling, collision behavior
  /// and welding type.
  ///
  /// \param pMesh
  ///   Input: Pointer to the mesh.
  ///
  /// \param vScale
  ///   Input: Scaling vector for the Havok mesh shape.
  ///
  /// \param szShapeCacheId
  ///   Output: ID string of corresponding shape into the cache table (points to memory in cache table), if shape could be retrieved.
  ///
  /// \param iCreationFlags
  ///   Input: Bit mask for multiple VShapeCreationFlags_e flags.
  ///
  /// \param eWeldingType
  ///   Input: Specifies the welding type. 
  ///      
  /// \returns
  ///   Pointer to Havok shape based on the given mesh data.
  /// 
  VHAVOK_IMPEXP static hkRefNew<hkpShape> CreateShapeFromMesh(VBaseMesh* pMesh, const hkvVec3& vScale, const char **szShapeCacheId, 
                                                              int iCreationFlags = VShapeCreationFlags_CACHE_SHAPE | VShapeCreationFlags_USE_VCOLMESH, 
                                                              VisWeldingType_e eWeldingType=VIS_WELDING_TYPE_NONE);


  /// 
  /// \brief
  ///   Factory function to create a Havok shape based on a Havok BvCompressedMesh Shape (hkvBvCompressedMeshShape).
  /// 
  /// \param meshInstances
  ///   Input: Collection of static meshes the hkvBvCompressedMeshShape gets created for.
  ///
  /// \param iCreationFlags
  ///   Input: Bit mask for multiple VShapeCreationFlags_e flags.
  ///
  /// \param szShapeCacheId
  ///   Output: ID string of corresponding shape into the cache table (points to memory in cache table), if shape could be retrieved.
  ///
  /// \returns
  ///   Pointer to Havok shape based on hkvBvCompressedMeshShape.
  /// 
  VHAVOK_IMPEXP static hkRefNew<hkpShape> CreateShapeFromStaticMeshInstances(const VisStaticMeshInstCollection &meshInstances, int iCreationFlags, const char **szShapeCacheId);

#ifdef SUPPORTS_TERRAIN

  /// 
  /// \brief
  ///   Factory function to create a Havok shape based on Havok Heightfield Shape (hkvSampledHeightfield). 
  ///   It can optionally be wrapped in a Triangle sampler so that collisions are exact.
  /// 
  /// \param terrain
  ///   Terrain sector the the Havok Heightfield shape gets created for.
  ///
  /// \returns
  ///   Pointer to Havok shape.
  /// 
  VHAVOK_IMPEXP static hkpShape* CreateShapeFromTerrain(const VTerrainSector &terrain);

#endif

  ///
  /// @}
  ///


  ///
  /// @name Havok Shape Caching
  /// @{
  ///
 
  /// 
  /// \brief
  ///   Adds a Havok shape with the given string ID to the cache table.
  /// 
  /// When adding a shape to the cache we increment the reference count by 1 so we
  /// always keep a reference to the shape even all rigid bodies that reference this 
  /// shape were removed. Therefore we don't need to create a shape multiple times
  /// when it has been added once.
  ///
  /// \param szShapeId
  ///   ID string for the shape to be added.
  /// 
  /// \param pShape
  ///   Pointer to Havok shape to be cached.
  /// 
  /// \return
  ///   ID string of added shape (points to memory in cache table).
  /// 
  static const char* AddShape(const char *szShapeId, hkpShape *pShape);

  /// 
  /// \brief
  ///   Removes a single shape from the cache if nothing references it.
  /// 
  /// \param szShapeId
  ///   The ID of the shape to remove.
  ///
  static void RemoveShape(const char *szShapeId);

  /// 
  /// \brief
  ///   Purges all cached shapes.
  /// 
  /// When adding a shape to the cache we increment the reference count by 1 so we
  /// always keep a reference to the shape even all rigid bodies that reference this 
  /// shape were removed. Therefore we don't need to create a shape multiple times
  /// when it has been added once.
  ///
  /// When clearing the cache we check whether all shapes have a reference count 
  /// of 1, meaning all rigid bodies removed their reference from this shape and only the
  /// cache keeps one single reference. This is the proper behavior. 
  ///
  static bool ClearCache();

  /// 
  /// \brief
  ///   Finds a Havok shape in the cache table.
  /// 
  /// \param szShapeId
  ///   Input: ID string of the shape to be found.
  /// 
  /// \param szShapeCacheId
  ///   Output (optional): ID string of cached shape (points to memory in cache table), if shape was found.
  /// 
  /// \return
  ///   hkpShape* : Pointer to Havok shape, or NULL if shape could not be found.
  ///
  static hkpShape* FindShape(const char *szShapeId, const char **szShapeCacheId=NULL);

  ///
  /// @}
  ///


  ///
  /// @name Helper
  /// @{
  ///

  /// 
  /// \brief
  ///   Gets the pivot offset of the bounding box in regard to the models pivot
  ///   
  /// \param pMesh
  ///   Pointer to the mesh.
  ///
  /// \param vScaleEntity
  ///   Scaling of the instance the model is applied to. The scaling is considered
  ///   in the computation of the pivot offset.
  ///
  /// \returns
  ///   Pivot offset vector (unscaled: in Vision space)
  /// 
  static hkvVec3 GetPivotOffset(const VDynamicMesh *pMesh, const hkvVec3& vScaleEntity);

  /// 
  /// \brief
  ///   Gets the pivot offset of the bounding box in regard to the models pivot
  ///   
  /// \param pMesh
  ///   Pointer to the mesh.
  ///
  /// \param fUniformScaleEntity
  ///   Uniform scaling of the instance the model is applied to. The scaling is considered
  ///   in the computation of the pivot offset.
  ///
  /// \returns
  ///   Pivot offset vector (unscaled: in Vision space)
  /// 
  static hkvVec3 GetPivotOffset(const VDynamicMesh *pMesh, float fUniformScaleEntity);

  /// 
  /// \brief
  ///   Gets the ID for a convex shape.
  ///   
  /// \param szIDString
  ///   Pointer to a char array of size at least 512 bytes.
  ///
  /// \param szMeshName
  ///   Mesh name to use.
  ///
  /// \param vScale
  ///   Scale to use.
  ///
  /// \param bShrinkByCvxRadius
  ///   Specifies whether convex shape is shrinked by convex radius so that it's surface is as close as possible to the graphical one.
  /// 
  static void GetIDStringForConvexShape(char *szIDString, const char *szMeshName, const hkvVec3& vScale, bool bShrinkByCvxRadius);

  /// 
  /// \brief
  ///   Gets the ID for a mesh shape.
  ///   
  /// \param szIDString
  ///   Pointer to a char array of size at least 512 bytes.
  ///
  /// \param szMeshName
  ///   Mesh name to use.
  ///
  /// \param vScale
  ///   Scale to use.
  ///
  /// \param eCollisionBehavior
  ///   Describes the collision behavior.
  ///
  /// \param eWeldingType
  ///   Specifies the type of welding. 
  ///
  static void GetIDStringForMeshShape(char *szIDString, const char *szMeshName, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior,
                                      VisWeldingType_e eWeldingType);

  /// 
  /// \brief
  ///   Builds a hkGeometry from the specified sub mesh of a collision mesh.
  /// 
  /// \param pColMesh
  ///   Collision mesh to which the sub mesh belongs from which geometry should be build.
  /// 
  /// \param iSubmeshIndex
  ///   Index of a sub mesh within collision mesh from which geometry should be build.  
  ///   
  /// \param transform
  ///   Transformation matrix that should be applied to hkGeometry.
  ///
  /// \param bConvex
  ///   Specifies whether hkGeometry will be used for convex or mesh shapes.
  /// 
  /// \param geom
  ///   Reference to a hkGeometry to which the results are written.
  ///
  static void BuildGeomFromCollisionMesh(const IVCollisionMesh *pColMesh, int iSubmeshIndex, const hkvMat4 &transform, 
                                         bool bConvex, hkGeometry& geom);

  /// \brief
  ///   Extract the non-uniform scaling from a full transformation matrix.
  static void ExtractScaling(const hkvMat4 &mat, hkvVec3& destScaling);  
  
  /// \brief
  ///   Creates a convex hull shape for specified static mesh instances.
  static hkpShape* CreateConvexShapeFromStaticMeshInstances(const VisStaticMeshInstCollection &meshInstances, hkvMat4 &transform, bool shrinkByCvxRadius);

  /// \brief
  ///   Creates a mesh shape for specified static mesh instances.
  static hkpShape* CreateMeshShapeFromStaticMeshInstances(const VisStaticMeshInstCollection &meshInstances, hkvMat4 &transform, 
                                                          bool bAllowPerTriCollisionInfo, VisWeldingType_e eWeldingType);
  
#ifdef SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Gets HKT file dependencies of a static mesh instance for precaching in a resource snapshot.
  static void GetHktDependencies(VResourceSnapshot &snapshot, VisStaticMeshInstance_cl *pMeshInstance);

  /// \brief
  ///   Gets HKT file dependencies of a entity for precaching in a resource snapshot.
  static void GetHktDependencies(VResourceSnapshot &snapshot, VisBaseEntity_cl *pEntity);

  /// \brief
  ///   Gets HKT file dependencies of a terrain sector for precaching in a resource snapshot.
  static void GetHktDependencies(VResourceSnapshot &snapshot, VTerrainSector *pSector);

#endif

  ///
  /// @}
  ///

private:
  static vHavokShapeCache *m_pShapeCacheTable;  ///< Cached shapes stored in hash map.
 
};

#endif // VHAVOKSHAPEFACTORY_HPP_INCLUDED

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
