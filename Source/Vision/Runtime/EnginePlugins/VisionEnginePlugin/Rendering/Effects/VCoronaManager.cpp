/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VCoronaManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

static int PROFILING_CORONA_UPDATE         = 0;
static int PROFILING_CORONA_RENDER         = 0;

VCoronaManager VCoronaManager::g_GlobalManager;

#if defined( SUPPORTS_OCCLUSION_QUERY ) && !defined( HK_ANARCHY )
  #define SUPPORTS_CORONAS
#endif

/////////////////////////////////////////////////////////////////////////////
// Constructor lens flare manager functions
/////////////////////////////////////////////////////////////////////////////

VCoronaManager::VCoronaManager()
: m_State(8,0,0), m_iCoronaRenderHookConst(VRH_CORONAS_AND_FLARES),
  m_bForceQueryOnTeleport(true), m_bTeleportedLastFrame(false)
{
  // Retrieving occlusions queries in the same frame they were rendered is not supported on these platforms.
#if defined(_VISION_PSP2)
  m_bForceQueryOnTeleport = false;
#endif
}

VCoronaManager::~VCoronaManager()
{
  VASSERT(m_Instances.Count() == 0); // everything must be deinitialised!
}


/////////////////////////////////////////////////////////////////////////////
// IVisCallbackHandler_cl Overrides
/////////////////////////////////////////////////////////////////////////////

void VCoronaManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
#ifdef SUPPORTS_CORONAS
  // hook into an existing renderloop to render the lens flares
  if (pData->m_pSender == &Vision::Callbacks.OnRenderHook && m_Instances.Count() )
  {
    if ( ((VisRenderHookDataObject_cl *)pData)->m_iEntryConst == m_iCoronaRenderHookConst)
    {
      RenderAllVisibleCoronas();
    }
    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnVisibilityPerformed)
  {
    if (m_Instances.Count())
    {
      if (m_bTeleportedLastFrame && m_bForceQueryOnTeleport)
      {
        UpdateCoronas(VCUF_ADD | VCUF_REMOVE | VCUF_UPDATE | VCUF_FORCE_SCHEDULE);
      }
      else
      {
        UpdateCoronas(VCUF_ADD | VCUF_REMOVE | VCUF_UPDATE);
      }
    }
    return;
  }
#endif

  // UnloadWorld : do some per-scene deinitialisation
  if (pData->m_pSender == &Vision::Callbacks.OnWorldInit)
  {
    OnWorldInit();
    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnWorldDeInit)
  {
    OnWorldDeInit();
    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    m_spCoronaTechnique = NULL;
    m_spCoronaShader = NULL;
    CreateShaders();
    return;
  }
}


/////////////////////////////////////////////////////////////////////////////
// Lens Flare Public Functions
/////////////////////////////////////////////////////////////////////////////
struct VCubeFormat
{
  hkvVec3 vPos;  // vertex position (3 floats)
};

void VCoronaManager::OneTimeInit()
{
  // listen to the following callbacks:
  Vision::Callbacks.OnWorldInit += this;
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnReassignShaders += this;

#ifdef SUPPORTS_CORONAS
  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnVisibilityPerformed += this;

  // init the profiling
  PROFILING_CORONA_UPDATE         = Vision::Profiling.GetFreeElementID();
  PROFILING_CORONA_RENDER         = Vision::Profiling.GetFreeElementID();

  VProfilingNode *pGroup = Vision::Profiling.AddGroup("Coronas");
  Vision::Profiling.AddElement(PROFILING_CORONA_UPDATE, "Update coronas", TRUE, pGroup);
  Vision::Profiling.AddElement(PROFILING_CORONA_RENDER, "Render coronas", TRUE, pGroup);
#endif
}

