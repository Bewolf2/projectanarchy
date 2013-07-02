/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionBase.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimSequenceSet.hpp> 
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>



// -------------------------------------------------------------------------- //
// Sequence Set                                                               //
// -------------------------------------------------------------------------- //

VSequenceSet::VSequenceSet()
{
  m_bLoaded = false;
  m_pOwnerSet = NULL;
}

VSequenceSet::~VSequenceSet()
{
  V_SAFE_DELETE(m_szFilename);
  m_pOwnerSet = NULL;
}

void VSequenceSet::Load(VDynamicMesh* pMesh, const char* szAnimFilePath)
{
  if (!m_szFilename)
    return;

  m_pOwnerSet = Vision::Animations.GetSequenceSetManager()->LoadAnimSequenceSet(szAnimFilePath);
  if (m_pOwnerSet)
  {
    pMesh->GetSequenceSetCollection()->AddSequenceSet(m_pOwnerSet);
    m_bLoaded = true;
  }
  else
  {
    m_bLoaded = false;
  }
}

V_IMPLEMENT_SERIALX( VSequenceSet );
void VSequenceSet::SerializeX( VArchive &ar )
{
  char iLocalVersion = TRANSITION_TABLE_VERSION_CURRENT;
  VASSERT(m_pTransitionTable);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT(iLocalVersion <= TRANSITION_TABLE_VERSION_CURRENT);

    // Read animation filename
    char szFilename[1024];
    ar.ReadStringBinary(szFilename, 1024);
    m_szFilename = vStrDup(szFilename);

    char szFilepath[FS_MAX_PATH];
    char szFullAnimFilePath[FS_MAX_PATH];
    if (iLocalVersion >= TRANSITION_TABLE_VERSION_2) 
      // Load anim files from the same folder as the transition file
      if(m_pTransitionTable->GetFilename())
        VFileHelper::GetFileDir(m_pTransitionTable->GetFilename(), szFilepath);  
      else
        VFileHelper::GetFileDir(m_pTransitionTable->GetMesh()->GetFilename(), szFilepath);
    else if (iLocalVersion <= TRANSITION_TABLE_VERSION_1) 
      // Load anim files from the same folder as the model file
      VFileHelper::GetFileDir(m_pTransitionTable->GetMesh()->GetFilename(), szFilepath);
    
    // Set the filename
    VFileHelper::CombineDirAndFile(szFullAnimFilePath, szFilepath, m_szFilename);

    // Read loaded state
    ar >> m_bLoaded;

    // Load sequence set when specified
    if (m_bLoaded) 
      Load(m_pTransitionTable->GetMesh(), szFullAnimFilePath);
  } 
  else
  {
    ar << iLocalVersion;

    // Write animation filename
    ar.WriteStringBinary(m_szFilename);

    // Write loaded state
    ar << m_bLoaded;
  }
}


// -------------------------------------------------------------------------- //
// Sequence Definition                                                        //
// -------------------------------------------------------------------------- //

VSequenceDef::VSequenceDef()
{
  m_pFollowUpAnimation = NULL;
  m_iFollowUpAnimationCount = 0;
  m_pAnimationEvent = NULL;
  m_iAnimationEventCount = 0;
  m_eAnimationEndType = ANIMATIONEND_TYPE_LOOP;
  m_bFinishSequence = false;
}

VSequenceDef::~VSequenceDef()
{
  V_SAFE_DELETE_ARRAY(m_pAnimationEvent);
  V_SAFE_DELETE_ARRAY(m_pFollowUpAnimation);
}

VisAnimSequence_cl* VSequenceDef::ReadSequenceProxy(VArchive &ar, VSequenceSet* pSequenceSet)
{
  char szName[1024];
  ar.ReadStringBinary(szName,1024);
  return (pSequenceSet && pSequenceSet->GetOwnerSet()) ? pSequenceSet->GetOwnerSet()->GetSequence(szName) : NULL;
}

