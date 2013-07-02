/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokCachedShape.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKCACHEDSHAPE_HPP_INCLUDED
#define VHAVOKCACHEDSHAPE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

#ifdef SUPPORTS_TERRAIN
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#endif

class hkpShape;

/// \brief
///   Class with static functions for saving/ loading Havok hkpShapes to/ from file
class vHavokCachedShape
{
public:

  ///
  /// @name Saving
  /// @{
  ///

  /// \brief
  ///   Saves hkvConvexVerticeShape to HKT file. 
  ///
  /// \param pMesh
  ///   Pointer to the mesh for which shape was created.
  /// 
  /// \param vScale
  ///   Scale of the corresponding mesh.
  /// 
  /// \param bShrinkToFit
  ///   Specifies whether convex shape is shrinked by convex radius so that it's surface is as close as possible to the graphical one.
  /// 
  /// \param pShape
  ///   Pointer to the shape to save.
  static bool SaveConvexShape(const VBaseMesh *pMesh, const hkvVec3& vScale, bool bShrinkToFit, const hkvConvexVerticesShape *pShape);

  /// \brief
  ///   Saves hkvBvCompressedMeshShape to HKT file. 
  ///
  /// \param pMesh
  ///   Pointer to the mesh for which shape was created.
  /// 
  /// \param vScale
  ///   Scale of the corresponding mesh.
  /// 
  /// \param eCollisionBehavior
  ///   Collision behavior.
  ///
  /// \param eWeldingType
  ///   Welding type.
  ///
  /// \param pShape
  ///   Pointer to the shape to save.
  static bool SaveMeshShape(const VBaseMesh *pMesh, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, 
                            VisWeldingType_e eWeldingType, const hkvBvCompressedMeshShape *pShape);

#ifdef SUPPORTS_TERRAIN
  /// \brief
  ///   Saves a terrain sector related shape to HKT file.
  ///
  /// \param pSector
  ///   Pointer to the terrain sector for which shape was created.
  /// 
  /// \param pShape
  ///   Pointer to the shape to save.
  static bool SaveTerrainSectorShape(const VTerrainSector *pSector, const hkpShape *pShape);
#endif

  ///
  /// @}
  ///


  ///
  /// @name Loading
  /// @{
  ///

  /// \brief
  ///   Loads hkvConvexVerticeShape from HKT file. 
  ///
  /// \param pMesh
  ///   Pointer to the mesh for which shape was created.
  /// 
  /// \param vScale
  ///   Scale of the corresponding mesh.
  /// 
  /// \param bShrinkToFit
  ///   Specifies whether convex shape is shrinked by convex radius so that it's surface is as close as possible to the graphical one.
  /// 
  /// \returns 
  ///   Pointer to loaded shape if succeeded, otherwise NULL.
  static hkvConvexVerticesShape* LoadConvexShape(VBaseMesh *pMesh, const hkvVec3& vScale, bool bShrinkToFit);

  /// \brief
  ///   Loads hkvBvCompressedMeshShape from HKT file. 
  ///
  /// \param pMesh
  ///   Pointer to the mesh for which shape was created.
  /// 
  /// \param vScale
  ///   Scale of the corresponding mesh.
  /// 
  /// \param eCollisionBehavior
  ///   Collision behavior.
  ///
  /// \param eWeldingType
  ///   Welding type.
  ///
  /// \returns 
  ///   Pointer to loaded shape if succeeded, otherwise NULL.
  static hkvBvCompressedMeshShape* LoadMeshShape(VBaseMesh *pMesh, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, 
                                                 VisWeldingType_e eWeldingType);

#ifdef SUPPORTS_TERRAIN
  /// \brief
  ///   Loads a terrain sector related shape from HKT file. 
  ///
  /// \param pSector
  ///   Pointer to the terrain sector for which shape was created.
  /// 
  /// \returns 
  ///   Pointer to loaded shape if succeeded, otherwise NULL.
  static hkpShape* LoadTerrainSectorShape(const VTerrainSector *pSector);
#endif

  ///
  /// @}
  ///


  ///
  /// @name Configuration
  /// @{
  ///

  /// \brief
  ///   Returns whether HKT files are checked on loading for being up to date.
  VHAVOK_IMPEXP static inline bool IsHktUpToDateCheckingEnabled() 
  {
    return s_bCheckHktUpToDate;
  }

  /// 
  /// \brief
  ///   Sets whether HKT files are checked on loading for being up to date.
  /// 
  /// \param bEnable
  ///   Toggles up to date checking of HKT files.
  /// 
  /// \note
  ///   Per default this feature is only enabled for debug and development builds.
  VHAVOK_IMPEXP static inline void EnableHktUpToDateChecking(bool bEnable)
  {
    s_bCheckHktUpToDate = bEnable;
  }

  ///
  /// @}
  ///


  ///
  /// @name Path helpers 
  /// @{
  ///

  /// \brief
  ///   Gets path to a convex shape.
  VHAVOK_IMPEXP static void GetConvexShapePath(VStaticString<FS_MAX_PATH>& szInOut, const hkvVec3& vScale, bool bShrinkToFit);

  /// \brief
  ///   Gets path to a mesh shape.
  VHAVOK_IMPEXP static void GetMeshShapePath(VStaticString<FS_MAX_PATH>& szInOut, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, 
                                             VisWeldingType_e eWeldingType);

#ifdef SUPPORTS_TERRAIN
  /// \brief
  ///   Gets path to a terrain sector shape.
  VHAVOK_IMPEXP static void GetTerrainSectorShapePath(VStaticString<FS_MAX_PATH>& szInOut, VTerrainSector::VPhysicsType_e ePhysicsType, bool bHasHoles);
#endif

  ///
  /// @}
  ///

private:

  static bool SaveShape(const char *szCachedShapeName, const hkClass& expectedClass, const hkpShape *pShape);

  static hkpShape* LoadShape(const char *szCachedShapeName, const hkClass &expectedClass);

  static bool IsHktUpToDate(VBaseMesh *pMesh, hkpShape *pShape, const hkClass &expectedClass);

  static bool s_bCheckHktUpToDate; // When true, HKT files are checked on loading whether they are still up to date.

};


#endif // VHAVOKCACHEDSHAPE_HPP_INCLUDED

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
