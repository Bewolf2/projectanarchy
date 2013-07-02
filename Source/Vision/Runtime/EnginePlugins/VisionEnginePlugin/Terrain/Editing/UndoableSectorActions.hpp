/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file UndoableSectorActions.hpp

#ifndef UNDOABLESECTORACTIONS_HPP_INCLUDED
#define UNDOABLESECTORACTIONS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/EditableTerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>

class VUndoableSectorAction;
typedef VSmartPtr<VUndoableSectorAction> VUndoableSectorActionPtr;



/// \brief
///   Interface that represents one update action applied to a sector
class VUndoableSectorAction : public VisTypedEngineObject_cl, public VRefCounter
{
public:
  TERRAIN_IMPEXP VUndoableSectorAction(VEditableTerrainSector *pOwner=NULL)
  {
    m_iSectorX = m_iSectorY = 0;
    if (pOwner)
    {
      m_iSectorX = pOwner->m_iIndexX;
      m_iSectorY = pOwner->m_iIndexY;
    }
  }
  TERRAIN_IMPEXP virtual ~VUndoableSectorAction()
  {
  }

  TERRAIN_IMPEXP virtual void GetData() = 0;
  TERRAIN_IMPEXP virtual void Undo() = 0;
  TERRAIN_IMPEXP virtual void Redo() = 0;

  TERRAIN_IMPEXP VEditableTerrain* GetTerrain();
  TERRAIN_IMPEXP VEditableTerrainSector* GetSector();

  inline VTerrainSectorManager &SectorManager() {return *GetSector()->GetSectorManager();}
  inline const VTerrainConfig &Config() {return SectorManager().Config();}

  int m_iSectorX,m_iSectorY; // reference a sector by index because the terrain instance might change

  V_DECLARE_SERIAL_DLLEXP( VUndoableSectorAction,  TERRAIN_IMPEXP );
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar ); 
};


/// \brief
///   Collection class for instances of type VUndoableSectorAction
class VUndoableSectorActionCollection : public VRefCountedCollection<VUndoableSectorAction>
{
public:
  VUndoableSectorActionCollection()
  {
    m_iSectorRect[0] = m_iSectorRect[1] = 10000000;
    m_iSectorRect[2] = m_iSectorRect[3] = -10000000;
  }

  TERRAIN_IMPEXP void Add(VUndoableSectorAction *pAction);
  TERRAIN_IMPEXP void TriggerRedo();
  TERRAIN_IMPEXP void TriggerUndo();

  inline bool IsRectValid() const {return m_iSectorRect[0]<=m_iSectorRect[2];}

  // affected sector bounding box
  int m_iSectorRect[4];
};





/// \brief
///   Implements the VUndoableSectorAction interface; Stores modified heightmap data
class VSectorActionHeightfieldModified : public VUndoableSectorAction
{
public:
  TERRAIN_IMPEXP VSectorActionHeightfieldModified(VEditableTerrainSector *pOwner);
  TERRAIN_IMPEXP virtual ~VSectorActionHeightfieldModified();

  TERRAIN_IMPEXP virtual void GetData();
  TERRAIN_IMPEXP virtual void Undo();
  TERRAIN_IMPEXP virtual void Redo();

  bool m_bFirstTime;
  int m_iDataSize;
  float *m_pOrigData;
  float *m_pCurrentData;
protected:
  VSectorActionHeightfieldModified() {m_bFirstTime=false;m_iDataSize=0;m_pOrigData=m_pCurrentData=NULL;}
  TERRAIN_IMPEXP void CopyDataToHeightmap(float *pSrcData);
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar ); 
  V_DECLARE_SERIAL_DLLEXP( VSectorActionHeightfieldModified,  TERRAIN_IMPEXP );
};


typedef VTBitfield<128*128> VSectorHoleBitmask; 

/// \brief
///   Implements the VUndoableSectorAction interface; Stores modified hole data
class VSectorActionHolesModified : public VUndoableSectorAction
{
public:
  TERRAIN_IMPEXP VSectorActionHolesModified(VEditableTerrainSector *pOwner);
  TERRAIN_IMPEXP virtual ~VSectorActionHolesModified();

  TERRAIN_IMPEXP virtual void GetData();
  TERRAIN_IMPEXP virtual void Undo();
  TERRAIN_IMPEXP virtual void Redo();

  void ToBitmask(VSectorHoleBitmask &dest);
  void FromBitmask(const VSectorHoleBitmask &src);
  bool m_bFirstTime;
  VSectorHoleBitmask m_OrigData;
  VSectorHoleBitmask m_CurrentData;
protected:
  VSectorActionHolesModified() {m_bFirstTime=false;}
  TERRAIN_IMPEXP void CopyDataToTerrain(const VSectorHoleBitmask& newMask);
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar ); 
  V_DECLARE_SERIAL_DLLEXP( VSectorActionHolesModified,  TERRAIN_IMPEXP );
};



/// \brief
///   Implements the VUndoableSectorAction interface; Stores modified decoration channel data
class VSectorActionDecoChannelModified : public VUndoableSectorAction
{
public:
  TERRAIN_IMPEXP VSectorActionDecoChannelModified(VEditableTerrainSector *pOwner, IVTerrainDecorationModel *pModel);
  TERRAIN_IMPEXP virtual ~VSectorActionDecoChannelModified();

  TERRAIN_IMPEXP virtual void GetData();
  TERRAIN_IMPEXP virtual void Undo();
  TERRAIN_IMPEXP virtual void Redo();

  bool m_bFirstTime;
  int m_iDataSize;
  UBYTE *m_pOrigData;
  UBYTE *m_pCurrentData;
  VTerrainLockObject m_LockObj;
  VDecorationDensityChannelPtr m_spChannel;
protected:
  VSectorActionDecoChannelModified() {m_bFirstTime=false;m_iDataSize=0;m_pOrigData=NULL;m_pCurrentData=NULL;}
  TERRAIN_IMPEXP void CopyDataToDecoChannel(UBYTE *pSrcData);
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar ); 
  V_DECLARE_SERIAL_DLLEXP( VSectorActionDecoChannelModified,  TERRAIN_IMPEXP );
};



/// \brief
///   Implements the VUndoableSectorAction interface; Stores modified weightmap channel data
class VWeightmapChannelsModified : public VUndoableSectorAction
{
public:
  TERRAIN_IMPEXP VWeightmapChannelsModified(VEditableTerrainSector *pOwner, const VTextureWeightmapChannelCollection &affectedChannels);
  TERRAIN_IMPEXP virtual ~VWeightmapChannelsModified();

  TERRAIN_IMPEXP virtual void GetData();
  TERRAIN_IMPEXP virtual void Undo();
  TERRAIN_IMPEXP virtual void Redo();

  bool m_bFirstTime;
  int m_iOverallDataSize;
  UBYTE *m_pOrigData;
  UBYTE *m_pCurrentData;
  VTerrainLockObject m_LockObj;
  VTextureWeightmapChannelCollection m_AffectedChannels;
protected:
  VWeightmapChannelsModified() {m_bFirstTime=false;m_iOverallDataSize=0;m_pOrigData=m_pCurrentData=NULL;}
  TERRAIN_IMPEXP void CopyDataToWeightChannels(const UBYTE *pData);
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar ); 
  V_DECLARE_SERIAL_DLLEXP( VWeightmapChannelsModified,  TERRAIN_IMPEXP );
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
