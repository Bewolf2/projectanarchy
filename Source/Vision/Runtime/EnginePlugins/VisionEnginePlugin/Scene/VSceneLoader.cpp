/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/Engine/SceneManagement/VisApiZone.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>
#include <Vision/Runtime/Base/System/VExceptions.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiSky.hpp>
#include <Vision/Runtime/Engine/Renderer/Lighting/VisApiLightGridManager.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisionMobileShaderProvider.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityLODHysteresis.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiSimpleRendererNode.hpp>

#define LOADINGPROGRESS   Vision::GetApplication()->GetLoadingProgress()

namespace 
{
  inline void DetermineFinalSceneFileName( char* pszFileNameFinal, const char* pszGivenFileName )
  {
    const char* pProfileName = AssetProfile::GetProfileName();
    char szFileNameNoExt[ FS_MAX_PATH ];
    VPathHelper::GetFilenameNoExt( szFileNameNoExt, pszGivenFileName );
    sprintf( pszFileNameFinal, "%s.%s.vscene", szFileNameNoExt, pProfileName );

    // Backwards compatibility: If we don't have a scene file containing our asset profile name, use the given file name.
    if ( !Vision::File.Exists( pszFileNameFinal ) )
    {
      Vision::Error.Warning( "Unable to find vscene file '%s' - using fallback to '%s' instead.", pszFileNameFinal, pszGivenFileName );
      strcpy( pszFileNameFinal, pszGivenFileName );
    }
  }
}

class VSceneShapesArchive : public VShapesArchive
{
public:
  VSceneShapesArchive(VSceneLoader &loader, IVFileInStream* pStream, VProgressStatus *pProgess)
    : VShapesArchive(pStream,pProgess), m_Loader(loader)
  {
  }
  
  template<class T>
  bool ReadObjectByType(T*& pObject)
  {
    return ReadObjectWithType(pObject,T::GetClassTypeId());
  }

  template<class T>
  bool ReadObject(T*& pObject)
  {
    return ReadObjectWithType(pObject,NULL);
  }

protected:

  template<class T>
  bool ReadObjectWithType(T*& pObject,const VType* pClassRefRequested)
  {
#ifdef HK_DEBUG
    unsigned int uiObjectStartPos = GetPos();
#endif
    unsigned int uiObjectSize = 0;
#ifdef WIN32
    try
    {
#endif
      pObject = (T*)ReadObject(pClassRefRequested,&uiObjectSize);
#ifdef WIN32
    }
    catch (VArchiveException& ex)
    {
      char szBuffer[2048];
      char szError[1024];
      szError[0] = 0;
#ifdef HK_DEBUG
      if(m_bUseObjectLengths)
      {
       if (ex.GetErrorMessage(szError,sizeof(szError)-1))
          sprintf_s(szBuffer,"Object %s class failed to serialize. Object will be ignored.\n\nDetailed error message:\n%s",ex.m_strClassName.GetSafeStr(),szError);
        else
          sprintf_s(szBuffer,"Object %s class failed to serialize. Object will be ignored.",ex.m_strClassName.GetSafeStr());
        Vision::Error.Warning(szBuffer);
        // dump the remaining bytes for the failed object into nirvana
        char temp = 0;
        VASSERT_MSG(uiObjectStartPos + uiObjectSize >= (size_t)GetPos(),"to many bytes read");
        size_t uiObjectBytesRemaining = (uiObjectStartPos + uiObjectSize) - GetPos();
        for(size_t i=0; i<uiObjectBytesRemaining; i++)
        {
          (*this) >> temp;
        }
        m_Loader.SetError(szBuffer);
        return false;
      }
#endif
      if (ex.GetErrorMessage(szError,sizeof(szError)-1))
        sprintf_s(szBuffer,"Object %s class failed to serialize. Shape serialization aborted.\n\nDetailed error message:\n%s",ex.m_strClassName.GetSafeStr(),szError);
      else
        sprintf_s(szBuffer,"Object %s class failed to serialize. Shape serialization aborted.",ex.m_strClassName.GetSafeStr());
      m_Loader.SetError(szBuffer);
      return false;
    }
#endif
    return true;
  }

  virtual VTypedObject* ReadObject( const VType* pClassRefRequested, unsigned int* objectSize )
  {
    VTypedObject* pObject = VShapesArchive::ReadObject(pClassRefRequested,objectSize);
    m_Loader.OnDeSerializeObject(*this, pObject);
    return pObject;
  }

  VSceneLoader &m_Loader;
};

/////////////////////////////////////////////////////////////////////////////
// VSceneLoader class
/////////////////////////////////////////////////////////////////////////////

VisCallback_cl VSceneLoader::OnCustomChunkSerialization;
VisCallback_cl VSceneLoader::OnCustomChunkBeforeShapesSerialization;
VString VSceneLoader::s_sLastLoadedScene;

VSceneLoader::VSceneLoader() 
  : m_bUsePrewarming(false)
  , m_bInterleavedLoading(false)
  , m_iNextPrewarmIndexStaticGeometry(0)
  , m_iNextPrewarmIndexEntities(0)
#ifdef _VR_GLES2
  , m_iNextPrewarmIndexMeshes(0)
  , m_iPrewarmLightSourceCount(0)
#endif
{ 
  m_bLoadPlugins = m_bAborted = m_bStreaming = false; 
  m_bIsFinished = true; 
  m_fUnitScaling = 1.f;
  m_eExportFlags = VExport_All;

  m_fNearClipPlane = 5.0f;
  m_fFarClipPlane = 32000.0f;
  m_fFovX	= 90.f;
  
#ifdef _VR_GLES2
  memset( m_aLightSources, 0, PREWARM_LIGHT_SOURCE_COUNT * sizeof( VisLightSource_cl * ) );
#endif
}

VSceneLoader::~VSceneLoader() 
{
}

#ifdef WIN32

bool VSceneLoader::EmbedFile(const char *szFilename, IVFileStreamManager* pManager, CHUNKIDTYPE iID)
{
  VASSERT(IsSaving());
  if (!pManager)
    pManager = Vision::File.GetManager();
  IVFileInStream *pIn = pManager->Open(szFilename);
  if (!pIn)
    return false;
  const int iSize = pIn->GetSize();

  StartChunk(iID);
  for (int i=0;i<iSize;i++)
  {
    char c;
    pIn->Read(&c,1);
    Write(&c,1);
  }
  pIn->Close();
  EndChunk();

  return true;
}

