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


// forward declaration of Fmod channel callback
FMOD_RESULT F_CALLBACK ChannelCallback(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2);


// -------------------------------------------------------------------------- //
// Constructor/ Destructor                                                 
// -------------------------------------------------------------------------- //

// for serialization
VFmodSoundObject::VFmodSoundObject() :
  VisObject3D_cl(),
  m_iFlags(0),
  m_iPriority(0),
  m_fVolume(1.0f),
  m_fPan(0.0f),
  m_fConeOutside(-1.0f), // no cone
  m_fConeInside(-1.0f),  // no cone
  m_fFadeMin(1.0f),
  m_fFadeMax(10000.0f),
  m_fVolumeFadeTime(0.0f),
  m_fVolumeFadeDuration(0.0f),
  m_bFading(false),
  m_fPitch(1.0f), // unmodified pitch
  m_pChannel(NULL),
  m_fInitialFreq(0.0f),
  m_bIsPlaying(false),
  m_bPlayedOnce(false),
  m_bUnpause(false)
{
  m_pOwner = NULL;
}

VFmodSoundObject::VFmodSoundObject(VFmodSoundObjectCollection* pOwner, VFmodSoundResource* pResource, const hkvVec3 &vPos, int iFlags, int iPriority) :
  VisObject3D_cl(),
  m_iFlags(iFlags),
  m_iPriority(iPriority),
  m_fVolume(1.0f),
  m_fPan(0.0f),
  m_fConeOutside(-1.0f), // no cone
  m_fConeInside(-1.0f),  // no cone
  m_fFadeMin(1.0f),
  m_fFadeMax(10000.0f),
  m_fStartTime(0.0f),
  m_fVolumeFadeTime(0.0f),
  m_fVolumeFadeDuration(0.0f),
  m_bFading(false),
  m_fPitch(1.0f), // unmodified pitch
  m_pChannel(NULL),
  m_fInitialFreq(0.0f),
  m_bIsPlaying(false),
  m_bPlayedOnce(false),
  m_bUnpause(false)
{
  VASSERT(pOwner && pResource); 
  
  SetPosition(vPos);

  m_spResource = pResource;
  m_pOwner = pOwner;
  pOwner->Add(this);

  if(Vision::Editor.IsAnimatingOrPlaying() && (iFlags & VFMOD_FLAG_PAUSED) == 0)
    Play();
}

VFmodSoundObject::~VFmodSoundObject()
{
  OnDisposeObject();
}

void VFmodSoundObject::DisposeObject()
{
  VFmodSoundObjectPtr keepRef = this; // ensure the object gets deleted at the end of the function

  Stop();

  VisObject3D_cl::DisposeObject();

  // Note: This might call VFmodSoundObject::dtor() which will dispose the sound object!
  if (m_pOwner)
    m_pOwner->SafeRemove(this);
  m_pOwner = NULL;
}


