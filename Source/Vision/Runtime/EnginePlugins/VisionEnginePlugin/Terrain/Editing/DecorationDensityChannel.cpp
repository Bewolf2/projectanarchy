/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/DecorationDensityChannel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/EditableTerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


void VDecorationDensityChannel::GetLockInfo(VTerrainSector *pSector, VTerrainLockObject &destInfo)
{
  VASSERT(pSector);
  destInfo.Init(pSector->m_iIndexX*m_iSizeX,pSector->m_iIndexY*m_iSizeY,m_iSizeX,m_iSizeY,0);
  destInfo.m_iStride = m_iSizeX;
  destInfo.m_iComponentSize = sizeof(UBYTE);
  destInfo.m_pData = NULL;
  //destInfo.m_pTerrain = 
  destInfo.m_iReserved[0] = m_spSource->GetID();
}


// static exchange function
VDecorationDensityChannel *VDecorationDensityChannel::ChunkFileExchange(VChunkFile &file, VDecorationDensityChannel *pChannel, VEditableTerrain *pTerrain)
{
  const VTerrainConfig &cfg(pTerrain->m_Config);
  int iVersion = 8;
  char szModelFilename[FS_MAX_PATH];
  if (file.IsLoading())
  {
    int iSizeX,iSizeY;
    IVTerrainDecorationModel::ModelProperties_t props;

    file.ReadInt(iVersion);
    IVTerrainDecorationModel::ModelType_e eForcedType = IVTerrainDecorationModel::AnyType;
    if (iVersion>=6)
      file.ReadInt((int &)eForcedType);
    file.ReadString(szModelFilename,FS_MAX_PATH);
    props.ChunkFileExchange(file,iVersion);
    file.ReadInt(iSizeX);
    file.ReadInt(iSizeY);

    IVTerrainDecorationModel *pRes = VTerrainDecorationModelManager::GlobalManager().CreateModel(szModelFilename,props,true,eForcedType);
    if (!pRes || !pRes->IsValid()) // discard this one, but keep the file order intact
    {
      file.SkipBytes(iSizeX*iSizeY);
      return NULL;
    }
    pChannel = new VDecorationDensityChannel(pRes,cfg);
    pChannel->ReadScaled(file,iSizeX,iSizeY); // considers and fixes different scaling
    pChannel->SetDirtyFlag();
    return pChannel;
  }

  // saving:
  VASSERT(pChannel && pChannel->m_spSource);
  file.WriteInt(iVersion);
  file.WriteInt((int)pChannel->m_spSource->GetDecorationType()); // version 6
  file.WriteString(pChannel->m_spSource->GetFilename());
  pChannel->m_spSource->m_Properties.ChunkFileExchange(file,iVersion);
  file.WriteInt(pChannel->m_iSizeX);
  file.WriteInt(pChannel->m_iSizeY);
  file.Write(pChannel->GetValuePtr(0,0),pChannel->GetSampleCount());

  return pChannel;
}



VDecorationDensityChannel *VDecorationDensityChannelCollection::FindModelChannel(IVTerrainDecorationModel *pModel) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->m_spSource == pModel)
      return GetAt(i);
  return NULL;
}


VDecorationDensityChannel *VDecorationDensityChannelCollection::FindModelChannelByID(int iID) const
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->m_spSource->GetID() == iID)
      return GetAt(i);
  return NULL;
}


VDecorationDensityChannel *VDecorationDensityChannelCollection::GetModelChannel(IVTerrainDecorationModel *pModel, const VTerrainConfig &cfg)
{
  VDecorationDensityChannel *pChannel = FindModelChannel(pModel);
  if (!pChannel)
  {
    pChannel = new VDecorationDensityChannel(pModel,cfg);
    Add(pChannel);
  }
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
