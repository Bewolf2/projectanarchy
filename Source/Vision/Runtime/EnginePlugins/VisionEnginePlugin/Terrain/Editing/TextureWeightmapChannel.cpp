/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TextureWeightmapChannel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>



VTextureWeightmapChannel* VTextureWeightmapChannelCollection::FindChannel(VTextureWeightmapChannelResource *pRes) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->m_spResource==pRes)
      return GetAt(i);
  return NULL;
}

int VTextureWeightmapChannelCollection::FindChannelIndex(VTextureWeightmapChannelResource *pRes) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->m_spResource==pRes)
      return i;
  return -1;
}


VTextureWeightmapChannel* VTextureWeightmapChannelCollection::FindChannelByResourceID(int iID) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->m_spResource->m_Properties.m_iTextureID==iID)
      return GetAt(i);
  return NULL;
}


VTextureWeightmapChannel* VTextureWeightmapChannelCollection::CreateChannel(const VTerrainConfig &config, VTextureWeightmapChannelResource *pRes, int iResX, int iResY)
{
  if (!pRes)
    return NULL;
  VTextureWeightmapChannel* pFound = FindChannel(pRes);
  if (!pFound)
  {
    pFound = new VTextureWeightmapChannel(pRes,iResX,iResY);
    Add(pFound);
    if (config.m_bSortTextureChannels)
      SortByResourceSortingKey(false); // keep this list sorted
  }
   
  return pFound;
}

bool VTextureWeightmapChannelCollection::RemoveChannel(VTextureWeightmapChannelResource *pRes)
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->m_spResource==pRes)
    {
      RemoveAt(i);
      return true;
    }
  return false;
}

namespace
{
  template<int numChannels> void BakeSingleWeightMap(const hkvRect& region, VisBitmap_cl* bitMap, VTextureWeightmapChannel** usedChannels)
  {
    for(int y = region.getTop(); y < region.getBottom(); y++)
    {
      VColorRef* dest = bitMap->GetDataPtr(region.getLeft(), y);

      for(int x = region.getLeft(); x < region.getRight(); x++)
      {
        dest->r = (numChannels > 0) ? usedChannels[0]->GetValue(x, y) : 0;
        dest->g = (numChannels > 1) ? usedChannels[1]->GetValue(x, y) : 0;
        dest->b = (numChannels > 2) ? usedChannels[2]->GetValue(x, y) : 0;
        dest->a = (numChannels > 3) ? usedChannels[3]->GetValue(x, y) : 0;

        dest++;
      }
    }
  }
}

namespace
{
  struct ChannelAndKey
  {
    VTextureWeightmapChannel* m_channel;
    int m_index;
    float m_key;

    static int Compare(const void* arg1, const void* arg2)
    {
      const ChannelAndKey* channelAndKey1 = reinterpret_cast<const ChannelAndKey*>(arg1);
      const ChannelAndKey* channelAndKey2 = reinterpret_cast<const ChannelAndKey*>(arg2);

      return static_cast<int>(hkvMath::sign(channelAndKey2->m_key - channelAndKey1->m_key));
    }
  };
}

TERRAIN_IMPEXP int VTextureWeightmapChannelCollection::GetMostRelevantChannels(const hkvRect& region, VTextureWeightmapChannel **pDestList, int iMaxCount, float fTolerance/*=1.f/255.f*/ )
{
  fTolerance *= region.getWidth() * region.getHeight();

  hkvRect accumRect(0, 0, region.getWidth(), region.getHeight());

  VLuminanceChannelPtr spAccum = new VLuminanceChannel(region.getWidth(), region.getHeight(), 0);

  // Compute visibility for each channel
  VArray<ChannelAndKey> channels;

  DynArray_cl<bool> channelUsed(Count(), false);

  for(int channelIdx = Count() - 1; channelIdx >= 0; channelIdx--)
  {
    ChannelAndKey channelAndKey;
    channelAndKey.m_index = channelIdx;
    channelAndKey.m_channel = GetAt(channelIdx);

    if(channelAndKey.m_channel->IsBlack() || channelAndKey.m_channel->m_spResource->IsHidden())
    {
      continue;
    }

    channelAndKey.m_key = channelAndKey.m_channel->AlphaMaskCompare(region, accumRect, spAccum);

    if(channelAndKey.m_key < fTolerance)
    {
      continue;
    }

    channels.Append(channelAndKey);

    if (channelIdx > 0)
    {
      spAccum->Blend(accumRect, region, channelAndKey.m_channel);
    }
  }

  qsort(channels.GetData(), channels.GetSize(), sizeof(ChannelAndKey), ChannelAndKey::Compare);

  // Mark at most maxChannels channels in order of relevance
  for(int listIdx = 0; listIdx < channels.GetSize(); listIdx++)
  {
    if(listIdx >= iMaxCount)
    {
      break;
    }

    channelUsed[channels[listIdx].m_index] = true;
  }

  // Write out the marked channels in original order
  int numUsedChannels = 0;
  for(int channelIdx = 0; channelIdx < Count(); channelIdx++)
  {
    if(channelUsed[channelIdx])
    {
      pDestList[numUsedChannels] = GetAt(channelIdx);
      numUsedChannels++;
    }
  }

  return numUsedChannels;
}


