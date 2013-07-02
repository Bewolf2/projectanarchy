/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/FmodEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


// forward declaration of Fmod event callback
FMOD_RESULT F_CALLBACK EventCallback(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata);


// -------------------------------------------------------------------------- //
// Constructor/ Destructor                                                 
// -------------------------------------------------------------------------- //

// for serialization
VFmodEvent::VFmodEvent() :
  VisObject3D_cl(),
  m_iFlags(0),
  m_sEventName(),
  m_pEvent(NULL),
  m_bIsPlaying(false),
  m_bPlayedOnce(false),
  m_bInfoOnly(false),
  m_bStartPlayback(false)
{
  m_pOwner = NULL;
}

VFmodEvent::VFmodEvent(const char *szEventName, VFmodEventCollection* pOwner, VFmodEventGroup* pEventGroup, const hkvVec3 &vPos, int iFlags) :
  VisObject3D_cl(),
  m_iFlags(iFlags),
  m_sEventName(szEventName),
  m_pEvent(NULL),
  m_bIsPlaying(false),
  m_bPlayedOnce(false),
  m_bInfoOnly(false),
  m_bStartPlayback(false)
{
  VASSERT(szEventName!=NULL && pOwner!=NULL && pEventGroup!=NULL);

  SetPosition(vPos);
  m_spEventGroup = pEventGroup;

  Init();

  m_pOwner = pOwner;
  pOwner->Add(this);

  if(Vision::Editor.IsAnimatingOrPlaying() && (iFlags & VFMOD_FLAG_PAUSED) == 0)
    Start();
}

VFmodEvent::~VFmodEvent()
{
  OnDisposeObject();
}

void VFmodEvent::DisposeObject()
{
  VFmodEventPtr keepRef = this; // ensure the event gets deleted at the end of the function

  Stop();

  VisObject3D_cl::DisposeObject();

  // Note: This might call VFmodEvent::dtor() which will dispose the event!
  if (m_pOwner)
    m_pOwner->Remove(this);
  m_pOwner = NULL;
}


// -------------------------------------------------------------------------- //
// Event property functions                                               
// -------------------------------------------------------------------------- //
void VFmodEvent::Start(bool bAlsoInEditor)
{
  if (IsPlaying() || m_bInfoOnly)
    return;   
  
  if (!bAlsoInEditor && !Vision::Editor.IsAnimatingOrPlaying())
    return;

  VFmodManager &manager = VFmodManager::GlobalManager();
  if (manager.IsInitialized() && m_pEvent)
  {
    // stop old playing 
    m_pEvent->stop(true);

    const hkvVec3 &vPos = GetPosition();
    const hkvVec3 &vDir = GetDirection();
    m_pEvent->set3DAttributes((FMOD_VECTOR *)&vPos, NULL, (FMOD_VECTOR *)&vDir); // no speed (yet)
 

    VFmodEventGroup* pEventGroup = GetEventGroup();
    if (!pEventGroup->m_pEventGroup) // event group not loaded successfully
      return;

    // Initially the event is not started, then within the first RunTick this is done. This is required since 
    // the 3D listener attributes has to be set prior to playing an event. Otherwise Fmod would perform 
    // occlusion-raycasts without having initialized listener-attributes, which potentially results in a
    // wrong occlusion-behavior.
    m_bStartPlayback = true;

    m_bIsPlaying = true;
    m_bPlayedOnce = true;
  }

  Helper_SetFlag(VFMOD_FLAG_PAUSED, !IsPlaying());
}

void VFmodEvent::Stop(bool bImmediate)
{
  if (!IsPlaying() || m_bInfoOnly)
    return;

  VFmodManager &manager = VFmodManager::GlobalManager();
  manager.m_bAnyStopped = true; // notify manager

  if (manager.IsInitialized() && m_pEvent)
  {
    bool bStopImmediate = (GetFlags()&VFMOD_FLAG_NODISPOSE)?bImmediate:true;
    FMOD_WARNINGCHECK(m_pEvent->stop(bStopImmediate));
  }

  m_bStartPlayback = false;
  m_bIsPlaying = false;
}

