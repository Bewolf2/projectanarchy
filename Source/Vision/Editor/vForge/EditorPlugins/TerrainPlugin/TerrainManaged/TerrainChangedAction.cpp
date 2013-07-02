/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainChangedAction.hpp>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/EngineInstanceTerrain.hpp>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainConversionHelpers.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;



namespace TerrainManaged
{

  TerrainChangedAction::TerrainChangedAction(EngineInstanceTerrain ^pTerrain, String ^ActionName) : TerrainChangedBaseAction(pTerrain->m_pSectorCallback)
	{
    if (ActionName!=nullptr)
      _name = ActionName;
    m_pTerrain = pTerrain->m_pTerrain;
    m_pSectorActions = NULL;
	}

  TerrainChangedAction::~TerrainChangedAction()
  {
    System::Diagnostics::Debug::Assert(m_pSectorActions==NULL,"OnDispose seems not be called");;
  }

  
  void TerrainChangedAction::Do()
  {
    if (m_pTerrain==NULL)
      m_pTerrain = (VEditableTerrain *)VTerrainManager::GlobalManager().GetResourceByIndex(0);
    DeserializeCustomDataBlock();

    if (m_pSectorActions==NULL) // first time collect affected sectors
    {
      m_pSectorActions = new VUndoableSectorActionCollection;
      m_pTerrain->OnSectorUpdateFinished(m_pSectorActions); // collect all sub-actions from touched sectors
    }

    // perform an undo on all sub-actions
    m_pSectorActions->TriggerRedo();
    m_pTerrain->UpdatePhysics(); // update physics of all pending sectors

    EditorManager::ActiveView->UpdateView(false);
    UpdateSectorRange();


    // do we have to do something about SectorX1,SectorY1, SectorX2, SectorY2?

  }

  void TerrainChangedAction::Undo()
  {
    if (m_pTerrain==NULL)
      m_pTerrain = (VEditableTerrain *)VTerrainManager::GlobalManager().GetResourceByIndex(0);
    DeserializeCustomDataBlock();

    // perform an undo on all sub-actions
    m_pSectorActions->TriggerUndo();
    m_pTerrain->UpdatePhysics(); // update physics of all pending sectors

    EditorManager::ActiveView->UpdateView(false);
    UpdateSectorRange();

    // do we have to do something about SectorX1,SectorY1, SectorX2, SectorY2?
  } 


  TerrainChangedAction::TerrainChangedAction(SerializationInfo ^info, StreamingContext context)
      : TerrainChangedBaseAction(info, context)
  {
    // get later (does not exist here)
    m_pTerrain = NULL;
  }

  void TerrainChangedAction::GetObjectData(SerializationInfo ^info, StreamingContext context)
  {
    if (m_pSectorActions!=NULL)
    {
      // serialize the native actions into a memory stream
      VMemoryStreamPtr spMem = new VMemoryStream(NULL);
      VMemoryOutStreamLocal outStream(spMem);
      VArchive ar(NULL,&outStream,Vision::GetTypeManager());
        ar << (int)Vision::GetArchiveVersion();
        m_pSectorActions->SerializeX(ar);
      ar.Close();
      outStream.Close();
      
      // and copy this stream to the C# object. The base class serializes CustomSerializationBlock
      int iSize = spMem->GetSize();
      AllocateCustomSerializationBlock(iSize);
      IntPtr srcPtr(spMem->GetDataPtr());
      Marshal::Copy(srcPtr, CustomSerializationBlock,0,iSize);
    }
    TerrainChangedBaseAction::GetObjectData(info, context);
  }