int VTextureWeightmapChannelCollection::BakeWeightMaps(unsigned int maxChannels, float fTolerance, const hkvRect& region, VisBitmap_cl** weightMaps, VTextureWeightmapChannel** usedChannels)
{
  int numUsedChannels = GetMostRelevantChannels(region, usedChannels, maxChannels, fTolerance);
  int numWeightMaps = (numUsedChannels + 2) / 4; 
  // Create weight maps
  for (int weightMapIdx = 0; weightMapIdx < numWeightMaps; weightMapIdx++)
  {
    VisBitmap_cl* bitMap = weightMaps[weightMapIdx];

    int channelIdx = 4 * weightMapIdx + 1;

    int remainingChannels = numUsedChannels - channelIdx;
    int channelsToBake = hkvMath::Min(remainingChannels, 4);

    // Dispatch to templated bake method to avoid branch inside inner loop
    switch(channelsToBake)
    {
    case 1:
      BakeSingleWeightMap<1>(region, bitMap, usedChannels + channelIdx);
      break;

    case 2:
      BakeSingleWeightMap<2>(region, bitMap, usedChannels + channelIdx);
      break;

    case 3:
      BakeSingleWeightMap<3>(region, bitMap, usedChannels + channelIdx);
      break;

    case 4:
      BakeSingleWeightMap<4>(region, bitMap, usedChannels + channelIdx);
      break;

      default:
        VASSERT(false);
    }
  }

  return numUsedChannels;
}

VLuminanceChannel* VTextureWeightmapChannelCollection::CreateMaterialIDMap(const VTerrainConfig &config, VLuminanceChannel* pOldIDMap) const
{
  int iSizeX = config.m_iMaterialMapResolution[0];
  int iSizeY = config.m_iMaterialMapResolution[1];
  if (iSizeX==0 || iSizeY==0)
    return NULL;

  int iRelevantCount = 0;
  VTextureWeightmapChannel *pRelevantChannel[1024];

  // Get relevant channels that have a material ID
  for (int iChannel=0;iChannel<Count();iChannel++)
    if (GetAt(iChannel)->m_spResource!=NULL && GetAt(iChannel)->m_spResource->m_Properties.m_iMaterialID>=0 && iRelevantCount<1024)
      pRelevantChannel[iRelevantCount++] = GetAt(iChannel);

  if (iRelevantCount==0)
    return NULL;

  VLuminanceChannel *pIDMap = pOldIDMap;
  if (pIDMap==NULL || pIDMap->m_iSizeX!=iSizeX || pIDMap->m_iSizeY!=iSizeY)
    pIDMap = new VLuminanceChannel(iSizeX,iSizeY,0);

  float fChannelAlpha[1024];
  UBYTE *pDest = pIDMap->GetValuePtr(0,0);
  float fInvSizeX = 1.f/(float)iSizeX;
  float fInvSizeY = 1.f/(float)iSizeY;
  for (int y=0;y<iSizeY;y++)
    for (int x=0;x<iSizeX;x++, pDest++)
    {
      for (int iChannel=0;iChannel<iRelevantCount;iChannel++)
      {
        VTextureWeightmapChannel *pChannel = pRelevantChannel[iChannel];
        // remap to channel's mapping
        int xx = (int) (((float)x+0.5f) * (float)pChannel->m_iSizeX * fInvSizeX);
        int yy = (int) (((float)y+0.5f) * (float)pChannel->m_iSizeY * fInvSizeY);
        float fOpacity = (float)pChannel->GetValue(xx,yy) * (1.f/255.f);
        fChannelAlpha[iChannel] = fOpacity;

        // other channels get multiplied with inverse:
        float fShineThrough = 1.f - fOpacity;
        for (int j=0;j<iChannel;j++)
          fChannelAlpha[j]*=fShineThrough;
      }

      int iBestID = 0;
      float fBestAlpha = 0.f;
      // now see which one is best:
      for (int iChannel=0;iChannel<iRelevantCount;iChannel++)
        if (fChannelAlpha[iChannel]>fBestAlpha)
        {
          iBestID = pRelevantChannel[iChannel]->m_spResource->m_Properties.m_iMaterialID;
          fBestAlpha = fChannelAlpha[iChannel];
        }

      // write best ID
      *pDest = iBestID;
    }

  pIDMap->SetDirtyFlag();
  return pIDMap;
}

