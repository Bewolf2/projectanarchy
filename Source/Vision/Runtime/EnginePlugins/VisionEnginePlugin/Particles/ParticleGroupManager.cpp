/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


// global instance of one particle group
VisParticleGroupManager_cl g_ParticleGroupManager("Particles");

VString VisParticleGroupManager_cl::g_sLastError;
VisCallback_cl VisParticleGroupManager_cl::OnLayerEvent;
float VisParticleGroupManager_cl::g_fGlobalFadeScaling = 1.0f;

VisParticleGroupManager_cl::VisParticleGroupManager_cl(const char *szManagerName)
: VisResourceManager_cl(szManagerName,VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER)
{
  m_fGlobalTimeScaling = 1.f;
  m_fLastToDUpdate = -1.f;
}

VisParticleGroupManager_cl::~VisParticleGroupManager_cl()
{
}

VisParticleGroupManager_cl& VisParticleGroupManager_cl::GlobalManager()
{
  return g_ParticleGroupManager;
}


void VisParticleGroupManager_cl::OneTimeInit()
{
  Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(100,200,255));

  Vision::Callbacks.OnWorldDeInit += this; // register callback
  Vision::Callbacks.OnUpdateSceneFinished += this; // register callback
  Vision::Callbacks.OnReassignShaders += this; // register callback
  Vision::Callbacks.OnAfterSceneUnloaded += this; // register callback
}

void VisParticleGroupManager_cl::OneTimeDeInit()
{
  Vision::ResourceSystem.UnregisterResourceManager(this);
  VisParticleConstraint_cl::GlobalConstraintList().ReleaseAllConstraints();
  Vision::Callbacks.OnWorldDeInit -= this; // deregister callback
  Vision::Callbacks.OnUpdateSceneFinished -= this; // deregister callback
  Vision::Callbacks.OnReassignShaders -= this; // deregister callback
  Vision::Callbacks.OnAfterSceneUnloaded -= this;  // deregister callback
  m_GlobalConstraints.ReleaseAllConstraints();
}

#ifdef WIN32
  bool VisParticleGroupManager_cl::g_bLoopAllEffects = false;
  void VisParticleGroupManager_cl::SetLoopAllEffects(bool bStatus)
  {
    g_bLoopAllEffects = bStatus;
  }
#endif

void VisParticleGroupManager_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      Instances().Tick(Vision::GetTimer()->GetTimeDifference() * m_fGlobalTimeScaling); // this handles all particle group instances in this collection 
    else
    {
      IVTimeOfDay *pToD = Vision::Renderer.GetTimeOfDayHandler();
      if (pToD!=NULL && pToD->GetDayTime()!=m_fLastToDUpdate)
      {
        Instances().EvaluateSceneBrightness();
        m_fLastToDUpdate = pToD->GetDayTime();
      }
      
    }
    return;
  }

  // clean up when a scene is unloaded
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    Instances().Purge();
    VisParticleConstraint_cl::GlobalConstraintList().ReleaseAllConstraints();
    PurgeUnusedResources();
    m_fLastToDUpdate = -1.f;
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneUnloaded)
  {
    Instances().Purge();
    VisParticleConstraint_cl::GlobalConstraintList().ReleaseAllConstraints();
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnReassignShaders)
  {
    Instances().ReassignShader(true);
    return;
  }
}


VisParticleEffectFile_cl* VisParticleGroupManager_cl::LoadFromFile(const char *szFilename, bool bForceUnique)
{
  VisParticleEffectFile_cl *pFile;
  
  g_sLastError.Reset();
  if (!bForceUnique)
  {
    // already in resource list? Then return that one
    pFile = (VisParticleEffectFile_cl *)VResourceManager::GetResourceByName(szFilename);
    if(pFile)
    {
      if(!pFile->IsLoaded())
        pFile->EnsureLoaded();
      return pFile;
    }   
  }

  // create a new one and try to load it from file
  pFile = new VisParticleEffectFile_cl(this);
  if (!pFile->LoadFromFile(szFilename))
  {
    pFile->Purge();
    return NULL;
  }
  return pFile;
}


