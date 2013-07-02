/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file DecorationDensityChannel.hpp

#ifndef DECORATIONDENSITYCHANNEL_HPP_INCLUDED
#define DECORATIONDENSITYCHANNEL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/LuminanceChannel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>



/// \brief
///   Per sector/per decoration model information about the decoration distribution (density)
///
/// The editor uses and stores this information so decoration can always be re-constructed from this density information.
/// The retail runtime on the other hand has all decoration positions pre-baked and thus does not need the density information.
class VDecorationDensityChannel : public VLuminanceChannel
{
public:
  inline VDecorationDensityChannel(IVTerrainDecorationModel *pSource, const VTerrainConfig &cfg)
    : VLuminanceChannel(cfg.m_iDensityMapSamplesPerSector[0],cfg.m_iDensityMapSamplesPerSector[1], 0) ///< start with black
  {
    VASSERT(pSource);
    m_spSource = pSource;
    m_spSource->EnsureLoaded();
  }

  TERRAIN_IMPEXP void GetLockInfo(VTerrainSector *pSector, VTerrainLockObject &destInfo);
  TERRAIN_IMPEXP virtual void CopyFrom(const VLuminanceChannel *pOther)
  {
    VLuminanceChannel::CopyFrom(pOther); ///< deep copies the data
    m_spSource = ((VDecorationDensityChannel *)pOther)->m_spSource;
  }

  TERRAIN_IMPEXP static VDecorationDensityChannel *ChunkFileExchange(VChunkFile &file, VDecorationDensityChannel *pChannel, VEditableTerrain *pTerrain);

  inline bool IsValid() const
  {
    return m_spSource!=NULL && m_spSource->IsValid();
  }

  inline bool IsRelevantForSaving() const
  {
    return IsValid() && !IsBlack();
  }


  IVTerrainDecorationModelPtr m_spSource;
};

typedef VSmartPtr<VDecorationDensityChannel> VDecorationDensityChannelPtr;



/// \brief
///   Refcounted collection for instances of type VDecorationDensityChannel
class VDecorationDensityChannelCollection : public VRefCountedCollection<VDecorationDensityChannel>
{
public:
  TERRAIN_IMPEXP VDecorationDensityChannel *FindModelChannel(IVTerrainDecorationModel *pModel) const;
  TERRAIN_IMPEXP VDecorationDensityChannel *FindModelChannelByID(int iID) const;
  TERRAIN_IMPEXP VDecorationDensityChannel *GetModelChannel(IVTerrainDecorationModel *pModel, const VTerrainConfig &cfg);
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