void VCoronaManager::OnWorldInit()
{
#ifdef SUPPORTS_CORONAS
  // Init mesh
  VisMBVertexDescriptor_t CubeFormatDescriptor;
  // now set all used components in the descriptor
  CubeFormatDescriptor.m_iPosOfs = offsetof(VCubeFormat, vPos) | VERTEXDESC_FORMAT_FLOAT3;

  // set the vertex stride
  CubeFormatDescriptor.m_iStride = sizeof(VCubeFormat);

  // first, we create a new mesh buffer
  m_spBillboardMesh = new VisMeshBuffer_cl();
  m_spBillboardMesh->AllocateVertices(CubeFormatDescriptor, 6);
  VCubeFormat *pVertex = (VCubeFormat *)m_spBillboardMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE);

  pVertex[0].vPos.set(0.0, -1.0, -1.0);
  pVertex[1].vPos.set(0.0, -1.0,  1.0);
  pVertex[2].vPos.set(0.0,  1.0, -1.0);
  pVertex[3].vPos.set(0.0,  1.0, -1.0);
  pVertex[4].vPos.set(0.0, -1.0,  1.0);
  pVertex[5].vPos.set(0.0,  1.0,  1.0);

  m_spBillboardMesh->UnLockVertices();
  m_spBillboardMesh->EnsureLoaded();

  CreateShaders();
#endif
}

void VCoronaManager::CreateShaders()
{
#ifdef SUPPORTS_CORONAS
  VShaderEffectLib* pFXLib = Vision::Shaders.LoadShaderLibrary ("\\Shaders\\Billboards.ShaderLib", SHADERLIBFLAG_HIDDEN);
  m_spCoronaShader = Vision::Shaders.CreateEffect("Corona", "", EFFECTCREATEFLAG_NONE, pFXLib);
  m_spCoronaTechnique = m_spCoronaShader->GetDefaultTechnique();

  m_spBillboardMesh->SetDefaultTechnique(m_spCoronaTechnique);
#endif
}

void VCoronaManager::OnWorldDeInit()
{
  PurgeCoronas();
#ifdef SUPPORTS_CORONAS
  m_spCoronaTechnique = NULL;
  m_spCoronaShader = NULL;
  m_spBillboardMesh = NULL;

#endif
}

void VCoronaManager::OneTimeDeInit()
{
#ifdef SUPPORTS_CORONAS
  Vision::Callbacks.OnVisibilityPerformed -= this;
  Vision::Callbacks.OnRenderHook -= this;
#endif

  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnWorldInit -= this;
  Vision::Callbacks.OnReassignShaders -= this;
  m_spCoronaShader = NULL;
  m_spBillboardMesh = NULL;
}


int VCoronaManager::AddCorona(VCoronaComponent *pCorona)
{
  int iIndex = m_Instances.AddUnique(pCorona);
  VASSERT (iIndex != -1);
  return iIndex;
}


void VCoronaManager::RemoveCorona(VCoronaComponent *pCorona)
{
  m_Instances.SafeRemove(pCorona); // might be removed already by UnloadWorld

  VCoronaCandidate temp(pCorona);
  // Check the candidate lists of all render contexts.
  const int iSize = m_State.GetSize();
  for (int i=0; i < iSize; ++i)
  {
    VCoronaRenderContextState& state = m_State[i];
    // Check inside candidate list
    int iIndex = state.m_Candidates.Find(temp);
    if (iIndex != -1)
    {
      // Replace pCorona with the last element in the array
      state.m_Candidates.SetAt(iIndex, state.m_Candidates.GetAt(state.m_Candidates.GetSize()-1) );
      state.m_Candidates.RemoveAt(state.m_Candidates.GetSize() -1);
      state.RemoveBit(pCorona->m_iIndex);
    }
  }
}


void VCoronaManager::PurgeCoronas()
{
  m_State.Reset();
  m_Instances.Clear();
}


