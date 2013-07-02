/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadowManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#define FOR_ALL_SHADOWS \
  const int iCount = m_Instances.Count();\
  for (int _i=0;_i<iCount;_i++)\
  {\
    VBlobShadow *pShadow = m_Instances.GetAt(_i);\
    if (!pShadow || !pShadow->IsEnabled()) continue;


static int PROFILING_BS_OVERALL           = 0;
static int PROFILING_BS_DETERMINE_PRIMS   = 0;
static int PROFILING_BS_PREPARE_SHADER    = 0;
static int PROFILING_BS_RENDER_PRIMS      = 0;
static int PROFILING_BS_SCISSORRECT       = 0;

VBlobShadowManager VBlobShadowManager::g_GlobalManager;


/////////////////////////////////////////////////////////////////////////////
// Renderloop functions
/////////////////////////////////////////////////////////////////////////////

VBlobShadowManager::VBlobShadowManager() : shadowGeom(), shadowGeomOfType()
{
  m_bFailedLoading = false;
  m_iRenderHookConst = VRH_DECALS;
}

VBlobShadowManager::~VBlobShadowManager()
{
  VASSERT(m_Instances.Count()==0); // everything must be deinitialised!
}

void VBlobShadowManager::OneTimeInit()
{
  // listen to the following callbacks:
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnReassignShaders += this;

  // init the profiling
  PROFILING_BS_OVERALL           = Vision::Profiling.GetFreeElementID();
  PROFILING_BS_DETERMINE_PRIMS   = Vision::Profiling.GetFreeElementID();
  PROFILING_BS_PREPARE_SHADER    = Vision::Profiling.GetFreeElementID();
  PROFILING_BS_RENDER_PRIMS      = Vision::Profiling.GetFreeElementID();
  PROFILING_BS_SCISSORRECT       = Vision::Profiling.GetFreeElementID();

  VProfilingNode *pGroup = Vision::Profiling.AddGroup("Blob Shadows");
  VProfilingNode *pOverall = Vision::Profiling.AddElement(PROFILING_BS_OVERALL,        "Blob Shadows Overall",TRUE,pGroup);
    Vision::Profiling.AddElement(PROFILING_BS_SCISSORRECT,    "Scissor rect test",TRUE,pOverall);
    Vision::Profiling.AddElement(PROFILING_BS_DETERMINE_PRIMS,"Determine primitives",TRUE,pOverall);
    Vision::Profiling.AddElement(PROFILING_BS_PREPARE_SHADER, "Prepare shader",TRUE,pOverall);
    Vision::Profiling.AddElement(PROFILING_BS_RENDER_PRIMS,   "Render primitives",TRUE,pOverall);
}

void VBlobShadowManager::OneTimeDeInit()
{
  Vision::Callbacks.OnRenderHook -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnReassignShaders -= this;
}


VBlobShadowManager& VBlobShadowManager::GlobalManager()
{
  return g_GlobalManager;
}

/////////////////////////////////////////////////////////////////////////////
// RemoveShadow : removes a shadow instance
/////////////////////////////////////////////////////////////////////////////
void VBlobShadowManager::RemoveShadow(VBlobShadow *pShadow)
{
  m_Instances.SafeRemove(pShadow); // might be removed already by UnloadWorld
}


/////////////////////////////////////////////////////////////////////////////
// PurgeShadows : cleanup list of shadows
/////////////////////////////////////////////////////////////////////////////
void VBlobShadowManager::PurgeShadows()
{
  m_Instances.Clear();
}



VTextureObject* VBlobShadowManager::GetDefaultShadowTexture()
{
  // load the default blob shadow texture
  if (!m_spDefaultShadowTex)
    m_spDefaultShadowTex = Vision::TextureManager.Load2DTexture("Textures\\blobShadow.dds");
  return m_spDefaultShadowTex;
}

VCompiledTechnique* VBlobShadowManager::GetDefaultTechnique(VisStaticGeometryType_e eGeomType)
{
  if (VISION_UNLIKELY(m_spDefaultFX==NULL))
  {
    if (m_bFailedLoading)
      return NULL;
    // load the shader library with the projection effect
    BOOL bResult = Vision::Shaders.LoadShaderLibrary("\\Shaders\\BlobShadow.ShaderLib",SHADERLIBFLAG_HIDDEN)!=NULL;
    VASSERT(bResult);
    
    // create the effect
    m_spDefaultFX = Vision::Shaders.CreateEffect("BlobShadow",NULL);
    m_bFailedLoading = m_spDefaultFX==NULL;
    VASSERT(m_spDefaultFX);
    if (!m_spDefaultFX)
      return NULL;
    VTechniqueConfig *pGlobalConfig = Vision::Shaders.GetGlobalTechniqueConfig();

    VTechniqueConfig terrainConfig("Terrain",NULL);
    m_spDefaultTech[0] = m_spDefaultFX->FindCompatibleTechnique(pGlobalConfig);
    m_spDefaultTech[1] = m_spDefaultFX->FindCompatibleTechnique(&terrainConfig, pGlobalConfig);
  }
  return m_spDefaultTech[eGeomType==STATIC_GEOMETRY_TYPE_TERRAIN ? 1:0];
}


void VBlobShadowManager::SetRenderHookConstant(int iConst)
{
  m_iRenderHookConst = iConst;
}


void VBlobShadowManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  // hook into an existing renderloop to render the shadows (before rendering particles)
  if (pData->m_pSender==&Vision::Callbacks.OnRenderHook)
  {
    if (m_Instances.Count() && ((VisRenderHookDataObject_cl *)pData)->m_iEntryConst == m_iRenderHookConst)
    {
      INSERT_PERF_MARKER_SCOPE("Blob Shadow Rendering (VBlobShadowManager::OnHandleCallback)");
      RenderAllShadows();
    }
  }

  // UnloadWorld : do some per-scene deinitialisation
  else if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    PurgeShadows();
    m_spDefaultFX = NULL;
    m_bFailedLoading = false;
    m_spDefaultShadowTex = NULL;
    m_spDefaultTech[0] = NULL;
    m_spDefaultTech[1] = NULL;
  }

  // Reassign shaders
  else if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    // set shader resources to NULL, shaders will be recreated when 
    // GetDefaultTechnique is called next time.
    m_spDefaultFX = NULL;
    m_spDefaultTech[0] = NULL;
    m_spDefaultTech[1] = NULL;
  }
}



