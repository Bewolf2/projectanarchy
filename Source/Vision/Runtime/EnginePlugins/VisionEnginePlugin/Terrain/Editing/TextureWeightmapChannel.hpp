/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TextureWeightmapChannel.hpp

#ifndef TEXTUREWEIGHTMAPCHANNEL_HPP_INCLUDED
#define TEXTUREWEIGHTMAPCHANNEL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/LuminanceChannel.hpp>
class VEditableTerrain;
class VTerrainConfig;
class IVTerrainTextureBaker;

/// \brief
///   Represents the resource part of a luminance channel, i.e. decribes the textures to mix etc.
class VTextureWeightmapChannelResource : public VRefCounter
{
public:
  ///\brief
	///Defines detail texture properties
	///
	struct DetailTextureProperties_t
  {
    DetailTextureProperties_t() : m_vTilingAndOfs(40.f,40.f,0.f,0.f)
    {
      m_iTextureID = 0;
      m_bUseThreeWayMapping = false;
      m_fBakePriority = 1.0f;
      m_iMaterialID = -1;
      m_bUseClamping = false;
      m_bHidden = false;
    }
    ///\brief
    ///Compares two instances
    ///
    ///\param other
    ///Other object
    ///
    ///\returns
    ///Reference to 'this'
    TERRAIN_IMPEXP bool operator == (const DetailTextureProperties_t &other) const;

    ///\brief
    ///Compares two instances
    ///
    ///\param other
    ///Other object
    ///
    ///\returns
    ///Reference to 'this'
    TERRAIN_IMPEXP bool operator != (const DetailTextureProperties_t &other) const;

    ///\brief
    ///Assignemtn operator
    ///
    ///\param other
    ///Other object
    ///
    ///\returns
    ///Reference to 'this'
    TERRAIN_IMPEXP DetailTextureProperties_t& operator = (const DetailTextureProperties_t &other);

    ///\brief
    ///Serializes information to or from file
    ///
    ///\param file
    ///Chunk file
    TERRAIN_IMPEXP void ChunkFileExchange(VChunkFile &file);

    int m_iTextureID; /// < internal ID
    hkvVec4 m_vTilingAndOfs;  ///< tiling of the detail texture
    bool m_bUseThreeWayMapping;
    bool m_bHidden;

    VTextureObjectPtr m_spDiffuseTexture;  ///< the detail texture object
    VTextureObjectPtr m_spNormalmapTexture;  ///< the normalmap texture object
    VTextureObjectPtr m_spSpecularTexture;  ///< the specularmap texture object
    float m_fBakePriority;  ///< As defined in vForge
    bool m_bUseClamping;    ///< sampler clamping
    short m_iMaterialID;    ///< value used for material map
    VString m_sDiffuseTexFile,m_sNormalmapTexFile,m_sSpecularTexFile; ///< texture filenames
    
    TERRAIN_IMPEXP void GetFinalTileAndOfs(const VTerrainConfig &config,hkvVec4& dest, int iSectorX, int iSectorY) const;

    VTextureObject *GetDiffuseTexture(int iSectorX, int iSectorY)   const {return GetPerSectorTexture(iSectorX, iSectorY, m_spDiffuseTexture, m_sDiffuseTexFile, VTM_FLAG_DEFAULT_MIPMAPPED|VTM_FLAG_ASSUME_SRGB);}
    VTextureObject *GetNormalmapTexture(int iSectorX, int iSectorY) const {return GetPerSectorTexture(iSectorX, iSectorY, m_spNormalmapTexture, m_sNormalmapTexFile, VTM_FLAG_DEFAULT_MIPMAPPED);}
    VTextureObject *GetSpecularTexture(int iSectorX, int iSectorY)  const {return GetPerSectorTexture(iSectorX, iSectorY, m_spSpecularTexture, m_sSpecularTexFile, VTM_FLAG_DEFAULT_MIPMAPPED);}

    static VTextureObject *GetPerSectorTexture(int iSectorX, int iSectorY, VTextureObject *pTex, const VString &texname, int iFlags)
    {
      if (texname.IsEmpty())
        return pTex;
      char szBuffer[FS_MAX_PATH];
      sprintf(szBuffer,texname.AsChar(),iSectorX,iSectorY);
      pTex = Vision::TextureManager.Load2DTexture(szBuffer, iFlags);

      // fixup SRGB flag, if required for old maps
      bool bWantsSRGB = (iFlags&VTM_FLAG_ASSUME_SRGB) != 0;
      if (pTex!=NULL && !pTex->IsMissing())
        pTex->SetSRGB(bWantsSRGB); // if set already, this function does not do anything
      return pTex;
    }
  };

  VTextureWeightmapChannelResource(const DetailTextureProperties_t &props) 
  {
    m_iSortingKey = 0;
    m_Properties = props;
  }

  // marks the resource as dead
  void FlagForRemoval()
  {
    m_Properties.m_iTextureID = 0; ///< invalid ID
    m_Properties.m_spDiffuseTexture = NULL;
    m_Properties.m_spNormalmapTexture = NULL;
    m_Properties.m_spSpecularTexture = NULL;
    m_Properties.m_sDiffuseTexFile.Reset();
    m_Properties.m_sNormalmapTexFile.Reset();
    m_Properties.m_sSpecularTexFile.Reset();
  }

  inline bool IsValid() const
  {
    return m_Properties.m_iTextureID>0;
  }

  inline bool IsHidden() const
  {
    return m_Properties.m_bHidden;
  }

  inline void SetHidden(bool bState)
  {
    m_Properties.m_bHidden = bState;
  }