void VCoronaManager::UpdateCoronas(int iCoronaUpdateFlags)
{
#ifdef SUPPORTS_CORONAS

  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();

  if ((iCoronaUpdateFlags & VCUF_USE_OC_CONTEXT) > 0)
  {
    // Determine relevant render context and visibility collector
    IVisVisibilityCollector_cl *pVisCollector = pContext->GetVisibilityCollector();
    if (!pVisCollector)
      return;
    VisRenderContext_cl *pOQContext = pVisCollector->GetOcclusionQueryRenderContext();
    if (pOQContext != NULL)
      pContext = pOQContext;
  }

  if (pContext == NULL)
    return;

  if ((pContext->GetRenderFlags() & VIS_RENDERCONTEXT_FLAG_USE_PIXELCOUNTER) == 0)
    return;

  if ((pContext->GetRenderFlags() & VIS_RENDERCONTEXT_FLAG_RENDER_CORONAS) == 0)
    return;

  // Get bitmask for this context.
  unsigned int iRenderFilterMask = pContext->GetRenderFilterMask();

  // get the collection of visible lights.
  IVisVisibilityCollector_cl* pVisCollector = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisCollector == NULL)
    return;

  VISION_PROFILE_FUNCTION(PROFILING_CORONA_UPDATE);

  // Get multi-sampling mode
  unsigned int iTexelsPerPixel = 1;
  VTextureObject* pDepthTex = pContext->GetDepthStencilTarget();
  if(pDepthTex == NULL)
  {
    // If no depth stencil target is available, we might work without a renderer node and we're in the main context
    if(Vision::Renderer.GetCurrentRendererNode() == NULL && pContext == VisRenderContext_cl::GetMainRenderContext())
    {
      // In this case get the multi-sampling type from the video config as it's used to set the actual backbuffer settings
      // where the main context will render to
      iTexelsPerPixel = hkvMath::Max(1, 1 << ((int)Vision::Video.GetCurrentConfig()->m_eMultiSample));     
    }  
  }
  else if (pDepthTex->GetTextureType() == VTextureLoader::Texture2D)
  {
    iTexelsPerPixel = hkvMath::Max(1u, ((VisRenderableTexture_cl*)pDepthTex)->GetConfig()->m_iMultiSampling);
  }
  
  const VisLightSrcCollection_cl* pVisibleLights = pVisCollector->GetVisibleLights();
  int iCandidates = 0;
  if (pVisibleLights != NULL)
    iCandidates = pVisibleLights->GetNumEntries();

  // Ensure size of coronas state structure.
  int iContextIndex = pContext->GetNumber();
  if (iContextIndex + 1 > m_State.GetSize())
    m_State.SetSize(iContextIndex + 1, -1);
  VCoronaRenderContextState& state = m_State[iContextIndex];
  int iCapacity = m_Instances.GetCapacity();
  state.EnsureSize(iCapacity);

  // Add visible lights with a lens flare component to the candidate list for this frame
  if ((iCoronaUpdateFlags & VCUF_ADD) > 0)
  {
    for (int iCandidate = 0; iCandidate < iCandidates; ++iCandidate)
    {
      VisLightSource_cl* pLight = pVisibleLights->GetEntry(iCandidate);
      if (pLight)
      {
        VCoronaComponent *pComponent = pLight->Components().GetComponentOfBaseType<VCoronaComponent>();
        if (pComponent != NULL && pComponent->IsEnabled() && !state.IsBitSet(pComponent->m_iIndex))
        {
          // The component is not in m_Candidates yet, so we check whether it is a valid candidate
          bool bIsLightOnScreen = pComponent->IsValidCandidate(pContext);

          if (bIsLightOnScreen)
          {
            state.SetBit(pComponent->m_iIndex);
            pContext->SetPixelCounterResult(pComponent->m_CoronaPixelCounter.GetNumber(), 0);
            state.m_Candidates.Append(pComponent);
          }
        }
      }
    }
  }
  

  // Forces the retrieval all pending queries.
  pContext->FetchPixelCounterTestResults( (iCoronaUpdateFlags & VCUF_FORCE_FETCH) > 0 );
   
  
  // Retrieve Queries and update status of lens flares
  if ((iCoronaUpdateFlags & VCUF_UPDATE) > 0)
  {
    for (int i=0; i < state.m_Candidates.GetSize(); ++i)
    {
      VCoronaCandidate& coronaCandidate = state.m_Candidates.ElementAt(i);
      VCoronaComponent* pCorona = coronaCandidate.m_pCorona;
      if (!pCorona || !pCorona->IsEnabled())
        continue;

      if (pCorona->GetOwner())
      {
        // Retrieve occlusion results of the last query
        int iElementIndex = pCorona->m_CoronaPixelCounter.GetNumber();
        bool bRes = !pContext->IsPixelCounterQueryInProgress(iElementIndex);

        // Reschedule query if the old on could be retrieved or if a teleport forces us to re-query everything.
        if (bRes | ((iCoronaUpdateFlags & VCUF_FORCE_SCHEDULE) > 0) )
          pContext->SchedulePixelCounterTest(iElementIndex);

        unsigned int iDrawnPixels = pContext->GetPixelCounterResult(iElementIndex) / iTexelsPerPixel;

        float fVisibility = (float)iDrawnPixels / ((int)pCorona->QueryRadius * (int)pCorona->QueryRadius * 4);

        // ATI fix for random insanely high return values.
        if (iDrawnPixels > ((unsigned int)pCorona->QueryRadius * 2 + 1) * ((unsigned int)pCorona->QueryRadius * 2 + 1))
        {
          fVisibility = coronaCandidate.m_fLastVisibilityQuery;
        }

        if ((iCoronaUpdateFlags & VCUF_FORCE_FETCH) > 0)
        {
          // Force lens flare visibility to the current query value.
          coronaCandidate.m_fCurrentVisibility = fVisibility;
          coronaCandidate.m_fLastVisibilityQuery = fVisibility;
          pCorona->UpdateVisibility(coronaCandidate.m_fLastVisibilityQuery, coronaCandidate.m_fCurrentVisibility);
        }
        else if (!m_bTeleportedLastFrame)
        {
          coronaCandidate.m_fLastVisibilityQuery = fVisibility;
          pCorona->UpdateVisibility(coronaCandidate.m_fLastVisibilityQuery, coronaCandidate.m_fCurrentVisibility);
        }
        else
        {
          // if we were teleported, the last frame's query results must be invalidated
          coronaCandidate.m_fCurrentVisibility = 0.0f;
          coronaCandidate.m_fLastVisibilityQuery = 0.0f;
        }
      }
    }
  }
  
  // Removes coronas that are outside the frustum and no longer visible.
  if ((iCoronaUpdateFlags & VCUF_REMOVE) > 0)
  {
    for (int i=0; i < state.m_Candidates.GetSize();)
    {
      VCoronaCandidate& coronaCandidate = state.m_Candidates.ElementAt(i);
      VCoronaComponent* pCorona = coronaCandidate.m_pCorona;

      int iElementIndex = pCorona->m_CoronaPixelCounter.GetNumber();
      // If the visibility reached zero and the corona is no longer potentially visible it is removed from the list
      if (!pCorona->IsEnabled() || !pCorona->GetOwner()
        || (pCorona->GetVisibleBitmask() & iRenderFilterMask) == 0
        || (((VisLightSource_cl*)pCorona->GetOwner())->GetVisibleBitmask() & iRenderFilterMask) == 0
        || ( coronaCandidate.m_fCurrentVisibility == 0.0f && !pCorona->IsValidCandidate(pContext) ) )
      {
        state.RemoveBit(pCorona->m_iIndex);
        state.m_Candidates.SetAt(i, state.m_Candidates.GetAt(state.m_Candidates.GetSize()-1) );
        state.m_Candidates.RemoveAt(state.m_Candidates.GetSize() -1);
        // Reset cache to zero, so we don't see the lens flare once it enters the frustum again.
        pContext->SetPixelCounterResult(iElementIndex, 0);
      }
      else
      {
        ++i;
      }
    }
  }