void VSequenceDef::WriteSequenceProxy(VArchive &ar, VisAnimSequence_cl* pSequence)
{
  ar.WriteStringBinary(pSequence ? pSequence->GetName() : NULL);
}

VSequenceSet* VSequenceDef::ReadSequenceSetProxy(VArchive &ar, VTransitionTable* pTable)
{
  char szName[1024];
  ar.ReadStringBinary(szName,1024);
  return pTable ? pTable->GetSequenceSet(szName) : NULL;
}

void VSequenceDef::WriteSequenceSetProxy(VArchive &ar, VSequenceSet* pSequenceSet)
{
  ar.WriteStringBinary(pSequenceSet ? pSequenceSet->GetFilename() : NULL);
}

V_IMPLEMENT_SERIALX( VSequenceDef );
void VSequenceDef::SerializeX( VArchive &ar )
{
  char iLocalVersion = TRANSITION_TABLE_VERSION_CURRENT;
  VASSERT(m_pTransitionTable);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT(iLocalVersion <= TRANSITION_TABLE_VERSION_CURRENT);
   
    // Read owner sequence and set
    m_pSequenceSet = ReadSequenceSetProxy(ar, m_pTransitionTable);
    m_pSequence = ReadSequenceProxy(ar, m_pSequenceSet);
    ar >> m_bFinishSequence;

    // Read end of Animation type
    ar >> (int &)m_eAnimationEndType;
    if (m_eAnimationEndType != ANIMATIONEND_TYPE_LOOP)
    {
      int iSeqCount;
      ar >> iSeqCount;
      VisAnimSequence_cl** pAnims = AllocateFollowUpAnimations(iSeqCount);
      for (int i = 0; i < iSeqCount; i++)
        pAnims[i] = ReadSequenceProxy(ar, m_pSequenceSet);
    }

    // Read animation events
    int iEventCount;
    ar >> iEventCount;
    VisAnimEvent_cl* pEvents = AllocateAnimationEvents(iEventCount);
    for (int i = 0; i < iEventCount; i++)
      ar >> pEvents[i];

    // Animation events will only be added to the owner animation sequence if the TSM's don't handle it
    // If events are non-global the TSM takes care of the animation events
    if(VTransitionManager::GlobalManager().GetUseGlobalAnimationEvents())
    {
      // Setup events
      InitAnimEvents();
    }  
  } 
  else
  {
    ar << iLocalVersion;

    // Write owner sequence and set
    WriteSequenceSetProxy(ar, m_pSequenceSet);
    WriteSequenceProxy(ar, m_pSequence);
    ar << m_bFinishSequence;

    // Write end of Animation type
    ar << m_eAnimationEndType;
    if (m_eAnimationEndType != ANIMATIONEND_TYPE_LOOP)
    {
      ar << m_iFollowUpAnimationCount;
      for (int i = 0; i < m_iFollowUpAnimationCount; i++)
        WriteSequenceProxy(ar, m_pFollowUpAnimation[i]);
    }

    // Write animation events
    ar << m_iAnimationEventCount;
    for (int i = 0; i < m_iAnimationEventCount; i++)
      ar << m_pAnimationEvent[i];
  }
}

// -------------------------------------------------------------------------- //
// Transition Set                                                             //
// -------------------------------------------------------------------------- //

VTransitionSet::VTransitionSet()
{
  m_iTargetCount = 0;
  m_pTransitionPairs = NULL;
  m_pSequenceSet = NULL;
}

VTransitionSet::~VTransitionSet()
{
  V_SAFE_DELETE_ARRAY(m_pTransitionPairs);
}

VTransitionDef* VTransitionSet::GetTransitionDef(VisAnimSequence_cl *pTargetSequence) const
{
  VASSERT(pTargetSequence);
  const VTransitionPair_t *pPair = m_pTransitionPairs;
  for (int i=0;i<m_iTargetCount;i++,pPair++)
    if (pPair->pTargetSequence==pTargetSequence)
      return pPair->pTransition;
  return NULL;
}