#define COMPARE_MEMBER(member) {if (member!=other.member) return false;}

bool VTextureWeightmapChannelResource::DetailTextureProperties_t::operator == (const DetailTextureProperties_t &other) const
{
  COMPARE_MEMBER(m_iTextureID);
  COMPARE_MEMBER(m_iMaterialID);
  COMPARE_MEMBER(m_vTilingAndOfs);
  COMPARE_MEMBER(m_bUseThreeWayMapping);
  COMPARE_MEMBER(m_spDiffuseTexture);
  COMPARE_MEMBER(m_spNormalmapTexture);
  COMPARE_MEMBER(m_spSpecularTexture);
  COMPARE_MEMBER(m_fBakePriority);
  COMPARE_MEMBER(m_sDiffuseTexFile);
  COMPARE_MEMBER(m_sNormalmapTexFile);
  COMPARE_MEMBER(m_sSpecularTexFile);
  COMPARE_MEMBER(m_bUseClamping);
  return true;
}

bool VTextureWeightmapChannelResource::DetailTextureProperties_t::operator != (const DetailTextureProperties_t &other) const
{
  return !((*this) == other);
}



VTextureWeightmapChannelResource::DetailTextureProperties_t& VTextureWeightmapChannelResource::DetailTextureProperties_t::operator = (const DetailTextureProperties_t &other)
{
  COPY_MEMBER(m_iTextureID);
  COPY_MEMBER(m_iMaterialID);
  COPY_MEMBER(m_vTilingAndOfs);
  COPY_MEMBER(m_bUseThreeWayMapping);
  COPY_MEMBER(m_spDiffuseTexture);
  COPY_MEMBER(m_spNormalmapTexture);
  COPY_MEMBER(m_spSpecularTexture);
  COPY_MEMBER(m_fBakePriority);
  COPY_MEMBER(m_sDiffuseTexFile);
  COPY_MEMBER(m_sNormalmapTexFile);
  COPY_MEMBER(m_sSpecularTexFile);
  COPY_MEMBER(m_bUseClamping);
  return *this;
}

void VTextureWeightmapChannelResource::DetailTextureProperties_t::ChunkFileExchange(VChunkFile &file)
{
  char szFilename[FS_MAX_PATH];
  int iVersion = 7;

  if (file.IsLoading())
  {
    int iDummy;
    file.ReadInt(iVersion); VASSERT(iVersion>=0 && iVersion<=7);
    file.ReadInt(m_iTextureID);
        
    if (iVersion>=5)
      file.ReadShort(m_iMaterialID); // version 5

    if (iVersion<=2)
    {
      file.ReadInt(iDummy);//m_iResX
      file.ReadInt(iDummy);//m_iResY
    } else 
    {
      // version 3
      file.ReadFloat(m_fBakePriority);
    }

    if (iVersion>=4)
      file.Readbool(m_bUseClamping);
    if (iVersion>=7)
      file.Readbool(m_bHidden);

    if (iVersion>=2)
      file.Read(&m_vTilingAndOfs,sizeof(hkvVec4),"ffff");
    else
    {
      file.Read(&m_vTilingAndOfs,sizeof(hkvVec2),"ff");
      m_vTilingAndOfs.z = 0.f; m_vTilingAndOfs.w = 0.f;
    }

    if (iVersion>=6)
      file.Readbool(m_bUseThreeWayMapping);

    file.ReadString(szFilename,FS_MAX_PATH);
    if (szFilename[0])
      m_spDiffuseTexture = Vision::TextureManager.Load2DTexture(szFilename, VTM_FLAG_DEFAULT_MIPMAPPED|VTM_FLAG_ASSUME_SRGB);
    if (iVersion>=1)
    {
      file.ReadString(szFilename,FS_MAX_PATH);
      if (szFilename[0])
        m_spNormalmapTexture = Vision::TextureManager.Load2DTexture(szFilename);
      file.ReadString(szFilename,FS_MAX_PATH);
      if (szFilename[0])
        m_spSpecularTexture = Vision::TextureManager.Load2DTexture(szFilename);
    }
  } else
  {
    file.WriteInt(iVersion);
    file.WriteInt(m_iTextureID);
    file.WriteShort(m_iMaterialID); // version 5
    file.WriteFloat(m_fBakePriority); // version 3
    file.Writebool(m_bUseClamping); // version 4
    file.Writebool(m_bHidden); // version 7
    file.Write(&m_vTilingAndOfs,sizeof(hkvVec4),"ffff"); // 4 floats: version 2
    file.Writebool(m_bUseThreeWayMapping);
    if (m_spDiffuseTexture)
      file.WriteString(m_spDiffuseTexture->GetFilename());
    else
      file.WriteString(NULL);
    if (m_spNormalmapTexture)
      file.WriteString(m_spNormalmapTexture->GetFilename());
    else
      file.WriteString(NULL);
    if (m_spSpecularTexture)
      file.WriteString(m_spSpecularTexture->GetFilename());
    else
      file.WriteString(NULL);
  }
}