VisParticleGroupDescriptor_cl *VisParticleGroupManager_cl::FindDescriptor(const char *szName) const
{
  const int iCount = GetResourceCount();
  for (int i=0;i<iCount;i++)
  {
    VisParticleEffectFile_cl *pFX = (VisParticleEffectFile_cl *)VisResourceManager_cl::GetResourceByIndex(i);
    if (!pFX) continue;
    VisParticleGroupDescriptor_cl *pDesc = pFX->FindDescriptor(szName);
    if (pDesc)
      return pDesc;
  }
  return NULL;
}

VisParticleGroupDescriptor_cl *VisParticleGroupManager_cl::DoArchiveExchange(VArchive &ar, VisParticleGroupDescriptor_cl *pSource)
{
  if (ar.IsLoading())
  {
    VString fxname,descname;
    ar >> fxname;
    ar >> descname;
    VisParticleEffectFile_cl* pFX = LoadFromFile(fxname);
    if (!pFX)
      return NULL;
    return pFX->FindDescriptor(descname);
  } else
  {
    // write out effect filename
    VASSERT(pSource && pSource->m_pOwner);
    ar << pSource->m_pOwner->GetFilename();

    // name of descriptor
    ar << pSource->m_sName;
  }
  return pSource;
}

VManagedResource *VisParticleGroupManager_cl::CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
{
  return LoadFromFile(szFilename);
}

const char *VisParticleGroupManager_cl::GetStreamingReplacementFilename(VResourceSnapshotEntry &resourceDesc, const char *szResolvedFilename, char *szBuffer)
{
  VFileHelper::AddExtension(szBuffer,szResolvedFilename,"vpfx");
  return szBuffer;
}