// -------------------------------------------------------------------------- //
// Sound property functions                                               
// -------------------------------------------------------------------------- //
void VFmodSoundObject::Play(float fStartTime, bool bAlsoInEditor)
{
  if (m_spResource == NULL || IsPlaying())
    return;

  if (!bAlsoInEditor && !Vision::Editor.IsAnimatingOrPlaying())
    return; 
  
  m_fStartTime = fStartTime;  
  
  VFmodManager &manager = VFmodManager::GlobalManager();
  if (manager.IsInitialized())
  {
    const hkvVec3 &vPos = GetPosition();

    // stop old playing
    if (m_pChannel)
      m_pChannel->stop();

    VFmodSoundResource* pResource = (VFmodSoundResource*)GetResource();
    if (pResource->m_pSound == NULL) // sound resource not loaded successfully
      return;

    FMOD_WARNINGCHECK(manager.m_pSystem->playSound(FMOD_CHANNEL_FREE, pResource->m_pSound, true, &m_pChannel));
    if (!m_pChannel)
      return;

    FMOD_WARNINGCHECK(m_pChannel->getFrequency(&m_fInitialFreq));

    // if this a music sound object assign it to the music channel group
    if (IsMusic())
      m_pChannel->setChannelGroup(manager.m_pMusicGroup);

    // set everything  
    m_pChannel->setUserData(this); // ...so that we can get from channel to VSoundObject
    m_pChannel->setCallback(ChannelCallback);
    m_pChannel->setMode(IsLooped() ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
    m_pChannel->setPriority(GetPriority());
    m_pChannel->set3DMinMaxDistance(Get3DFadeMinDistance(), Get3DFadeMaxDistance());
    m_pChannel->set3DAttributes((FMOD_VECTOR *)&vPos, NULL); // no speed (yet)
    m_pChannel->setVolume(GetVolume());
    m_pChannel->setPan(GetPan());

    SetPitch(GetPitch());
    SetConeAngles(GetConeAngleInside(), GetConeAngleOutside());

    // Initially the sound is left paused, then within the first RunTick it is unpaused. This is required since 
    // the 3D listener attributes has to be set prior to playing a sound. Otherwise Fmod would perform 
    // occlusion-raycasts without having initialized listener-attributes, which potentially results in a
    // wrong occlusion-behavior.
    m_bUnpause = true;

    m_bIsPlaying = true;
    m_bPlayedOnce = true;

    unsigned int ims = (int)(fStartTime*1000.f); // milliseconds
    m_pChannel->setPosition(ims,FMOD_TIMEUNIT_MS);
    m_pChannel->setMute(IsMuted());
  }

  Helper_SetFlag(VFMOD_FLAG_PAUSED, !IsPlaying());
}

void VFmodSoundObject::Stop()
{
  if (!IsPlaying())
    return;

  VFmodManager &manager = VFmodManager::GlobalManager();
  manager.m_bAnyStopped = true; // notify manager

  if (manager.IsInitialized())
  {
    FMOD_WARNINGCHECK(m_pChannel->stop());
    m_pChannel = NULL;
  }

  m_bUnpause = false;
  m_bIsPlaying = false;
  m_fStartTime = 0.0f;
}

bool VFmodSoundObject::IsPlaying() const
{
  return m_bIsPlaying;
}

bool VFmodSoundObject::IsReady() const
{
  return (m_spResource) ? m_spResource->IsReady() : false;
}

float VFmodSoundObject::GetCurrentPosition(bool bAbsolute) const
{
  if (m_pChannel)
  {
    unsigned int iPos = 0;
    FMOD_WARNINGCHECK(m_pChannel->getPosition(&iPos,FMOD_TIMEUNIT_MS));
    float fSecs = ((float)iPos*(1.0f/1000.0f));
    return bAbsolute ? fSecs : fSecs - GetStartTime();
  }
  return 0.0f;
}

void VFmodSoundObject::SetPaused(bool bStatus)
{
  if (IsPlaying() != bStatus)
    return;

  VFmodManager &manager = VFmodManager::GlobalManager();
  if (!manager.IsInitialized())
    return;

  if (m_pChannel)
  {
    if (bStatus)
    {
      m_pChannel->setPaused(true);
      m_bUnpause = false;
    }
    else
    {
      m_bUnpause = true;
      m_bPlayedOnce = true;
    }
    m_bIsPlaying = !bStatus;
  }
  else 
  {
    if (!bStatus)
      Play();
    else
    {
      m_bUnpause = false;
      m_bIsPlaying = false;
    }
  }

  Helper_SetFlag(VFMOD_FLAG_PAUSED, !IsPlaying());
}

void VFmodSoundObject::SetMuted(bool bStatus)
{
  Helper_SetFlag(VFMOD_FLAG_MUTE, bStatus); 
  if (m_pChannel)
    m_pChannel->setMute(bStatus);
}

void VFmodSoundObject::SetVolume(float fVol)
{
	if (m_fVolume != fVol)
	{
		m_fVolume = fVol;

		if (m_fVolume <= 0.001f)
		{
			Helper_SetFlag(VFMOD_FLAG_MUTE, true);
			m_fVolume = 0.0f;
		}
		else
		{
			Helper_SetFlag(VFMOD_FLAG_MUTE, false);
      if (m_pChannel)
        m_pChannel->setMute(false);
		}

    if (m_pChannel)
      m_pChannel->setVolume(m_fVolume);
	}
}

void VFmodSoundObject::SetPan(float fPan)
{
  VASSERT(GetResource()->Is2D());
  m_fPan = fPan;

  if (m_pChannel)
    m_pChannel->setPan(fPan);
}

void VFmodSoundObject::Set3DFadeDistance(float fMin, float fMax)
{
  m_fFadeMin = fMin;
  m_fFadeMax = fMax;
  VASSERT(m_fFadeMax>=m_fFadeMin);

  if (m_pChannel)
    m_pChannel->set3DMinMaxDistance(fMin, fMax);
}

void VFmodSoundObject::FadeTo(float fNewVolume, float fDuration)
{
  if (fDuration <= 0.0f)
  {
    SetVolume(fNewVolume);
    return;
  }

  m_fVolumeFadeStart = m_fVolume;
  m_fVolumeFadeTarget = fNewVolume;
  m_fVolumeFadeTime = 0.0f;
  m_fVolumeFadeDuration = fDuration;
  m_bFading = true;

  Play();
}

void VFmodSoundObject::FadeOut(float fDuration)
{
  if(fDuration <= 0.0f)
  {
    SetVolume(m_fVolume);
    return;
  }

  m_fVolumeFadeStart = m_fVolume;
  m_fVolumeFadeTarget = 0.0f;
  m_fVolumeFadeTime = 0.0f;
  m_fVolumeFadeDuration = fDuration;
  m_bFading = true;
}

void VFmodSoundObject::FadeIn(float fDuration)
{
  if(fDuration <= 0.0f)
  {
    SetVolume(m_fVolume);
    return;
  }
  if(m_fVolume < 0.001f)
  {
    m_fVolume = 1.0f;
  }

  m_fVolumeFadeStart = 0.0f;
  m_fVolumeFadeTarget = m_fVolume;
  m_fVolumeFadeTime = 0.0f;
  m_fVolumeFadeDuration = fDuration;
  m_bFading = true;

  SetVolume(0.0f);
  Play();
}

void VFmodSoundObject::FadeFromTo(float fStartVolume, float fTargetVolume, float fDuration)
{
  if(fDuration <= 0.0f)
  {
    SetVolume(m_fVolume);
    return;
  }

  m_fVolumeFadeStart = fStartVolume;
  m_fVolumeFadeTarget = fTargetVolume;
  m_fVolumeFadeTime = 0.0f;
  m_fVolumeFadeDuration = fDuration;
  m_bFading = true;
}

void VFmodSoundObject::SetPitch(float fPitch)
{
  m_fPitch = fPitch;

  if (m_pChannel)
  {
    VASSERT_MSG(m_fInitialFreq!=0.0f, "Uninitialized channel");
    m_pChannel->setFrequency(m_fInitialFreq * fPitch);
  }
}

void VFmodSoundObject::SetConeAngles(float fInside, float fOutside)
{
  m_fConeOutside = fOutside;
  m_fConeInside = fInside;

  if (!m_pChannel)
    return;

  if (m_fConeOutside>0.0f)
  {
    hkvVec3 vDir = GetDirection(); // from object3D
    m_pChannel->set3DConeSettings(m_fConeInside, m_fConeOutside, 0.0f); // outside 0 volume
    m_pChannel->set3DConeOrientation((FMOD_VECTOR*)&vDir);
  }
  else // omni
  {
    m_pChannel->set3DConeSettings(360.0f, 360.0f, 1.0f);
  }
}

#define SO_EULER_CONST 2.718281828459045f // exp(1)
#define SO_AIR_ATT_COEF 1.3f

float VFmodSoundObject::CalculateAttenuatedVolume(const hkvVec3& vListenerPos) const
{
  float fVol = 1.0f; 

  if(m_spResource && m_spResource->Is3D())
  {
    const hkvVec3& vSoundEmitterPos = GetPosition();
    float fDist = (vSoundEmitterPos - vListenerPos).getLength();
    float fInvRange = 1.0f / fabs(m_fFadeMax - m_fFadeMin);

    if (!m_spResource->LogAttenuation())
    {
      // Simple mapping from [FadeMin,FadeMax] to [0,1]
      fVol = (1.0f - fDist * fInvRange) + (fInvRange * m_fFadeMin);
      fVol = hkvMath::clamp (fVol, 0.0f, 1.0f);
    }
    else
    {
      fVol = (1.0f - fDist * fInvRange) + (fInvRange * m_fFadeMin);
      fVol = hkvMath::clamp (fVol, 0.0f, 1.0f);
      fVol *= 0.5f;
      hkvMath::pow(SO_EULER_CONST,-fVol * SO_AIR_ATT_COEF);
    }
  }
  return fVol;
}

int VFmodSoundObject::GetChannelIndex() const
{
  int idx = 0;
  if (m_pChannel)
    FMOD_WARNINGCHECK(m_pChannel->getIndex(&idx));
  return idx;
}


// -------------------------------------------------------------------------- //
// Overridden functions                                                 
// -------------------------------------------------------------------------- //
void VFmodSoundObject::OnDisposeObject()
{
  if (m_pChannel)
    m_pChannel->setUserData(NULL);
}

void VFmodSoundObject::OnObject3DChanged(int iO3DFlags)
{
  VisObject3D_cl::OnObject3DChanged(iO3DFlags);

  if (iO3DFlags & VisObject3D_cl::VIS_OBJECT3D_POSCHANGED)
  {
    if (m_pChannel)
    {
      const hkvVec3 &vPos = GetPosition();
      m_pChannel->set3DAttributes((FMOD_VECTOR *)&vPos, NULL); // no speed (yet)
      SetConeAngles(GetConeAngleInside(), GetConeAngleOutside()); // also updates direction
    }
  }
}

static int VFMOD_TRIGGER_ID_PAUSE = -1;
static int VFMOD_TRIGGER_ID_RESUME = -1;

void VFmodSoundObject::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
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
        Play();
      }
    }
    return;
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