bool VFmodEvent::IsPlaying() const
{
  return m_bIsPlaying;
}

bool VFmodEvent::IsReady() const
{
  return (m_spEventGroup) ? m_spEventGroup->IsReady() : false;
}

void VFmodEvent::SetPaused(bool bStatus)
{
  if (IsPlaying()!=bStatus || m_bInfoOnly)
    return;

  VFmodManager &manager = VFmodManager::GlobalManager();
  if (!manager.IsInitialized() || !m_pEvent)
    return;

  FMOD_EVENT_STATE eventState;
  m_pEvent->getState(&eventState);

  if (!bStatus && (eventState & FMOD_EVENT_STATE_PLAYING)==0)
    Start();
  else
  {
    m_pEvent->setPaused(bStatus);
    m_bIsPlaying = !bStatus;
    if (m_bIsPlaying)
      m_bPlayedOnce = true;
    else m_bStartPlayback = false;
  }
 
  Helper_SetFlag(VFMOD_FLAG_PAUSED, !IsPlaying());
}

void VFmodEvent::SetMuted(bool bStatus)
{
  Helper_SetFlag(VFMOD_FLAG_MUTE, bStatus);
  if (m_pEvent && !m_bInfoOnly)
    m_pEvent->setMute(bStatus);
}

bool VFmodEvent::GetInfo(int *pEventIndex, char **pszEventName, FMOD_EVENT_INFO *pInfo) const
{
  if (m_pEvent)
  {
    FMOD_RESULT result = m_pEvent->getInfo(pEventIndex, pszEventName, pInfo);
    return (result==FMOD_OK);
  }
  return false;
}

bool VFmodEvent::GetParameter(const char *szParamName, FMOD::EventParameter **pParameter) const
{ 
  VASSERT(szParamName!=NULL && pParameter!=NULL);

  if (m_pEvent)
  {
    FMOD_RESULT result = m_pEvent->getParameter(szParamName, pParameter);
    return (result==FMOD_OK);
  }
  return false;
}

bool VFmodEvent::GetParameter(int iParamIndex, FMOD::EventParameter **pParameter) const
{
  VASSERT(pParameter!=NULL);

  if (m_pEvent)
  {
    FMOD_RESULT result = m_pEvent->getParameterByIndex(iParamIndex, pParameter);
    return (result==FMOD_OK);
  }
  return false;
}

bool VFmodEvent::GetProperty(const char *szPropertyName, void *pValue, bool bInstanceValue) const
{
  VASSERT(szPropertyName!=NULL && pValue!=NULL);

  if (m_pEvent)
  {
    FMOD_RESULT result = m_pEvent->getProperty(szPropertyName, pValue, bInstanceValue);
    return (result==FMOD_OK);
  }
  return false;
}

void VFmodEvent::SetProperty(const char *szPropertyName, void *pValue, bool bInstanceValue)
{
  VASSERT(szPropertyName!=NULL && pValue!=NULL);

  if (m_pEvent && !m_bInfoOnly)
    FMOD_WARNINGCHECK(m_pEvent->setProperty(szPropertyName, pValue, bInstanceValue));
}

bool VFmodEvent::GetProperty(int iPropertyIndex, void *pValue, bool bInstanceValue) const
{
  VASSERT(pValue!=NULL);

  if (m_pEvent)
  {
    FMOD_RESULT result = m_pEvent->getPropertyByIndex(iPropertyIndex, pValue, bInstanceValue);
    return (result==FMOD_OK);
  }
  return false;
}

void VFmodEvent::SetProperty(int iPropertyIndex, void *pValue, bool bInstanceValue)
{
  VASSERT(pValue!=NULL);

  if (m_pEvent && !m_bInfoOnly)
    FMOD_WARNINGCHECK(m_pEvent->setPropertyByIndex(iPropertyIndex, pValue, bInstanceValue));
}


