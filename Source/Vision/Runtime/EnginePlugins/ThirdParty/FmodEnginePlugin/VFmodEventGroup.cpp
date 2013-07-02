/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/FmodEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodEventGroup.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VManagedResource* VFmodEventGroupManager::CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
{
  // split resource name into project-path and group-name
  char szFilenameCopy[2*FS_MAX_PATH];
  strcpy(szFilenameCopy, szFilename);
  VStringTokenizerInPlace tokenizer(szFilenameCopy, '|');
  const char *szEventProjectPath = tokenizer.Next();
  const char *szEventGroupName = tokenizer.Next();
  VASSERT(szEventProjectPath!=NULL && szEventGroupName!=NULL);

  VManagedResource *pRes = VFmodManager::GlobalManager().LoadEventGroup(szEventProjectPath, szEventGroupName);
  if (!pRes)
    return NULL;

  pRes->EnsureLoaded();

  return pRes;
}


// -------------------------------------------------------------------------- //
// Constructor/ Destructor                                                 
// -------------------------------------------------------------------------- //
VFmodEventGroup::VFmodEventGroup(VFmodEventGroupManager *pManager) : VManagedResource(pManager)
{
  m_iSysMem = 0;
  m_pEventGroup = NULL;
}

VFmodEventGroup::~VFmodEventGroup()
{
}


// -------------------------------------------------------------------------- //
// Instances                                                 
// -------------------------------------------------------------------------- //
VFmodEvent *VFmodEventGroup::CreateEvent(const char *szEventName, const hkvVec3 &vPos, int iFlags)
{
  VFmodEvent *pEvent = new VFmodEvent(szEventName, &VFmodManager::GlobalManager().m_events, this, vPos, iFlags);
  return pEvent;
}


// -------------------------------------------------------------------------- //
// Resource property functions                                               
// -------------------------------------------------------------------------- //
bool VFmodEventGroup::GetProperty(const char *szPropertyName, void *pValue) const
{   
  VASSERT(szPropertyName!=NULL && pValue!=NULL);

  if (m_pEventGroup)
  {
    FMOD_RESULT result = m_pEventGroup->getProperty(szPropertyName, pValue);
    return (result==FMOD_OK);
  }
  return false;
}

bool VFmodEventGroup::GetProperty(int iPropertyIndex, void *pValue) const
{
  VASSERT(pValue!=NULL);

  if (m_pEventGroup)
  {
    FMOD_RESULT result = m_pEventGroup->getPropertyByIndex(iPropertyIndex, pValue);
    return (result==FMOD_OK);
  }
  return false;
}

bool VFmodEventGroup::GetInfo(int *pGroupIndex, char **pszGroupName) const
{
  VASSERT(pGroupIndex!=NULL && pszGroupName!=NULL);

  if (m_pEventGroup) 
  {
    FMOD_RESULT result = m_pEventGroup->getInfo(pGroupIndex, pszGroupName);
    return (result==FMOD_OK);
  }
  return false;
}

bool VFmodEventGroup::IsReady() const
{
  if (!VFmodManager::GlobalManager().IsInitialized())
    return true;

  VASSERT(m_pEventGroup!=NULL);
  FMOD_EVENT_STATE eventState;
  FMOD_WARNINGCHECK(m_pEventGroup->getState(&eventState));
  return (eventState & FMOD_EVENT_STATE_READY);
}




// -------------------------------------------------------------------------- //
// Overridden functions                                                 
// -------------------------------------------------------------------------- // 
BOOL VFmodEventGroup::Reload()
{
  VFmodManager &manager = VFmodManager::GlobalManager();
  if (!manager.IsInitialized())
    return TRUE;

  VASSERT(!m_pEventGroup);

  FMOD::EventSystem *pEventSystem = manager.GetFmodEventSystem();
  VASSERT(pEventSystem!=NULL);

  // the resource filename is concatenated from the corresponding project-path and group-name, 
  // which are separated by '|'
  const char *szResourceName = GetFilename();

  // split resource name into project-path and group-name
  char szResourceNameCopy[2*FS_MAX_PATH];
  strcpy(szResourceNameCopy, szResourceName);
  VStringTokenizerInPlace tokenizer(szResourceNameCopy, '|');
  const char *szEventProjectPath = tokenizer.Next();
  const char *szEventGroupName = tokenizer.Next();
  VASSERT(szEventProjectPath!=NULL && szEventGroupName!=NULL);
  m_sEventProjectPath = szEventProjectPath;
  m_sEventGroupName = szEventGroupName;

  FMOD::EventProject *pEventProject = NULL;
  pEventProject = manager.LoadEventProject(m_sEventProjectPath.GetChar());
  if (!pEventProject)
    return FALSE;

  // retrieve event group from event project
  FMOD_WARNINGCHECK(pEventProject->getGroup(m_sEventGroupName, true, &m_pEventGroup));
  if (!m_pEventGroup)
    return FALSE;

  // load wave data and allocate event instances for all events within an event group 
  FMOD_EVENT_MODE mode = manager.m_config.bLoadEventDataAsyncronous ? FMOD_EVENT_NONBLOCKING : FMOD_EVENT_DEFAULT;
  FMOD_RESULT result = m_pEventGroup->loadEventData(FMOD_EVENT_RESOURCE_STREAMS_AND_SAMPLES, mode);
  FMOD_WARNINGCHECK(result);
  if(result!=FMOD_OK)
    return FALSE;

  // get the size in memory
  unsigned int iSize = 0;
  FMOD_WARNINGCHECK(m_pEventGroup->getMemoryInfo(0, FMOD_EVENT_MEMBITS_EVENTINSTANCE_GROUP|FMOD_EVENT_MEMBITS_SOUNDDEF_GROUP, &iSize, 0));

  SetNewMemSize(VRESOURCEMEMORY_SYSTEM, iSize);  
  m_iSysMem = (int)iSize;

  return TRUE;
}

BOOL VFmodEventGroup::Unload()
{
  if (!VFmodManager::GlobalManager().IsInitialized())
    return TRUE;

  VASSERT(m_pEventGroup!=NULL);
  FMOD_WARNINGCHECK(m_pEventGroup->freeEventData());
  m_pEventGroup = NULL;

  m_iSysMem = 0;
  SetNewMemSize(VRESOURCEMEMORY_ALLTYPES, m_iSysMem);

  return TRUE;
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

void VFmodEventGroup::GetDependencies(VResourceSnapshot &snapshot)
{
  VManagedResource::GetDependencies(snapshot); 
}

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