void VFmodSoundObject::GetDependencies(VResourceSnapshot &snapshot)
{
  VisObject3D_cl::GetDependencies(snapshot);
  if (m_spResource)
    m_spResource->GetDependencies(snapshot);
}

#endif


// -------------------------------------------------------------------------- //
// Serialization                                                
// -------------------------------------------------------------------------- //
V_IMPLEMENT_SERIAL( VFmodSoundObject, VisObject3D_cl, 0, &g_FmodModule );

void VFmodSoundObject::Serialize( VArchive &ar )
{
  VisObject3D_cl::Serialize(ar);
  char iVersion = -1;
  if (ar.IsLoading())
  {
    ar >> iVersion; 
    VASSERT(iVersion >= VFMOD_SOUNDOBJECT_VERSION_0 && iVersion <= VFMOD_SOUNDOBJECT_VERSION_CURRENT);

    // first, load resource file
    int iResourceFlags;
    char szResourceFile[FS_MAX_PATH+1];
    ar >> iResourceFlags;
    ar.ReadStringBinary(szResourceFile, FS_MAX_PATH); 
    m_spResource = VFmodManager::GlobalManager().LoadSoundResource(szResourceFile, iResourceFlags);

    // sound properties
    ar >> m_iFlags;
    ar >> m_iPriority;
    ar >> m_fVolume >> m_fPan >> m_fConeInside >> m_fConeOutside;
    ar >> m_fFadeMin >> m_fFadeMax;
    ar >> m_bFading;
    ar >> m_fVolumeFadeTime >> m_fVolumeFadeDuration;
    ar >> m_fVolumeFadeStart >> m_fVolumeFadeTarget;
    ar >> m_fPitch;

    if(iVersion >= VFMOD_SOUNDOBJECT_VERSION_1)
    {
      ar >> m_bPlayedOnce;
    }

    // @@@ Force sound objects to be non-looping
    //m_iFlags &= ~VFMOD_FLAG_LOOPED;

    // @@@ Force sound objects to be non-disposable
    //m_iFlags |= VFMOD_FLAG_NODISPOSE;

    // add to global list
    m_pOwner = &VFmodManager::GlobalManager().SoundInstances();
    m_pOwner->Add(this);
  }
  else
  {
    ar << (char)VFMOD_SOUNDOBJECT_VERSION_CURRENT;

    // save resource properties (rather than SerializeX of resource)
    if (m_spResource!=NULL)
    {
      ar << m_spResource->GetResourceFlags();
      ar.WriteStringBinary(m_spResource->GetFilename());
    }
    else
    {
      ar << (int)0;
      ar.WriteStringBinary(NULL);
    }

    // sound object properties
    ar << m_iFlags;
    ar << m_iPriority;
    ar << m_fVolume << m_fPan << m_fConeInside << m_fConeOutside;
    ar << m_fFadeMin << m_fFadeMax;
    ar << m_bFading;
    ar << m_fVolumeFadeTime << m_fVolumeFadeDuration;
    ar << m_fVolumeFadeStart << m_fVolumeFadeTarget;
    ar << m_fPitch;
    ar << m_bPlayedOnce;
  }
}