bool VTransitionSet::AddTransitionPair(VisAnimSequence_cl *pTargetSequence, VTransitionDef *pTransition, int iIndex)
{
  m_pTransitionPairs[iIndex].pTargetSequence = pTargetSequence;
  m_pTransitionPairs[iIndex].pTransition = pTransition;
  return true;
}

VSequenceSet* VTransitionSet::ReadSequenceSetProxy(VArchive &ar, VTransitionTable* pTable)
{
  char szName[1024];
  ar.ReadStringBinary(szName,1024);
  return pTable ? pTable->GetSequenceSet(szName) : NULL;
}

void VTransitionSet::WriteSequenceSetProxy(VArchive &ar, VSequenceSet* pSequenceSet)
{
  ar.WriteStringBinary(pSequenceSet ? pSequenceSet->GetFilename() : NULL);
}

VisAnimSequence_cl *VTransitionSet::ReadSequenceProxy(VArchive &ar, VSequenceSet* pSequenceSet, VDynamicMesh *pMesh)
{
  char szName[1024];
  ar.ReadStringBinary(szName,1024);
  return (pSequenceSet && pSequenceSet->GetOwnerSet()) ? pSequenceSet->GetOwnerSet()->GetSequence(szName) : (pMesh ? pMesh->GetSequence(szName) : NULL);
}

void VTransitionSet::WriteSequenceProxy(VArchive &ar, VisAnimSequence_cl *pSequence)
{
  ar.WriteStringBinary(pSequence ? pSequence->GetName() : NULL);
}

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=187
#endif


V_IMPLEMENT_SERIALX( VTransitionSet );
void VTransitionSet::SerializeX( VArchive &ar )
{
  char iLocalVersion = TRANSITION_TABLE_VERSION_CURRENT;
  VisAnimSequence_cl *pTargetSequence; 
  VTransitionDef *pTransition;

  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT(iLocalVersion <= TRANSITION_TABLE_VERSION_CURRENT);
    VDynamicMesh *pMesh = m_pTransitionTable->GetMesh();

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_1)
    {
      // Read owner sequence and set
      m_pSequenceSet = ReadSequenceSetProxy(ar, m_pTransitionTable);
    }

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_0)
    {
      m_pTransitionSource = ReadSequenceProxy(ar, m_pSequenceSet, pMesh);
      int iTargetCount,iIndex;
      ar >> iTargetCount;
      AllocateTargets(iTargetCount);
      for (int i = 0; i < iTargetCount; i++)
      {
        pTargetSequence = ReadSequenceProxy(ar, m_pSequenceSet, pMesh);
        ar >> iIndex;
        pTransition = iIndex >= 0 ? m_pTransitionTable->GetTransitionDefByIndex(iIndex) : NULL;
        AddTransitionPair(pTargetSequence, pTransition, i);
      }
    }
  } 
  else
  {
    ar << iLocalVersion;

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_1)
    {
      // Write owner sequence and set
      WriteSequenceSetProxy(ar, m_pSequenceSet);
    }

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_0)
    {

      WriteSequenceProxy(ar, m_pTransitionSource);
      ar << m_iTargetCount;
      const VTransitionPair_t *pPair = m_pTransitionPairs;
      for (int i = 0; i < m_iTargetCount; i++, pPair++)
      {
        // the order of the sequences is important for the animation tool
        pTargetSequence = pPair->pTargetSequence;
        WriteSequenceProxy(ar, pTargetSequence);
        pTransition = pPair->pTransition;

        // rather than the ID we store the index in the flat list for faster lookup at loading time
        int iIndex = (pTransition!=NULL) ? (int)(pTransition - m_pTransitionTable->m_pTransitionDef) : -1;
        ar << iIndex;
      }
    }
  }
}

#if defined(__SNC__)
#pragma diag_pop
#endif


// -------------------------------------------------------------------------- //
// Transition Table                                                           //
// -------------------------------------------------------------------------- //