void VTextureWeightmapChannelResource::DetailTextureProperties_t::GetFinalTileAndOfs(const VTerrainConfig &config, hkvVec4& dest, int iSectorX, int iSectorY) const
{
  dest = m_vTilingAndOfs;
  if (m_bUseClamping)
  {
    float fOfsX = (float)(iSectorX)*hkvMath::Abs (dest.x)/(float)config.m_iSectorCount[0];
    float fOfsY = (float)(iSectorY)*hkvMath::Abs (dest.y)/(float)config.m_iSectorCount[1];
    if (m_vTilingAndOfs.x<0.f)
      dest.z += fOfsX + 1.f;
    else
      dest.z -= fOfsX;
    if (m_vTilingAndOfs.y<0.f)
      dest.w += fOfsY + 1.f;
    else
      dest.w -= fOfsY;
  }
}


VTextureWeightmapChannelResource *VTextureWeightmapChannelResourceCollection::CreateDetailTexture(const VTextureWeightmapChannelResource::DetailTextureProperties_t &props)
{
  VTextureWeightmapChannelResource *pFound = FindByID(props.m_iTextureID);
  if (pFound)
  {
    if (pFound->m_Properties==props)
      return pFound;

    // update properties in existing channel:
    pFound->m_Properties = props;
    return pFound;
  }


  // create a new one
  VTextureWeightmapChannelResource *pNew = new VTextureWeightmapChannelResource(props);
  pNew->m_iSortingKey = Count(); // do not re-enum all others
  Add(pNew);
  return pNew;
}


VTextureWeightmapChannelResource *VTextureWeightmapChannelResourceCollection::FindByID(int iID) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->m_Properties.m_iTextureID==iID)
      return GetAt(i);
  return NULL;
}



// static exchange function
VTextureWeightmapChannel *VTextureWeightmapChannel::ChunkFileExchange(VChunkFile &file, VTextureWeightmapChannel *pChannel, VEditableTerrain *pTerrain)
{
  if (file.IsLoading())
  {
    const VTerrainConfig &cfg(pTerrain->Config());

    int iSizeX,iSizeY;
    VTextureWeightmapChannelResource::DetailTextureProperties_t props;
    props.ChunkFileExchange(file);

    file.ReadInt(iSizeX);
    file.ReadInt(iSizeY);

    VTextureWeightmapChannelResource *pRes = pTerrain->m_WeightmapChannels.FindByID(props.m_iTextureID);

    // discard this channel
    if (!cfg.m_bUseTempFolder && props.m_bHidden)
      pRes = NULL;

    // create it when missing (only outside the editor)
    if (!Vision::Editor.IsInEditor())
    {
      if (!pRes)
        pRes = pTerrain->m_WeightmapChannels.CreateDetailTexture(props);
    }
    if (!pRes || !pRes->IsValid()) // discard this one, but keep the file order intact
    {
      file.SkipBytes(iSizeX*iSizeY);
      return NULL;
    }

    // alllocate the channel with the config's size, and scale in the next function
    pChannel = new VTextureWeightmapChannel(pRes,cfg.m_iDefaultWeightmapResolution[0],cfg.m_iDefaultWeightmapResolution[1]);
    pChannel->ReadScaled(file,iSizeX,iSizeY); // considers and fixes different scaling
    pChannel->SetDirtyFlag();
    return pChannel;
  }

  // saving:
  VASSERT(pChannel);
  pChannel->m_spResource->m_Properties.ChunkFileExchange(file);
  file.WriteInt(pChannel->m_iSizeX);
  file.WriteInt(pChannel->m_iSizeY);
  file.Write(pChannel->GetValuePtr(0,0),pChannel->GetSampleCount());

  return pChannel;
}

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