// -------------------------------------------------------------------------- //
// Overridden functions                                                 
// -------------------------------------------------------------------------- //
void VFmodEvent::OnDisposeObject()
{
  if (m_pEvent)
    m_pEvent->setUserData(NULL);
}

void VFmodEvent::OnObject3DChanged(int iO3DFlags)
{
  VisObject3D_cl::OnObject3DChanged(iO3DFlags);

  if (iO3DFlags & VisObject3D_cl::VIS_OBJECT3D_POSCHANGED)
  {
    if (m_pEvent && !m_bInfoOnly)
    {
      const hkvVec3& vPos = GetPosition();
      const hkvVec3& vDir = GetDirection();
      m_pEvent->set3DAttributes((FMOD_VECTOR *)&vPos, NULL, (FMOD_VECTOR *)&vDir); // no speed (yet)
    }
  }
}

static int VFMOD_TRIGGER_ID_PAUSE = -1;
static int VFMOD_TRIGGER_ID_RESUME = -1;

void VFmodEvent::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VisObject3D_cl::MessageFunction(iID, iParamA, iParamB);

  if (iID==VIS_MSG_TRIGGER)
  {
    // cache IDs to avoid string comparisons
    if (VFMOD_TRIGGER_ID_PAUSE == -1)
    {
      VFMOD_TRIGGER_ID_PAUSE = IVObjectComponent::RegisterStringID(VFMOD_TRIGGER_PAUSE);
      VFMOD_TRIGGER_ID_RESUME = IVObjectComponent::RegisterStringID(VFMOD_TRIGGER_RESUME);
    }

    // the trigger components are added by vForge and serialized if used
    VisTriggerTargetComponent_cl *pTarget = (VisTriggerTargetComponent_cl *)iParamB;
    if (pTarget->m_iComponentID==VFMOD_TRIGGER_ID_PAUSE)
      SetPaused(true);
    else if (pTarget->m_iComponentID==VFMOD_TRIGGER_ID_RESUME)
    {
      if (IsPaused())
        SetPaused(false);
      else
      {
        Helper_SetFlag(VFMOD_FLAG_PAUSED, false);
        Start();
      }
    }
    return;
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

void VFmodEvent::GetDependencies(VResourceSnapshot &snapshot)
{
  VisObject3D_cl::GetDependencies(snapshot);
  if (m_spEventGroup)
    m_spEventGroup->GetDependencies(snapshot);
}

#endif


// -------------------------------------------------------------------------- //
// Serialization                                                
// -------------------------------------------------------------------------- //
V_IMPLEMENT_SERIAL( VFmodEvent, VisObject3D_cl, 0, &g_FmodModule );

void VFmodEvent::Serialize( VArchive &ar )
{
  VisObject3D_cl::Serialize(ar);
  char iVersion = -1;
  if (ar.IsLoading())
  {
    ar >> iVersion; 
    VASSERT(iVersion >= VFMOD_EVENT_VERSION_0 && iVersion <= VFMOD_EVENT_VERSION_CURRENT);

    VFmodManager &manager = VFmodManager::GlobalManager();

    // load resource 
    char szEventProjectPath[FS_MAX_PATH+1];
    char szEventGroupName[FS_MAX_PATH+1];
    ar.ReadStringBinary(szEventProjectPath, FS_MAX_PATH);
    ar.ReadStringBinary(szEventGroupName, FS_MAX_PATH);
    m_spEventGroup = manager.LoadEventGroup(szEventProjectPath, szEventGroupName);  

    // event properties
    ar >> m_sEventName;

    // @@@ Force events to be non-disposable
    //m_iFlags |= VFMOD_FLAG_NODISPOSE;
    
    ar >> m_iFlags;

    if(iVersion >= VFMOD_EVENT_VERSION_1)
    {
      ar >> m_bPlayedOnce;
    }

    // init instance
    Init();

    // add to global list
    m_pOwner = &VFmodManager::GlobalManager().Events();
    m_pOwner->Add(this);
  }
  else
  {
    ar << (char)VFMOD_EVENT_VERSION_CURRENT;

    // save resource properties (rather than SerializeX of resource)
    if (m_spEventGroup)
    {
      ar.WriteStringBinary(m_spEventGroup->m_sEventProjectPath);
      ar.WriteStringBinary(m_spEventGroup->m_sEventGroupName);
    }
    else
    {
      ar << (int)0;
      ar.WriteStringBinary(NULL);
      ar.WriteStringBinary(NULL);
    }

    // event properties
    VASSERT(m_sEventName.IsEmpty() == false);
    ar << m_sEventName;
    ar << m_iFlags;
    ar << m_bPlayedOnce;
  }
}

void VFmodEvent::OnDeserializationCallback(const VSerializationContext &context)
{
  VisObject3D_cl::OnDeserializationCallback(context);

  if(Vision::Editor.IsAnimatingOrPlaying() && (m_iFlags & VFMOD_FLAG_PAUSED) == 0)
    Start();  
}


// -------------------------------------------------------------------------- //
// Internal functions                                                
// -------------------------------------------------------------------------- //
void VFmodEvent::Init()
{
  VASSERT(m_spEventGroup && !m_sEventName.IsEmpty());

  VFmodManager &manager = VFmodManager::GlobalManager();
  if (!manager.IsInitialized() || !m_spEventGroup->m_pEventGroup)
    return; 
  
  const hkvVec3 &vPos = GetPosition();
  const hkvVec3 &vDir = GetDirection();

  // First try to get an event instance, which might fail depend upon the "Max playbacks behavior" of this event.
  FMOD_EVENT_MODE mode = manager.m_config.bLoadEventDataAsyncronous ? FMOD_EVENT_NONBLOCKING : FMOD_EVENT_DEFAULT;
  FMOD_RESULT result = m_spEventGroup->m_pEventGroup->getEvent(m_sEventName, mode, &m_pEvent);

  // If event instance could be retrieved, set all relevant properties. Otherwise retrieve info-only-event to ensure, 
  // that "Just fail if quietest" event behavior (see FMOD documentation) works, and mark event as info-only, so that 
  // later on it can try to retrieve the actual event instance.
  if (result==FMOD_OK) 
  {  
    m_pEvent->setUserData(this); // ... so that we can get from event to VFmodEvent
    m_pEvent->setCallback(EventCallback, NULL);
    m_pEvent->setMute(IsMuted());
  }
  else 
  {
    FMOD_WARNINGCHECK(m_spEventGroup->m_pEventGroup->getEvent(m_sEventName, FMOD_EVENT_INFOONLY, &m_pEvent));
    if (!m_pEvent)
      return;
    m_bInfoOnly = true;
  }

  // Set volume-affecting properties.
  m_pEvent->set3DAttributes((FMOD_VECTOR *)&vPos, NULL, (FMOD_VECTOR *)&vDir); // no speed (yet)
}

void VFmodEvent::Reset()
{
  VASSERT(m_spEventGroup!=NULL);

  VFmodManager &manager = VFmodManager::GlobalManager();
  if (!manager.IsInitialized())
    return; 
  
  VASSERT(m_pEvent && !m_sEventName.IsEmpty() && !m_bInfoOnly);

  m_bStartPlayback = m_bStartPlayback || m_bIsPlaying;
  m_bIsPlaying = false;

  // get current "Max playbacks behavior" of this event instance
  // 1 = steal oldest, 2 = steal newest, 3 = steal quietest, 4 = just fail, 5 = just fail if quietest
  int iMaxPlaybackBehavior = 0;
  m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_MAX_PLAYBACKS_BEHAVIOR, &iMaxPlaybackBehavior, true);
  
  // check, whether this event instance is currently looped
  int iOneshot = 0;
  m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_ONESHOT, &iOneshot, true);

  // since this event instance handle becomes invalid after the instance will be stolen, remove reference to VFmodEvent
  // instance in event user-data and set event pointer to NULL
  m_pEvent->setUserData(NULL);
  m_pEvent = NULL;

  // All stolen event instances, which have a "Max playbacks behavior" without failing (1 = steal oldest, 2 = steal newest, 
  // 3 = steal quietest), are not marked as info-only. Instead these instances will be purged in the next RunTick() 
  // of the VFmodManager. Even event instances with VFMOD_FLAG_NODISPOSE will be purged. This is absolute necessary, in order 
  // to avoid an endless cycle of stealing and recovering of such event instances, as none of the requests for a not info-only-
  // event will fail.
  if (iMaxPlaybackBehavior<4)
  {
    m_iFlags &= ~VFMOD_FLAG_NODISPOSE;
    manager.SetAnyStopped(true);
    return;
  }

  // Stolen event instances, which are not looped and not flagged as not to dispose, are not marked as info-only.
  // Instead these instances will be purged in the next RunTick() of the VFmodManager. However, within vForge in 
  // editing mode these event instances are flagged as info-only, in order to allow starting the playback by the 
  // corresponding hotspot button.
  if (iOneshot==1 && (m_iFlags & VFMOD_FLAG_NODISPOSE)!=0 && Vision::Editor.IsAnimatingOrPlaying())
  {
    manager.SetAnyStopped(true);
    return;
  }

  // retrieve info-only-event
  FMOD_ERRORCHECK(m_spEventGroup->m_pEventGroup->getEvent(m_sEventName, FMOD_EVENT_INFOONLY, &m_pEvent));
  VASSERT(m_pEvent!=NULL);

  // mark instance as info-only 
  m_bInfoOnly = true;
}