VTransitionTable::VTransitionTable(VTransitionManager *pParentManager, VDynamicMesh *pMesh)
  : VManagedResource(pParentManager)
{
  VASSERT(pMesh);
  m_pMesh = pMesh;
  
  m_iSequenceSetCount = 0;
  m_pSequenceSet = NULL;
  m_iSequenceDefCount = 0;
  m_pSequenceDef = NULL;
  m_iTransitionDefCount = 0;
  m_pTransitionDef = NULL;
  m_iTransitionSetCount = 0;
  m_pTransitionSet = NULL;
  m_bBrokenVersion3 = false;
}

VTransitionTable::~VTransitionTable()
{
  V_SAFE_DELETE_ARRAY(m_pTransitionDef);
  V_SAFE_DELETE_ARRAY(m_pTransitionSet);
  V_SAFE_DELETE_ARRAY(m_pSequenceDef);
  V_SAFE_DELETE_ARRAY(m_pSequenceSet);
}

VTransitionSet* VTransitionTable::GetTransitionSet(VisAnimSequence_cl *pSourceSequence) const
{
  // todo: possibly optimized by using better lookup strategy (e.g. map)
  VASSERT(pSourceSequence);

  for (int i = 0; i < m_iTransitionSetCount; i++)
    if (pSourceSequence == m_pTransitionSet[i].GetSourceSequence())
      return &m_pTransitionSet[i];
  return NULL;
}

VTransitionDef* VTransitionTable::GetTransitionDef(VisAnimSequence_cl *pSourceSequence, VisAnimSequence_cl *pTargetSequence) const
{
  if (pSourceSequence == pTargetSequence)
    return NULL;

  VASSERT(pSourceSequence);
  VASSERT(pTargetSequence);

  // Get transition set of source animation
  VTransitionSet *pTransitionSet = GetTransitionSet(pSourceSequence);
  if (!pTransitionSet)
    return NULL;

  // Get transition from set
  VTransitionDef *pTransitionDef = pTransitionSet->GetTransitionDef(pTargetSequence);
  if (!pTransitionDef)
    return NULL;

  return pTransitionDef;
}
  
VTransitionDef* VTransitionTable::GetTransitionDef(int iID) const
{
  // todo: possibly optimized by using better lookup strategy (e.g. map)
  for (int i = 0; i < m_iTransitionDefCount; i++)
    if (m_pTransitionDef[i].m_iID == iID)
      return &m_pTransitionDef[i];
  return NULL;
}

VSequenceDef* VTransitionTable::GetSequenceDef(VisAnimSequence_cl* pSequence) const
{
  // todo: possibly optimized by using better lookup strategy (e.g. map)
  VASSERT(pSequence);

  for (int i = 0; i < m_iSequenceDefCount; i++)
    if (pSequence == m_pSequenceDef[i].GetOwnerSequence())
      return &m_pSequenceDef[i];
  return NULL;
}

VSequenceSet* VTransitionTable::GetSequenceSet(const char* szFilename) const
{
  // todo: possibly optimized by using better lookup strategy (e.g. map)
  VASSERT(szFilename);

  for (int i = 0; i < m_iSequenceSetCount; i++)
    if (strcmp(szFilename, m_pSequenceSet[i].m_szFilename) == 0)
      return &m_pSequenceSet[i];
  return NULL;
}

VisAnimSequence_cl* VTransitionTable::GetSequence(const char *szSequenceName, VisModelAnimType_e eType) 
{
  VisAnimSequence_cl *pSequence = NULL;
  for (int i = 0; i < GetNumSequenceSets(); i++)
  {
    if (GetSequenceSetByIndex(i) && GetSequenceSetByIndex(i)->GetOwnerSet())
    {
      pSequence = GetSequenceSetByIndex(i)->GetOwnerSet()->GetSequence(szSequenceName, eType);
      if (pSequence)
      {
        return pSequence;
      }
    } 
  }
  return NULL;
}

BOOL VTransitionTable::Reload()
{
  const char *szFilename = GetFilename();
  return LoadFromFile(szFilename);
}

BOOL VTransitionTable::Unload()
{
  AllocateTransitionDefs(0);
  AllocateTransitionSets(0);
  return TRUE;
}

