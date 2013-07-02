/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/UndoableSectorActions.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>



VEditableTerrain* VUndoableSectorAction::GetTerrain()
{
  VEditableTerrain *pTerrain = (VEditableTerrain *)VTerrainManager::GlobalManager().GetResourceByIndex(0);
  pTerrain->EnsureLoaded();
  return pTerrain;
}

VEditableTerrainSector* VUndoableSectorAction::GetSector()
{
  VEditableTerrain *pTerrain = GetTerrain();
  VASSERT(pTerrain);
  VEditableTerrainSector *pSector = pTerrain->GetSector(m_iSectorX,m_iSectorY);
  pSector->EnsureLoaded();
  return pSector;
}


V_IMPLEMENT_SERIAL_ABSTRACT( VUndoableSectorAction, VisTypedEngineObject_cl, 0, &g_VisionEngineModule );
void VUndoableSectorAction::Serialize( VArchive &ar )
{
  VisTypedEngineObject_cl::Serialize(ar);
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;VASSERT_MSG(iLocalVersion==0, "Invalid version number");
    ar >> m_iSectorX >> m_iSectorY;
  }
  else
  {
    ar << iLocalVersion;
    ar << m_iSectorX  << m_iSectorY;
  }
}


void VUndoableSectorActionCollection::TriggerRedo() 
{
  int iCount = Count();
  for (int i=0;i<iCount;i++) 
    GetAt(i)->Redo();
}

void VUndoableSectorActionCollection::TriggerUndo() 
{
  int iCount = Count();
  for (int i=0;i<iCount;i++) 
    GetAt(i)->Undo();
}