void VFmodSoundObject::OnDeserializationCallback(const VSerializationContext &context)
{
  VisObject3D_cl::OnDeserializationCallback(context);

  if (Vision::Editor.IsAnimatingOrPlaying() && (m_iFlags & VFMOD_FLAG_PAUSED) == 0)
    Play();
}


// -------------------------------------------------------------------------- //
// Misc                                                
// -------------------------------------------------------------------------- //
float VFmodSoundObject::UpdateFading(bool bApplyVol)
{
  if (!m_bFading)
  {
    return GetVolume();
  }

  if (m_fVolumeFadeTime > m_fVolumeFadeDuration)
  {
    m_bFading = false;
    if (bApplyVol)
      SetVolume(m_fVolumeFadeTarget);
    return m_fVolumeFadeTarget;
  }

  float fNewVolume = m_fVolumeFadeStart + (m_fVolumeFadeTime / m_fVolumeFadeDuration) * (m_fVolumeFadeTarget - m_fVolumeFadeStart);
  if (bApplyVol)
    SetVolume(fNewVolume);

  // Update timer
  m_fVolumeFadeTime += Vision::GetTimer()->GetTimeDifference();

  return fNewVolume;
}

void VFmodSoundObject::DebugRender(IVRenderInterface* pRI)
{
  const hkvVec3 &vCenter = GetPosition();
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA);

  VColorRef iInnerColor(80, 80, 255, 50);
  VColorRef iOuterColor(50, 50, 150, 40);
  
  if (m_fConeOutside>0.f) // render oriented sound
  {
    const hkvVec3 &vDir = GetDirection();

    // inner cones
    pRI->RenderCone(vCenter, vDir, m_fConeInside, m_fFadeMin, iInnerColor, state);
    pRI->RenderCone(vCenter, vDir, m_fConeInside, m_fFadeMax, iInnerColor, state); 
    
    // outer cones 
    pRI->RenderCone(vCenter, vDir, m_fConeOutside, m_fFadeMin, iOuterColor, state);
    pRI->RenderCone(vCenter, vDir, m_fConeOutside, m_fFadeMax, iOuterColor, state);
  }
  else // render omni-directional sound
  {
    // inner sphere
    pRI->RenderSphere(vCenter, m_fFadeMin, iInnerColor, state);

    // outer sphere
    pRI->RenderSphere(vCenter, m_fFadeMax, iOuterColor, state);
  }
}