bool VTransitionTable::LoadFromFile(const char* filePath)
{
  IVFileInStream *pIn = Vision::File.Open(filePath);
  if (!pIn)
    return false;
  bool bOldBroken = m_bBrokenVersion3;
  VArchive ar(NULL,pIn,Vision::GetTypeManager());
    int iArchiveVersion;
    ar >> iArchiveVersion;
    ar.SetLoadingVersion(iArchiveVersion);
    SerializeX(ar);
  ar.Close();
  pIn->Close();

  // load the whole file again...
  if (m_bBrokenVersion3 && !bOldBroken)
    return LoadFromFile(filePath); 

  return true;
}

#ifdef TRANSITION_SUPPORTS_SAVING

bool VTransitionTable::SaveToFile(const char* filePath)
{
  if (filePath==NULL)
    filePath = GetFilename();
  else
    SetFilename(filePath);

  IVFileOutStream *pOut = Vision::File.Create(filePath);
  if (!pOut)
    return false;
  VArchive ar(NULL,pOut,Vision::GetTypeManager());
    ar.SetLoadingVersion(Vision::GetArchiveVersion());
    ar << ar.GetLoadingVersion();
    SerializeX(ar);
  ar.Close();

  pOut->Close();
  return true;
}

#endif

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=187
#endif

V_IMPLEMENT_SERIALX( VTransitionTable );
void VTransitionTable::SerializeX( VArchive &ar )
{
  char dataLayout;
  char iLocalVersion = TRANSITION_TABLE_VERSION_CURRENT;
  if (ar.IsLoading())
  { 
    // Check data layout
    ar >> dataLayout; if (dataLayout != 'V') return;
    ar >> dataLayout; if (dataLayout != 'T') return;
    ar >> dataLayout; if (dataLayout != 'R') return;
    ar >> dataLayout; if (dataLayout != 'A') return;
    ar >> iLocalVersion; VASSERT(iLocalVersion <= TRANSITION_TABLE_VERSION_CURRENT);

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_1)
    {
      // Read sequence sets
      int iSequenceSetCount;
      ar >> iSequenceSetCount;
      VSequenceSet* pSequenceSets = AllocateSequenceSets(iSequenceSetCount);
      for (int i = 0; i < iSequenceSetCount; i++)
      {
        m_pSequenceSet[i].m_pTransitionTable = this;
        ar >> m_pSequenceSet[i];
      }

      // Read associated animation sets
      int iSequenceDefCount;
      ar >> iSequenceDefCount;
      VSequenceDef* pSequenceDefs = AllocateSequenceDefs(iSequenceDefCount);
      for (int i = 0; i < iSequenceDefCount; i++)
      {
        pSequenceDefs[i].m_pTransitionTable = this;
        ar >> pSequenceDefs[i];
      }
    }

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_0)
    {
      int iDefCount, iSetCount;

      // Read transition definitions
      ar >> iDefCount;
      VTransitionDef* pDefs = AllocateTransitionDefs(iDefCount);
      for (int i = 0; i < iDefCount; i++)
      {
        if (iLocalVersion == TRANSITION_TABLE_VERSION_3)
        {
          bool bOldBroken = m_bBrokenVersion3;
          // special treatment for version 3
          if (m_bBrokenVersion3)
            ar.Read(&pDefs[i], sizeof(VTransitionDef), VTransitionDef::GetDataLayout(iLocalVersion));
          else
            ar.Read(&pDefs[i], VTransitionDef::GetSize(iLocalVersion), VTransitionDef::GetDataLayout(iLocalVersion));

          if (pDefs[i].IsBrokenVersion3()) // sanity check
            m_bBrokenVersion3 = true;
          else
            pDefs[i].m_BlendSequence = DeserializeBlendSequence(ar,iLocalVersion);

          if (m_bBrokenVersion3 && !bOldBroken)
          {
            Vision::Error.SystemMessage("Transition table '%s' has been saved with incompatible version. Fixing it, but please re-save file", GetFilename());
            VASSERT_MSG(FALSE,"Transition table has been saved with incompatible version. Fixing it, but please re-save file. See log for filename");
            return; // read the whole file again
          }

        } else
        {
          ar.Read(&pDefs[i], VTransitionDef::GetSize(iLocalVersion), VTransitionDef::GetDataLayout(iLocalVersion));
          if(iLocalVersion >= TRANSITION_TABLE_VERSION_3)
            pDefs[i].m_BlendSequence = DeserializeBlendSequence(ar,iLocalVersion);
        }
      }

      // Read transition sets
      ar >> iSetCount;
      VTransitionSet* pSets = AllocateTransitionSets(iSetCount);
      for (int i = 0; i < iSetCount; i++)
      {
        pSets[i].m_pTransitionTable = this;
        ar >> pSets[i];
      }
    }
  }
  else
  {
    // Data layout
    ar << 'V';
    ar << 'T';
    ar << 'R';
    ar << 'A';
    ar << iLocalVersion;

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_1)
    {
      // Write associated animation definitions
      ar << m_iSequenceSetCount;
      for (int i = 0; i < m_iSequenceSetCount; i++)
      {
        m_pSequenceSet[i].m_pTransitionTable = this;
        ar << m_pSequenceSet[i];
      }

      // Write associated animation definitions
      ar << m_iSequenceDefCount;
      for (int i = 0; i < m_iSequenceDefCount; i++)
      {
        m_pSequenceDef[i].m_pTransitionTable = this;
        ar << m_pSequenceDef[i];
      }
    }

    if (iLocalVersion >= TRANSITION_TABLE_VERSION_0)
    {
      // Write out transition definitions
      ar << m_iTransitionDefCount;
      for (int i = 0; i < m_iTransitionDefCount; i++)
      {
        ar.Write(&m_pTransitionDef[i], VTransitionDef::GetSize(TRANSITION_TABLE_VERSION_CURRENT), VTransitionDef::GetDataLayout(TRANSITION_TABLE_VERSION_CURRENT));  

        if(iLocalVersion >= TRANSITION_TABLE_VERSION_3)
        {
          // Write sequence name of blending sequence, or if blend sequence 
          // not available write zero-terminated string.
          if(m_pTransitionDef[i].m_BlendSequence != NULL)
            VSequenceDef::WriteSequenceProxy(ar,m_pTransitionDef[i].m_BlendSequence);
          else
            ar.WriteStringBinary("");
        }
      }
      // Write out transition sets
      ar << m_iTransitionSetCount;
      for (int i = 0; i < m_iTransitionSetCount; i++)
        ar << m_pTransitionSet[i];
    }
  }
}