  DetailTextureProperties_t m_Properties;
  int m_iSortingKey;
};



typedef VSmartPtr<VTextureWeightmapChannelResource> VTextureWeightmapChannelResourcePtr;


/// \brief
///   Represents a per-sector instance of VTextureWeightmapChannelResource. An instance holds the weighting of a single detail texture on a single sector.
class VTextureWeightmapChannel : public VLuminanceChannel
{
public:
  inline VTextureWeightmapChannel(VTextureWeightmapChannelResource *pRes, int iResX, int iResY) : VLuminanceChannel(iResX,iResY, 0)
  {
    m_spResource = pRes;
  }

  inline bool IsValid() const
  {
    return m_spResource!=NULL && m_spResource->IsValid();
  }

  inline bool IsRelevantForSaving() const
  {
    return IsValid() && !IsBlack();
  }

  TERRAIN_IMPEXP virtual void CopyFrom(const VLuminanceChannel *pOther)
  {
    VLuminanceChannel::CopyFrom(pOther); ///< deep copies the data
    m_spResource = ((VTextureWeightmapChannel *)pOther)->m_spResource;
  }

  // additionally weight with the resource's priority
  TERRAIN_IMPEXP virtual float AlphaMaskCompare(const hkvRect& destRect, const hkvRect& srcRect, const VLuminanceChannel *pOther) const HKV_OVERRIDE
  {
    float fValue = VLuminanceChannel::AlphaMaskCompare(destRect, srcRect, pOther);
    VASSERT(m_spResource);
    fValue *= m_spResource->m_Properties.m_fBakePriority;
    return fValue;
  }


  TERRAIN_IMPEXP static VTextureWeightmapChannel *ChunkFileExchange(VChunkFile &file, VTextureWeightmapChannel *pChannel, VEditableTerrain *pTerrain);

  // additional members
  VTextureWeightmapChannelResourcePtr m_spResource;

};



/// \brief
///   Refcounted collection for classes of type VTextureWeightmapChannel
class VTextureWeightmapChannelCollection : public VRefCountedCollection<VTextureWeightmapChannel>
{
public:
  TERRAIN_IMPEXP VTextureWeightmapChannel* FindChannel(VTextureWeightmapChannelResource *pRes) const;
  TERRAIN_IMPEXP VTextureWeightmapChannel* FindChannelByResourceID(int iID) const;
  TERRAIN_IMPEXP int FindChannelIndex(VTextureWeightmapChannelResource *pRes) const;
  TERRAIN_IMPEXP VTextureWeightmapChannel* CreateChannel(const VTerrainConfig &config, VTextureWeightmapChannelResource *pRes, int iResX, int iResY);
  TERRAIN_IMPEXP bool RemoveChannel(VTextureWeightmapChannelResource *pRes);
  
  TERRAIN_IMPEXP int GetMostRelevantChannels(const hkvRect& region, VTextureWeightmapChannel **pDestList, int iMaxCount, float fTolerance=1.f/255.f);

  TERRAIN_IMPEXP int BakeWeightMaps(unsigned int maxChannels, float fTolerance, const hkvRect& region, VisBitmap_cl** weightMaps, VTextureWeightmapChannel** usedChannels);
  TERRAIN_IMPEXP VLuminanceChannel* CreateMaterialIDMap(const VTerrainConfig &config, VLuminanceChannel* pOldIDMap) const;

  static inline int CompareResourceSortingKey( const void *arg1, const void *arg2)
  {
    VTextureWeightmapChannel *pElem1 = *(VTextureWeightmapChannel **)arg1;
    VTextureWeightmapChannel *pElem2 = *(VTextureWeightmapChannel **)arg2;
    return pElem1->m_spResource->m_iSortingKey - pElem2->m_spResource->m_iSortingKey; ///< ascending
  }

  inline void SortByResourceSortingKey(bool bRemoveSortingKeyMinusOne)
  {
    if (bRemoveSortingKeyMinusOne)
    {
      for (int i=0;i<Count();i++)
      {
        if (GetAt(i)->m_spResource->m_iSortingKey==-1)
        {
          RemoveAt(i);
          i--;
        }
      }
    }
    int iCount = Count();
    if (iCount<2) 
      return;
    qsort(this->GetPtrs(),iCount,sizeof(VTextureWeightmapChannel *),CompareResourceSortingKey);

  }

};


/// \brief
///   Refcounted collection for classes of type VTextureWeightmapChannelResource
class VTextureWeightmapChannelResourceCollection : public VRefCountedCollection<VTextureWeightmapChannelResource>
{
public:
  TERRAIN_IMPEXP VTextureWeightmapChannelResource *CreateDetailTexture(const VTextureWeightmapChannelResource::DetailTextureProperties_t &props);
  TERRAIN_IMPEXP VTextureWeightmapChannelResource *FindByID(int iID) const;

  static inline int CompareResourceSortingKey( const void *arg1, const void *arg2)
  {
    VTextureWeightmapChannelResource *pElem1 = *(VTextureWeightmapChannelResource **)arg1;
    VTextureWeightmapChannelResource *pElem2 = *(VTextureWeightmapChannelResource **)arg2;
    return pElem1->m_iSortingKey - pElem2->m_iSortingKey; ///< ascending
  }

  inline void EnumSortingKey()
  {
    for (int i=0;i<Count();i++)
    {
      GetAt(i)->m_iSortingKey = i;
    }
  }

  inline void SortByResourceSortingKey()
  {
    qsort(this->GetPtrs(), Count(), sizeof(VTextureWeightmapChannelResource *),CompareResourceSortingKey);
  }
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