// -------------------------------------------------------------------------- //
// Internal functions                                                
// -------------------------------------------------------------------------- //
void VFmodSoundObject::Update()
{
  UpdateFading();

  // unpause playback of sound
  if (m_bUnpause)
  {
    if (m_pChannel)
      FMOD_WARNINGCHECK(m_pChannel->setPaused(false));
    m_bUnpause = false;
  }

  // update last time used when playing sound 
  if (IsPlaying())
    GetResource()->UpdateTimeStamp();  
}


// -------------------------------------------------------------------------- //
// Fmod channel callback (called from update() when the channel has finished 
// playing)           
// -------------------------------------------------------------------------- //
// 
FMOD_RESULT F_CALLBACK ChannelCallback(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
  VFmodSoundObject* pObj;
  ((FMOD::Channel*)channel)->getUserData((void**)&pObj);

  if (pObj)
  {
    // retrieves manager
    VFmodManager* pMan = &VFmodManager::GlobalManager(); 

    if (pMan)
    {
      switch(type)
      {
      case FMOD_CHANNEL_CALLBACKTYPE_END:
        {  
          pObj->Stop();
          pMan->SetAnyStopped(true);

          VFmodSoundDataObject dataObj(&pMan->OnSoundCallback, pObj, type);
          dataObj.Trigger();
        
        }
        break;

      case FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT:
        {
          VFmodSoundDataObject dataObj(&pMan->OnSoundCallback, pObj, type);
          VFmodSoundResource* pResource = (VFmodSoundResource*)pObj->GetResource();
          
          // get name of the sync point
          FMOD::Sound* pFmodSound = pResource->GetFmodSoundInstance();
          FMOD_SYNCPOINT* pSyncPoint = NULL;
          pFmodSound->getSyncPoint((int)commanddata1, &pSyncPoint);
          pFmodSound->getSyncPointInfo(pSyncPoint, dataObj.m_sParam, sizeof(dataObj.m_sParam), NULL, 0);
          
          dataObj.Trigger();
        }
        break;
      }
    }
  }
  return FMOD_OK;
}