VisAnimSequence_cl* VTransitionTable::DeserializeBlendSequence(VArchive &ar, char iLocalVersion )
{
  char szSequenceName[1024];
  int iRead = ar.ReadStringBinary(szSequenceName,1024);

  // extra sanity checks for this version:
  if (iLocalVersion==3)
  {
    // basically checking strlen(szSequenceName)==iRead, but using strlen is dangerous here
    for (int i=0;i<iRead;i++)
      if (szSequenceName[i]==0)
      {
        m_bBrokenVersion3 = true;
        return NULL;
      }
    if (szSequenceName[iRead]!=0)
    {
      m_bBrokenVersion3 = true;
      return NULL;
    }
  }

  VSequenceSet *pSequenceSet = NULL;
  VisAnimSequence_cl *pSequence = NULL;

  // Iterate through all sequences sets and try to find sequence with given name
  if(!(szSequenceName[0] == '\0'))
  {
    for(int i=0; i < this->GetNumSequenceSets(); i++)
    {
      pSequenceSet = this->GetSequenceSetByIndex(i);
      
      if(pSequenceSet->GetOwnerSet())
        pSequence = pSequenceSet->GetOwnerSet()->GetSequence(szSequenceName);
      
      if(pSequence)
        return pSequence;
    }
  }

  return NULL;
}

#if defined(__SNC__)
#pragma diag_pop
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