#endif
}


void VCoronaManager::RenderAllVisibleCoronas()
{
#ifdef SUPPORTS_CORONAS
  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();

  // Determine relevant render context and visibility collector
  IVisVisibilityCollector_cl *pVisCollector = pContext->GetVisibilityCollector();
  if (!pVisCollector)
    return;
  VisRenderContext_cl *pOQContext = pVisCollector->GetOcclusionQueryRenderContext();
  if (pOQContext != NULL)
    pContext = pOQContext;


  if ((pContext->GetRenderFlags() & VIS_RENDERCONTEXT_FLAG_USE_PIXELCOUNTER) == 0)
    return;

  if ((pContext->GetRenderFlags() & VIS_RENDERCONTEXT_FLAG_RENDER_CORONAS) == 0)
    return;

  INSERT_PERF_MARKER_SCOPE("VCoronaManager::RenderAllVisibleCoronas");
  VISION_PROFILE_FUNCTION(PROFILING_CORONA_RENDER);

  // Force for the queries to finish so they are available in this frame.
  if (m_bTeleportedLastFrame && m_bForceQueryOnTeleport)
  {
    UpdateCoronas(VCUF_UPDATE | VCUF_FORCE_FETCH | VCUF_USE_OC_CONTEXT);
  }

  // Ensure size of corona state structure.
  int iContextIndex = pContext->GetNumber();
  if (iContextIndex + 1 > m_State.GetSize())
    m_State.SetSize(iContextIndex + 1, -1);
  VCoronaRenderContextState& state = m_State[iContextIndex];
  int iCapacity = m_Instances.GetCapacity();
  state.EnsureSize(iCapacity);

  const int iCoronasToRender = state.m_Candidates.GetSize();

  // Sort candidates by texture?
  
  VTextureObject* pTexture = NULL;

  // Render all corona components
  Vision::RenderLoopHelper.BeginMeshRendering();
  Vision::RenderLoopHelper.AddMeshStreams(m_spBillboardMesh,VERTEX_STREAM_POSITION);

  for (int i=0; i < iCoronasToRender; ++i)
  {
    VCoronaCandidate& coronaCandidate = state.m_Candidates.ElementAt(i);
    if (coronaCandidate.m_fCurrentVisibility > 0.0f)
    {
      RenderCorona (coronaCandidate, pTexture);
    }
  }

  Vision::RenderLoopHelper.EndMeshRendering();

  m_bTeleportedLastFrame = (pContext->GetCamera()->GetLastTeleported() >= pContext->GetLastRenderedFrame());
#endif
}