void VFmodEvent::Update(bool bForceUpdate)
{
  if (m_pEvent && m_bInfoOnly && (m_bStartPlayback || bForceUpdate))
  { 
    VASSERT(m_spEventGroup && !m_sEventName.IsEmpty());

    // update volume-affecting properties of info-only-event
    const hkvVec3 &vPos = GetPosition();
    const hkvVec3 &vDir = GetDirection();
    m_pEvent->set3DAttributes((FMOD_VECTOR *)&vPos, NULL, (FMOD_VECTOR *)&vDir); // no speed (yet)

    // Now try to get an event instance, which might will fail depend upon the "Max playbacks behavior" of this event. 
    // In case of failure m_pEvent will be set to NULL, therefore save the info-only-event pointer, in order to restore
    // it later on.
    FMOD::Event *pInfoOnlyEvent = m_pEvent; 
    FMOD_EVENT_MODE mode = VFmodManager::GlobalManager().m_config.bLoadEventDataAsyncronous ? FMOD_EVENT_NONBLOCKING : FMOD_EVENT_DEFAULT;  
    FMOD_RESULT result = m_spEventGroup->m_pEventGroup->getEvent(m_sEventName, mode, &m_pEvent);
    
    // If event instance could be retrieved, set all relevant properties (except 3D - Attributes, which are copied
    // from the info-only-event) and start playback, when it is necessary.
    if (result==FMOD_OK) 
    {   
      m_pEvent->setUserData(this); // ... so that we can get from event to VFmodEvent
      m_pEvent->setCallback(EventCallback, NULL);
      m_pEvent->setMute(IsMuted());
      m_bInfoOnly = false;

      if (Vision::Editor.IsAnimatingOrPlaying() && (m_iFlags & VFMOD_FLAG_PAUSED) == 0)
        Start();
    }
    else
    {
      m_pEvent = pInfoOnlyEvent;
    }
  }

  // start playback
  if (m_bStartPlayback && !m_bInfoOnly)
  {
    if (m_pEvent)
      FMOD_WARNINGCHECK(m_pEvent->start());
    m_bStartPlayback = false;
  }

  // update last time used when playing event 
  if (IsPlaying())
    GetEventGroup()->UpdateTimeStamp(); 
}