void VFmodSoundObjectCollection::DebugOutput()
{
  int yk = 20;
  int iCount = Count();
  for (int i=0;i<iCount;i++,yk+=12)
  {
    VFmodSoundObject* pInst = GetAt(i);
    /*
    Vision::Message.Print(1, 40, yk, "%i. '%s' (Channel %i, time :%.2f), playing: %s",
      i,
      pInst->GetSoundResource()->GetFilename(),
      pInst->GetChannelIndex(),pInst->GetCurrentTime(),
      pInst->IsPlaying() ? "yes":"no"
      );
    */
    float x,y;
    if (Vision::Contexts.GetCurrentContext()->Project2D(pInst->GetPosition(),x,y))
    {
      float fCurrentIts = 0.0f;
      /*
      if (pInst->m_pChannel)
        pInst->m_pChannel->getAudibility(&fCurrentIts);
      */
      Vision::Message.Print(1, (int)x, (int)y, "%.3f", fCurrentIts);
      //pInst->IncPosition(0.0f, 0.0f, 0.01f);
    }
  }
}

void VFmodSoundObjectCollection::Update()
{
  int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    VFmodSoundObject* pInst = GetAt(i);
    pInst->Update();
  }
}

void VFmodSoundObjectCollection::PurgeNotPlaying()
{ 
  int iCount = Count();
  for (int i=iCount-1;i>=0;i--)
  {
    VFmodSoundObject* pInst = GetAt(i);
    if(!pInst->m_bPlayedOnce || pInst->IsPlaying() ||
       (pInst->m_iFlags&VFMOD_FLAG_NODISPOSE) != 0)
    {
      continue; // Valid sound - cannot be removed
    }

    if (pInst->m_pChannel)
      pInst->m_pChannel->setUserData(NULL);

    if (!Vision::Editor.IsInEditor())
      VASSERT(pInst->GetRefCount() == 1 && "SoundObject removed from collection but still referenced (non-disposeable object)! "
                                           "This results in sound objects not being held in Fmod sound manager collection any longer but still reside in memory. "
                                           "Check if VFMOD_FLAG_NODISPOSE is set on sound creation for this purpose.");

    pInst->m_pOwner = NULL;
    RemoveAt(i);
  }
}


void VFmodSoundObjectCollection::StopAll(bool bReasonIsUnloadWorld)
{
  int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    VFmodSoundObject* pObj = GetAt(i);
    
    if (bReasonIsUnloadWorld && pObj->HasFlag(VFMOD_FLAG_SURVIVE_UNLOAD_WORLD))
      continue;

    pObj->Stop();
  }

  PurgeNotPlaying();
}

VFmodSoundObject* VFmodSoundObjectCollection::SearchObject(const char* szName) const
{
  int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    VFmodSoundObject* pObject = GetAt(i);
    if(pObject->HasObjectKey(szName))
      return pObject;
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