#endif


void VSceneLoader::OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs)
{
  VChunkFile_cl::OnError(szError,chunkID,iChunkOfs);
  Vision::Error.Warning("Error loading vscene file: ChunkID:%08x, offs:%i : %s",chunkID,iChunkOfs,szError);

}

void VSceneLoader::OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs)
{
  VChunkFile_cl::OnWarning(szWarning,chunkID,iChunkOfs);
  Vision::Error.Warning("Warning loading vscene file: ChunkID:%08x, offs:%i : %s",chunkID,iChunkOfs,szWarning);
}


///////////////////////////////////////////////////////////////////////////////////////
// LoadScene : opens a scene file and starts processing all chunks
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::LoadScene(const char *szFilename, int iLoadingFlags)
{
  // Incorporate the currently active asset profile name into the file name.
  char szFileNameFinal[ FS_MAX_PATH ];
  DetermineFinalSceneFileName( szFileNameFinal, szFilename );

  m_bAborted = m_bIsFinished = false;
  m_sSceneFilename = szFileNameFinal;
  s_sLastLoadedScene = szFileNameFinal;
  m_bForceMobileMode = (iLoadingFlags & LF_ForceMobileMode) != 0;
  m_bUsePrewarming = (iLoadingFlags & LF_UsePrewarming) != 0;
  m_bInterleavedLoading = (iLoadingFlags & LF_UseInterleavedLoading) != 0;

  if (iLoadingFlags & LF_UseStreamingIfExists)
  {
    // Load the .vres file if available
    VStaticString<FS_MAX_PATH> resourceName(m_sSceneFilename);
    resourceName += "_data\\resources.vres";

    // if .vres file exists use streaming otherwise regular loading
    m_bStreaming = m_vsceneResources.LoadFromBinaryFile(resourceName, Vision::File.GetManager());
    if (m_bStreaming)
    {
      // Success
      Vision::Error.SystemMessage("Resource file found: %s", resourceName.AsChar());
    }
    else
    {
      Vision::Error.Warning("Resource file %s not found, this may affect scene loading performance.", resourceName.AsChar());
    }
  }
  
  { // Trigger callback OnBeforeSceneLoaded
    VisSceneLoadedDataObject_cl data(&Vision::Callbacks.OnBeforeSceneLoaded, szFileNameFinal);
    Vision::Callbacks.OnBeforeSceneLoaded.TriggerCallbacks(&data);
  }

  m_iSceneVersion = -1;
  if (!Open(szFileNameFinal, Vision::File.GetManager()))
    return false;
  
  //check the LOADINGPROGRESS already has a stack. if it exists, skip the start/finish function here
  m_bExternalProgress = LOADINGPROGRESS.GetStackPos() > 0;

  // On Android we need the default granularity of 0.0f so the screen is always updated!
  LOADINGPROGRESS.SetGranularity(0.25f);

  if (!m_bExternalProgress)
    LOADINGPROGRESS.OnStart();

  BOOL bResult = TRUE;
  if (m_bStreaming)
  {
    // if streaming is used, load scene chunks including the _V3D chunk.
    // we need the information (shader provider, lighting equation etc.) in these chunks to 
    // assign the correct shader to meshes while loading them via .vres file
    do 
    {
      if (!ProcessSubChunk()) 
      {
        bResult = FALSE;
        break;
      }
    } 
    while (m_currentChunkID != '_FOG');

    if (bResult)
    {
      // if loading was successful trigger the resource streaming here.
      // after that we return here. The 'IsFinished'-Method must be called periodically to advance streaming
      m_vsceneResources.ScheduleResources(&m_resourceCreator, &Vision::File.GetMemoryStreamManager(),
        Vision::File.GetManager());
      LOADINGPROGRESS.PushRange(20.0f,85.0f);
      return true;
    }
    return false;
  }
  else
  {
    // no streaming, load the scene here using the blocking 'ParseFile'-Method
    bResult = ParseFile();
  }
  
  PrewarmResources();

  if (!m_bExternalProgress)
    LOADINGPROGRESS.OnFinish();

  Close();

  if (!bResult)
    return false;

  FinalizeSceneLoading();
  return !IsInErrorState();
}

bool VSceneLoader::Tick()
{
  // if no streaming is used we are already finished
  if (!m_bStreaming)
    return true;
  
  if (!m_vsceneResources.IsFinished())
  {
    // advance the streaming
    float dtime = Vision::GetUITimer()->GetTimeDifference();
    Vision::ResourceSystem.HandleAllResourceManager(dtime);
    m_vsceneResources.TickFunction(dtime);
    
    LOADINGPROGRESS.SetProgress(m_vsceneResources.GetProgress());

    if (m_vsceneResources.IsFinished())
      LOADINGPROGRESS.PopRange();

    return false;
  }

  // load the remaining chunks of the scene file
  if (!PrewarmingStarted())
  {
    if (m_bInterleavedLoading)
    {
      // process only one chunk at once
      if (!IsEOF() && IsLoading())
      {
        ProcessSubChunk();
        return false;
      }
    }
    else
    {
      // load all the remaining chunks
    ParseFile();
  }
  }
  
  if (!PrewarmResources())
  {
    return false;
  }

  if (!m_bExternalProgress)
    LOADINGPROGRESS.OnFinish();

  Close();
  
  FinalizeSceneLoading();
  m_vsceneResources.Reset();
  return true;
}