void VFmodEvent::SetPlaying(bool bStatus) 
{ 
  m_bIsPlaying = bStatus; 
}


// -------------------------------------------------------------------------- //
// Fmod event callback 
// -------------------------------------------------------------------------- //
FMOD_RESULT F_CALLBACK EventCallback(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata)
{
  VFmodEvent* pEvent;
  ((FMOD::Event*)event)->getUserData((void**)&pEvent);

  if(pEvent)
  {
    // retrieves manager
    VFmodManager* pMan = &VFmodManager::GlobalManager(); 

    if (pMan)
    {
      switch(type)
      {    
      case FMOD_EVENT_CALLBACKTYPE_STOLEN:
        {
          pEvent->Reset();
        }
        break;

      case FMOD_EVENT_CALLBACKTYPE_EVENTFINISHED:
        {  
          pEvent->SetPlaying(false);
          pMan->SetAnyStopped(true);
        }
        break;
      }
      VFmodEventDataObject dataObj(&pMan->OnEventCallback, pEvent, type, param1, param2);
      dataObj.Trigger();
    }
  }
  return FMOD_OK;
}


void VFmodEventCollection::DebugOutput()
{
  int yk = 20;
  int iCount = Count();
  for (int i=0;i<iCount;i++,yk+=12)
  {
    VFmodEvent* pEvent = GetAt(i);

    char *szEventName = NULL;
    pEvent->GetInfo(NULL, &szEventName, NULL);
    if (szEventName)
    {
      /*
      Vision::Message.Print(1, 40, yk, "%i. '%s' (Event %s), playing: %s",
        i,
        pEvent->GetEventGroup()->m_szEventGroupName,
        szEventName,
        pEvent->IsPlaying() ? "yes":"no"
        );
      */
    }
  }
}

