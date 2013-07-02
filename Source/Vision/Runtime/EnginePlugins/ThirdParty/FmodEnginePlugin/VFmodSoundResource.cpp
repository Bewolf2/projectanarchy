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


VManagedResource* VFmodSoundResourceManager::CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
{
  int iUsageFlags = VFMOD_RESOURCEFLAG_DEFAULT;
  if (pExtraInfo)
    iUsageFlags = pExtraInfo->GetCustomIntValue(0, iUsageFlags); 
  VManagedResource *pRes = VFmodManager::GlobalManager().LoadSoundResource(szFilename, iUsageFlags);
  if (!pRes)
    return NULL;
  pRes->EnsureLoaded();
  return pRes;
}


// -------------------------------------------------------------------------- //
// Constructor/ Destructor                                                 
// -------------------------------------------------------------------------- //
VFmodSoundResource::VFmodSoundResource(VFmodSoundResourceManager *pManager,int iUsageFlags) : VManagedResource(pManager)
{
  m_iSysMem = 0;
  m_iSoundFlags = iUsageFlags;
  m_pSound = NULL;

  if (IsStreaming()) // since unique
    SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
}

VFmodSoundResource::~VFmodSoundResource()
{
}


// -------------------------------------------------------------------------- //
// Instances                                                 
// -------------------------------------------------------------------------- //
VFmodSoundObject *VFmodSoundResource::CreateInstance(const hkvVec3 &vPos, int iFlags, int iPriority)
{
  VFmodSoundObject *pInst = new VFmodSoundObject(&VFmodManager::GlobalManager().m_soundInstances, this, vPos, iFlags, iPriority);
  return pInst;
}


// -------------------------------------------------------------------------- //
// Resource property functions                                               
// -------------------------------------------------------------------------- //
bool VFmodSoundResource::IsReady() const
{
  if (!VFmodManager::GlobalManager().IsInitialized())
    return true;

  VASSERT(m_pSound!=NULL);

  FMOD_OPENSTATE eOpenstate; 
  unsigned int iPercentBuffered; 
  bool bStarving;
  bool bDiskBusy; 
  FMOD_RESULT result = m_pSound->getOpenState( &eOpenstate, &iPercentBuffered, &bStarving, &bDiskBusy);

  if( (result != FMOD_OK) || ((eOpenstate != FMOD_OPENSTATE_READY) && (eOpenstate != FMOD_OPENSTATE_PLAYING)))
    return false;

  return true;
}


// -------------------------------------------------------------------------- //
// Overridden functions                                                 
// -------------------------------------------------------------------------- //
BOOL VFmodSoundResource::Reload()
{
  VFmodManager &manager = VFmodManager::GlobalManager();
  if (!manager.IsInitialized())
    return TRUE;

  VASSERT(!m_pSound);

  int iFMODFlags = FMOD_SOFTWARE; // software must be used when geometry is used
  iFMODFlags |= Is3D() ? FMOD_3D:FMOD_2D;
  iFMODFlags |= IsStreaming() ? FMOD_CREATESTREAM : FMOD_CREATESAMPLE;
  if (!LogAttenuation())
    iFMODFlags |= FMOD_3D_LINEARROLLOFF;
  if(IsCompressedData())
    iFMODFlags |= FMOD_CREATECOMPRESSEDSAMPLE;
  iFMODFlags |= LoadsAsyncronous() ? FMOD_NONBLOCKING : FMOD_DEFAULT;

  FMOD_WARNINGCHECK(manager.m_pSystem->createSound(GetFilename(), iFMODFlags, NULL, &m_pSound));
  if (!m_pSound)
    return FALSE;

  // get the size in memory
  unsigned int iSize = 0;
  FMOD_WARNINGCHECK(m_pSound->getMemoryInfo(FMOD_MEMBITS_ALL, 0, &iSize, NULL));

  SetNewMemSize(VRESOURCEMEMORY_SYSTEM,iSize);  
  m_iSysMem = (int)iSize;

  return TRUE;
}

BOOL VFmodSoundResource::Unload()
{
  if (!VFmodManager::GlobalManager().IsInitialized())
    return TRUE;

  VASSERT(m_pSound!=NULL);

  m_pSound->release();
  m_pSound = NULL;

  m_iSysMem = 0;
  SetNewMemSize(VRESOURCEMEMORY_ALLTYPES,m_iSysMem);

  return TRUE;
}

int VFmodSoundResource::GetAdditionalOutputString(char *szDestBuffer, int iMaxChars)
{
  szDestBuffer[0] = 0;
  char *szStart = szDestBuffer;

  if (Is2D()) 
    szDestBuffer += sprintf(szDestBuffer,"2D;"); 
  else 
    szDestBuffer += sprintf(szDestBuffer,"3D;");
  if (LinearAttenuation()) 
    szDestBuffer += sprintf(szDestBuffer,"linear;"); 
  else 
    szDestBuffer += sprintf(szDestBuffer,"log;");
  if (IsStreaming()) 
    szDestBuffer += sprintf(szDestBuffer,"streaming;");

  return int(szDestBuffer-szStart);
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

void VFmodSoundResource::GetDependencies(VResourceSnapshot &snapshot)
{
  if (!IsStreaming())
  {
    VManagedResource::GetDependencies(snapshot);
    IVFileInStream *pFile = Vision::File.Open(GetFilename());
    if (pFile)
    {
      // patch the file size afterwards
      VResourceSnapshotEntry *pEntry = snapshot.FindResourceEntry(this);
      VASSERT(pEntry!=NULL)
        if (pEntry)
        {
          pEntry->m_iFileSize = pFile->GetSize();
          pEntry->SetCustomIntValue(0,m_iSoundFlags);
        }
        pFile->Close();
    }
  }
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
