/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file SectorVisibilityZone.hpp

#ifndef SECTORVISIBILITYZONE_HPP_INCLUDED
#define SECTORVISIBILITYZONE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>

class VTerrain;
class VTerrainSector;
class VTerrainVisibilityInfo;
class VTerrainVisibilityCollectorComponent;

/// \brief
///   Implements a VisVisibilityZone_cl with terrain specific features. There is a 1:1 relationship
///   between sectors and sector visibility zones
class VSectorVisibilityZone : public VisVisibilityZone_cl
{
public:
  /// \brief
	///   Constructor
	///
	/// \param pSector
	///   Owner sector
	TERRAIN_IMPEXP VSectorVisibilityZone(VTerrainSector *pSector);

  /// \brief
	///   Constructor
	///
	TERRAIN_IMPEXP VSectorVisibilityZone(); ///< for serialization

  /// \brief
	///   Overridden VisVisibilityZone_cl function
	///
	/// \param pCollector
	///   See base class description.
	///
	/// \param pFrustum
	///   See base class description.
	///
	/// \returns
	///   See base class description.
	///
	/// \remarks
	///   Write remarks for OnTagVisible here.
	///
	/// \see
	///   VisVisibilityZone_cl
	TERRAIN_IMPEXP virtual void OnTagVisible(IVisVisibilityCollector_cl *pCollector, VisFrustum_cl *pFrustum);

  /// \brief
	///   Overridden VisVisibilityZone_cl function
	///
	/// \param pManager
	///   See base class description.
	///
	/// \returns
	///   See base class description.
	///
	/// \see
	///   VisVisibilityZone_cl
	TERRAIN_IMPEXP virtual void OnAddedToSceneManager(IVisSceneManager_cl *pManager);

  /// \brief
	///   Overridden VisVisibilityZone_cl function
	///
	/// \param pManager
	///   See base class description.
	///
	/// \returns
	///   See base class description.
	///
	/// \see
	///   VisVisibilityZone_cl
	TERRAIN_IMPEXP virtual void OnRemovedFromSceneManager(IVisSceneManager_cl *pManager);

  /// \brief
	///   Overridden VisVisibilityZone_cl function
	///
	/// \param pSceneManager
	///   See base class description.
	///
	/// \returns
	///   See base class description.
	///
	/// \see
	///   VisVisibilityZone_cl
	TERRAIN_IMPEXP virtual void OnFinishVisibilityBuilt(IVisSceneManager_cl *pSceneManager);

	V_DECLARE_SERIAL_DLLEXP( VSectorVisibilityZone,  TERRAIN_IMPEXP )

  /// \brief
	///   Overridden Serialize function
	///
	/// \param ar
	///   Binary archive
	///
	TERRAIN_IMPEXP virtual void Serialize( VArchive &ar );

  VTerrain *m_pTerrain;       ///< owner terrain
  VTerrainSector *m_pSector;  ///< owner sector
};

typedef VSmartPtr<VSectorVisibilityZone> VSectorVisibilityZonePtr;

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
