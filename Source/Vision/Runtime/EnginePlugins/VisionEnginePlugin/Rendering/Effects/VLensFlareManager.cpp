/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLensFlareManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

static int PROFILING_LF_UPDATE         = 0;
static int PROFILING_LF_RENDER         = 0;

VLensFlareManager VLensFlareManager::g_GlobalManager;

#if defined( SUPPORTS_OCCLUSION_QUERY ) && !defined( HK_ANARCHY )
  #define SUPPORTS_LENSFLARES
#endif


/////////////////////////////////////////////////////////////////////////////
// Constructor lens flare manager functions
/////////////////////////////////////////////////////////////////////////////

VLensFlareManager::VLensFlareManager()
: m_State(8,0,0), m_iLensFlareRenderHookConst(VRH_CORONAS_AND_FLARES),
  m_bForceQueryOnTeleport(true), m_bTeleportedLastFrame(false)
{
// Retrieving occlusions queries in the same frame they were rendered is not supported on these platforms.
#if defined(_VISION_PSP2)
  m_bForceQueryOnTeleport = false;
#endif
}

VLensFlareManager::~VLensFlareManager()
{
  VASSERT(m_Instances.Count() == 0); // everything must be deinitialised!
}


/////////////////////////////////////////////////////////////////////////////
// IVisCallbackHandler_cl Overrides
/////////////////////////////////////////////////////////////////////////////

void VLensFlareManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
#ifdef SUPPORTS_LENSFLARES
  // hook into an existing renderloop to render the lens flares
  if (pData->m_pSender == &Vision::Callbacks.OnRenderHook && m_Instances.Count() )
  {
    if ( ((VisRenderHookDataObject_cl *)pData)->m_iEntryConst == m_iLensFlareRenderHookConst)
    {
      RenderAllVisibleLensFlares();
    }
    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnVisibilityPerformed)
  {
    if (m_Instances.Count())
    {
      if (m_bTeleportedLastFrame && m_bForceQueryOnTeleport)
      {
        UpdateLensFlares(VLFUF_ADD | VLFUF_REMOVE | VLFUF_UPDATE | VLFUF_FORCE_SCHEDULE);
      }
      else
      {
        UpdateLensFlares(VLFUF_ADD | VLFUF_REMOVE | VLFUF_UPDATE);
      }
    }
    return;
  }
#endif

  // UnloadWorld : do some per-scene deinitialisation
  if (pData->m_pSender == &Vision::Callbacks.OnWorldDeInit)
  {
    PurgeLensFlares();
    return;
  }
}


/////////////////////////////////////////////////////////////////////////////
// Lens Flare Public Functions
/////////////////////////////////////////////////////////////////////////////

void VLensFlareManager::OneTimeInit()
{
  // listen to the following callbacks:
  Vision::Callbacks.OnWorldDeInit += this;

#ifdef SUPPORTS_LENSFLARES
  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnVisibilityPerformed += this;

  // init the profiling
  PROFILING_LF_UPDATE         = Vision::Profiling.GetFreeElementID();
  PROFILING_LF_RENDER         = Vision::Profiling.GetFreeElementID();

  VProfilingNode *pGroup = Vision::Profiling.AddGroup("Lens Flares");
  Vision::Profiling.AddElement(PROFILING_LF_UPDATE, "Update lens flares", TRUE, pGroup);
  Vision::Profiling.AddElement(PROFILING_LF_RENDER, "Render lens flares", TRUE, pGroup);
#endif
}


void VLensFlareManager::OneTimeDeInit()
{
#ifdef SUPPORTS_LENSFLARES
  Vision::Callbacks.OnVisibilityPerformed -= this;
  Vision::Callbacks.OnRenderHook -= this;
#endif

  Vision::Callbacks.OnWorldDeInit -= this;
}


int VLensFlareManager::AddLensFlare(VLensFlareComponent *pLensFlare)
{
  int iIndex = m_Instances.AddUnique(pLensFlare);
  VASSERT (iIndex != -1);
  return iIndex;
}


void VLensFlareManager::RemoveLensFlare(VLensFlareComponent *pLensFlare)
{
  m_Instances.SafeRemove(pLensFlare); // might be removed already by UnloadWorld

  VLensFlareCandidate temp(pLensFlare);
  // Check the candidate lists of all render contexts.
  const int iSize = m_State.GetSize();
  for (int i=0; i < iSize; ++i)
  {
    VLensFlareRenderContextState& state = m_State[i];
    // Check inside candidate list
    int iIndex = state.m_Candidates.Find(temp);
    if (iIndex != -1)
    {
      // Replace pLensFlare with the last element in the array
      state.m_Candidates.SetAt(iIndex, state.m_Candidates.GetAt(state.m_Candidates.GetSize()-1) );
      state.m_Candidates.RemoveAt(state.m_Candidates.GetSize() -1);
      state.RemoveBit(pLensFlare->m_iIndex);
    }
  }
}