void VCoronaManager::RenderCorona (VCoronaCandidate& coronaCandidate, VTextureObject*& pTexture)
{
#ifdef SUPPORTS_CORONAS
  VCoronaComponent *pCorona = coronaCandidate.m_pCorona;
  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
  VisLightSource_cl* pLight = (VisLightSource_cl*)pCorona->GetOwner();

  hkvVec3 vLightPos(hkvNoInitialization);
  pLight->GetVirtualPosition(vLightPos, pContext);
  hkvVec3 vEyePos(hkvNoInitialization);
  pContext->GetCamera()->GetPosition(vEyePos);
  hkvVec3 vDir = pContext->GetCamera()->GetDirection();

  // Corona texture
  VTextureObject *pTex = pCorona->GetCoronaTexture();
  if (pTex == NULL)
    return;

  if (pTexture != pTex)
  {
    pTexture = pTex;
    Vision::RenderLoopHelper.BindMeshTexture(pTexture,0);
  }

  // Get light color
  VColorRef color = pLight->GetColor();
  hkvVec3 vDist = vLightPos - vEyePos;
  float fEyeDist = vDir.dot(vDist);

  //determine if camera is in light cone if the light is directional
  float fDirectionalDampening = 1.0f;
  if ( pLight->GetType() == VIS_LIGHT_SPOTLIGHT && pCorona->GetOnlyVisibleInSpotLight() )
  {
    fDirectionalDampening = 0.0f;
    float fConeAngle = pLight->GetProjectionAngle();
    float fConeLength = pLight->GetRadius();
    hkvVec3 fConeDirection = pLight->GetDirection();
    fConeDirection.normalize();

    hkvVec3 vLightEyeDist = vEyePos - vLightPos;

    //#2 check if the camera is inside the angle of the cone
    float cosinusAngle = (vLightEyeDist/vLightEyeDist.getLength()).dot(fConeDirection);
    float fDegree = hkvMath::acosDeg(cosinusAngle);
    float normRadius = fDegree / (fConeAngle/2.0f);

    if (normRadius < 1.0f)
    {
      //hardcoded falloff. For better performance, we avoid sampling the projection texture here.
      const float fEpsilon = 64.0f/256.0f;
      const float fQuadFactor = 1.0f/fEpsilon - 1.0f;
      fDirectionalDampening = 1.0f / (1.0f + fQuadFactor*normRadius*normRadius);

      // scale the function so that the value is exactly 0.0 at the edge and 1.0 in the center
      fDirectionalDampening = (fDirectionalDampening - fEpsilon) / (1.0f - fEpsilon);  
    }
  }
  // Fog params
  float fFogDampening = 1.0f;
  if (pLight->GetType() != VIS_LIGHT_DIRECTED && Vision::World.IsLinearDepthFogEnabled())
  {  
    const VFogParameters &fog = Vision::World.GetFogParameters();
    float fFogStart = fog.fDepthStart;
    float fFogEnd = fog.fDepthEnd;

    float fFogFactor = (fFogEnd > fFogStart) ? ((fEyeDist - fFogStart) / (fFogEnd - fFogStart)) : 0.f;
    fFogDampening = 1.0f - hkvMath::clamp(fFogFactor, 0.0f, 1.0f);
  }

  // Get corona rotation
  float fRotation = 0.0f;
  hkvVec4 vRotation(1.0f, 0.0f, 0.0f, 1.0f);
  if (pCorona->CoronaFlags & VIS_CORONASCALE_ROTATING)
  { 
    fRotation = hkvMath::mod (fEyeDist * 0.5f, 360.f);

    vRotation.x = hkvMath::cosDeg (fRotation);
    vRotation.y = -hkvMath::sinDeg (fRotation);
    vRotation.z = -vRotation.y;
    vRotation.w = vRotation.x;
  }

  // Texture dimensions
  int iSizeX, iSizeY, depth;
  pTex->GetTextureDimensions(iSizeX, iSizeY, depth);

  hkvVec4 vScale(0.0f, 0.0f, 0.0f, 0.0f);

  int iMainWidth, iMainHeight, iWidth, iHeight;
  pContext->GetSize(iWidth, iHeight);
  VisRenderContext_cl::GetMainRenderContext()->GetSize(iMainWidth, iMainHeight);

  // Preserve texture aspect ratio
  int iTexHeight = pTex->GetTextureHeight();
  int iTexWidth = pTex->GetTextureWidth();

  // Perspective scaling
  // This scaling ensures roughly the same size on 720p as the old implementation.
  vScale.z = iTexWidth * pCorona->CoronaScaling * 0.25f;
  vScale.w = iTexHeight * pCorona->CoronaScaling * 0.25f;

  // Screen-space scaling
  // This scaling ensures roughly the same size on 720p as the old implementation.
  const float fScaleFactor = pCorona->CoronaScaling * iMainHeight / 11.0f;
  vScale.x = ((float)iTexWidth / 128.0f) * fScaleFactor * (float(iWidth) / float(iMainWidth));
  vScale.y = ((float)iTexHeight / 128.0f) * fScaleFactor * (float(iHeight) / float(iMainHeight));
  vScale.x *= 2.0f / iWidth;
  vScale.y *= 2.0f / iHeight;
  
  // Scale by visibility
  if (pCorona->CoronaFlags & VIS_CORONASCALE_VISIBLEAREA)
  {
    vScale.x *= coronaCandidate.m_fCurrentVisibility;
    vScale.y *= coronaCandidate.m_fCurrentVisibility;
    vScale.z *= coronaCandidate.m_fCurrentVisibility;
    vScale.w *= coronaCandidate.m_fCurrentVisibility;
  }

  VCompiledShaderPass* pShader = m_spCoronaTechnique->GetShader(0);
  VShaderConstantBuffer *pVertexConstBuffer = pShader->GetConstantBuffer(VSS_VertexShader);
  // xyz = worldspace position, w = 1.0 if VIS_CORONASCALE_DISTANCE is true, otherwise zero.
  pVertexConstBuffer->SetSingleParameterF("coronaPosition", vLightPos.x, vLightPos.y, vLightPos.z, (pCorona->CoronaFlags & VIS_CORONASCALE_DISTANCE) ? 1.0f : 0.0f);
  // xyz = light color, w = corona visibility.
  pVertexConstBuffer->SetSingleParameterF("coronaColor", color.r/255.0f, color.g/255.0f, color.b/255.0f, coronaCandidate.m_fCurrentVisibility * fFogDampening * fDirectionalDampening);
  // xyzw = 2x2 rotation matrix. float2x2 is not supported in shader model 2, so a float4 is used and multiplication is done manually in the shader.
  pVertexConstBuffer->SetSingleParameterF("coronaRotation", vRotation.x, vRotation.y, vRotation.z, vRotation.w);
  // xy = screen-space scaling. zw = view-space scaling.
  pVertexConstBuffer->SetSingleParameterF("coronaScale", vScale.x, vScale.y, vScale.z, vScale.w);
    
  Vision::RenderLoopHelper.RenderMeshes(pShader, VisMeshBuffer_cl::MB_PRIMTYPE_TRILIST, 0, 2, 6);
#endif
}

VCoronaManager& VCoronaManager::GlobalManager()
{
  return g_GlobalManager;
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