void VUndoableSectorActionCollection::Add(VUndoableSectorAction *pAction)
{
  VRefCountedCollection<VUndoableSectorAction>::Add(pAction);
  VASSERT(pAction->GetSector());
  // min/max the affected sector region
  m_iSectorRect[0] = hkvMath::Min(m_iSectorRect[0],pAction->m_iSectorX);
  m_iSectorRect[1] = hkvMath::Min(m_iSectorRect[1],pAction->m_iSectorY);
  m_iSectorRect[2] = hkvMath::Max(m_iSectorRect[2],pAction->m_iSectorX);
  m_iSectorRect[3] = hkvMath::Max(m_iSectorRect[3],pAction->m_iSectorY);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// Class VSectorActionHeightfieldModified
/////////////////////////////////////////////////////////////////////////////////////////////////////

VSectorActionHeightfieldModified::VSectorActionHeightfieldModified(VEditableTerrainSector *pOwner)
 : VUndoableSectorAction(pOwner)
{
  m_iDataSize = pOwner->GetHeightmapSampleCount();
  m_pOrigData = new float[m_iDataSize];
  m_pCurrentData = new float[m_iDataSize];
  const float *pSrc = GetSector()->GetHeightmapValues();
  memcpy(m_pOrigData, pSrc, m_iDataSize*sizeof(float));
  memcpy(m_pCurrentData, pSrc, m_iDataSize*sizeof(float));
  m_bFirstTime = true;
}


VSectorActionHeightfieldModified::~VSectorActionHeightfieldModified()
{
  V_SAFE_DELETE_ARRAY(m_pOrigData);
  V_SAFE_DELETE_ARRAY(m_pCurrentData);
}


void VSectorActionHeightfieldModified::GetData()
{
  const float *pSrc = GetSector()->GetHeightmapValues();
  memcpy(m_pCurrentData, pSrc, m_iDataSize*sizeof(float));
}


void VSectorActionHeightfieldModified::CopyDataToHeightmap(float *pSrcData)
{
  VEditableTerrainSector* pSector = GetSector();
  VTerrainLockObject lockObj(pSector,0);
  lockObj.m_pData = pSrcData;
  lockObj.m_iStride = pSector->m_iSampleStrideX;

  // update this single sector. We don't have to care about neightbor sectors, since they have their own action
  // when they overlap

  // copy back height values
  pSector->GetRelevantHeightValues(lockObj);

  // invalidate meshes
  pSector->InvalidateMesh();
  pSector->InvalidateRelevantCollisionMeshes(&lockObj);

  // put decoration on meshes inside sector bounding box
  VLargeBoundingBox bbox;
  Config().GetSectorBBox(bbox,m_iSectorX,m_iSectorY);
//  SectorManager().UpdateDecorationChannels(bbox,-1,NULL);
}


void VSectorActionHeightfieldModified::Undo()
{
  CopyDataToHeightmap(m_pOrigData);
  GetSector()->UpdateDecoration(true); // drop all to floor
}

void VSectorActionHeightfieldModified::Redo()
{
  if (!m_bFirstTime)
    CopyDataToHeightmap(m_pCurrentData);
  GetSector()->UpdateDecoration(true); // drop all to floor
  m_bFirstTime = false;
}

V_IMPLEMENT_SERIAL( VSectorActionHeightfieldModified, VUndoableSectorAction, 0, &g_VisionEngineModule );
void VSectorActionHeightfieldModified::Serialize( VArchive &ar )
{
  VUndoableSectorAction::Serialize(ar);
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;VASSERT_MSG(iLocalVersion==0, "Invalid version number");
    ar >> m_iDataSize;
    m_pOrigData = new float[m_iDataSize];
    m_pCurrentData = new float[m_iDataSize];
    ar.Read(m_pOrigData,m_iDataSize*sizeof(float),"f",m_iDataSize);
    ar.Read(m_pCurrentData,m_iDataSize*sizeof(float),"f",m_iDataSize);
  }
  else
  {
    ar << iLocalVersion;
    ar << m_iDataSize;
    ar.Write(m_pOrigData,m_iDataSize*sizeof(float),"f",m_iDataSize);
    ar.Write(m_pCurrentData,m_iDataSize*sizeof(float),"f",m_iDataSize);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// VSectorActionDecoChannelModified
/////////////////////////////////////////////////////////////////////////////////////////////////////


VSectorActionDecoChannelModified::VSectorActionDecoChannelModified(VEditableTerrainSector *pOwner, IVTerrainDecorationModel *pModel)
 : VUndoableSectorAction(pOwner)
{
  const VTerrainConfig &cfg(pOwner->m_Config);
  m_spChannel = pOwner->m_DecorationChannels.GetModelChannel(pModel, cfg);
   
  if (m_spChannel)
  {
    m_spChannel->GetLockInfo(pOwner, m_LockObj); // fill in the lock obj data
    m_iDataSize = m_spChannel->GetSampleCount();
    m_pOrigData = new UBYTE[m_iDataSize];
    m_pCurrentData = new UBYTE[m_iDataSize];
    const UBYTE *pSrc = m_spChannel->GetValuePtr(0,0);
    memcpy(m_pOrigData, pSrc, m_iDataSize*sizeof(UBYTE));
    memcpy(m_pCurrentData, pSrc, m_iDataSize*sizeof(UBYTE));
  }
  else
  {
    m_iDataSize = 0;
    m_pOrigData = NULL;
    m_pCurrentData = NULL;
  }

  m_bFirstTime = true;
}


VSectorActionDecoChannelModified::~VSectorActionDecoChannelModified()
{
  V_SAFE_DELETE_ARRAY(m_pOrigData);
  V_SAFE_DELETE_ARRAY(m_pCurrentData);
}


void VSectorActionDecoChannelModified::GetData()
{
  if (m_spChannel)
  {
    const UBYTE *pSrc = m_spChannel->GetValuePtr(0,0);
    memcpy(m_pCurrentData, pSrc, m_iDataSize*sizeof(UBYTE));
  }
}

void VSectorActionDecoChannelModified::CopyDataToDecoChannel(UBYTE *pSrcData)
{
  if (!m_spChannel)
    return;
  m_LockObj.m_pData = pSrcData;

  // put decoration on meshes inside sector bounding box
  GetSector()->GetRelevantDensityMap(m_LockObj,true);

  VLargeBoundingBox bbox;
  Config().GetSectorBBox(bbox,m_iSectorX,m_iSectorY);

  //TODO:
//  SectorManager().UpdateDecorationChannels(bbox,1,&m_spChannel->m_iUniqueID);
}


void VSectorActionDecoChannelModified::Undo()
{
  CopyDataToDecoChannel(m_pOrigData);
}

void VSectorActionDecoChannelModified::Redo()
{
  //if (!m_bFirstTime)
    CopyDataToDecoChannel(m_pCurrentData); // always update since we want CreateFromDensity to be called with bFinalUpdate==true
  m_bFirstTime = false;
}


V_IMPLEMENT_SERIAL( VSectorActionDecoChannelModified, VUndoableSectorAction, 0, &g_VisionEngineModule );
void VSectorActionDecoChannelModified::Serialize( VArchive &ar )
{
  VUndoableSectorAction::Serialize(ar);
  VEditableTerrainSector* pSector = GetSector();
  char iLocalVersion = 0;
  int iModelID = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;VASSERT_MSG(iLocalVersion==0, "Invalid version number");
    ar >> iModelID;
    const VTerrainConfig &cfg(Config());
    IVTerrainDecorationModel *pModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(iModelID); 
    m_spChannel = pSector->m_DecorationChannels.GetModelChannel(pModel, cfg);   
    if (m_spChannel)
      m_spChannel->GetLockInfo(pSector, m_LockObj); // fill in the lock obj data

    ar >> m_iDataSize;
    m_pOrigData = new UBYTE[m_iDataSize];
    m_pCurrentData = new UBYTE[m_iDataSize];
    ar.Read(m_pOrigData,m_iDataSize);
    ar.Read(m_pCurrentData,m_iDataSize);
  }
  else
  {
    ar << iLocalVersion;
    if (m_spChannel)
      iModelID = m_spChannel->m_spSource->m_Properties.m_iModelID;
    ar << iModelID;
    ar << m_iDataSize;
    ar.Write(m_pOrigData,m_iDataSize);
    ar.Write(m_pCurrentData,m_iDataSize);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// VWeightmapChannelsModified
/////////////////////////////////////////////////////////////////////////////////////////////////////



VWeightmapChannelsModified::VWeightmapChannelsModified(VEditableTerrainSector *pOwner, const VTextureWeightmapChannelCollection &affectedChannels)
 : VUndoableSectorAction(pOwner)
{
  
  m_iOverallDataSize = 0;
  m_AffectedChannels.EnsureCapacity(affectedChannels.Count());

  // first count:
  for (int i=0;i<affectedChannels.Count();i++)
  {
    m_AffectedChannels.Add(affectedChannels.GetAt(i));
    m_iOverallDataSize += affectedChannels.GetAt(i)->GetSampleCount();
  }

  // allocate
  if (m_iOverallDataSize>0)
  {
    m_pOrigData = new UBYTE[m_iOverallDataSize];
    m_pCurrentData = new UBYTE[m_iOverallDataSize];
  }

  // then copy
  int iPos = 0;
  for (int i=0;i<affectedChannels.Count();i++)
  {
    int iCount = affectedChannels.GetAt(i)->GetSampleCount();
    UBYTE *pSrc = affectedChannels.GetAt(i)->GetValuePtr(0,0);
    memcpy(&m_pOrigData[iPos],pSrc,iCount*sizeof(UBYTE));
    memcpy(&m_pCurrentData[iPos],pSrc,iCount*sizeof(UBYTE));
    iPos += iCount;
  }

  m_bFirstTime = true;
}


VWeightmapChannelsModified::~VWeightmapChannelsModified()
{
  V_SAFE_DELETE_ARRAY(m_pOrigData);
  V_SAFE_DELETE_ARRAY(m_pCurrentData);
}


void VWeightmapChannelsModified::GetData()
{
  // copy data into current buffer
  int iPos = 0;
  for (int i=0;i<m_AffectedChannels.Count();i++)
  {
    VTextureWeightmapChannel *pChannel = m_AffectedChannels.GetAt(i);
    int iCount = pChannel->GetSampleCount();
    UBYTE *pSrc = pChannel->GetValuePtr(0,0);
    memcpy(&m_pCurrentData[iPos],pSrc,iCount*sizeof(UBYTE));
    iPos += iCount;
  }
}

void VWeightmapChannelsModified::CopyDataToWeightChannels(const UBYTE *pData)
{
  // copy data into terrain's data
  int iPos = 0;
  for (int i=0;i<m_AffectedChannels.Count();i++)
  {
    VTextureWeightmapChannel *pChannel = m_AffectedChannels.GetAt(i);
    int iCount = pChannel->GetSampleCount();
    UBYTE *pDest = pChannel->GetValuePtr(0,0);
    memcpy(pDest,&pData[iPos],iCount*sizeof(UBYTE));
    pChannel->SetDirtyFlag();
    iPos += iCount;
  }
}


void VWeightmapChannelsModified::Undo()
{
  CopyDataToWeightChannels(m_pOrigData);
}

void VWeightmapChannelsModified::Redo()
{
  if (!m_bFirstTime)
    CopyDataToWeightChannels(m_pCurrentData);
  m_bFirstTime = false;
}


V_IMPLEMENT_SERIAL( VWeightmapChannelsModified, VUndoableSectorAction, 0, &g_VisionEngineModule );
void VWeightmapChannelsModified::Serialize( VArchive &ar )
{
  VUndoableSectorAction::Serialize(ar);
  VEditableTerrainSector* pSector = GetSector();
  char iLocalVersion = 0;
  int iCount, iTextureID;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;VASSERT_MSG(iLocalVersion==0, "Invalid version number");

    ar >> iCount;
    m_AffectedChannels.EnsureCapacity(iCount);
    int iResX = Config().m_iDefaultWeightmapResolution[0];
    int iResY = Config().m_iDefaultWeightmapResolution[1];
    for (int i=0;i<iCount;i++)
    {
      ar >> iTextureID;
      // this is too early because the terrain is unloaded in the meanwhile
      VTextureWeightmapChannelResource *pTexRes = GetTerrain()->m_WeightmapChannels.FindByID(iTextureID);
      VASSERT(pTexRes);
      VTextureWeightmapChannel *pChannel = pSector->m_UsedWeightmapChannels.CreateChannel(Config(),pTexRes,iResX,iResY);
      VASSERT(pChannel);
      m_AffectedChannels.Add(pChannel);
    }
    ar >> m_iOverallDataSize;
    m_pOrigData = new UBYTE[m_iOverallDataSize];
    m_pCurrentData = new UBYTE[m_iOverallDataSize];
    ar.Read(m_pOrigData,m_iOverallDataSize);
    ar.Read(m_pCurrentData,m_iOverallDataSize);
    for (int i=0;i<m_iOverallDataSize;i++) m_pCurrentData[i] = rand()&255;
  }
  else
  {
    ar << iLocalVersion;

    iCount = m_AffectedChannels.Count();
    ar << iCount;
    for (int i=0;i<iCount;i++)
    {
      VTextureWeightmapChannel *pChannel = m_AffectedChannels.GetAt(i);
      iTextureID = pChannel->m_spResource->m_Properties.m_iTextureID;
      ar << iTextureID;
    }
    ar << m_iOverallDataSize;
    ar.Write(m_pOrigData,m_iOverallDataSize);
    ar.Write(m_pCurrentData,m_iOverallDataSize);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// VSectorActionHolesModified
/////////////////////////////////////////////////////////////////////////////////////////////////////


VSectorActionHolesModified::VSectorActionHolesModified(VEditableTerrainSector *pOwner) : VUndoableSectorAction(pOwner)
{
  m_bFirstTime = true;
  int iTileCount = Config().m_iTilesPerSector[0] * Config().m_iTilesPerSector[1];
  m_OrigData.AllocateBitfield(iTileCount);
  m_CurrentData.AllocateBitfield(iTileCount);
  ToBitmask(m_CurrentData);
  ToBitmask(m_OrigData);
}

VSectorActionHolesModified::~VSectorActionHolesModified()
{
}

// store tile holes in bitmask
void VSectorActionHolesModified::ToBitmask(VSectorHoleBitmask &dest)
{
  const VTerrainConfig &cfg(Config());
  VEditableTerrainSector* pSector = GetSector();
  int iBit = 0;
  dest.Clear();
  for (int y=0;y<cfg.m_iTilesPerSector[1];y++)
    for (int x=0;x<cfg.m_iTilesPerSector[0];x++,iBit++)
      if (pSector->GetTile(x,y)->IsHole())
        dest.SetBit(iBit);
}

// get tile holes from bitmask
void VSectorActionHolesModified::FromBitmask(const VSectorHoleBitmask &src)
{
  const VTerrainConfig &cfg(Config());
  int iBit = 0;
  VEditableTerrainSector* pSector = GetSector();

  for (int y=0;y<cfg.m_iTilesPerSector[1];y++)
  {
    for (int x=0;x<cfg.m_iTilesPerSector[0];x++,iBit++)
    {
      if (src.IsBitSet(iBit))
      {
        //bAnyHole = true;
        pSector->GetTile(x,y)->m_iTileFlags |= VSECTORTILEFLAGS_HOLE;
      }
      else
      {
        pSector->GetTile(x,y)->m_iTileFlags &= (~VSECTORTILEFLAGS_HOLE);
      }
    }
  }
  
  pSector->InvalidateMesh();
  pSector->m_bHoleTextureDirty = true;
  pSector->UpdateHoleMask();
  pSector->InvalidateRelevantCollisionMeshes(NULL); // for real physics update
}


void VSectorActionHolesModified::GetData()
{
  ToBitmask(m_CurrentData);
}

void VSectorActionHolesModified::Undo()
{
  FromBitmask(m_OrigData);
}

void VSectorActionHolesModified::Redo()
{
//  if (!m_bFirstTime)
  FromBitmask(m_CurrentData);
  m_bFirstTime = false;
}


V_IMPLEMENT_SERIAL( VSectorActionHolesModified, VUndoableSectorAction, 0, &g_VisionEngineModule );
void VSectorActionHolesModified::Serialize( VArchive &ar )
{
  VUndoableSectorAction::Serialize(ar);
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;VASSERT_MSG(iLocalVersion==0, "Invalid version number");
    m_OrigData.SerializeX(ar);
    m_CurrentData.SerializeX(ar);
  }
  else
  {
    ar << iLocalVersion;
    m_OrigData.SerializeX(ar);
    m_CurrentData.SerializeX(ar);
  }
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