  void TerrainChangedAction::DeserializeCustomDataBlock()
  {
    if (CustomSerializationBlock!=nullptr)
    {
      if (m_pSectorActions==NULL)
        m_pSectorActions = new VUndoableSectorActionCollection;
      m_pSectorActions->Clear();

      // copy from C# array
      int iSize = CustomSerializationBlock->Length;
      VMemoryStreamPtr spMem = new VMemoryStream(NULL);
      IntPtr dstPtr(spMem->AllocateBytes(iSize));
      Marshal::Copy(CustomSerializationBlock, 0, dstPtr, iSize);

      // de-serialize native actions
      VMemoryInStreamLocal inStream(spMem);
      VArchive ar(NULL,&inStream,Vision::GetTypeManager());
        int iArchiveVersion;
        ar >> iArchiveVersion;
        ar.SetLoadingVersion(iArchiveVersion);
        m_pSectorActions->SerializeX(ar);
      ar.Close();
      inStream.Close();
      CustomSerializationBlock = nullptr;
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // class TerrainClipboardObject
  ////////////////////////////////////////////////////////////////////////////

  bool TerrainClipboardObject::TryPaste(System::Object ^opHint)
  {
    EngineInstanceTerrain ^pTerrain = nullptr;
    try {
      pTerrain = safe_cast<EngineInstanceTerrain ^>(opHint);
    } 
    catch(InvalidCastException^) 
    {
    }
    if (pTerrain == nullptr)
      return true;
    TerrainSelection ^sel = pTerrain->m_pSectorCallback->CurrentSelection;
    if (sel == nullptr)
      return true;
    
    pTerrain->PasteSelection(sel, this);
    return true;
  }
  



  ////////////////////////////////////////////////////////////////////////////
  // class TerrainClipboardAction
  ////////////////////////////////////////////////////////////////////////////

  TerrainClipboardAction::TerrainClipboardAction(EngineInstanceTerrain ^pTerrain, String ^ActionName) : TerrainChangedBaseAction(pTerrain->m_pSectorCallback)
  {
    if (ActionName!=nullptr)
      _name = ActionName;
    m_pTerrain = pTerrain->m_pTerrain;
    m_pOldHeightValues = NULL;
    m_pNewHeightValues = NULL;

    m_iDetailTextureCount = m_iDecorationChannelCount = 0;
    m_pOldDetailTextures = m_pNewDetailTextures = NULL;
    m_pOldDecorationChannels = m_pNewDecorationChannels = NULL;

    sx1=sy1=0;
    sx2=sy2=-1;
  }

  void TerrainClipboardAction::OnDispose()
  {
    V_SAFE_DELETE(m_pOldHeightValues);
    V_SAFE_DELETE(m_pNewHeightValues);
    V_SAFE_DELETE_ARRAY(m_pOldDetailTextures);
    V_SAFE_DELETE_ARRAY(m_pNewDetailTextures);
    V_SAFE_DELETE_ARRAY(m_pOldDecorationChannels);
    V_SAFE_DELETE_ARRAY(m_pNewDecorationChannels);
    TerrainChangedBaseAction::OnDispose();
  }

  void TerrainClipboardAction::Do()
  {
    if (m_pOldHeightValues)
      CopyTerrainData(*m_pOldHeightValues, *m_pNewHeightValues);
    if (m_pOldDetailTextures)
      CopyDetailTextures(m_pNewDetailTextures);
    if (m_pOldDecorationChannels)
      CopyDecoration(m_pNewDecorationChannels);
        
    m_pTerrain->UpdatePhysics(); // update physics of all pending sectors

    UpdateSectorRange();
  }

  void TerrainClipboardAction::Undo()
  {
    if (m_pOldHeightValues)
      CopyTerrainData(*m_pOldHeightValues, *m_pOldHeightValues);
    if (m_pOldDetailTextures)
      CopyDetailTextures(m_pOldDetailTextures);
    if (m_pOldDecorationChannels)
      CopyDecoration(m_pOldDecorationChannels);

    m_pTerrain->UpdatePhysics(); // update physics of all pending sectors

    UpdateSectorRange();
  }

  void TerrainClipboardAction::CopyTerrainData(VTerrainLockObject &targetRect, VTerrainLockObject &newData)
  {
    VTerrainLockObject dest(targetRect.m_iPos[0],targetRect.m_iPos[1],targetRect.m_iSize[0],targetRect.m_iSize[1],VIS_LOCKFLAG_DISCARDABLE);
    float fScaleX = (float)newData.m_iSize[0]/(float)dest.m_iSize[0];
    float fScaleY = (float)newData.m_iSize[1]/(float)dest.m_iSize[1];
    m_pTerrain->LockHeightValues(dest);
    float *pDest = (float *)dest.GetData();
    const float *pSrc = (float *)newData.GetData();
    for (int y=0;y<dest.m_iSize[1];y++,pDest+=dest.m_iStride)
      for (int x=0;x<dest.m_iSize[0];x++)
      {
        float xx = (float)x*fScaleX;
        float yy = (float)y*fScaleY;
        pDest[x] = BlittingHelpers::BiLerpF(pSrc,newData.m_iSize[0],newData.m_iSize[1],newData.m_iStride,xx,yy);
      }
    m_pTerrain->UnlockHeightValues(dest);
  }

  void TerrainClipboardAction::CopyDetailTextures(VTerrainLockObject *pNewArray)
  {
    for (int i=0;i<m_iDetailTextureCount;i++)
    {
      const VTerrainLockObject &targetRect(m_pOldDetailTextures[i]);
      VTerrainLockObject dest(targetRect.m_iPos[0],targetRect.m_iPos[1],targetRect.m_iSize[0],targetRect.m_iSize[1],VIS_LOCKFLAG_DISCARDABLE);
      VTerrainLockObject &newData(pNewArray[i]);
      if (newData.m_pData==NULL) // channel not pasted
        continue;
      float fScaleX = (float)newData.m_iSize[0]/(float)dest.m_iSize[0];
      float fScaleY = (float)newData.m_iSize[1]/(float)dest.m_iSize[1];
      VTextureWeightmapChannelResource *pRes = newData.m_pTerrain->m_WeightmapChannels.FindByID(newData.m_iReserved[0]); // see also VEditableTerrainSector::GetRelevantDetailTextureValues 
      m_pTerrain->LockDetailTexture(dest, pRes);
      UBYTE *pDest = (UBYTE *)dest.GetData();
      const UBYTE *pSrc = (UBYTE *)newData.GetData();
      for (int y=0;y<dest.m_iSize[1];y++,pDest+=dest.m_iStride)
        for (int x=0;x<dest.m_iSize[0];x++)
        {
          float xx = (float)x*fScaleX;
          float yy = (float)y*fScaleY;
          pDest[x] = (UBYTE)BlittingHelpers::BiLerpB(pSrc,newData.m_iSize[0],newData.m_iSize[1],newData.m_iStride,xx,yy);
        }
      m_pTerrain->UnlockDetailTexture(dest);
    }
  }

  void TerrainClipboardAction::CopyDecoration(VTerrainLockObject *pNewArray)
  {
    for (int i=0;i<m_iDecorationChannelCount;i++)
    {
      const VTerrainLockObject &targetRect(m_pOldDecorationChannels[i]);
      VTerrainLockObject dest(targetRect.m_iPos[0],targetRect.m_iPos[1],targetRect.m_iSize[0],targetRect.m_iSize[1],VIS_LOCKFLAG_DISCARDABLE);
      VTerrainLockObject &newData(pNewArray[i]);
      if (newData.m_pData==NULL) // channel not pasted
        continue;
      float fScaleX = (float)newData.m_iSize[0]/(float)dest.m_iSize[0];
      float fScaleY = (float)newData.m_iSize[1]/(float)dest.m_iSize[1];
      
      IVTerrainDecorationModel *pRes = VTerrainDecorationModelManager::GlobalManager().FindModelByID(newData.m_iReserved[0]); // see also VEditableTerrain::LockDecorationChannel 
      m_pTerrain->LockDecorationChannel(dest, pRes);
      UBYTE *pDest = (UBYTE *)dest.GetData();
      const UBYTE *pSrc = (UBYTE *)newData.GetData();
      for (int y=0;y<dest.m_iSize[1];y++,pDest+=dest.m_iStride)
        for (int x=0;x<dest.m_iSize[0];x++)
        {
          float xx = (float)x*fScaleX;
          float yy = (float)y*fScaleY;
          pDest[x] = (UBYTE)BlittingHelpers::BiLerpB(pSrc,newData.m_iSize[0],newData.m_iSize[1],newData.m_iStride,xx,yy);
        }
      m_pTerrain->UnlockDecorationChannel(dest,true);
    }
  }


  TerrainClipboardAction::TerrainClipboardAction(SerializationInfo ^info, StreamingContext context)
      : TerrainChangedBaseAction(info, context)
  {
  }

  void TerrainClipboardAction::GetObjectData(SerializationInfo ^info, StreamingContext context)
  {
    TerrainChangedBaseAction::GetObjectData(info, context);
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