bool VSceneLoader::PrewarmResources()
{
  if (!m_bUsePrewarming)
    return true;
  
  if (!PrewarmingStarted())
  {
    LOADINGPROGRESS.PushRange(95.0f, 100.0f);
#ifdef _VR_GLES2
    GeneratePrewarmLights();
#endif
  }
  
  const int iStaticGeometryCount = VisStaticGeometryInstance_cl::ElementManagerGetSize();
  const int iEntityCount = VisBaseEntity_cl::ElementManagerGetSize();
  
  const int iBatchSize = 20;
  int iStaticGeometryEnd = iStaticGeometryCount;
  int iEntityEnd = iEntityCount;
  
#ifdef _VR_GLES2
  const int iMeshCount = VMeshManager::GetMeshManager()->GetResourceCount();
  int iMeshEnd = iMeshCount;
#endif
  
  if (m_bStreaming)
  {
    iStaticGeometryEnd = hkvMath::Min(m_iNextPrewarmIndexStaticGeometry + iBatchSize, iStaticGeometryCount);
    iEntityEnd = hkvMath::Min(m_iNextPrewarmIndexEntities + iBatchSize, iEntityCount);
#ifdef _VR_GLES2
    iMeshEnd = hkvMath::Min(m_iNextPrewarmIndexMeshes + iBatchSize, iMeshCount);
#endif
  }
  
  VisStaticGeometryInstanceCollection_cl staticGeometry(iBatchSize);
  for (int i = m_iNextPrewarmIndexStaticGeometry; i < iStaticGeometryEnd; i++)
  {
    VisStaticGeometryInstance_cl* pInstance = VisStaticGeometryInstance_cl::ElementManagerGetAt(i);
    if (pInstance != NULL)
      staticGeometry.AppendEntry(pInstance);
  }
    
#ifdef _VR_GLES2
  VisEntityCollection_cl generatedEntities(iBatchSize);
  
  if (VVideo::GetVideoConfig()->LazyShaderCompilation)
    GenerateEntitiesFromMeshes(generatedEntities, m_iNextPrewarmIndexMeshes, iMeshEnd);
#endif
    
  Vision::Renderer.BeginRendering();
  Vision::Renderer.BeginRenderLoop();

  PrewarmStaticInstances( staticGeometry );
  
  Vision::RenderLoopHelper.BeginEntityRendering();
  for (int i = m_iNextPrewarmIndexEntities; i < iEntityEnd; i++)
  {
    VisBaseEntity_cl* pEntity = VisBaseEntity_cl::ElementManagerGetAt(i);
    if (pEntity != NULL && pEntity->GetAnimConfig() == NULL)
    {
      PrewarmEntity( pEntity );
    }
    }

#ifdef _VR_GLES2
  int iGeneratedEntityCount = generatedEntities.GetNumEntries();
  for ( int i = 0; i < iGeneratedEntityCount; ++i)
  {
    VisBaseEntity_cl* pEntity = generatedEntities.GetEntry(i);
    PrewarmEntity( pEntity );
  }
#endif

  Vision::RenderLoopHelper.EndEntityRendering();
  
  Vision::Renderer.EndRenderLoop();
  Vision::Renderer.EndRendering();
  
  Vision::Contexts.GetMainRenderContext()->Activate();
  Vision::RenderScreenMasks();
  
  m_iNextPrewarmIndexStaticGeometry = iStaticGeometryEnd;
  m_iNextPrewarmIndexEntities = iEntityEnd;
  
#ifdef _VR_GLES2
  m_iNextPrewarmIndexMeshes = iMeshEnd;
  DestroyEntities( generatedEntities );
#endif
  
  
  float fProgress = hkvMath::Min((float)iStaticGeometryEnd / iStaticGeometryCount, (float)iEntityEnd / iEntityCount);
  LOADINGPROGRESS.SetProgress(fProgress * 100.0f);
  
  if (iStaticGeometryEnd == iStaticGeometryCount 
    && iEntityEnd == iEntityCount
#ifdef _VR_GLES2
    && iMeshEnd == iMeshCount
#endif
	)
  {
    LOADINGPROGRESS.PopRange();
    m_iNextPrewarmIndexStaticGeometry = 0;
    m_iNextPrewarmIndexEntities = 0;
	
#ifdef _VR_GLES2
    m_iNextPrewarmIndexMeshes = 0;
    DestroyPrewarmLights();
#endif

    return true;
  }
  return false;
}

void VSceneLoader::FinalizeSceneLoading()
{
  IVisSceneManager_cl *pSceneManager = Vision::GetSceneManager();

  // add a dummy zone if there is none:
  if (pSceneManager != NULL && pSceneManager->GetNumVisibilityZones()==0)
  {
    float r = 10000000.f;
    hkvAlignedBBox bbox(hkvVec3 (-r,-r,-r), hkvVec3 (r,r,r));
    VisVisibilityZone_cl *pZone = new VisVisibilityZone_cl(bbox);
    pSceneManager->AddVisibilityZone(pZone);
  }

  { // Trigger callback OnAfterSceneLoaded
    VisSceneLoadedDataObject_cl data(&Vision::Callbacks.OnAfterSceneLoaded, m_sSceneFilename);
    Vision::Callbacks.OnAfterSceneLoaded.TriggerCallbacks(&data);
  }

  Vision::TextureManager.GetManager().ResetGlobalScratchBuffer();
  m_bIsFinished = true;

  Vision::Error.ShowReportDialog(TRUE); // report missing model files etc. in the scene
}

#ifdef _VR_GLES2
void VSceneLoader::GeneratePrewarmLights()
{
  if (!VVideo::GetVideoConfig()->LazyShaderCompilation)
  	return;

  VASSERT( m_iPrewarmLightSourceCount == 0 );
  
  m_aLightSources[ m_iPrewarmLightSourceCount ] = Vision::Game.CreateLight( hkvVec3::ZeroVector(), VIS_LIGHT_POINT, 100000.0f );
  ++m_iPrewarmLightSourceCount;

  // use an empty <cough> cubemap as projection texture (and see how far it takes us) - it would be nice 
  // if we could just create / retrieve a nice and white cubemap (similar to what we do with 2D textures)
  m_aLightSources[ m_iPrewarmLightSourceCount ] = Vision::Game.CreateLight( hkvVec3::ZeroVector(), VIS_LIGHT_POINT, 100000.0f );
  VTextureCubeObject* pCubemap = Vision::TextureManager.CreateCubemapTextureObject( "<prewarm_dummy>", 2, 1, VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP, 0 );
  pCubemap->SetResourceFlag( VRESOURCEFLAG_AUTODELETE );
  m_aLightSources[ m_iPrewarmLightSourceCount ]->SetProjectionTexture( pCubemap );
  ++m_iPrewarmLightSourceCount;

  m_aLightSources[ m_iPrewarmLightSourceCount ] = Vision::Game.CreateLight( hkvVec3::ZeroVector(), VIS_LIGHT_SPOTLIGHT, 100000.0f );
  ++m_iPrewarmLightSourceCount;

  m_aLightSources[ m_iPrewarmLightSourceCount ] = Vision::Game.CreateLight( hkvVec3::ZeroVector(), VIS_LIGHT_DIRECTED, 100000.0f );
  ++m_iPrewarmLightSourceCount;
  
  VASSERT( m_iPrewarmLightSourceCount == PREWARM_LIGHT_SOURCE_COUNT );
}

