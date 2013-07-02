/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file SectorTile.hpp

#ifndef SECTORTILE_HPP_INCLUDED
#define SECTORTILE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>

class VTerrainSector;
class VTerrainVisibilityInfo;
class VTerrainDecorationInstance;
class VTerrainVisibilityCollectorComponent;
struct VTerrainSectorDecorationVisibilityMask;

#define VSECTORTILEFLAGS_NONE   0x00000000
#define VSECTORTILEFLAGS_HOLE   V_BIT(0)


/// \brief
///   VSectorTile defines a tile inside a sector. Tiles have more fine-grained information about the terrain such as decoration references or holes.
///
/// However, tiles are not the granularity for the render draw calls. Draw calls are reprsented by class VTerrainSectorMeshPageInfo.
/// The number of tiles per sector can be specified at terrain creation time. Typically this is something like 32x32. Tiles also define the granularity for holes in the terrain.
class VSectorTile
{
public:
  ///\brief
	/// Constructor
	TERRAIN_IMPEXP VSectorTile();

  ///\brief
	///Destructor
	TERRAIN_IMPEXP ~VSectorTile();

  /// \brief
	///   Internal function to initialize this tile
	///
	/// \param pOwner
	///   Owner sector
	///
	/// \param iIndexX
	///   Tile index inside sector
	///
	/// \param iIndexY
	///   Tile index inside sector
	///
	/// \param bbox
	///   Default bounding box of the tile
	///
	TERRAIN_IMPEXP void Init(VTerrainSector *pOwner, int iIndexX, int iIndexY, const hkvAlignedBBox &bbox);

  /// \brief
	///   Run visibility on this tile
	///
	/// \param infoComp
	///   Visibility context information
	///
	/// \param pDecoVisMask
	///   Mask to tag visibility results
	///
	TERRAIN_IMPEXP void PerformVisibility(VTerrainVisibilityCollectorComponent &infoComp, VTerrainSectorDecorationVisibilityMask *pDecoVisMask);

  /// \brief
	///   Serialize tile information
	///
	/// \param ar
	///   Binary archive
	///
	/// \param iLocalVersion
	///   Local version from sector
	///
	TERRAIN_IMPEXP void SerializeX(VArchive &ar, int iLocalVersion);

  /// \brief
	///   Indicates whether this tile is a hole or not
	///
	/// \returns
	///   true if this tile is a hole.
	inline bool IsHole() const {return (m_iTileFlags&VSECTORTILEFLAGS_HOLE)!=0;}

  /// \brief
	///   Removes all decoration instance references from this tile. Note that relevant decoration instances can still be referenced by neighbor tiles
  inline void RemoveAllDecorationReferences()
  {
    m_iReferencedDecorationCount = 0;
  }

  /// \brief
	///   Selectively removes the reference of the specified instance index. The index is an index from the sector's Decoration collection
  TERRAIN_IMPEXP void RemoveDecorationInstanceReference(int iIndex);

  /// \brief
	///   Selectively removes the references that are marked in the passed bit field. The bits in the passed bit field correspond to indices from the sector's Decoration collection
  TERRAIN_IMPEXP void RemoveDecorationInstanceReferences(const VTBitfield<1024> &removeIndices);

public:

  VTerrainSector *m_pOwner;   ///< owner sector
  short m_iIndexX, m_iIndexY; ///< index in the sector
  hkvAlignedBBox m_TileBBox;  ///< bounding box of this tile, including vegetation in z-dir
  float m_fMaxDecorationFarClip; ///< internal value 
  int m_iTileFlags; ///< internal value 
  int m_iFirstReferencedDecoration; ///< vegetation reference range in sector
  int m_iReferencedDecorationCount; ///< vegetation reference range in sector
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