void VLensFlareManager::PurgeLensFlares()
{
  m_State.Reset();
  m_Instances.Clear();
}


void VLensFlareManager::UpdateLensFlares(int iLensFlareUpdateFlags)
{
#ifdef SUPPORTS_LENSFLARES

  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();

  if ((iLensFlareUpdateFlags & VLFUF_USE_OC_CONTEXT) > 0)
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

  if ((pContext->GetRenderFlags() & VIS_RENDERCONTEXT_FLAG_RENDER_LENSFLARES) == 0)
    return;

  // get the collection of visible lights.
  IVisVisibilityCollector_cl* pVisCollector = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisCollector == NULL)
    return;

  VISION_PROFILE_FUNCTION(PROFILING_LF_UPDATE);

  // Get multi-sampling mode
  unsigned int iTexelsPerPixel = 1;
  VTextureObject* pDepthTex = pContext->GetDepthStencilTarget();
  if (pDepthTex != NULL && pDepthTex->GetTextureType() == VTextureLoader::Texture2D)
  {
    iTexelsPerPixel = hkvMath::Max(1u, ((VisRenderableTexture_cl*)pDepthTex)->GetConfig()->m_iMultiSampling);
  }

  const VisLightSrcCollection_cl* pVisibleLights = pVisCollector->GetVisibleLights();
  int iCandidates = 0;
  if (pVisibleLights != NULL)
    iCandidates = pVisibleLights->GetNumEntries();

  // Ensure size of lens flare state structure.
  int iContextIndex = pContext->GetNumber();
  if (iContextIndex + 1 > m_State.GetSize())
    m_State.SetSize(iContextIndex + 1, -1);
  VLensFlareRenderContextState& state = m_State[iContextIndex];
  int iCapacity = m_Instances.GetCapacity();
  state.EnsureSize(iCapacity);

  // Add visible lights with a lens flare component to the candidate list for this frame
  if ((iLensFlareUpdateFlags & VLFUF_ADD) > 0)
  {
    for (int iCandidate = 0; iCandidate < iCandidates; ++iCandidate)
    {
      VisLightSource_cl* pLight = pVisibleLights->GetEntry(iCandidate);
      if (pLight)
      {
        VLensFlareComponent *pComponent = pLight->Components().GetComponentOfBaseType<VLensFlareComponent>();
        if (pComponent != NULL && pComponent->IsEnabled() && !state.IsBitSet(pComponent->m_iIndex))
        {
          // The component is not in m_Candidates yet, so we check whether it is a valid candidate
          bool bIsLightOnScreen = pComponent->IsValidCandidate(pContext);

          if (bIsLightOnScreen)
          {
            state.SetBit(pComponent->m_iIndex);
            pContext->SetPixelCounterResult(pComponent->m_LensFlarePixelCounter.GetNumber(), 0);
            state.m_Candidates.Append(pComponent);
          }
        }
      }
    }
  }
  

  // Forces the retrieval all pending queries.
  pContext->FetchPixelCounterTestResults( (iLensFlareUpdateFlags & VLFUF_FORCE_FETCH) > 0 );
   
  
  // Retrieve Queries and update status of lens flares
  if ((iLensFlareUpdateFlags & VLFUF_UPDATE) > 0)
  {
    for (int i=0; i < state.m_Candidates.GetSize(); ++i)
    {
      VLensFlareCandidate& lensFlareCandidate = state.m_Candidates.ElementAt(i);
      VLensFlareComponent *pLensFlare = lensFlareCandidate.m_pLensFlare;
      if (!pLensFlare || !pLensFlare->IsEnabled())
        continue;

      if (pLensFlare->GetOwner())
      {
        // Retrieve occlusion results of the last query
        int iElementIndex = pLensFlare->m_LensFlarePixelCounter.GetNumber();
        bool bRes = !pContext->IsPixelCounterQueryInProgress(iElementIndex);

        // Reschedule query if the old on could be retrieved or if a teleport forces us to re-query everything.
        if (bRes | ((iLensFlareUpdateFlags & VLFUF_FORCE_SCHEDULE) > 0) )
          pContext->SchedulePixelCounterTest(iElementIndex);

        unsigned int iDrawnPixels = pContext->GetPixelCounterResult(iElementIndex) / iTexelsPerPixel;
        float fVisibility = (float)iDrawnPixels / ((int)pLensFlare->QueryRadius * (int)pLensFlare->QueryRadius * 4);

        // ATI fix for random insanely high return values.
        if (iDrawnPixels > ((unsigned int)pLensFlare->QueryRadius * 2 + 1) * ((unsigned int)pLensFlare->QueryRadius * 2 + 1))
        {
          fVisibility = lensFlareCandidate.m_fLastVisibilityQuery;
        }

        if ((iLensFlareUpdateFlags & VLFUF_FORCE_FETCH) > 0)
        {
          // Force lens flare visibility to the current query value.
          lensFlareCandidate.m_fCurrentVisibility = fVisibility;
          lensFlareCandidate.m_fLastVisibilityQuery = fVisibility;
          pLensFlare->UpdateVisibility(lensFlareCandidate.m_fLastVisibilityQuery, lensFlareCandidate.m_fCurrentVisibility);

        }
        else if (!m_bTeleportedLastFrame)
        {
          lensFlareCandidate.m_fLastVisibilityQuery = fVisibility;
          pLensFlare->UpdateVisibility(lensFlareCandidate.m_fLastVisibilityQuery, lensFlareCandidate.m_fCurrentVisibility);
        }
        else
        {
          // if we were teleported, the last frame's query results must be invalidated
          lensFlareCandidate.m_fCurrentVisibility = 0.0f;
          lensFlareCandidate.m_fLastVisibilityQuery = 0.0f;
        }
      }
    }
  }
  
  // Removes lens flares that are outside the frustum and no longer visible.
  if ((iLensFlareUpdateFlags & VLFUF_REMOVE) > 0)
  {
    for (int i=0; i < state.m_Candidates.GetSize();)
    {
      VLensFlareCandidate& lensFlareCandidate = state.m_Candidates.ElementAt(i);
      VLensFlareComponent *pLensFlare = lensFlareCandidate.m_pLensFlare;

      int iElementIndex = pLensFlare->m_LensFlarePixelCounter.GetNumber();
      // If the visibility reached zero and the lens flare is no longer potentially visible it is removed from the list
      if (!pLensFlare->IsEnabled() || !pLensFlare->GetOwner()
        || (((VisLightSource_cl*)pLensFlare->GetOwner())->GetVisibleBitmask() & pContext->GetRenderFilterMask()) == 0
        || ( lensFlareCandidate.m_fCurrentVisibility == 0.0f && !pLensFlare->IsValidCandidate(pContext) ) )
      {
        state.RemoveBit(pLensFlare->m_iIndex);
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


void VLensFlareManager::RenderAllVisibleLensFlares()
{
#ifdef SUPPORTS_LENSFLARES
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

  if ((pContext->GetRenderFlags() & VIS_RENDERCONTEXT_FLAG_RENDER_LENSFLARES) == 0)
    return;

  INSERT_PERF_MARKER_SCOPE("VLensFlareManager::RenderAllVisibleLensFlares");
  VISION_PROFILE_FUNCTION(PROFILING_LF_RENDER);

  // Force for the queries to finish so they are available in this frame.
  if (m_bTeleportedLastFrame && m_bForceQueryOnTeleport)
  {
    UpdateLensFlares(VLFUF_UPDATE | VLFUF_FORCE_FETCH | VLFUF_USE_OC_CONTEXT);
  }

  // Ensure size of lens flare state structure.
  int iContextIndex = pContext->GetNumber();
  if (iContextIndex + 1 > m_State.GetSize())
    m_State.SetSize(iContextIndex + 1, -1);
  VLensFlareRenderContextState& state = m_State[iContextIndex];
  int iCapacity = m_Instances.GetCapacity();
  state.EnsureSize(iCapacity);

  // Render all lens flare components
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  const int iLensFlaresToRender = state.m_Candidates.GetSize();
  for (int i=0; i < iLensFlaresToRender; ++i)
  {
    VLensFlareCandidate& lensFlareCandidate = state.m_Candidates.ElementAt(i);
    if (lensFlareCandidate.m_fCurrentVisibility > 0.0f)
    {
      RenderLensFlares (lensFlareCandidate, pRI);
    }
  }

  Vision::RenderLoopHelper.EndOverlayRendering();

  m_bTeleportedLastFrame = (pContext->GetCamera()->GetLastTeleported() >= pContext->GetLastRenderedFrame());
#endif
}


void VLensFlareManager::RenderLensFlares (VLensFlareCandidate& lensFlareCandidate, IVRender2DInterface *pRI)
{
#ifdef SUPPORTS_LENSFLARES
  VLensFlareComponent* pLensFlare = lensFlareCandidate.m_pLensFlare;
  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
  VisLightSource_cl* pLight = (VisLightSource_cl*)pLensFlare->GetOwner();

  hkvVec3 vLightPos(hkvNoInitialization);
  pLight->GetVirtualPosition(vLightPos, pContext);
  hkvVec3 vEyePos(hkvNoInitialization);
  pContext->GetCamera()->GetPosition(vEyePos);
  hkvVec3 vEyeDir = pContext->GetCamera()->GetDirection();
  hkvVec2 vLightScreenPos(true);
  pContext->Project2D(vLightPos, vLightScreenPos.x, vLightScreenPos.y);

  // Screen center
  hkvVec2 vScreenCenter(false);
  int iWidth, iHeight;
  VisRenderContext_cl::GetCurrentContext()->GetSize(iWidth, iHeight);
  vScreenCenter.x = (float)iWidth / 2.0f;
  vScreenCenter.y = (float)iHeight / 2.0f;

  // Angle attenuation. Lens flare will be most visible in the screen center and fade out towards the edge of the display.
  float fMaxDisplayDistance = hkvMath::sqrt((float)((iWidth>>1) * (iWidth>>1) + (iHeight>>1) * (iHeight>>1)));
  float fDistanceFromScreenCenter = hkvMath::sqrt(((iWidth>>1) - vLightScreenPos.x) * ((iWidth>>1) - vLightScreenPos.x)
    + ((iHeight>>1) - vLightScreenPos.y) * ((iHeight>>1) - vLightScreenPos.y)) / fMaxDisplayDistance;
  fDistanceFromScreenCenter = hkvMath::Min(fDistanceFromScreenCenter, 1.0f);

  // Direction for the lens flare positioning. Flares will be positioned along vDir.
  hkvVec2 vDir = vScreenCenter - vLightScreenPos;

  // Light color
  VColorRef color = pLight->GetColor();
  hkvVec3 vDist = vLightPos - vEyePos;
  float fEyeDist = vEyeDir.dot(vDist);

  //determine if camera is in light cone if the light is directional
  float fDirectionalDampening = 1.0f;
  if ( pLight->GetType() == VIS_LIGHT_SPOTLIGHT && pLensFlare->GetOnlyVisibleInSpotLight() )
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

  // Maximum intensity is 0.5 for lens flares, so we multiply with 128 here
  color.a = (BYTE)(fDirectionalDampening * fFogDampening * lensFlareCandidate.m_fCurrentVisibility * (1.0f - fDistanceFromScreenCenter) * 128);
  VLensFlareDescriptor desc;

  // Lens flare quad
  Overlay2DVertex_t vertices[6];

  // Iterate over all flares and render them using IVRender2DInterface
  for (int iFlare = 0; iFlare < MAX_NUM_LENS_FLARES; ++iFlare)
  {
    VTextureObject *pTex = pLensFlare->GetLensFlareTexture(iFlare);
    if (!pTex)
      continue;

    pLensFlare->GetLensFlareDescriptor(iFlare, desc);

    // Create transform matrix for this flare
    int iTexHeight = pTex->GetTextureHeight();
    int iTexWidth = pTex->GetTextureWidth();
    int iMaxSize = hkvMath::Max (iTexWidth, iTexHeight);
    // Preserve texture aspect ratio
    const float fScaleFactor = desc.m_fScale * iHeight;
    float fScaleX = ((float)iTexWidth / (float)iMaxSize) * fScaleFactor;
    float fScaleY = ((float)iTexHeight / (float)iMaxSize) * fScaleFactor;
    hkvVec2 vTranslate = vLightScreenPos + vDir * desc.m_fPositionOnRay;

    // Set quad data
    vertices[0].Set(-fScaleX + vTranslate.x, -fScaleY + vTranslate.y, 0.0f, 0.0f, color);
    vertices[1].Set(-fScaleX + vTranslate.x,  fScaleY + vTranslate.y, 0.0f, 1.0f, color);
    vertices[2].Set( fScaleX + vTranslate.x, -fScaleY + vTranslate.y, 1.0f, 0.0f, color);
    vertices[3].Set( fScaleX + vTranslate.x, -fScaleY + vTranslate.y, 1.0f, 0.0f, color);
    vertices[4].Set(-fScaleX + vTranslate.x,  fScaleY + vTranslate.y, 0.0f, 1.0f, color);
    vertices[5].Set( fScaleX + vTranslate.x,  fScaleY + vTranslate.y, 1.0f, 1.0f, color);

    VSimpleRenderState_t renderState(VIS_TRANSP_ADDITIVE, RENDERSTATEFLAG_USEADDITIVEALPHA|RENDERSTATEFLAG_DOUBLESIDED|RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_FILTERING);
    pRI->Draw2DBuffer(6, vertices, pTex, renderState);
  }
#endif
}

VLensFlareManager& VLensFlareManager::GlobalManager()
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