void VisParticleGroupManager_cl::SetGlobalFadeDistanceScaling(float fScale)
{
  if (VisParticleGroupManager_cl::g_fGlobalFadeScaling==fScale)
    return;
  VisParticleGroupManager_cl::g_fGlobalFadeScaling = fScale;
  for (int i=0;i<Instances().Count();i++)
  {
    VisParticleEffect_cl *pFX = Instances().GetAt(i);
    const int iGroups = pFX->GetParticleGroupCount();
    for (int j=0;j<iGroups;j++)
    {
      ParticleGroupBase_cl *pLayer = pFX->GetParticleGroup(j);
      if (pLayer==NULL)
        continue;
      pLayer->FadeDistancesFromDesc();
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
// Particle effect file functions
/////////////////////////////////////////////////////////////////////////////


VisParticleEffectFile_cl::VisParticleEffectFile_cl(VisParticleGroupManager_cl *pManager)
: VManagedResource(pManager)
{
//  SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
}


// for serialization
VisParticleEffectFile_cl::VisParticleEffectFile_cl() : VManagedResource(&VisParticleGroupManager_cl::GlobalManager())
{
//  SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
}


VisParticleEffectFile_cl::~VisParticleEffectFile_cl()
{
}


bool VisParticleEffectFile_cl::LoadFromFile(const char *szFilename)
{
  SetFilename(szFilename);
  EnsureLoaded();
  return IsLoaded()==TRUE;
}



bool VisParticleEffectFile_cl::LoadFromBinaryFile(IVFileInStream *pIn, bool bCloseFile)
{
  if (!pIn)
    return false;

  m_Descriptors.Reset();

  VArchive ar(NULL,pIn,Vision::GetTypeManager());
    //char iLocalVersion = 0;
    int iArchiveVersion;
    ar >> iArchiveVersion;
    ar.SetLoadingVersion(iArchiveVersion);
    Serialize(ar);
  ar.Close();
  if (bCloseFile) pIn->Close();
  FinalizeLoading();
  return true;
}


bool VisParticleEffectFile_cl::SaveToBinaryFile(IVFileOutStream *pOut, bool bCloseFile)
{
  if (!pOut)
    return false;
  VArchive ar(NULL,pOut,Vision::GetTypeManager());
//    char iLocalVersion = 0;
    ar << Vision::GetArchiveVersion();
    Serialize(ar);
  ar.Close();
  if (bCloseFile) pOut->Close();
  return true;
}



void VisParticleEffectFile_cl::FinalizeLoading()
{
  // postprocess all descriptors : fixup the descriptor names with pointers
  for (int i=0;i<m_Descriptors.Count();i++)
  {
    VisParticleGroupDescriptor_cl *pDesc = m_Descriptors.GetAt(i);
    pDesc->m_pOwner = this; // fixup owner (important)
    if (pDesc->m_sDestroyCreateGroup.IsEmpty())
      continue;
    pDesc->m_spDestroyCreateDesc = m_Descriptors.FindDescriptor(pDesc->m_sDestroyCreateGroup);
    VASSERT(pDesc->m_spDestroyCreateDesc);
    pDesc->m_spDestroyCreateDesc->m_bActive = false; // mark it as inactive
  }
}



BOOL VisParticleEffectFile_cl::Reload()
{
  const char *szFilename = GetFilename();
  if (!szFilename || !szFilename[0]) // maybe the effect resource does not come from file
    return TRUE;

  VisParticleGroupDescriptor_cl::SetCurrentRelPath(szFilename);

  // first, try via binary file
  if (!Vision::Editor.IsInEditor())
  {
    char szBinFile[FS_MAX_PATH];
    VFileHelper::AddExtension(szBinFile,szFilename,"vpfx");
    IVFileInStream *pIn = GetParentManager()->CreateFileInStream(szBinFile,this);
    if (LoadFromBinaryFile(pIn)) // file is closed
      return TRUE;
  }

  TiXmlDocument doc;
  IVFileInStream *pIn = GetParentManager()->CreateFileInStream(szFilename,this);
  if (pIn==NULL || !doc.LoadFile(pIn))
  {
    if (pIn) pIn->Close();
    VisParticleGroupManager_cl::g_sLastError = doc.ErrorDesc();
    return FALSE;
  }
  pIn->Close();

  VASSERT(doc.RootElement());
  TiXmlElement* pElement;


  // load particle group descriptors
  m_Descriptors.Clear();

  const char *szLayerNodeName = XML_GROUPDESCRIPTOR_NAME;
  TiXmlNode *pFirstNode = doc.RootElement()->FirstChild(szLayerNodeName);
  if (!pFirstNode) // try with old name
  {
    szLayerNodeName = XML_OLD_GROUPDESCRIPTOR_NAME;
    pFirstNode = doc.RootElement()->FirstChild(szLayerNodeName);
  }
  if (pFirstNode)
  {
    // iterate through all XML_GROUPDESCRIPTOR_NAME nodes 
    for (pElement=pFirstNode->ToElement(); pElement!=NULL; pElement=pElement->NextSiblingElement(szLayerNodeName))
    {
      VisParticleGroupDescriptor_cl *pNewDesc = new VisParticleGroupDescriptor_cl(this);
      V_VERIFY_MALLOC(pNewDesc);
      pNewDesc->DataExchangeXML(pElement,false);
      m_Descriptors.Add(pNewDesc);
    }
  }

  // load constraints
  m_Constraints.ReleaseAllConstraints();

  pFirstNode = doc.RootElement()->FirstChild(XML_CONSTRAINT_NAME);
  if (pFirstNode)
  {
    // iterate through all XML_CONSTRAINT_NAME nodes 
    for (pElement=pFirstNode->ToElement(); pElement!=NULL; pElement=pElement->NextSiblingElement(XML_CONSTRAINT_NAME))
    {
      VisParticleConstraint_cl *pConstraint = VisParticleConstraint_cl::CreateConstraint(pElement);
      if (!pConstraint) continue;
      m_Constraints.AddConstraint(pConstraint);
    }
  }

  FlagAsLoaded();// important here, since the next loop calls EnsureLoaded
  FinalizeLoading();
  return TRUE;
}

BOOL VisParticleEffectFile_cl::Unload()
{
  m_Constraints.ReleaseAllConstraints();
  m_Descriptors.Clear();
  m_Emitter.Clear();
  return TRUE;
}


void VisParticleEffectFile_cl::AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
{
  VManagedResource::AccumulateMemoryFootprint(iUniqueSys,iUniqueGPU,iDependentSys,iDependentGPU);
  const int iCount = m_Descriptors.Count();
  for (int i=0;i<iCount;i++) 
    if (m_Descriptors.GetAt(i))
      iUniqueSys += m_Descriptors.GetAt(i)->GetMemSize();

  // textures...
}




#ifdef SUPPORTS_SNAPSHOT_CREATION
void VisParticleEffectFile_cl::GetDependencies(VResourceSnapshot &snapshot)
{
  VManagedResource::GetDependencies(snapshot);
  // all relevant textures are covered by GetDependencies of the instances, but it better to have them in here as well:
  for (int i=0;i<m_Descriptors.Count();i++)
    m_Descriptors.GetAt(i)->GetDependencies(snapshot, this);
}
#endif


bool VisParticleEffectFile_cl::SaveToXML(const char *szFilename)
{
  if (szFilename != NULL && szFilename[0])
    SetFilename(szFilename);

  TiXmlDocument doc;
  TiXmlElement root("root");
  TiXmlElement *pRoot = doc.InsertEndChild(root)->ToElement();
  VASSERT(pRoot);
  int i;

  // write out emitters(?)
  for (i=0;i<m_Emitter.Count();i++)
  {
    VisParticleEmitter_cl *pEmitter = m_Emitter.GetAt(i);
    if (!pEmitter) continue;
    pEmitter->DataExchangeXML(XMLHelper::SubNode(pRoot,"emitter",true),true);
  }

  // write out descriptors
  for (i=0;i<m_Descriptors.Count();i++)
  {
    VisParticleGroupDescriptor_cl *pDesc = m_Descriptors.GetAt(i);
    if (!pDesc) continue;
    pDesc->DataExchangeXML(XMLHelper::SubNode(pRoot,XML_GROUPDESCRIPTOR_NAME,true),true);
  }

  // in case of an absolute path, we have to tweak the filename after saving
  int tweakFilenamePos = 0;
  IVFileStreamManager* fileStreamManager = Vision::File.GetManager();
  if (szFilename != NULL && strncmp(&szFilename[1], ":\\", 2)==0)
  {
    for (int j=0; j<fileStreamManager->GetNumDataDirectories(); ++j)
    {
      const char * szDirectory = fileStreamManager->GetDataDirectory(j);
      VASSERT(szDirectory != NULL);
      int szDirectoryLength = (int)strlen(szDirectory);
      if (strncmp(szFilename, szDirectory, strlen(szDirectory)) == 0)
      {
        tweakFilenamePos = szDirectoryLength;
        break; 
      }
    }
  }

  //TODO: RCS	or remove readonly	
  // save the XML
  if (!doc.SaveFile(GetFilename(),fileStreamManager))
  {  
    if (tweakFilenamePos > 0)
    {
      SetFilename(&szFilename[tweakFilenamePos]);
    }
    VisParticleGroupManager_cl::g_sLastError = doc.ErrorDesc();
    return false;
  }

  // also save to binary file along with it:
  char szBinFile[FS_MAX_PATH];
  VFileHelper::AddExtension(szBinFile,GetFilename(),"vpfx");
  IVFileOutStream *pOut = fileStreamManager->Create(szBinFile);
  SaveToBinaryFile(pOut);

  if (tweakFilenamePos > 0)
  {
    SetFilename(&szFilename[tweakFilenamePos]);
  }
  return true;
}




V_IMPLEMENT_SERIAL( VisParticleEffectFile_cl, VisTypedEngineObject_cl, 0, &g_VisionEngineModule );
void VisParticleEffectFile_cl::Serialize( VArchive &ar )
{
  char iVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iVersion; VASSERT(iVersion==0);
    m_Descriptors.SerializeX(ar,this);
    ar >> m_Constraints;
    ar >> m_Emitter;
    FinalizeLoading();
  } else
  {
    ar << iVersion;
    m_Descriptors.SerializeX(ar,this);
    ar << m_Constraints;
    ar << m_Emitter;
  }
}


bool VisParticleEffectFile_cl::GetBoundingBox(hkvAlignedBBox &destBox) const
{
  destBox.setInvalid();
  hkvAlignedBBox tempBox;
  for (int i=0;i<m_Descriptors.Count();i++)
  {
    VisParticleGroupDescriptor_cl *pDesc = m_Descriptors.GetAt(i);
    if (!pDesc || !pDesc->m_bActive) continue;
    if (!pDesc->GetBoundingBox(tempBox)) continue;
    destBox.expandToInclude(tempBox);
  }
  return destBox.isValid();
}


int VisParticleEffectFile_cl::CountGroupInstances(VisParticleEffect_cl *pInstance, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, bool bSpawnParticles)
{
  EnsureLoaded();
  int iCount = 0;
  for (int i=0;i<m_Descriptors.Count();i++)
  {
    VisParticleGroupDescriptor_cl *pDesc = m_Descriptors.GetAt(i);
    if (!pDesc || !pDesc->m_bActive) continue;
    // do some other checks, whether descriptor should actually create an instance?
    if (pInstance)
    {
      ParticleGroupBase_cl *pLayer = new ParticleGroupBase_cl(pDesc,NULL, vPos, vOri, fScaling, /*bSpawnParticles*/false, pInstance->GetRandomBaseSeed());
      pInstance->m_spGroups[iCount] = pLayer;
      pLayer->m_pParentEffect = pInstance;
      pLayer->m_iChildIndex = i;
      pLayer->AttachToParent(pInstance);
      pLayer->Finalize();

      if (bSpawnParticles) // do this after AttachToParent to have correct world space position
        pLayer->RespawnAllParticles(false);

    }
    iCount++;
  }
  return iCount;
}


void VisParticleEffectFile_cl::InitParticleEffectInstance(VisParticleEffect_cl *pInstance)
{
  InitParticleEffectInstance(pInstance,pInstance->GetPosition(),pInstance->GetOrientation(),1.0f,false); // since it has no positions and orientation, it does not spawn particles
}


void VisParticleEffectFile_cl::InitParticleEffectInstance(VisParticleEffect_cl *pInstance, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, bool bSpawnParticles)
{
  VASSERT(pInstance);
  EnsureLoaded();
  V_SAFE_DELETE_ARRAY(pInstance->m_spGroups);
  pInstance->m_iGroupCount = CountGroupInstances(NULL, vPos, vOri, fScaling, false);
  pInstance->m_spSourceFXFile = this;

  if (pInstance->m_iGroupCount>0)
  {
    // allocate the particle group pointers and fill them out
    pInstance->m_spGroups = new ParticleGroupBasePtr[pInstance->m_iGroupCount];
    CountGroupInstances(pInstance, vPos, vOri, fScaling, bSpawnParticles);
  }

  // add this particle effect to the collection of instances (handled by the particle manager)
  GetParentManager()->Instances().AddUnique(pInstance);

  // initialize the combined mask for all layers
  pInstance->SetVisibleBitmask(0xffffffff);
}

VisParticleEffect_cl* VisParticleEffectFile_cl::CreateParticleEffectInstance(const hkvVec3& vPos, const hkvVec3& vOri, unsigned int uiRandomSeed)
{
  return CreateParticleEffectInstance(vPos, vOri, 1.f, uiRandomSeed);
}

VisParticleEffect_cl* VisParticleEffectFile_cl::CreateParticleEffectInstance(const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, unsigned int uiRandomSeed)
{
  return new VisParticleEffect_cl(this, vPos, vOri, fScaling, uiRandomSeed);
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