/////////////////////////////////////////////////////////////////////////////
// RenderAllShadows : render all shadow instances
/////////////////////////////////////////////////////////////////////////////
void VBlobShadowManager::RenderAllShadows()
{
  // if enabled, a 2D bounding box is additionally used for clipping, which saves a lot of fillrate!

  // TODO: PSP2 - fix 2d clipping
#if defined(_VISION_PSP2)
  static bool bClipScissor = false;
#else
  static bool bClipScissor = true;
#endif
  VisFrustum_cl viewFrustum;
  IVisVisibilityCollector_cl *pVisColl = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisColl==NULL || pVisColl->GetBaseFrustum()==NULL)
    return;
  viewFrustum.CopyFrom((VisFrustum_cl&)*pVisColl->GetBaseFrustum());

  // render all shadows
  VISION_PROFILE_FUNCTION(PROFILING_BS_OVERALL);

  // get the collection of visible (opaque) primitives. For each shadow instance determine
  // the primitives in this list, which intersect with the shadow box
  // (we do not want to render primitives that are not visible)
  const VisStaticGeometryInstanceCollection_cl *pVisibleGeom = pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_PrimaryOpaquePass);

  VRectanglef clipRect(false);
  VRectanglef screenRect(0.f,0.f,(float)Vision::Video.GetXRes(),(float)Vision::Video.GetYRes());
  hkvVec3 vBoxCorner[8];
  hkvVec2 vCorner2D(false);

  // now render the shadows:
  FOR_ALL_SHADOWS

    if (pShadow->GetOwner())
      pShadow->SetBoundingBoxFromOwnerProperties();

    // shadow box visible?
    if (!viewFrustum.Overlaps(pShadow->m_ShadowBox))
      continue;

    // build 2D bounding box for scissor clipping
    if (bClipScissor)
    {
      VISION_PROFILE_FUNCTION(PROFILING_BS_SCISSORRECT);
      clipRect.Reset();
      pShadow->m_ShadowBox.getCorners(vBoxCorner);
      for (int i=0;i<8;i++)
      {
        // if one vertex is behind camera, do not use clipping
        if (!Vision::Contexts.GetCurrentContext()->Project2D(vBoxCorner[i],vCorner2D.x,vCorner2D.y))
        {
          Vision::RenderLoopHelper.SetScissorRect(NULL);
          goto render_shadow;
        }
        clipRect.Add(vCorner2D);
      }
      VASSERT(clipRect.IsValid());
      clipRect = clipRect.GetIntersection(screenRect);
      if (!clipRect.IsValid())
        continue; // do not render shadows at all if rect is outside the screen
      Vision::RenderLoopHelper.SetScissorRect(&clipRect);
    }

render_shadow:

    // get the visible primitives in the shadow bounding box
    {
      VISION_PROFILE_FUNCTION(PROFILING_BS_DETERMINE_PRIMS);
      // affected static geometry:
      shadowGeom.Clear();
      pVisibleGeom->DetermineEntriesTouchingBox(pShadow->m_ShadowBox,shadowGeom);
    }

    // split into geometry types:
    if (!shadowGeom.GetNumEntries())
      continue;

    const VisStaticGeometryType_e relevantTypes[2] = {STATIC_GEOMETRY_TYPE_MESHINSTANCE,STATIC_GEOMETRY_TYPE_TERRAIN};

    // two relevant geometry types:
    for (int iType=0;iType<2;iType++)
    {
      shadowGeomOfType.Clear();
      shadowGeom.GetEntriesOfType(shadowGeomOfType,relevantTypes[iType]);
      VCompiledTechnique *pFX = GetDefaultTechnique(relevantTypes[iType]);
      if (shadowGeomOfType.GetNumEntries()==0 || pFX==NULL)
        continue;

      // for all the shader in the projection effect (usually 1 shader), render the primitive collection
      const int iShaderCount = pFX->GetShaderCount();

      for (int j=0;j<iShaderCount;j++)
      {
        VBlobShadowShader *pShader = (VBlobShadowShader *)pFX->GetShader(j);

        { // code block for easier profiling
          VISION_PROFILE_FUNCTION(PROFILING_BS_PREPARE_SHADER);
          // prepare the shader, i.e. setup shadow specific projection planes, colors etc.
          pShader->UpdateShadow(pShadow);
        }
        { // code block for easier profiling
          VISION_PROFILE_FUNCTION(PROFILING_BS_RENDER_PRIMS);
          Vision::RenderLoopHelper.RenderStaticGeometryWithShader(shadowGeomOfType,*pShader);
        }
      }
    }


  }
    

  // same for affected static geometry:

  Vision::RenderLoopHelper.SetScissorRect(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// DebugRenderShadowBoxes : for debugging purposes, render the boxes
/////////////////////////////////////////////////////////////////////////////
void VBlobShadowManager::DebugRenderShadowBoxes()
{
  FOR_ALL_SHADOWS
    Vision::Game.DrawBoundingBox(pShadow->m_ShadowBox,V_RGBA_WHITE);
  }
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