void VSceneLoader::DestroyPrewarmLights()
{
  for ( int i = 0; i < m_iPrewarmLightSourceCount; ++i )
  {
    m_aLightSources[ i ]->DisposeObject();
    m_aLightSources[ i ] = NULL;
  }

  m_iPrewarmLightSourceCount = 0;
}

void VSceneLoader::GenerateEntitiesFromMeshes(VisEntityCollection_cl &entities, int iStartIndex, int iEndIndex)
{
  VMeshManager *pMeshManager = VMeshManager::GetMeshManager();
  VASSERT_MSG( iEndIndex <= pMeshManager->GetResourceCount(), "Invalid end index for mesh resource!" );

  for ( int i = iStartIndex; i < iEndIndex; ++i )
  {
    VBaseMesh *pMesh = (VBaseMesh*) pMeshManager->GetResourceByIndex( i );
    if (!pMesh)
      continue;

    switch ( pMesh->GetMeshType() )
    {
    case VMESH_STATICMESH:
      {
        // skip those for now... 
        // (after all, the majority of static meshes should be sitting in static geometry instances by now)
      }
      break;
    case VMESH_DYNAMICMESH:
      {
        VisBaseEntity_cl *pEntity = Vision::Game.CreateEntity( "VisBaseEntity_cl", hkvVec3::ZeroVector(), pMesh->GetFilename() );
        entities.AppendEntry( pEntity );
      }
      break;
    }
  }

}

void VSceneLoader::DestroyEntities(VisEntityCollection_cl &entities)
{
  // TODO - we might have to wait for the GPU before we can safely dispose of all these objects...
  unsigned int iNumEntities = entities.GetNumEntries();
  for ( unsigned int i = 0; i < iNumEntities; ++i )
  {
    VisBaseEntity_cl *pEntity = entities.GetEntry( i );
    pEntity->DisposeObject();
  }
  entities.Clear();
}
#endif

void VSceneLoader::PrewarmStaticInstances(VisStaticGeometryInstanceCollection_cl& staticInstances)
{
  Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(staticInstances, VPT_PrimaryOpaquePass);

#ifdef _VR_GLES2
  if ( VVideo::GetVideoConfig()->LazyShaderCompilation )
  {
    IVisShaderProvider_cl *pShaderProvider = Vision::GetApplication()->GetShaderProvider();
    int iNumStaticInstances = staticInstances.GetNumEntries();
    VisStaticGeometryInstanceCollection_cl currentInstances( iNumStaticInstances );

    for ( int iLightIdx = 0; iLightIdx < PREWARM_LIGHT_SOURCE_COUNT; ++iLightIdx )
    {
      VASSERT( currentInstances.IsEmpty() );

      VisLightSource_cl *pLight = m_aLightSources[ iLightIdx ];
      VisSurface_cl *pLastSurface = NULL;
      VCompiledTechnique *pLastTechnique = NULL;
      VCompiledTechnique *pTechnique = NULL;

      // Iterate over and collect static instances. Once a relevant property changes, all collected instances
      // get rendered before the collection is reset and we keep iterating (and collecting).
      for ( int iInstanceIdx = 0; iInstanceIdx < iNumStaticInstances; ++iInstanceIdx )
      {
        VisStaticGeometryInstance_cl *pInstance = staticInstances.GetEntry( iInstanceIdx );
        VisSurface_cl *pSurface = pInstance->GetSurface();

        if ( pLastSurface != pSurface )
        {
          pTechnique = pShaderProvider->GetDynamicLightShader( pLight, pSurface, false );
          pLastSurface = pSurface;
        }
        if ( !pTechnique || !pTechnique->GetShaderCount() )
          continue;

        if ( pLastTechnique != pTechnique )
        {
          // as the state information has changed, we have to render everything we have collected so far
          if ( currentInstances.GetNumEntries() > 0 )
          {
            VASSERT( pLastTechnique != NULL && pLastTechnique->GetShaderCount() > 0 );
            Vision::RenderLoopHelper.RenderStaticGeometryWithShader( currentInstances, *pLastTechnique->m_Shaders.GetAt( 0 ) );
            currentInstances.Clear();
          }
          pLastTechnique = pTechnique;
        }
        currentInstances.AppendEntryFast( pInstance );
      }
      if ( currentInstances.GetNumEntries() > 0 )
      {
        VASSERT( pLastTechnique != NULL && pLastTechnique->GetShaderCount() > 0 );
        Vision::RenderLoopHelper.RenderStaticGeometryWithShader( currentInstances, *pLastTechnique->m_Shaders.GetAt( 0 ) );
        currentInstances.Clear();
      }
    }
  }
#endif
}

void VSceneLoader::PrewarmEntity(VisBaseEntity_cl *pEntity)
{
  VASSERT( pEntity );

  VisDrawCallInfo_t SurfaceShaderList[1024];

  VisShaderSet_cl* pShaderSet = pEntity->GetActiveShaderSet();
  if (pShaderSet == NULL)
    return;
  int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(SurfaceShaderList, VPT_PrimaryOpaquePass, V_ARRAY_SIZE(SurfaceShaderList));
  Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, SurfaceShaderList);

