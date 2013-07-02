/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionBase.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

///////////////////////////////////////////////////////////////////////////////////////
// VTransitionManager
///////////////////////////////////////////////////////////////////////////////////////

VTransitionManager VTransitionManager::g_GlobalManager;

VTransitionManager::VTransitionManager(const char *szName)
: VisResourceManager_cl(szName,VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER), m_bGlobalAnimationEvents(false)
{

}

void VTransitionManager::OneTimeInit()
{
  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(80,80,80));
}

void VTransitionManager::OneTimeDeInit()
{
  Vision::Callbacks.OnUpdateSceneFinished -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::ResourceSystem.UnregisterResourceManager(this);
}

VTransitionManager& VTransitionManager::GlobalManager()
{
  return g_GlobalManager;
}

VTransitionTable *VTransitionManager::LoadTransitionTable(VDynamicMesh *pTargetMesh, const char *szFilename)
{
  VTransitionTable *pTable = (VTransitionTable *)LoadResource(szFilename);
  if (pTable)
    return pTable;

  pTable = new VTransitionTable(this,pTargetMesh);
  pTable->SetFilename(szFilename);
  pTable->EnsureLoaded();
  return pTable;
}

VTransitionTable *VTransitionManager::CreateDefaultTransitionTable(VDynamicMesh *pTargetMesh)
{
  // Create a default transition definition
  VTransitionDef transition;
  transition.m_eType = TRANSITION_TYPE_CROSSFADE;
  transition.m_fBlendDuration = 0.5;
  transition.m_iID = 0;

  // Create transition table with given transition definition
  return CreateDefaultTransitionTable(pTargetMesh, transition);
}

VTransitionTable *VTransitionManager::CreateDefaultTransitionTable(VDynamicMesh *pTargetMesh, VTransitionDef transition)
{
  VTransitionTable *pTable = new VTransitionTable(this, pTargetMesh);
  
  VASSERT(pTargetMesh->GetSequenceSetCollection()->GetSequenceSetCount() > 0);

  // Get number of sequences from first sequence set
  VisAnimSequenceSet_cl *pSequences = pTargetMesh->GetSequenceSetCollection()->GetSequenceSet(0);
  int iCount = pSequences->GetSequenceCount();

  // Allocate arrays
  VTransitionDef *defs = pTable->AllocateTransitionDefs(1);
  VTransitionSet *sets = pTable->AllocateTransitionSets(iCount);

  // Set given default transition definition
  defs[0] = transition;

  // Set up all transition sets
  for (int i = 0; i < iCount; i++)
  {
    sets[i].AllocateTargets(iCount);
    sets[i].Init(pSequences->GetSequence(i), pTable, NULL);
    
    int  iIndex = 0;
    for (int j = 0; j < iCount; j++)
    {
      if (sets[i].GetSourceSequence() != pSequences->GetSequence(j))
      {
        sets[i].AddTransitionPair(pSequences->GetSequence(j), defs, iIndex);
        iIndex++;
      }
    }
  }

  return pTable;
}

void VTransitionManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    if (Vision::Editor.IsPlaying())
    {
      const int iCount = m_Instances.Count();
      for (int i=0;i<iCount;i++)
        m_Instances.GetAt(i)->OnThink();
    }      
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    m_Instances.Clear();
    return;
  }
}

bool VTransitionManager::GetUseGlobalAnimationEvents()
{
  return m_bGlobalAnimationEvents;
}

void VTransitionManager::SetUseGlobalAnimationEvents(bool bGlobalEvents)
{
  m_bGlobalAnimationEvents = bGlobalEvents;
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
