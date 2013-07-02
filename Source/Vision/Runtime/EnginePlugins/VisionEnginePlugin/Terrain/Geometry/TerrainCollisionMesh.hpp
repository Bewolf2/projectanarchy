/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainCollisionMesh.hpp

#ifndef TERRAINCOLLISIONMESH_HPP_INCLUDED
#define TERRAINCOLLISIONMESH_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>

class VTerrainSector;


/// \brief
///   Resource manager that keeps track of all terrain collision meshes
class VTerrainCollisionMeshManager : public VisResourceManager_cl
{
public:
  ///\brief
	///Constructor
	TERRAIN_IMPEXP VTerrainCollisionMeshManager();
};


/// \brief
///   A grid of n x n small collision meshes, where n is the number of tile subdivisions of the
///   sectors. Each mesh (VSimpleCollisionMesh) is as large as a tile.
class VSectorCollisionMeshes : public VManagedResource
{
public:
  /// \brief
	///   Constructor
	///
	/// \param pOwnerSector
	///   owner sector
	TERRAIN_IMPEXP VSectorCollisionMeshes(VTerrainSector *pOwnerSector);

  /// \brief
	///   Destructor
	TERRAIN_IMPEXP virtual ~VSectorCollisionMeshes();

  /// \brief
	///   Access a tile's collision mesh inside this sector.
	///
	/// \param iTileX
	///   tile index inside sector
	///
	/// \param iTileY
	///   tile index inside sector
	///
	/// \returns
	///   collision mesh for the tile
	///
	inline VSimpleCollisionMesh* GetTileMesh(int iTileX, int iTileY) const
  {
    VASSERT(iTileX>=0 && iTileX<m_iCountX);
    VASSERT(iTileY>=0 && iTileY<m_iCountY);
    return &m_pCollMesh[iTileY*m_iCountX + iTileX];
  }

  /// \brief
	///   Invalidate a range of meshes (e.g. after editing).
	///
	/// \param iTileX1
  ///   tile range (min x)
	///
	/// \param iTileY1
	///   tile range (min y)
	///
	/// \param iTileX2
	///   tile range (max x)
	///
	/// \param iTileY2
	///   tile range (max y)
	inline void InvalidateMeshes(int iTileX1, int iTileY1, int iTileX2, int iTileY2)
  {
    VASSERT(iTileX1>=0 && iTileX2<m_iCountX && iTileX2>=iTileX1);
    VASSERT(iTileY1>=0 && iTileY2<m_iCountY && iTileY2>=iTileY1);
    for (int y=iTileY1;y<=iTileY2;y++)
      for (int x=iTileX1;x<=iTileX2;x++)
        m_pCollMesh[y*m_iCountX + x].SetPrimitiveCount(0);
  }

  /// \brief
	///   Same as InvalidateMeshes(0,0,m_iCountX-1,m_iCountY-1)
	inline void InvalidateAllMeshes()
  {
    InvalidateMeshes(0,0,m_iCountX-1,m_iCountY-1);
  }

  /// \brief
	///   Internal function for memory statistics.
	TERRAIN_IMPEXP void EvaluateMemSize();

protected:
  /// \brief
	///   Overridden reload function
	///
	/// \returns
	///   always TRUE
	///
	TERRAIN_IMPEXP virtual BOOL Reload() {return TRUE;}

  /// \brief
	///   Overridden unload function
	///
	/// \returns
	///   always TRUE
	///
  TERRAIN_IMPEXP virtual BOOL Unload();

  VSimpleCollisionMesh *m_pCollMesh;      ///< Same amount of collision meshes.
  short m_iCountX,m_iCountY;        ///< Same as number of tiles in a sector.
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