#ifdef _VR_GLES2
  if ( VVideo::GetVideoConfig()->LazyShaderCompilation )
  {
    IVisShaderProvider_cl *pShaderProvider = Vision::GetApplication()->GetShaderProvider();

    VDynamicMesh *pMesh = pEntity->GetMesh();
    VisSurface_cl **ppSurfaces = pEntity->GetSurfaceArray();

    iNumSurfaceShaders = 0;
    int iNumSubmeshes = pMesh->GetSubmeshCount();
    for ( int iSubmeshIdx = 0; iSubmeshIdx < iNumSubmeshes; ++iSubmeshIdx )
    {
      VDynamicSubmesh *pSubmesh = pMesh->GetSubmesh( iSubmeshIdx );
      VisSurface_cl *pSurface = ppSurfaces[ pSubmesh->m_iMaterialIndex ];

      for ( int iLightIdx = 0; iLightIdx < PREWARM_LIGHT_SOURCE_COUNT; ++iLightIdx )
      {
        VCompiledTechnique *pTechnique = pShaderProvider->GetDynamicLightShader( m_aLightSources[ iLightIdx ], pSurface, true );
        if ( !pTechnique || !pTechnique->GetShaderCount() )
          continue;

        VisDrawCallInfo_t &drawcall( SurfaceShaderList[ iNumSurfaceShaders++ ] );
        drawcall.Set(pSubmesh, pSurface, pTechnique->m_Shaders.GetAt(0));

        if ( iNumSurfaceShaders == V_ARRAY_SIZE( SurfaceShaderList ) )
        {
          // we have filled up our (static) array so we have to render those guys first and reset it
          Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList( pEntity, iNumSurfaceShaders, SurfaceShaderList );
          iNumSurfaceShaders = 0;
        }
      }
    }
    if ( iNumSurfaceShaders > 0 )
    {
      Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList( pEntity, iNumSurfaceShaders, SurfaceShaderList );
    }
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////////////
// ReadShapeChunk : loads a full chunk that contains a serialisation archive
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::ReadShapeChunk()
{
  IVisApp_cl *pApp = Vision::GetApplication();
  float fEnd = m_bUsePrewarming ? 95.0f : 100.0f;
  if (m_bStreaming)
    LOADINGPROGRESS.PushRange(85.0f, fEnd);
  else
    LOADINGPROGRESS.PushRange(20.f, fEnd);

  IVFileInStream *pIn = GetChunkInStream(); // get embedded chunk stream
  VASSERT(pIn);
  int iVers;

  VSceneShapesArchive ar(*this, pIn, &LOADINGPROGRESS);
  ar >> iVers;
  ar.SetLoadingVersion(iVers);

  //check if we support object sizes
  if(m_iSceneVersion >= SCENE_FILE_VERSION13)
  {
    ar.m_bUseObjectLengths = true;
  }
  
  // the first objects is the scene are reserved and saved in a fixed order. That is..
  if (m_iSceneVersion>=SCENE_FILE_VERSION7)
  {
    int iReserved;
    ar >> ar.m_iObjectCount;
    ar >> ar.m_iNonNullCount;
    ar >> ar.m_iRootObjectCount;
    ar >> iReserved; VASSERT(iReserved==0);
    if (ar.m_iNonNullCount>0)
      ar.m_fProgressStep = 100.f/(float)ar.m_iNonNullCount;

    // scene version 9 : supports accurate per-shape progress info
    if (m_iSceneVersion>=SCENE_FILE_VERSION9)
      ar.m_bHasPerObjectRangeInfo = true;

    // #0 read some count statistics and resize engine arrays to avoid fragmentation
    VStaticIntDictionary<1024> statistics;
    statistics.SerializeX(ar);
      VisBaseEntity_cl::SetExpectedCount(statistics.Get('_ENT',0));
      VisLightSource_cl::ElementManagerEnsureSize(statistics.Get('_LGT',0));
      VisParticleGroup_cl::ElementManagerEnsureSize(statistics.Get('_PTG',0));
      VisVisibilityObject_cl::ElementManagerEnsureSize(statistics.Get('_VOB',0));
      VisStaticMeshInstance_cl::ElementManagerEnsureSize(statistics.Get('_SMI',0));
      VisStaticGeometryInstance_cl::ElementManagerEnsureSize(statistics.Get('_SGI',0));
      VisMeshBufferObject_cl::ElementManagerEnsureSize(statistics.Get('_MBO',0));
      VisPath_cl::ElementManagerEnsureSize(statistics.Get('_PTH',0));
  }

  OnStartShapeDeSerialization(ar); // virtual function (relevant object count should be available)

  if (m_iSceneVersion>=SCENE_FILE_VERSION6)
  {
    // #1 snapshot of all visibility zones
    int iCount;
    ar >> iCount;
    IVisSceneManager_cl *pSM = Vision::GetSceneManager();
    if (pSM)
      pSM->VisibilityZones().EnsureCapacity(iCount);

    for (int i=0;i<iCount;i++)
    {
      VisVisibilityZone_cl *pZone;
      ar.ReadObjectByType(pZone);
      if (pZone) 
        pSM->AddVisibilityZone(pZone);
    }
  }

  IVScriptInstance *pSceneScript = NULL;
  if (m_iSceneVersion>=SCENE_FILE_VERSION5)
  {
    // #2 : script object
    ar.ReadObject(pSceneScript);

    // the OnBeforeSceneLoaded has already been called, but at this time the scene script was not known,
    // therefore we call it here right now, before all the shapes are going to be created. (Hansoft #4654)
    if(pSceneScript!=NULL) pSceneScript->ExecuteFunction("OnBeforeSceneLoaded");

    if (m_iSceneVersion>=SCENE_FILE_VERSION14)
    {
      // #14 : Global script think interval
      float fThinkInterval;
      ar >> fThinkInterval;
      if(Vision::GetScriptManager()!=NULL)
        Vision::GetScriptManager()->SetThinkInterval(fThinkInterval);
    }

    // #3 : sky
    IVSky *pSky;
    ar.ReadObjectByType(pSky);
    if (m_eExportFlags & VExport_Sky) // only set it when exported
      Vision::World.SetActiveSky(pSky);
  }
   
  // remove renderer node so that no renderer node is set during de-serialization
  Vision::Renderer.SetRendererNode(0, NULL);
  Vision::Renderer.SetGlobalAmbientColor(hkvVec4::ZeroVector ());

  IVRendererNode *pRenderer = NULL;
  if (m_iSceneVersion>=SCENE_FILE_VERSION10)
  {
    // #4 : renderer node
    ar.ReadObjectByType(pRenderer);
  }

  if ((m_eExportFlags & VExport_RendererNode) == 0 || pRenderer == NULL)
  {
    pRenderer = new VSimpleRendererNode();
  }

  // set renderer node properties like near and far clip plane and initialize renderer node
  {
    pRenderer->SetFinalTargetContext(Vision::Contexts.GetMainRenderContext());

    pRenderer->GetViewProperties()->setClipPlanes(m_fNearClipPlane, m_fFarClipPlane);
    pRenderer->GetViewProperties()->setFov(m_fFovX, 0.0f);
    pRenderer->OnViewPropertiesChanged();
    
    pRenderer->InitializeRenderer();
      Vision::Renderer.SetRendererNode(0, pRenderer);
  }  

  if (m_iSceneVersion>=SCENE_FILE_VERSION10)
  {
    // #5 : time of day
    IVTimeOfDay *pToD;
    ar.ReadObject(pToD);
    if (m_eExportFlags & VExport_TimeOfDay) // only set it when exported
    {
      Vision::Renderer.SetTimeOfDayHandler(pToD);
    }
    
    // Previously, scenes using the forward renderer stored a single on/off flag for the fog, which controlled three 
    // possible behaviors based on whether or not there was a time of day system present.  
    //
    // The new behavior is tri-state (on, off or driven by time of day).  In order for old scenes to load correctly, 
    // the flag is treated as false=off, true=time of day during deserialization.  The check below is to handle the
    // third case (the flag is on, no time of day is present) and convert it to the current system.
    VFogParameters fog = Vision::World.GetFogParameters();
    if (fog.depthMode == VFogParameters::TimeOfDay)
    {
      fog.depthMode = Vision::Renderer.GetTimeOfDayHandler() != NULL ? VFogParameters::TimeOfDay : VFogParameters::On;
      Vision::World.SetFogParameters(fog);
    }
    
  }

  // #15: Serialize global LOD Hysteresis settings
  if (m_iSceneVersion>=SCENE_FILE_VERSION15)
    VLODHysteresisManager::SerializeX(ar);

  // now read all other shapes
  int iObjCount = 0;
  bool bValid = true;
  bool bWantsAbort = false;
  while (!ar.IsEOF() )
  {
    VTypedObject* pObj = NULL;

    bValid = ar.ReadObject(pObj);

#ifdef HK_DEBUG
    //if the object is not valid and we don't have object lengths => fatal error
    if(bValid == false && !ar.m_bUseObjectLengths)
      break;
    //we have object lengths, so the object was skipped, we can continue
    else if(bValid == false)
    {
      bValid = true;
    }
#else
    if(bValid == false)
      break;
#endif

    // We may only abort when de-serializing the root level of shapes, otherwise we end up with semi-serialized objects.
    if (LOADINGPROGRESS.WantsAbort())
    {
      pSceneScript = NULL;
      bWantsAbort = true;
      break;
    }

    if (!pObj)
      continue; // there might be NULL references in the file

    iObjCount++;
  }

  OnFinishShapeDeSerialization(ar); // virtual function (relevant object count should be available)

  ar.Close();
  pIn->Close();

  if (bWantsAbort)
    OnAbort();
  else
  {
    // set scene script even when it is NULL
    if (Vision::GetScriptManager() && (m_eExportFlags & VExport_SceneScript)) // only set it when exported
      Vision::GetScriptManager()->SetSceneScript(pSceneScript);
  }

  LOADINGPROGRESS.PopRange();
  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////
// ReadSceneChunk : loads scene related data. Always the first chunk
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::ReadSceneChunk()
{
  int iMagic;
  if (!ReadInt(iMagic) || iMagic!=SCENE_MAGIG_NUMBER)
  {
    SetError("Scene chunk is not valid!");
    return false;
  }
  int iVersion = -1;
  if (!ReadInt(iVersion) || iVersion>SCENE_FILE_CURRENT_VERSION || iVersion<SCENE_FILE_VERSION3)
  {
    char buf[1024];
    sprintf(buf,
      "Unsupported Scene version number: %i. Please re-export your scene from vForge for this engine version.",
      iVersion);
    SetError(buf);
    return false;
  }

  m_eExportFlags = VExport_All;
  if (iVersion>=SCENE_FILE_VERSION11)
  {
    int iReserved;
    ReadInt(m_eExportFlags); // read the flags that were used for export
    ReadInt(iReserved);
  }
  
  m_iSceneVersion = iVersion;
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////
// ReadViewChunk : loads a chunk that contains some view information, e.g. the clip planes
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::ReadViewChunk()
{
  // near and far clip plane are set on the renderer node later
  if (!ReadFloat(m_fNearClipPlane)) 
    return false;
  if (!ReadFloat(m_fFarClipPlane)) 
    return false;
  VASSERT(m_fFarClipPlane > m_fNearClipPlane);
  VASSERT(m_fNearClipPlane > 0.f);

  // initial camera position/orientation
  if (m_iSceneVersion>=SCENE_FILE_VERSION2)
  {
    if (!Read(m_vDefaultCamPos.data, 3*sizeof(float), "3f")) 
      return false;

    for (int i = 0; i < 9; ++i)
    {
      float f;
      if (!Read(&f,sizeof(float),"1f")) 
        return false;

      m_DefaultCamRot.m_ElementsCM[(i/3) + (i%3)*3] = f;
    }

    // Moved to _V3D chunk
    if(m_iSceneVersion <= SCENE_FILE_VERSION15)
    {
      if (!ReadFloat(m_fUnitScaling)) 
        return false;

      if (m_fUnitScaling<=0.f) 
        m_fUnitScaling=1.f; // default

      Vision::World.SetGlobalUnitScaling(m_fUnitScaling);
    }
	
    // fov is set on the renderer node later
    if (!ReadFloat(m_fFovX)) 
      return false;	
    if (m_fFovX<=0.f) 
      m_fFovX=90.f; // default
  }
  return true;
}

  
///////////////////////////////////////////////////////////////////////////////////////
// ReadFogChunk : loads a chunk that contains depth fog information
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::ReadFogChunk()
{
  int iVersion = VSCENE_FOG_VERSION_0;
  VFogParameters fog = Vision::World.GetFogParameters();

  ReadInt(iVersion); 
  if (iVersion == VSCENE_FOG_VERSION_0)
  {
    bool bEnabled;
    float fStart,fEnd;
    VColorRef color;

    Readbool(bEnabled);
    ReadFloat(fStart);
    ReadFloat(fEnd);
    ReadColorRef(color);
    if (IsInErrorState())
      return false;

    fog.fDepthStart = fStart;
    fog.fDepthEnd = fEnd;
    fog.iDepthColor = color;
    fog.depthMode = bEnabled ? VFogParameters::Off : VFogParameters::TimeOfDay;
  }
  else 
  {
    int depthMode, heightFogMode;
    ReadInt(depthMode);
    ReadFloat(fog.fDepthStart);
    ReadFloat(fog.fDepthEnd);
    ReadColorRef(fog.iDepthColor);
    ReadInt(heightFogMode);
    ReadFloat(fog.fHeightFogDensity);
    ReadColorRef(fog.iHeightFogColor);
    ReadFloat(fog.fHeightFogHalfDensityHeight);
    ReadFloat(fog.fHeightFogOffset);
    ReadFloat(fog.fHeightFogStart);
    ReadFloat(fog.fHeightFogEnd);
    Readbool(fog.bHeightFogAddScattering);

    if(iVersion > VSCENE_FOG_VERSION_1)
      Readbool(fog.bMaskSky);

    if (IsInErrorState())
      return false;
	  
    fog.iHeightFogColorOffset.SetRGBA(0,0,0,0);
    fog.depthMode = (VFogParameters::Mode)depthMode;
    fog.heightFogMode = (VFogParameters::Mode)heightFogMode;
  }

  Vision::World.SetFogParameters(fog);
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////
// ReadV3DChunk : loads a chunk that contains the V3D related data
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::ReadV3DChunk()
{
  char szV3DFilename[FS_MAX_PATH];
  VColorRef iDefaultColor = V_RGBA_WHITE;

  int iVers=0; // local chunk version
  if (m_iSceneVersion>=SCENE_FILE_VERSION4)
    ReadInt(iVers); 

  hkvVec3d worldCoordinateReference(0,0,0);
  if (iVers>=6)
    Read(worldCoordinateReference.data,sizeof(double)*3,"qqq"); // version 6

  // Moved here from VIEW chunk
  if(m_iSceneVersion >= SCENE_FILE_VERSION16)
  {
    if (!ReadFloat(m_fUnitScaling)) 
      return false;

    if (m_fUnitScaling<=0.f) 
      m_fUnitScaling=1.f; // default	

      Vision::World.SetGlobalUnitScaling(m_fUnitScaling);
  }


  if (!ReadString(szV3DFilename,FS_MAX_PATH))
  {
    SetError("Invalid V3D chunk!");
    return false;
  }
  if (iVers>=1) // version number of this chunk
  {
    ReadColorRef(iDefaultColor);
  }

  int iLightmapEquation = VIS_LIGHTING_MODULATE;
  if (iVers>=2) // version number of this chunk
  {
    ReadInt(iLightmapEquation);
  }

  VSRGBMode eSRGBMode = V_SRGB_DISABLE;
  if (iVers == 4)
  {
    bool bAssumeSRGB;
    Readbool(bAssumeSRGB);
    eSRGBMode = bAssumeSRGB ? V_SRGB_ASSUME_FOR_DIFFUSE : V_SRGB_DISABLE;
  }
  else if (iVers > 4)
  {
    int iMode;
    ReadInt(iMode);
    eSRGBMode = (VSRGBMode)iMode;
  }

  IVisApp_cl *pApp = Vision::GetApplication();
  IVisSceneManager_cl *pSceneManager = Vision::GetSceneManager();

  // load world+lightgrid only makes the first 20%
  LOADINGPROGRESS.PushRange(0.f,15.f);
  
//   int iFlags = LOADWORLDFLAGS_STANDARD|LOADWORLDFLAGS_NO_VISIBILITY;
//   Vision::World.L oadWorldEx(szV3DFilename[0] ? szV3DFilename:NULL, iFlags); // NULL loads an empty map
//   VASSERT(szV3DFilename[0] || pSceneManager->GetNumVisibilityZones() == 0);

  // Make sure this is off for normal scene loading behavior
  bool bPrevSetting = Vision::Renderer.GetCreateVisibilityForEmptyWorld();
  Vision::Renderer.SetCreateVisibilityForEmptyWorld(false);

  Vision::InitWorld();
  Vision::World.GetCoordinateSystem()->SetSceneReferencePosition(worldCoordinateReference);

  // Turn it back to the previous setting just in case the application uses it
  Vision::Renderer.SetCreateVisibilityForEmptyWorld(bPrevSetting);

  LOADINGPROGRESS.PopRange();
  LOADINGPROGRESS.PushRange(15.f,20.f);

  // load lightgrid file ref.
  char szLGFilename[FS_MAX_PATH];
  VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
  if (iVers>=2 && ReadString(szLGFilename,FS_MAX_PATH) && szLGFilename[0] != '\0')
  {  
    VisLightGridManager_cl &pManager = VisLightGridManager_cl::GlobalManager();
    
    pGrid = pManager.LoadLightGrid(pManager.GetLightGridFilename(szLGFilename).AsChar());
  }

  Vision::RenderLoopHelper.SetLightGrid(pGrid);
  Vision::Renderer.SetDefaultLightingColor(iDefaultColor);
  Vision::Renderer.SetLightingMode((VIS_CFG_LightingMode)iLightmapEquation); // this sets the HasLightmaps() properties as well
  Vision::Renderer.SetSRGBMode(eSRGBMode);

  if (m_bForceMobileMode)
  {
    Vision::GetApplication()->SetShaderProvider(new VisionMobileShaderProvider());
  }
  else if (m_iSceneVersion >= SCENE_FILE_VERSION12)
  {    
    char szShaderProvider[256];
    ReadString(szShaderProvider, 256);
    VType *pType = Vision::GetTypeManager()->GetType(szShaderProvider);
    if (pType != NULL && !pType->IsAbstract())
    {
      IVisShaderProvider_cl* pShaderProvider = static_cast<IVisShaderProvider_cl*>(pType->CreateInstance());
      if (pShaderProvider != NULL)
        Vision::GetApplication()->SetShaderProvider(pShaderProvider);
    }  
  }

  LOADINGPROGRESS.PopRange();

  return true;
}


///////////////////////////////////////////////////////////////////////////////////////
// ReadReferencedPluginsChunk : reads a list of plugin names that were loaded at export
//    time. Outputs a warning if any of the plugins is currently not loaded by the engine
//    However this isn't fatal since there is no possibility to check wether the plugin
//    is actually needed or not.
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::ReadReferencedPluginsChunk(bool bLoadPlugins)
{
#if defined(_DLL) && !defined(VISIONDLL_LIB)
  char szPluginName[FS_MAX_PATH];
  int iCount;
  ReadInt(iCount);
  VASSERT(iCount>=0 && iCount<1024); // sanity check
  for (int i=0;i<iCount;i++)
  {
    if (!ReadString(szPluginName,FS_MAX_PATH))
      return false;
    bool bLoaded = Vision::Plugins.IsEnginePluginLoaded(szPluginName) ||
        					 Vision::Plugins.GetRegisteredPlugin(szPluginName) ;
    if (bLoaded)
      continue;

    if (bLoadPlugins)
    {
      bLoaded = Vision::Plugins.LoadEnginePlugin(szPluginName);
      if (!bLoaded)
      {
        Vision::Error.Warning("Plugin '%s' referenced in scene file but failed to load", szPluginName);
      }
    } 
    else // warnings only
    {
      Vision::Error.Warning("Plugin '%s' referenced in scene file but not loaded by the engine", szPluginName);
    }
  }
#endif
  return true;
}

bool VSceneLoader::ReadEmbeddedFileChunk()
{
  CHUNKIDTYPE iChunkID;
  while (GetRemainingChunkByteCount()>0)
  {
    VVERIFY(OpenChunk(&iChunkID) && "Invalid sub-chunk");
    if (iChunkID=='_LIT') // parse this known file type
    {
      IVFileInStream *pIn = GetChunkInStream(); // get embedded chunk stream
      VASSERT(pIn);
      VLightmapSceneInfo scene;

      // load the embedded output lit file
      if (scene.Open(pIn))
      {
        if (scene.LoadOutputFile(NULL, NULL, &Vision::TextureManager.GetManager()))
        {
          // if successful, trigger callback
          VLightmapInfoDataObject_cl data(scene, &Vision::Callbacks.OnLightmapFileLoaded);
          Vision::Callbacks.OnLightmapFileLoaded.TriggerCallbacks(&data);
        }
        scene.Close();
      }
      pIn->Close();
    }
    EndChunk();
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////////////
// ReadZoneChunk : loads a chunk file with zone references
///////////////////////////////////////////////////////////////////////////////////////
bool VSceneLoader::ReadZoneChunk()
{
  int iVersion;
  if (!ReadInt(iVersion) || iVersion>1)
  {
    SetError("Invalid zone chunk version");
    return false;
  }

  hkvAlignedBBox zoneBox;
  char szZoneName[FS_MAX_PATH];
  char szZoneFile[FS_MAX_PATH];
  char szLightGridFile[FS_MAX_PATH];
  int iCount,iReserved;
  ReadInt(iCount);
  if (iCount<1)
    return true;


  float fNear,fFarClip;
  Vision::Contexts.GetMainRenderContext()->GetClipPlanes(fNear,fFarClip);

  for (int i=0;i<iCount;i++)
  {
    ReadString(szZoneName,FS_MAX_PATH);
    ReadString(szLightGridFile,FS_MAX_PATH);
    Read(zoneBox.m_vMin.data,3*sizeof(float),"fff");
    Read(zoneBox.m_vMax.data,3*sizeof(float),"fff");
    VASSERT_MSG(zoneBox.isValid(),"Zone bounding box must be valid");
    sprintf(szZoneFile,"%s_data\\%s.vzone",m_sSceneFilename.AsChar(),szZoneName);
    VisZoneResource_cl *pZone = VisZoneResourceManager_cl::GlobalManager().CreateZone(szZoneFile,zoneBox);
    VASSERT(pZone);
    ReadFloat(pZone->m_fLoadedDistance);
    if (pZone->m_fLoadedDistance<0.f)
      pZone->m_fLoadedDistance = fFarClip;

    ReadFloat(pZone->m_fCacheDistance); // just the margin value
    ReadFloat(pZone->m_fCacheOutDistance); // just the margin value
    if (iVersion>=1)
      Read(pZone->m_vZonePivot.data, sizeof(pZone->m_vZonePivot), "ddd"); // version 1
    ReadInt(iReserved);

    // add the previous distances as we loaded margin values
    pZone->m_fCacheDistance += pZone->m_fLoadedDistance;
    pZone->m_fCacheOutDistance += pZone->m_fCacheDistance;

    if (szLightGridFile[0])
      pZone->m_sLightgridFilename = szLightGridFile;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////
// OnStartChunk : processes the chunks in the vScene file
///////////////////////////////////////////////////////////////////////////////////////
BOOL VSceneLoader::OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen)
{
  if (m_bAborted)
    return TRUE;

  m_currentChunkID = chunkID;

  if (chunkID=='SCNE')
    return ReadSceneChunk();

  // the version number must be already set here
  if (m_iSceneVersion<0)
  {
    SetError("Missing scene chunk. Invalid scene file?");
    return FALSE;
  }

  if (chunkID=='EPLG')
    return ReadReferencedPluginsChunk(m_bLoadPlugins);

  // parse scene elements
  if (chunkID=='_V3D')
    return ReadV3DChunk();
  if (chunkID=='_SKY')
  {
    Vision::Message.Add("Warning: Please re-export scene due to sky changes");
    return TRUE; //VisSky_cl::SerializeSkyConfig(*this);  DEPRECATED, now in shapes chunk
  }
  if (chunkID=='VIEW')
    return ReadViewChunk();
  if (chunkID=='_FOG')
    return ReadFogChunk();
  if (chunkID=='EMBD')
    return ReadEmbeddedFileChunk();

  // trigger callbacks for custom chunks before 'SHPS' chunk
  {
    VCustomSceneChunkDataObject data(*this, chunkID, &VSceneLoader::OnCustomChunkBeforeShapesSerialization);
    OnCustomChunkBeforeShapesSerialization.TriggerCallbacks(&data);
  }

  // chunk with serialized shapes
  if (chunkID=='SHPS')
    return ReadShapeChunk();

  // chunk with zone references
  if (chunkID=='ZONE')
    return ReadZoneChunk();
  
  // if not recognized, it is probably a custom chunk
  {
    VCustomSceneChunkDataObject data(*this, chunkID, &VSceneLoader::OnCustomChunkSerialization);
    OnCustomChunkSerialization.TriggerCallbacks(&data);
  }
 
  return TRUE;
}


#if defined(_DLL) && !defined(VISIONDLL_LIB)

bool VSceneLoader::LoadEnginePlugins(const char *szFilename, IVFileStreamManager *pManager)
{
  // Incorporate the currently active asset profile name into the file name.
  char szFileNameFinal[ FS_MAX_PATH ];
  DetermineFinalSceneFileName( szFileNameFinal, szFilename );

  m_bLoadPlugins = true;
  m_iSceneVersion = -1;
  if (!Open(szFileNameFinal, pManager))
    return false;

  bool bResult = true;
  CHUNKIDTYPE iChunkID;
  int iLen;

  // the header must be valid (and version 8 because otherwise plugin refs are not accurate)
  if (!OpenChunk(&iChunkID,&iLen,'SCNE') || m_iSceneVersion<SCENE_FILE_VERSION8)
  {
    bResult = false;
    goto close_and_return;
  }
  EndChunk();

  if ((m_eExportFlags&VExport_Plugins)==0) // flags are stored in 'SCNE' chunk
  {
    bResult = false;
    goto close_and_return;
  }

  bResult &= OpenChunk(&iChunkID,&iLen,'EPLG')==TRUE;
  EndChunk();

close_and_return:
  bResult &= Close()==TRUE;
  return bResult;
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