void VFmodEventCollection::Update()
{
  int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    VFmodEvent* pEvent = GetAt(i);
    pEvent->Update();
  }
}

void VFmodEventCollection::PurgeNotPlaying()
{ 
  int iCount = Count();
  for (int i=iCount-1;i>=0;i--)
  {
    VFmodEvent* pEvent = GetAt(i);
    if(!pEvent->m_bPlayedOnce || pEvent->IsPlaying() || (pEvent->m_iFlags&VFMOD_FLAG_NODISPOSE)!=0 || pEvent->IsInfoOnly())
    {
      continue; // Valid event - cannot be removed
    }

    if (pEvent->m_pEvent)
      pEvent->m_pEvent->setUserData(NULL);

    if(!Vision::Editor.IsInEditor())
      VASSERT(pEvent->GetRefCount() == 1 && "Event removed from collection but still referenced (non-disposeable object)! This results in events not being held in Fmod sound manager collection any longer but still reside in memory. Check if VFMOD_FLAG_NODISPOSE is set on event creation for this purpose.");  
    
    pEvent->m_pOwner = NULL;
    RemoveAt(i);
  }
}


void VFmodEventCollection::StopAll(bool bReasonIsUnloadWorld)
{
  int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    VFmodEvent* pEvent = GetAt(i);
    
    if(bReasonIsUnloadWorld && pEvent->HasFlag(VFMOD_FLAG_SURVIVE_UNLOAD_WORLD))
      continue;

    pEvent->Stop();
  }

  PurgeNotPlaying();
}

VFmodEvent* VFmodEventCollection::SearchEvent(const char* szName) const
{
  int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    VFmodEvent* pEvent = GetAt(i);
    if(pEvent->HasObjectKey(szName))
      return pEvent;
  }

  return NULL;
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
