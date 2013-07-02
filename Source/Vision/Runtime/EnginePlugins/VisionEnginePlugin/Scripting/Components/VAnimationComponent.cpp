/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Components/VAnimationComponent.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

VAnimationComponent::VAnimationComponent(int iComponentFlags /* = VIS_OBJECTCOMPONENTFLAG_NONE */) :
  IVObjectComponent(0, iComponentFlags),
  m_pAnimCtrl(NULL),
  m_sCurrentAnim()
{
}

VAnimationComponent::~VAnimationComponent()
{
}

bool VAnimationComponent::Play(const char * szAnimationName, bool bLoop /* = true */, const char * szEndEventName /* = NULL */, bool bEndEventOnce /* = true */)
{
  if(m_pAnimCtrl) m_pAnimCtrl->RemoveEventListener(m_pOwner);

  //we know, that the owner is at least a VisBaseEntity_cl,
  //since the component is only attachable to VisBaseEntity_cl or inherited classes
  m_pAnimCtrl = VisAnimConfig_cl::StartSkeletalAnimation((VisBaseEntity_cl *)m_pOwner, szAnimationName, bLoop ? VANIMCTRL_LOOP : VSKELANIMCTRL_DEFAULTS);

  //fall back for vertex animations
  if(m_pAnimCtrl==NULL)
  {
    m_pAnimCtrl = VisAnimConfig_cl::StartVertexAnimation((VisBaseEntity_cl *)m_pOwner, szAnimationName, bLoop ? VANIMCTRL_LOOP : VVERTANIMCTRL_DEFAULTS);

    if(!m_pAnimCtrl) return false;
  }

  //register event if specified
  if(szEndEventName)
    AddEndEvent(szEndEventName, bEndEventOnce);

  m_pAnimCtrl->AddEventListener(m_pOwner);

  m_sCurrentAnim = szAnimationName;
  return true;
}


bool VAnimationComponent::Resume(const char * szAnimationName /* = NULL */, bool bLoop /* = true */, const char * szEndEventName /* = NULL*/, bool bEndEventOnce /* = true*/)
{
  //try to resume
  if(m_pAnimCtrl && (szAnimationName==NULL || (m_sCurrentAnim == szAnimationName)))
  {
    int iFlags = m_pAnimCtrl->GetFlags();
    bool bLoopActive = (iFlags&VANIMCTRL_LOOP)!=0;

    if(bLoop)
    {
      if(!bLoopActive)m_pAnimCtrl->SetFlags(iFlags|VANIMCTRL_LOOP);
    }
    else
    {
      if(bLoopActive) m_pAnimCtrl->SetFlags(iFlags&~VANIMCTRL_LOOP);
    }
    
    //resume if the animation is not running...
    if(!m_pAnimCtrl->IsPlaying())
    {
      m_pAnimCtrl->Resume();
    }

    //register event if specified
    if(szEndEventName)
      AddEndEvent(szEndEventName, bEndEventOnce);

    return true;
  }
  
  //just in case the user calls resume for a different animation, we rest using VAnimationComponent::Play
  return Play(szAnimationName, bLoop, szEndEventName, bEndEventOnce);
}

bool VAnimationComponent::Stop()
{
  if(m_pAnimCtrl)
  {
    m_pAnimCtrl->SetCurrentSequenceTime(0);
    m_pAnimCtrl->Pause();
    return true;
  }

  return false;
}

bool VAnimationComponent::Pause()
{
  if(m_pAnimCtrl)
  {
    m_pAnimCtrl->Pause();
    return true;
  }

  return false;
}

bool VAnimationComponent::AddAnimationSequence(const char * szAnimSequence)
{
  if(szAnimSequence==NULL) return false;

  VDynamicMesh * pMesh = ((VisBaseEntity_cl *)m_pOwner)->GetMesh();
  if(pMesh==NULL)
  {
    Vision::Error.Warning("AddAnimationSequence: No mesh present!");
    return false;
  }

  VASSERT_MSG(pMesh->GetSequenceSetCollection()!=NULL, "No animation sequence set present!");

  VisAnimSequenceSet_cl *pSet = Vision::Animations.GetSequenceSetManager()->LoadAnimSequenceSet(szAnimSequence);

  if(pSet==NULL)
  {
    Vision::Error.Warning("AddAnimationSequence: Could not load '%s' animation sequence.", szAnimSequence);
    return false;
  }

  pMesh->GetSequenceSetCollection()->Add(pSet);

  return true;
}

bool VAnimationComponent::SetTime(float fTime, bool bRelativeTime /*= false*/)
{
  if(m_pAnimCtrl)
  {
    if(bRelativeTime)
      m_pAnimCtrl->SetCurrentSequenceTime(m_pAnimCtrl->GetCurrentSequenceTime()*fTime);
    else
      m_pAnimCtrl->SetCurrentSequenceTime(fTime);

    return true;
  }

  return false;
}

void VAnimationComponent::AddEndEvent(const char * szEventName, bool bAutoRemove)
{
  if(m_pAnimCtrl)
  {
    //get the current event list
    VisAnimEventList_cl* pControlEventList = m_pAnimCtrl->GetEventList();
    if(!pControlEventList) return;

    //get or create anim event
    int iEventId = Vision::Animations.LookUpEvent(szEventName);

    if(!iEventId)
      iEventId = Vision::Animations.RegisterEvent(szEventName);

    float fLength = pControlEventList->GetSequenceLength();
    pControlEventList->RemoveEvent(fLength, iEventId); //just to be sure...
    pControlEventList->AddEvent(fLength, iEventId, bAutoRemove);
  }
}

/// =========================================================================== ///
/// IVObjectComponent Overrides                                                 ///
/// =========================================================================== ///

BOOL VAnimationComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return false;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }

  return IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut);
}

//TODO: serialize added events!
void VAnimationComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion == 0, "Invalid local version. Please re-export");

    char pBuffer[512];
    ar.ReadStringBinary(pBuffer, 512);

    if(pBuffer && pBuffer[0])
    {
      unsigned int flags = 0;
      float time = 0;
      bool isPlaying = true;

      ar >> flags;
      ar >> time;
      ar >> isPlaying;

      Play(pBuffer, (flags&VANIMCTRL_LOOP)!=0);
      if(!isPlaying) Pause();
      SetTime(time);
    }
    else
    {
      Stop();
    }
  } 
  else
  {
    ar << iLocalVersion;

    if(m_pAnimCtrl)
    {
      ar.WriteStringBinary(m_pAnimCtrl->GetAnimSequence()->GetName());
      ar << m_pAnimCtrl->GetFlags();
      ar << m_pAnimCtrl->GetCurrentSequenceTime();
      ar << m_pAnimCtrl->IsPlaying();
    }
    else
    {
      ar << 0;
    }
  }
}


V_IMPLEMENT_SERIAL(VAnimationComponent, IVObjectComponent, 0, Vision::GetEngineModule() );

START_VAR_TABLE(VAnimationComponent, IVObjectComponent, "LUA Animation component. Gives convenient access to the animation interface.", VFORGE_HIDECLASS, "LUA Animation Component" )
END_VAR_TABLE

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
