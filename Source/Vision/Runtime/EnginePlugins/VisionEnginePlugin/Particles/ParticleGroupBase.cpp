/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessTranslucencies.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>


#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=178
#endif

#if defined(__ghs__)
#pragma ghs nowarning 177
#endif

static const int PARTICLEGROUPBASE_VERSION_000      = 0;      // initial version
static const int PARTICLEGROUPBASE_VERSION_001      = 1;      // changed class hierarchy (particles now derived from VisObject3D_cl)
static const int PARTICLEGROUPBASE_VERSION_002      = 2;      // ambient color
static const int PARTICLEGROUPBASE_VERSION_003      = 3;      // visible bitmask
static const int PARTICLEGROUPBASE_VERSION_004      = 4;      // individual wind speed
static const int PARTICLEGROUPBASE_VERSION_005      = 5;      // wind speed in local space
static const int PARTICLEGROUPBASE_VERSION_006      = 6;      // Time-Of-Day Light state
static const int PARTICLEGROUPBASE_VERSION_007      = 7;      // Emitter mesh entity
static const int PARTICLEGROUPBASE_VERSION_008      = 8;      // Emitter intesity
#define PARTICLEGROUPBASE_VERSION_CURRENT   PARTICLEGROUPBASE_VERSION_008

#if defined(__SNC__)
#pragma diag_pop
#endif

#if defined(__ghs__)
#pragma ghs endnowarning
#endif

VisParticleLayerEventData_cl::VisParticleLayerEventData_cl(ParticleGroupBase_cl *pLayer, VisAnimEvent_cl *pEvent)
    : IVisCallbackDataObject_cl(&VisParticleGroupManager_cl::OnLayerEvent)
{
  m_pLayer = pLayer;
  m_pEvent = pEvent;
}


V_IMPLEMENT_DYNCREATE( HandleParticlesTask_cl, VThreadedTask, &g_VisionEngineModule );

HandleParticlesTask_cl::HandleParticlesTask_cl(ParticleGroupBase_cl *pGroup) : VThreadedTask()
{
  m_fTimeDelta = 0.f;
  VASSERT(pGroup != NULL);
  m_pParticleGroup = pGroup;
}

// compare function for qsort
static int CompareParticles(const void *arg1, const void *arg2)
{
  ParticleSort_t *pSort1 = (ParticleSort_t *)arg1;
  ParticleSort_t *pSort2 = (ParticleSort_t *)arg2;
  return (int)pSort2->sortkey - (int)pSort1->sortkey;
}

void HandleParticlesTask_cl::Run(VManagedThread *pThread)
{
  // Update the seed value for this particle, this thread, this frame
  m_pParticleGroup->UpdateSeed();
  
  int i;
  float fScaledTime = m_fTimeDelta * m_pParticleGroup->m_fTimeScale;

  int &iHighWaterMark = m_pParticleGroup->m_iHighWaterMark;
  ParticleExt_t *p;
    
  // if the scene-brightness should be used and the time-of-day component is used too, check if it needs to be updated
  if ((m_pParticleGroup->m_bApplyTimeOfDayLight) && (m_pParticleGroup->m_spDescriptor->m_fApplySceneBrightness > 0.0f) && (m_pParticleGroup->m_fLastTimeOfDayUpdate >= 0.0f))
  {
		IVTimeOfDay* tod =  Vision::Renderer.GetTimeOfDayHandler();
		if(tod != NULL)
		{
      if (m_pParticleGroup->m_fLastTimeOfDayUpdate != tod->GetDayTime ())
        m_pParticleGroup->m_bEvaluateBrightnessNextFrame = true;
    }
  }

  // setup global lighting color
  if (m_pParticleGroup->m_bEvaluateBrightnessNextFrame)
    m_pParticleGroup->EvaluateSceneBrightness();

  // setup per frame variables
  m_pParticleGroup->SetPerFrameConstants(fScaledTime); // called after emitter handling (which might change it)
  m_pParticleGroup->m_vFrameWind = m_pParticleGroup->m_vWindSpeed*fScaledTime;
  if (m_pParticleGroup->m_bWindInLocalSpace)
  {
    const hkvMat3 &mRot = m_pParticleGroup->GetRotationMatrix();
    m_pParticleGroup->m_vFrameWind = mRot * m_pParticleGroup->m_vFrameWind;
  }

  hkvVec3& vNoInertia = (hkvVec3&) m_pParticleGroup->m_vFrameWindNoInertia;
  if (m_pParticleGroup->m_bInertiaAffectsGravity)
  {
    m_pParticleGroup->m_vFrameWind += m_pParticleGroup->m_spDescriptor->m_vGravity * fScaledTime;
    vNoInertia.setZero();
  }
  else
  {
    vNoInertia = m_pParticleGroup->m_spDescriptor->m_vGravity * fScaledTime;
  }

  // transform per-frame speeds back into local space
  if (m_pParticleGroup->GetUseLocalSpaceMatrix())
  {
    hkvMat3 transposedRot = m_pParticleGroup->GetRotationMatrix();
    transposedRot.transpose();
    m_pParticleGroup->m_vFrameWind = transposedRot * m_pParticleGroup->m_vFrameWind;
    vNoInertia = transposedRot * vNoInertia;
  }

  // now move all particles
  const int iCount = iHighWaterMark; // m_pParticleGroup->GetNumOfParticles();
  iHighWaterMark = 0;
  p = m_pParticleGroup->GetParticlesExt();
  int iValidCount = 0;
  for (i=0;i<iCount;i++,p++) if (p->valid)
  {
    if (!m_pParticleGroup->HandleSingleParticle(p, fScaledTime))
      continue;
    iHighWaterMark = i+1;
    iValidCount++;
  }


  m_pParticleGroup->m_iValidCount = iValidCount;

  // spawn new particles (after handling)
  if (m_pParticleGroup->GetEmitter() && !m_pParticleGroup->m_bPaused && !m_pParticleGroup->IsLifeTimeOver())
    m_pParticleGroup->GetEmitter()->HandleEmitter(m_pParticleGroup,fScaledTime,m_pParticleGroup->m_iRemainingParticleCount);

  // handle constraints
  if (m_pParticleGroup->m_bHandleConstraints)
    m_pParticleGroup->HandleAllConstraints(m_fTimeDelta);

  // sort the particles according to camera distance if requested
  if (m_pParticleGroup->m_bSortParticles)
  {
    const VisContextCamera_cl *pCam = VisRenderContext_cl::GetMainRenderContext()->GetCamera();
    const hkvVec3& vCamPos = pCam->GetPosition();
    const hkvVec3& vCamDir = pCam->GetDirection();
    hkvVec3 vTemp;
    float fDist;
    
    ParticleSort_t *pSort;
    pSort = m_pParticleGroup->m_pIndexList;
    VASSERT(pSort);

    ParticleExt_t *particles;
    particles = m_pParticleGroup->GetParticlesExt();
    int iIndexCount = iHighWaterMark;
    if (iIndexCount>65536) iIndexCount=65536;
    for (i=0;i<iHighWaterMark;i++,pSort++)
    {
      p = particles + pSort->index;
      if (p->valid)
      {
        vTemp.x = p->pos[0] - vCamPos.x;
        vTemp.y = p->pos[1] - vCamPos.y;
        vTemp.z = p->pos[2] - vCamPos.z;
        fDist = vTemp.dot (vCamDir);
        if (fDist>65535.f) fDist=65535.f;
        pSort->sortkey = (unsigned short)fDist;
      }
    }

    qsort(m_pParticleGroup->m_pIndexList, iIndexCount, sizeof(ParticleSort_t), CompareParticles);
  }
  // if this particle group has a child, then its task should be handled in this thread
  if (m_pParticleGroup->m_spOnDestroyCreateGroup) {
    m_pParticleGroup->m_spOnDestroyCreateGroup->m_pHandlingTask->Run(pThread);
  }

  // trail particles -> connect all (needs to be performed after all constraints etc. that might modify position)
  if (m_pParticleGroup->m_cUseDistortion==DISTORTION_TYPE_TRAIL)
  {
    p = m_pParticleGroup->GetParticlesExt();
    const float fOverlap = m_pParticleGroup->m_spDescriptor->m_fTrailOverlap;
    for (i=0;i<iCount;i++) if (p[i].valid)
    {
      int iPrevIndex = (int)p[i].m_fDistortionMult; // stores the index to the previous...
      VASSERT(iPrevIndex>=0 && iPrevIndex<m_pParticleGroup->GetNumOfParticles());
      const ParticleExt_t *pOther = &p[iPrevIndex];
      if (pOther->valid) // the chain might be interrupted when particles have different lifetimes
      {
        p[i].distortion[0] = (pOther->pos[0] - p[i].pos[0]) * fOverlap;
        p[i].distortion[1] = (pOther->pos[1] - p[i].pos[1]) * fOverlap;
        p[i].distortion[2] = (pOther->pos[2] - p[i].pos[2]) * fOverlap;

        iPrevIndex = (int)pOther->m_fDistortionMult; // stores the index to the previous...
        VASSERT(iPrevIndex>=0 && iPrevIndex<m_pParticleGroup->GetNumOfParticles());
        const ParticleExt_t *pOther2 = &p[iPrevIndex];
        // in order to be able make a continuous trail we need information about the next particle as well
        if (pOther2->valid)
        {
          p[i].normal[0] = (pOther2->pos[0] - pOther->pos[0]) * fOverlap;
          p[i].normal[1] = (pOther2->pos[1] - pOther->pos[1]) * fOverlap;
          p[i].normal[2] = (pOther2->pos[2] - pOther->pos[2]) * fOverlap;
        }

      }
    }
  }

  // update internal bounding box:
  float fInterval = m_pParticleGroup->m_spDescriptor->m_fDynamicInflateInterval;
  if (fInterval>=0.f)
  {
    m_pParticleGroup->m_fBBoxUpdateTimePos += m_fTimeDelta;
    if (m_pParticleGroup->m_fBBoxUpdateTimePos>=fInterval)
    {
      if (fInterval>=0.f)
        m_pParticleGroup->m_fBBoxUpdateTimePos = hkvMath::mod (m_pParticleGroup->m_fBBoxUpdateTimePos,fInterval);
      m_pParticleGroup->m_BoundingBox.setInvalid();
      m_pParticleGroup->InflateBoundingBox(true);
      m_pParticleGroup->m_bVisibilityUpdate |= m_pParticleGroup->m_bBBoxValid; // set new bbox in main thread
    }
  }

  // make sure the bounding box is up-to-date
  const hkvAlignedBBox *pBBox = m_pParticleGroup->GetCurrentBoundingBox();
}



///////////////////////////////////////////////////////////////////////////////////
// Particle group base
///////////////////////////////////////////////////////////////////////////////////


void ParticleGroupBase_cl::InitGroup(bool bSpawnParticles, int iGeneration)
{
  UpdateSeed(); // For Initializing random values using seed when particle created

  m_iChildIndex = 0;
  m_bInertiaAffectsGravity = true;
  m_pColorLookup = NULL;
  m_iGeneration = iGeneration;
  m_bPaused = m_bHalted = m_bIsDead = false;
  m_bUpdateLifetimeIfInvisible = false;
  m_fColorBitmapSizeX  = m_fColorBitmapSizeY = 0.f;
  m_bHandleConstraints = true;
  m_bRenderConstraints = false;
  m_bDistortionPlaneAligned = false;
  m_bBBoxValid = false;
  m_bVisibilityUpdate = true;
  m_bAttachedToCam = false;
  m_bRepeatLifetime = false;
  m_fInitialDelay = 0.f;
  m_fTransformationCurveTime = m_fTransformationCurveSpeed = 0.f;
  m_MeshBoundingBox.setZero();
  m_eTopology = PARTICLE_TOPOLOGY_BILLBOARDS;
  m_iTrailIndex = 0;
  m_fLastTimeOfDayUpdate = -1.0f;
  m_bApplyTimeOfDayLight = true;

  m_iCachedParticleCount = 0;
  m_iMaxAnimFrame = 0;
  m_iMaxCachedParticleCount = PARTICLECACHESIZE_NOALLOC;
  if (m_iCachedParticleNoAlloc!=m_piCachedParticle)
      V_SAFE_DELETE_ARRAY(m_piCachedParticle);
  m_piCachedParticle = m_iCachedParticleNoAlloc;
  SetLocalFactors(0.f,0.f); // not at all in local space

  m_fBBoxUpdateTimePos = 0.f;
  m_bHandleWhenVisible = false; // always handle
  m_iValidCount = 0;
  m_iConstraintAffectBitMask = 0xffffffff;
  m_bHasTransformationCurves = m_bHasEvents = false;

  // modify properties from descriptor
  OnDescriptorChanged();

  VisParticleEmitter_cl *pEmitter = GetEmitter();
  VASSERT(pEmitter || m_pParentGroup);

  m_iRemainingParticleCount = -1;
  int iParticleCount = 0;
  float fPps; // particles per second
  if (m_pParentGroup) 
  {
    // if this group belongs to a parent group, calc the particle count in a different way:
    // particles per second from parent group * number of particles to respawn * lifetime of particles in this group
    fPps = m_pParentGroup->GetEmitter()->m_ParticlesPerSec.m_fAverage;
    float fRespawn = m_pParentGroup->m_spDescriptor->m_OnDestroyCreateCount.m_fAverage;

    iParticleCount = m_spDescriptor->GetEstimatedMaxParticleCount(-1.f, fPps*fRespawn) + 8; // leave extra margin
    m_iRemainingParticleCount = 0; // do not spawn particles on own
  } 
  else
  {
    VASSERT(pEmitter);
    fPps = pEmitter->m_ParticlesPerSec.m_fAverage;
    iParticleCount = m_spDescriptor->GetEstimatedMaxParticleCount(-1.f,fPps) + 8; // leave extra margin
    if (pEmitter->m_FixParticleCount.IsDefined())
    {
      m_iRemainingParticleCount = (int)pEmitter->m_FixParticleCount.GetRandomValue(GetRandom());  
      if (m_iRemainingParticleCount<1) m_iRemainingParticleCount = 1;
      if (m_iRemainingParticleCount<iParticleCount)
        iParticleCount = m_iRemainingParticleCount;
    }
  }    
  // init the particle array
  VASSERT(iParticleCount>0);
  SetParticleStride(sizeof(ParticleExt_t));
  Init( 0, iParticleCount);
  InitParticleIndexList(m_bSortParticles);

  // init cache and estimate how much we need
  // there is no reason to scale it with the overall number particles, as long as we have aenough particles per frame
  int iWantedCacheSize = (int)(fPps*0.08f); // (fPps*0.08f) is number of spawned p at low fps
  if (iWantedCacheSize<PARTICLECACHESIZE_NOALLOC)
    iWantedCacheSize = PARTICLECACHESIZE_NOALLOC;
  else if (iWantedCacheSize>512)
    iWantedCacheSize = 512;

  m_iCachedParticleCount = 0;
  if (iWantedCacheSize>m_iMaxCachedParticleCount)
  {
    if (m_iCachedParticleNoAlloc!=m_piCachedParticle)
      V_SAFE_DELETE_ARRAY(m_piCachedParticle);
    if (iWantedCacheSize>PARTICLECACHESIZE_NOALLOC)
      m_piCachedParticle = new unsigned short[iWantedCacheSize];
    else 
      m_piCachedParticle = m_iCachedParticleNoAlloc;
  }
  m_iMaxCachedParticleCount = iWantedCacheSize;


  // texture/transparency properties
  char szTexFile[FS_MAX_PATH];
  if (m_spDescriptor->m_sTextureFilename.IsEmpty())
    SetTextureObject(Vision::TextureManager.GetPlainWhiteTexture());
  else
  {
    // make the texture filename relative to XML filename
    m_spDescriptor->MakeFilenameAbsolute(szTexFile,m_spDescriptor->m_sTextureFilename);
    SetTexture(szTexFile);
  }

  if (!m_spDescriptor->m_sNormalmapFilename.IsEmpty())
  {
    m_spDescriptor->MakeFilenameAbsolute(szTexFile,m_spDescriptor->m_sNormalmapFilename);
    SetNormalmap(Vision::TextureManager.Load2DTexture(szTexFile));
  }
  if (!m_spDescriptor->m_sSpecularmapFilename.IsEmpty())
  {
    m_spDescriptor->MakeFilenameAbsolute(szTexFile,m_spDescriptor->m_sSpecularmapFilename);
    SetNormalmap(Vision::TextureManager.Load2DTexture(szTexFile));
  }

  SetTransparencyType(m_spDescriptor->m_eTransp);
  SetAnimationSubdivisions(m_spDescriptor->m_iAnimSubDiv[0],m_spDescriptor->m_iAnimSubDiv[1]);
  SetUseSmoothAnimationFading(m_spDescriptor->m_bUseSmoothAnimation);
  SetParticleSizeAspectRatio(m_spDescriptor->m_fSizeAspect);
  SetAlwaysInForeGround(m_spDescriptor->m_bAlwaysInForeground);
  SetObjectFlag(VObjectFlag_AutoDispose); // so the particle effect layer gets deleted automatically

  ////////////////////////////////////////////////////////
  // fill the group with initial count
  ////////////////////////////////////////////////////////
  if (bSpawnParticles)
    RespawnAllParticles(false);

  if (!m_pHandlingTask)
    m_pHandlingTask = new HandleParticlesTask_cl(this);

  //VASSERT(GetVisibilityObject());
}


ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, unsigned int uiRandomSeed) :
  IVPhysicsParticleCollection_cl(uiRandomSeed)
{
  VASSERT(pDescr);
  m_fTransformationCurveTime = 0.f; // need to be set before SetScaling
  m_fScaling = -1.f; // force update in SetScaling
  m_pHandlingTask = NULL;
  m_vOldPos = vSpawnPos;
  m_pParentGroup = NULL;
  m_pParentEffect = NULL;
  m_spDescriptor = pDescr;
  m_piCachedParticle = NULL;
  SetPosition(vSpawnPos);
  SetOrientation(0.f,0.f,0.f);
  SetScaling(1.f);
  SetEmitter(pEmitter);
  m_pEmitterMeshEntity = NULL;
  m_AmbientColor.SetRGBA(0,0,0,0);
  SetWindSpeed(hkvVec3::ZeroVector (), false);
  InitGroup(true);
}

ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, bool bSpawnParticles, unsigned int uiRandomSeed) :
  IVPhysicsParticleCollection_cl(uiRandomSeed)
{
  VASSERT(pDescr);
  m_fTransformationCurveTime = 0.f; // need to be set before SetScaling
  m_fScaling = -1.f; // force update in SetScaling
  m_pHandlingTask = NULL;
  m_vOldPos = vSpawnPos;
  m_pParentGroup = NULL;
  m_pParentEffect = NULL;
  m_spDescriptor = pDescr;
  m_piCachedParticle = NULL;
  m_AmbientColor.SetRGBA(0,0,0,0);
  SetPosition(vSpawnPos);
  SetOrientation(vOrientation.x,vOrientation.y,vOrientation.z);
  SetScaling(1.f);
  SetEmitter(pEmitter);
  m_pEmitterMeshEntity = NULL;
  SetWindSpeed(hkvVec3::ZeroVector (), false);
  InitGroup(bSpawnParticles);
}

ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, float fScaling, bool bSpawnParticles, unsigned int uiRandomSeed) :
  IVPhysicsParticleCollection_cl(uiRandomSeed)
{
  VASSERT(pDescr);
  m_fTransformationCurveTime = 0.f; // need to be set before SetScaling
  m_fScaling = -1.f; // force update in SetScaling
  m_pHandlingTask = NULL;
  m_vOldPos = vSpawnPos;
  m_pParentGroup = NULL;
  m_pParentEffect = NULL;
  m_spDescriptor = pDescr;
  m_AmbientColor.SetRGBA(0,0,0,0);
  m_piCachedParticle = NULL;
  SetPosition(vSpawnPos);
  SetOrientation(vOrientation.x,vOrientation.y,vOrientation.z);
  SetScaling(fScaling);
  SetEmitter(pEmitter);
  m_pEmitterMeshEntity = NULL;
  SetWindSpeed(hkvVec3::ZeroVector (), false);
  InitGroup(bSpawnParticles);
}



ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, ParticleGroupBase_cl *pParent, int iGeneration, unsigned int uiRandomSeed) :
  IVPhysicsParticleCollection_cl(uiRandomSeed)
{
  VASSERT(pDescr);
  VASSERT(pParent);
  m_fTransformationCurveTime = 0.f; // need to be set before SetScaling
  m_fScaling = -1.f; // force update in SetScaling
  m_AmbientColor.SetRGBA(0,0,0,0);
  m_pHandlingTask = NULL;
  m_pParentEffect = NULL;
  m_pParentGroup = pParent;
  m_spDescriptor = pDescr;
  m_piCachedParticle = NULL;
  SetScaling(pParent->GetScaling());
  SetWindSpeed(hkvVec3::ZeroVector (), false);
  m_pEmitterMeshEntity = NULL;
  InitGroup(false,iGeneration);
}


void ParticleGroupBase_cl::RemoveUpdaterTaskRecursive(ParticleGroupBase_cl *pGroup)
{
  if (pGroup==NULL)
    return;
  if (pGroup->GetUpdateTask()==this->m_pHandlingTask)
    pGroup->SetUpdateTask(NULL);
  RemoveUpdaterTaskRecursive(pGroup->m_spOnDestroyCreateGroup);
}


ParticleGroupBase_cl::~ParticleGroupBase_cl()
{
  EnsureUpdaterTaskFinished();
  RemoveUpdaterTaskRecursive(m_spOnDestroyCreateGroup); // make sure nothing else references this task
  V_SAFE_DELETE(m_pHandlingTask);
  if (m_iCachedParticleNoAlloc!=m_piCachedParticle)
    V_SAFE_DELETE_ARRAY(m_piCachedParticle);
}

void ParticleGroupBase_cl::ReassignShader(bool bRecreateFX)
{
  bool bTrackLightGrid = false;
  VisEffectConfig_cl &fxConfig = m_spDescriptor->m_CustomEffectConfig;
  if (bRecreateFX)
    fxConfig.ReAssignEffect();

  VCompiledEffect *pFX = fxConfig.GetEffect();
#if !defined(_VISION_MOBILE)
  // assign default effect for geometry particles here
  if (GetGeometry()!=NULL && pFX==NULL)
  {
    VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("\\Shaders\\GeometryParticles.ShaderLib",SHADERLIBFLAG_HIDDEN);
    // choose depth write automatically when the transparency type suggests this
    bool bOpaque = GetTransparencyType()==VIS_TRANSP_NONE || GetTransparencyType()==VIS_TRANSP_ALPHATEST;
    char szParams[128];
    sprintf(szParams,"DepthWrite=%s",bOpaque ? "true":"false");
    pFX = Vision::Shaders.CreateEffect("GeometryParticles",szParams,EFFECTCREATEFLAG_NONE,pLib);
  } 
  else if (m_eTopology==PARTICLE_TOPOLOGY_TRAIL && pFX==NULL)
  {
    VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("\\Shaders\\TrailParticles.ShaderLib",SHADERLIBFLAG_HIDDEN);
    pFX = Vision::Shaders.CreateEffect("TrailParticles",NULL,EFFECTCREATEFLAG_NONE,pLib);
  }
  else if (m_spDescriptor->m_bSoftParticles && pFX==NULL)
  {
    // soft particles only work when a renderer node is set
    VRendererNodeCommon* pRenderer = NULL;
    if ((pRenderer = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(0))) &&
        (pRenderer->GetSupportedBufferFlags() & VBUFFERFLAG_DEPTH) != 0)
    {
      VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("\\Shaders\\SoftParticles.ShaderLib",SHADERLIBFLAG_HIDDEN);
      pFX = Vision::Shaders.CreateEffect("SoftParticles",NULL,EFFECTCREATEFLAG_NONE,pLib);
    }
    else
    {
      Vision::Error.Warning("Particle layer \"%s\": Soft particles aren't supported by the current renderer node.", m_spDescriptor->GetName());
    }
  }
#endif

  // get technique
  if (pFX!=NULL)
  {
    // select the right sub-technique from the effect - smooth animation and hardware spanning flag
    int iFlags = GetHardwareSpanning() ? PARTICLESHADERFLAGS_HARDWARESPANNING : PARTICLESHADERFLAGS_NONE;
    if (GetUseSmoothAnimationFading())
      iFlags |= PARTICLESHADERFLAGS_SMOOTHANIMATION;
#ifndef SUPPORTS_ALPHATEST_BLENDSTATE
    if (GetTransparencyType() == VIS_TRANSP_ALPHATEST)
      iFlags |= PARTICLESHADERFLAGS_ALPHATESTINSHADER;
#endif

    // create effect config
    VTechniqueConfig config;
    Vision::RenderLoopHelper.GetParticleEffectConfig((ParticleShaderFlags_e)iFlags, config);

    // find and set technique
    VCompiledTechnique *pTech = pFX->FindCompatibleTechnique(&config);
    if (pTech==NULL) 
      pTech = pFX->GetDefaultTechnique();
    SetTechnique(pTech);
    bTrackLightGrid = VisParticleGroupDescriptor_cl::TechniqueUsesLightGrid(pTech);
  }
  else
  {
    // select the default particles shader
    SetTechnique(NULL);
    VCompiledShaderPass *pShader = GetDefaultShader();
    if (pShader)
    {
      int iTrackingMask = pShader->GetRenderState()->GetTrackingMask();
      bTrackLightGrid = (iTrackingMask & 
        (VSHADER_TRACKING_LIGHTGRID_PS | VSHADER_TRACKING_LIGHTGRID_VS | VSHADER_TRACKING_LIGHTGRID_GS)) > 0;
    }
  }

  if (bTrackLightGrid)
  {
    SetLightGridColorPtr(m_pParentEffect->m_OwnLGColors);
    m_pParentEffect->m_bUseLightgrid = true;
  }
  else
  {
    SetLightGridColorPtr(NULL);
  }

}

void ParticleGroupBase_cl::Finalize()
{
  ReassignShader(false);
  SetInitialTransformation();
}


float ParticleGroupBase_cl::GetSortingDistance(const VisObject3D_cl *pCamera)
{
  hkvVec3 vPos(GetParentEffect()==NULL ? GetPosition() : GetParentEffect()->GetPosition());
  hkvVec3 vDiff = (hkvVec3) vPos - pCamera->GetPosition(); // all particle layers should use same pivot for sorting
  float fDist = vDiff.getLength();
  fDist -= (float)m_iChildIndex*Vision::World.GetGlobalUnitScaling(); // there should be a well defined order between particle layers
  return fDist;
}



void ParticleGroupBase_cl::SetInitialTransformation()
{
  if (m_spDescriptor->m_bUseTransformationCurve)
  {
    float x = m_fTransformationCurveTime;
    if (m_spDescriptor->m_spPositionCurve!=NULL)
      m_vLocalPosition = m_spDescriptor->m_spPositionCurve->GetValueFast(x) * m_fScaling;
    if (m_spDescriptor->m_spOrientationCurve!=NULL)
    {
      hkvVec3 vLocalYPR = m_spDescriptor->m_spOrientationCurve->GetValueFast(x);
      vLocalYPR *= 360.f;

      m_localOrientation.setFromEulerAngles (vLocalYPR.z, vLocalYPR.y, vLocalYPR.x);
    }
  } else
  {
    m_vLocalPosition = m_spDescriptor->m_vRelativePosition * m_fScaling;
    m_localOrientation.setFromEulerAngles (m_spDescriptor->m_vRelativeOrientation.z, m_spDescriptor->m_vRelativeOrientation.y, m_spDescriptor->m_vRelativeOrientation.x);
  }
  UpdateBinding();
}

void ParticleGroupBase_cl::OnDescriptorChanged()
{
  ////////////////////////////////////////////////
  // default emitter
  ////////////////////////////////////////////////
  if (!GetEmitter())
    SetEmitter(m_spDescriptor->GetDefaultEmitter()->Clone(GetRandom()));

  //////////////////////////////////////////////
  // animation
  //////////////////////////////////////////////
  m_fAnimFrameCount = (float)m_spDescriptor->m_iUsedAnimFrames;
  m_iMaxAnimFrame = m_spDescriptor->m_iUsedAnimFrames-1;
  m_eParticleAnimMode = m_spDescriptor->m_eParticleAnimMode;

  //////////////////////////////////////////////
  // geometry
  //////////////////////////////////////////////
  m_eTopology = m_spDescriptor->m_eTopology;
  SetGeometry(m_spDescriptor->m_spGeometry);
  m_MeshBoundingBox = m_spDescriptor->m_MeshBoundingBox;

  //////////////////////////////////////////////
  // rotation
  //////////////////////////////////////////////
  m_eRotationMode = m_spDescriptor->m_eRotationMode;
  m_spRotationCurve = (m_eRotationMode==PARTICLE_ROTATION_ANGLECURVE) ? m_spDescriptor->m_spRotationCurve : NULL;

  //////////////////////////////////////////////
  // lookup curves from descriptor
  //////////////////////////////////////////////
  m_spColorLookup = m_spDescriptor->m_spColorLookup;
  m_pColorLookup = NULL;
  if (m_spColorLookup)
  {
    m_spColorLookup->RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD); // don't unload while we have m_pColorLookup
    m_fColorBitmapSizeX = (float)m_spColorLookup->GetWidth();
    m_fColorBitmapSizeY = (float)m_spColorLookup->GetHeight();
    m_pColorLookup = m_spColorLookup->GetDataPtr();
  }
  m_spSizeCurve = m_spDescriptor->m_spSizeCurve;
  m_spAnimCurve = m_spDescriptor->m_spAnimCurve;

  //////////////////////////////////////////////
  // other parameters from descriptor
  //////////////////////////////////////////////

  // using lightgrid overrides the simple modulation color
  m_bEvaluateBrightnessNextFrame = (GetLightGridColorPtr()==NULL) && (m_spDescriptor->m_fApplySceneBrightness > 0.0f);

  m_InstanceColor.SetRGBA(255,255,255,255);
  m_iConstraintAffectBitMask = m_spDescriptor->m_iConstraintAffectBitMask;

  m_fLifeTime = m_spDescriptor->GetRandomGroupLifeTime(GetRandom());
  m_fTimeScale = m_spDescriptor->m_TimeScaling.GetRandomValue(GetRandom());
  m_fInitialDelay = m_spDescriptor->m_InitialDelay.GetRandomValue(GetRandom());
  m_bRepeatLifetime = m_spDescriptor->m_bRepeatLifetime;

  m_bInfiniteLifeTime = (m_fLifeTime<0.f) || (m_pParentGroup!=NULL);
  if (!m_pParentGroup && GetEmitter()->m_FixParticleCount.IsDefined())
  {
    m_iRemainingParticleCount = (int)GetEmitter()->m_FixParticleCount.GetRandomValue(GetRandom());  
    if (m_iRemainingParticleCount<1) m_iRemainingParticleCount = 1;
  }
  m_bInertiaAffectsGravity = m_spDescriptor->m_bInertiaAffectsGravity;

  // fading
  SetFadeMode(m_spDescriptor->m_eFadeMode);
  FadeDistancesFromDesc();

  // start at random time position past m_fDynamicInflateInterval to invoke the first update of the bounding box immediately.
  m_fBBoxUpdateTimePos = (1.0f + GetRandom().GetFloat()) * m_spDescriptor->m_fDynamicInflateInterval;
  m_bBBoxValid = m_spDescriptor->m_BoundingBox.isValid();
  if (m_bBBoxValid)
  {
    if (m_spDescriptor->m_bLocalSpace)
    {
      m_BoundingBox = m_spDescriptor->m_BoundingBox;
    } else
    {
      m_BoundingBox.setInvalid();

      hkvAlignedBBox temp = m_spDescriptor->m_BoundingBox;
      temp.transformFromOrigin (hkvMat4 (m_cachedRotMatrix,m_vPosition));
      m_BoundingBox.expandToInclude (temp);
    }
    VisParticleGroup_cl::SetBoundingBox(m_BoundingBox);
  }

  m_bIsDead = false;
  m_bVisibilityUpdate = true;
  m_bHandleWhenVisible = m_spDescriptor->m_bHandleWhenVisible;


  // local positioning:
  // 0% : no local space at all
  // 50% : in local space at particle lifetime start but none at end
  // 100% : fully in local space
  float fAtStart = hkvMath::Min(m_spDescriptor->m_fApplyEmitterSpeed*2.f, 1.f);
  float fAtEnd = hkvMath::Max(m_spDescriptor->m_fApplyEmitterSpeed*2.f-1.f, 0.f);
  SetLocalFactors(fAtStart,fAtEnd);
  SetUseLocalSpaceMatrix(m_spDescriptor->m_bLocalSpace); // real local space?

  // distortion & normal
  m_bDistortionPlaneAligned = m_spDescriptor->m_bDistortionPlaneAligned;

  if (m_spDescriptor->m_bDistorted)
  {
    if (m_spDescriptor->m_bDistortionSizeMode)
    {
      m_cUseDistortion = DISTORTION_TYPE_SIZEMODE;
      m_vSizeMultiplier = m_spDescriptor->m_vSizeMultiplier;
    }
    else if (m_spDescriptor->m_FixDistortionLength.IsDefined())
    {
      m_cUseDistortion = DISTORTION_TYPE_FIXLEN;
    }
    else
    {
      m_cUseDistortion = DISTORTION_TYPE_VELOCITY;
    }
  }
  else
  {
    m_cUseDistortion = 0;
  }
  if (m_eTopology==PARTICLE_TOPOLOGY_TRAIL)
  {
    m_cUseDistortion = DISTORTION_TYPE_TRAIL;
    SetParticleCenter(0.f,0.5f);
  }else
  {
    const hkvVec2 &vCenter(m_spDescriptor->m_vParticleCenter);
    SetParticleCenter(vCenter.x,vCenter.y);
  }
  SetDepthOffset(m_spDescriptor->m_fDepthOffset);

  // normal
  m_bUseNormals = m_spDescriptor->m_bUseNormal || m_bDistortionPlaneAligned;


  // sorting (upon initilization, the index list for sorting is initialized in the InitGroup function)
  m_bSortParticles = m_spDescriptor->m_bSortParticles;
  InitParticleIndexList(m_bSortParticles);

  // render order - under certain circumstances, force a specific order constant
  #if defined(WIN32) || defined(_VR_GLES2)
    const bool bQuarterSizeParticles = false;

  #else
    // On consoles, get quarter size particle mode from the translucency render loop
    bool bQuarterSizeParticles = false;

    if (VRendererNodeCommon* pRendererNode = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(0)))
    {
      if (VPostProcessTranslucencies* pTranslucencies = pRendererNode->Components().GetComponentOfBaseType<VPostProcessTranslucencies>())
      {
        bQuarterSizeParticles = pTranslucencies->IsQuarterSizeParticles();
      }
    }

  #endif

  ANALYSIS_IGNORE_WARNING_ONCE(6237)
  if (bQuarterSizeParticles && m_spDescriptor->m_iRenderOrder==VRH_PARTICLES && m_spDescriptor->m_eTransp==VIS_TRANSP_ADDITIVE)
  {
    SetRenderOrder(VRH_ADDITIVE_PARTICLES);
  }
  else
    SetRenderOrder((unsigned int)m_spDescriptor->m_iRenderOrder);

  // group sorting key
  SetUserSortKey(m_spDescriptor->m_iGroupSortKey);

  ////////////////////////////////////////////////
  // create a new group for destroying particles
  ////////////////////////////////////////////////

  if (m_spDescriptor->m_OnDestroyCreateCount.IsDefined() && m_iGeneration<MAX_ONDESTROY_GROUPDEPTH)
  {
    VisParticleGroupDescriptor_cl *pDestroyDesc = m_spDescriptor->m_spDestroyCreateDesc;
    if (pDestroyDesc)
    {
      m_spOnDestroyCreateGroup = new ParticleGroupBase_cl(pDestroyDesc,this,m_iGeneration+1, GetBaseSeed());
      m_spOnDestroyCreateGroup->ReassignShader(false);
    }
  }

  // transformation curves
  m_bHasTransformationCurves = m_spDescriptor->m_bUseTransformationCurve && (m_spDescriptor->m_spPositionCurve!=NULL || m_spDescriptor->m_spOrientationCurve!=NULL);
  float fTransformTime = m_spDescriptor->m_fTransformationCurveTime;
  if (fTransformTime<=HKVMATH_LARGE_EPSILON)
  {
    fTransformTime = m_fLifeTime;
    if (fTransformTime<=HKVMATH_LARGE_EPSILON) fTransformTime = 1.f;
  }
  m_fTransformationCurveSpeed = 1.f / fTransformTime;

  // emitter relative positioning
  SetInitialTransformation();

  // events
  m_bHasEvents = m_spDescriptor->m_EventList.GetEventCount()>0;
}

void ParticleGroupBase_cl::FadeDistancesFromDesc()
{
  SetCustomFadeRange(m_spDescriptor->m_fFadeStart * VisParticleGroupManager_cl::g_fGlobalFadeScaling, m_spDescriptor->m_fFadeEnd * VisParticleGroupManager_cl::g_fGlobalFadeScaling);
}


#ifdef SUPPORTS_SNAPSHOT_CREATION

void ParticleGroupBase_cl::GetDependencies(VResourceSnapshot &snapshot)
{
  VisParticleGroup_cl::GetDependencies(snapshot);

  // TODO: textures from the descriptor...
}

#endif

void ParticleGroupBase_cl::ModSysNotifyFunctionCommand(int command)
{
  if (command==VIS_MODSYSCMD_RECOMPUTEVISIBILITY)
  {
    if (GetVisibilityObject())
      GetVisibilityObject()->ReComputeVisibility();
  }
}


int ParticleGroupBase_cl::CountActiveParticles() const
{
  const int iParticleCount = m_iHighWaterMark;
  const ParticleExt_t *p = GetParticlesExt();
  int iFound = 0;
  for (int i=0;i<iParticleCount;i++,p++)
    if (p->valid) iFound++;

  return iFound;
}

void ParticleGroupBase_cl::TeleportSpawnPosition()
{
  if (!m_spEmitter)
    return;

  // Force emitter to current particle group position, without any interpolation
  m_spEmitter->m_vLastEmitterPos = m_vPosition;
  m_spEmitter->m_fParticleAge = 1.0f;
}

void ParticleGroupBase_cl::RespawnAllParticles(bool bUseOldCount)
{
  int iParticleCount = GetNumOfParticles();
  if (iParticleCount<1) return;

  if (!m_spEmitter) return;

  // no interpolation of position
  m_spEmitter->m_vLastEmitterPos = m_vPosition;
  m_spEmitter->m_fParticleAge = 1.0f;

  int iStartupCount = 0;

  // either use same number as currently active or determine startup count
  if (bUseOldCount)
    iStartupCount = CountActiveParticles();
  else if (hkvMath::isZero(m_fInitialDelay))
  {
    float fPps = m_spEmitter->m_ParticlesPerSec.m_fAverage;
    int iAllParticles = m_spDescriptor->GetEstimatedMaxParticleCount(-1.f,fPps);
    iStartupCount = (int)(m_spEmitter->m_StartupFillPercentage.GetRandomValue(GetRandom())*(float)iAllParticles*(1.f/99.9f)); // avoid rounding issues
  }

  if (iStartupCount>iParticleCount)
    iStartupCount = iParticleCount;
  else if (iStartupCount<0) iStartupCount = 0;

  m_iCachedParticleCount = 0;
  ParticleExt_t *pParticle = GetParticlesExt(); // particle array is still empty!
  ParticleExt_t *pArray = pParticle;
  memset(pParticle,0,iParticleCount*sizeof(ParticleExt_t));

  m_iHighWaterMark = m_iValidCount = iStartupCount;
  m_vGroupMoveDelta.setZero();
  m_vGroupMoveDeltaAccum.setZero(); // all particles start at right position...

  for (int i=0;i<iStartupCount;i++,pParticle++)
  {
    VASSERT(!pParticle->valid);
    InitSingleParticle(pParticle); // asserts when index>=m_iHighWaterMark 
    float fLifeTimeInc = pParticle->m_fLifeTimeInc;
    if (i>0 && fLifeTimeInc>0.f)
    {
      const int iIterations = 8;
      float dtime = ((float)i)/(fLifeTimeInc*(float)iIterations * (float)iStartupCount);
      // setup per frame variables
      SetPerFrameConstants(dtime);
      for (int j=0;j<iIterations;j++)
        HandleSingleParticle(pParticle,dtime);
    } else
    {
      SetPerFrameConstants(0.f);
      HandleSingleParticle(pParticle,0.f);
    }
  }
  
  HandleAllConstraints(0.f);

  if (m_cUseDistortion==DISTORTION_TYPE_TRAIL)
  {
    pParticle = pArray;
    for (int i=0;i<iStartupCount;i++,pParticle++)
    {
      int iPrevIndex = hkvMath::Max(i-1,0);
      pParticle->m_fDistortionMult = (float)iPrevIndex +0.1f; // stores the index to the previous...
      const ParticleExt_t *pOther = &pArray[iPrevIndex];
      pParticle->distortion[0] = pOther->pos[0] - pParticle->pos[0];
      pParticle->distortion[1] = pOther->pos[1] - pParticle->pos[1];
      pParticle->distortion[2] = pOther->pos[2] - pParticle->pos[2];
      // in order to be able make a continuous trail we need information about the next particle as well
      iPrevIndex = hkvMath::Max(i-2,0);
      const ParticleExt_t *pOther2 = &pArray[iPrevIndex];
      pParticle->normal[0] = pOther2->pos[0] - pOther->pos[0];
      pParticle->normal[1] = pOther2->pos[1] - pOther->pos[1];
      pParticle->normal[2] = pOther2->pos[2] - pOther->pos[2];

    }
    m_iTrailIndex = iStartupCount-1;
  }

}


void ParticleGroupBase_cl::IncPosition(const hkvVec3& vDelta)
{
  SetPosition(GetPosition()+vDelta);
}

//TODO
hkvVec3 ParticleGroupBase_cl::GetDirection()
{
  return m_vDirection;
}

void ParticleGroupBase_cl::MoveParticles(const hkvVec3& vDelta)
{
  EnsureUpdaterTaskFinished();

  const int iParticleCount = m_iHighWaterMark;
  int i;
  ParticleExt_t *p = GetParticlesExt();
  for (i=0;i<iParticleCount;i++,p++) if (p->valid)
  {
    p->pos[0] += vDelta.x;
    p->pos[1] += vDelta.y;
    p->pos[2] += vDelta.z;
  }

  if (m_spEmitter!=NULL)
    m_spEmitter->m_vLastEmitterPos += vDelta;
  m_vOldPos += vDelta;
}

void ParticleGroupBase_cl::SetLocalFactors(float fAtLifetimeStart, float fAtLifetimeEnd)
{
  m_fLocalFactorStart = fAtLifetimeStart;
  m_fLocalFactorDiff = fAtLifetimeEnd-fAtLifetimeStart;
  m_bMovesWithEmitter = fAtLifetimeStart>0.f || fAtLifetimeEnd>0.f;
}

 
void ParticleGroupBase_cl::UpdateVisibilityObject()
{
  m_bVisibilityUpdate = false;
  const hkvAlignedBBox *pBBox = GetCurrentBoundingBox(); 

  if (!pBBox) // no valid visibility box
  {
    //VisParticleGroup_cl::SetCheckVisibility(FALSE); // this destroys it...
    VisVisibilityObject_cl *pVisObj = GetVisibilityObject();
    if (pVisObj)
      pVisObj->SetActivate(FALSE);
    return;
  }
  VASSERT(pBBox->isValid());
  VisParticleGroup_cl::SetBoundingBox(*pBBox); // enables visibility
  GetVisibilityObject()->SetActivate(TRUE);
}


void ParticleGroupBase_cl::CopyParentPosition()
{
  VASSERT(m_pParentGroup);
  SetPosition(m_pParentGroup->GetPosition());
  SetOrientation(m_pParentGroup->GetRotationMatrix());
}


void ParticleGroupBase_cl::SetAmbientColor(VColorRef iColor)
{
  // clamp to white
  iColor.r = (UBYTE) (hkvMath::Min (iColor.r, 255));
  iColor.g = (UBYTE) (hkvMath::Min (iColor.g, 255));
  iColor.b = (UBYTE) (hkvMath::Min (iColor.b, 255));

  if (m_spOnDestroyCreateGroup) 
    m_spOnDestroyCreateGroup->SetAmbientColor(iColor);

  m_AmbientColor = iColor;

  // update the final color
  EvaluateSceneBrightness ();
}


void ParticleGroupBase_cl::EvaluateSceneBrightness()
{
  // reset this state
  m_bEvaluateBrightnessNextFrame = false;

  if (m_spDescriptor->m_fApplySceneBrightness <= 0.0f)
  {
    // if scene brightness should not be used, just pass through the ambient color
    m_InstanceColor.SetRGB (255, 255, 255);;
  }
  else
  {
    VColorRef SceneBrightnessColor;

    // init scene color with the default lighting color
    SceneBrightnessColor = Vision::Renderer.GetDefaultLightingColor();

    VLightGrid_cl* pLightGrid = GetParentEffect()->GetRelevantLightGrid();
    if (pLightGrid)
    {
      const hkvVec3 evalPos = GetPosition();
      const hkvVec3 vLGPos (evalPos.x, evalPos.y, evalPos.z);

      hkvVec3 destColor;

      // get the light grid color, depending on the type of lightgrid
      if (pLightGrid->GetLightGridType() == VLIGHTGRIDTYPE_6COLORS)
      {
        pLightGrid->GetMaximumColorAtPositionI(vLGPos, destColor, 0xFF);
      }
      else
      {
        hkvVec3 tmpColor[3];
        pLightGrid->GetColorsAtPositionI(vLGPos, tmpColor, 0xFF);
        destColor = tmpColor[0];
      }

      destColor *= 255.0f;

      // clamp color to white
      {
        destColor.x = hkvMath::Min (255.0f, destColor.x);
        destColor.y = hkvMath::Min (255.0f, destColor.y);
        destColor.z = hkvMath::Min (255.0f, destColor.z);
      }

      // set the light-grid color as the basic light color
      SceneBrightnessColor.SetRGBA ((UBYTE) destColor.x, (UBYTE) destColor.y, (UBYTE) destColor.z, 255);
    }

    IVTimeOfDay* ToD = Vision::Renderer.GetTimeOfDayHandler();

    // init the current ambient with black
    VColorRef CurrentAmbient (0, 0, 0);

    // Time-of-Day Brightness
    if ((m_bApplyTimeOfDayLight) && (ToD != NULL))
    {
      // store when we last updated the time-of-day stuff
      m_fLastTimeOfDayUpdate = ToD->GetDayTime ();
      CurrentAmbient = ToD->GetAmbientColor() + ToD->GetSunColor(); // heuristic to determine ToD contribution
    }

    // take the maximum of the current own light value (light-grid) and the current ambient light (time-of-day OR scene-ambient)
    {
      SceneBrightnessColor.r = hkvMath::Max (SceneBrightnessColor.r, CurrentAmbient.r);
      SceneBrightnessColor.g = hkvMath::Max (SceneBrightnessColor.g, CurrentAmbient.g);
      SceneBrightnessColor.b = hkvMath::Max (SceneBrightnessColor.b, CurrentAmbient.b);
    }

    // add the entities ambient color to the final scene brightness
    {
      SceneBrightnessColor.r = hkvMath::Min ((int) SceneBrightnessColor.r + (int) m_AmbientColor.r, 255);
      SceneBrightnessColor.g = hkvMath::Min ((int) SceneBrightnessColor.g + (int) m_AmbientColor.g, 255);
      SceneBrightnessColor.b = hkvMath::Min ((int) SceneBrightnessColor.b + (int) m_AmbientColor.b, 255);
    }

    // interpolate the final color between the own ambient and the scene brightness, depending on how much scene brightness should be used
    m_InstanceColor = VColorRef (255, 255, 255) * (1.0f - m_spDescriptor->m_fApplySceneBrightness) + 
                      SceneBrightnessColor * m_spDescriptor->m_fApplySceneBrightness;

    // no need to clamp the instance color, m_AmbientColor and SceneBrightnessColor are already clamped
  }
}


bool ParticleGroupBase_cl::FillFreeParticleCache()
{
  // reset the cache
  m_iCachedParticleCount = 0;
  const int iParticleCount = GetNumOfParticles();
  int i;
  ParticleExt_t *p = GetParticlesExt();
  for (i=0;i<iParticleCount;i++,p++)
    if (!p->valid)
    {
	  // Fill backwards so we get the lowest indices first in the cache
      m_piCachedParticle[m_iMaxCachedParticleCount - ++m_iCachedParticleCount] = i;

      if (m_iMaxCachedParticleCount == m_iCachedParticleCount)
        break; // cache is full
    }
  if (!m_iCachedParticleCount)
    return false; // no free particles at all
  if (m_iMaxCachedParticleCount > m_iCachedParticleCount)
  {
    // Need to shift value down since we weren't able to fill it
    memmove(&m_piCachedParticle[0], 
            &m_piCachedParticle[m_iMaxCachedParticleCount - m_iCachedParticleCount], 
            m_iCachedParticleCount*sizeof(unsigned short));
  }

  return true;
}


ParticleExt_t* ParticleGroupBase_cl::GetFreeParticle()
{
  if (m_eTopology == PARTICLE_TOPOLOGY_TRAIL) // trail is handled differently
  {
    int iPrevIndex = hkvMath::Max (m_iTrailIndex,0);// the first one point to itself
    m_iTrailIndex = (m_iTrailIndex+1) % GetNumOfParticles();
    m_iHighWaterMark = hkvMath::Max(m_iHighWaterMark,m_iTrailIndex+1);
    ParticleExt_t *p = &GetParticlesExt()[m_iTrailIndex];
    p->m_fDistortionMult = (float)iPrevIndex + 0.1f; // in this mode, we use this variable to store the index to the previous particle

    // make sure the end of the trail does not reference the beginning
    int iNext = (m_iTrailIndex+1) % GetNumOfParticles();
    ParticleExt_t *pNext = &GetParticlesExt()[iNext];
    pNext->m_fDistortionMult = (float)iNext + 0.1f; // reference itself
    return p;
  }
  if (m_iCachedParticleCount>0) // take particle from cache
  {
    m_iCachedParticleCount--;
    int iIndex = m_piCachedParticle[m_iCachedParticleCount];
    ParticleExt_t *p = &GetParticlesExt()[iIndex];
    if (!p->valid)
    {
      iIndex++;
      m_iHighWaterMark = hkvMath::Max(m_iHighWaterMark,iIndex);
      return p;
    }
    // particle has been reused although it was in cache, try the next one
    return GetFreeParticle();
  }

  // cache was empty, so fill it up again
  if (!FillFreeParticleCache())
    return NULL; // no free particle at all

  return GetFreeParticle();
}

void ParticleGroupBase_cl::DestroyParticle(Particle_t *pParticle,float fTimeDelta)
{
  if (m_spOnDestroyCreateGroup)
  {
    int i;
    int iCount = (int)m_spDescriptor->m_OnDestroyCreateCount.GetRandomValue(GetRandom());
    // create iCount new particles from other group
    if (iCount)
    {
      int iCopyFlags = m_spDescriptor->m_iOnDestroyCopyFlags;
      float fStep = fTimeDelta/(float)iCount;
      float fTimePos = 0.f;
      for (i=0;i<iCount;i++,fTimePos+=fStep)
        if (!m_spOnDestroyCreateGroup->CreateParticleFromSource((ParticleExt_t *)pParticle,fTimePos,iCopyFlags))
          break;
    }
  }
  AddParticleToCache((ParticleExt_t *)pParticle);
}




void ParticleGroupBase_cl::RenderParticleBoundingBoxes()
{
  ParticleExt_t *p = GetParticlesExt();
  const int iCount = m_iHighWaterMark;
  for (int i=0;i<iCount;i++,p++) if (p->valid)
    Vision::Game.DrawCube(hkvVec3(p->pos[0],p->pos[1],p->pos[2]), p->size, p->color);
}

void ParticleGroupBase_cl::HandleParticles(float dtime)
{
  EnsureUpdaterTaskFinished();

  if (m_bVisibilityUpdate)
    UpdateVisibilityObject();

  VISION_PROFILE_FUNCTION(VIS_PROFILE_PARTICLES_HANDLE);

  bool bNeedLifetimeUpdateOnly = false;

  if (m_bHandleWhenVisible && !WasRecentlyRendered()) // actual vischeck of particle group
  {
    if(!m_bUpdateLifetimeIfInvisible)
    {
      return;
    }
    bNeedLifetimeUpdateOnly = true;
  }

  if (m_bHalted) 
    return; // paused mode

  if (m_fInitialDelay>0.f)
  {
    float fScaledTime = dtime * m_fTimeScale;
    m_fInitialDelay -= fScaledTime;
    return;
  }

  if (m_bAttachedToCam)
  {
    hkvVec3 vCamPos;
    Vision::Camera.GetCurrentCameraPosition(vCamPos);
    SetPosition(vCamPos+m_vCamRelPos);
  }

  // also handle child particle group
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->HandleParticles(dtime);

  bool bDead = m_bIsDead;

  // handle group lifetime
  if (!m_bInfiniteLifeTime && !m_bPaused) // pause will also save lifetime
  {
    float fScaledTime = dtime * m_fTimeScale;
    m_fLifeTime -= fScaledTime;
    if (m_fLifeTime <= 0.0f)
    {
      // Make sure that we get full particle simulation again once the lifetime is zero,
      // even if we only update lifetime and do not simulate. This ensures that the particles
      // get correctly faded out and destroyed (which happens in simulation).
      bNeedLifetimeUpdateOnly = false;
      if(m_iValidCount == 0)
      {
        bDead = true;
      }
    }
  } 
  else if (m_iRemainingParticleCount==0 && m_iValidCount==0 && !m_pParentGroup) // also die when all particles are used up
    bDead = true;

  if (bDead)
  {
    m_bIsDead = true;
    if (m_pParentEffect)
      m_pParentEffect->OnSingleGroupFinished(this); // detach from parent
    // from now on, group might not be valid anymore, so return
    return;
  }

  if(bNeedLifetimeUpdateOnly)
  {
    // Early out if we only care about life update
    return; 
  }

  if (VISION_UNLIKELY(m_bRenderConstraints))
    m_Constraints.RenderConstraints(VisRenderContext_cl::GetCurrentContext()->GetRenderInterface());

  m_pHandlingTask->m_fTimeDelta = dtime;
  
  if (!m_pParentGroup) {
    m_vGroupMoveDelta = m_vGroupMoveDeltaAccum;
    m_vGroupMoveDeltaAccum.setZero();
    Vision::GetThreadManager()->ScheduleTask(m_pHandlingTask, 5);
    SetUpdateTask(m_pHandlingTask);
  }
  else
  {
    // if this is a child particle group, copy position and orientation from parent
    CopyParentPosition();
    ParticleGroupBase_cl *pRootParent = m_pParentGroup;
    while (pRootParent->m_pParentGroup) {
      pRootParent = pRootParent->m_pParentGroup;
    }
    SetUpdateTask(pRootParent->m_pHandlingTask);
  }
}


void ParticleGroupBase_cl::InflateBoundingBox(bool bForceValid)
{
  const int iCount = m_iHighWaterMark;
  ParticleExt_t *p = GetParticlesExt();
  int i;
  bool bAnyValid = false;
  
  for (i=0;i<iCount;i++,p++) if (p->valid)
  {
    hkvVec3 pos(p->pos[0],p->pos[1],p->pos[2]);
    hkvVec3 rad(p->size,p->size,p->size);

    hkvAlignedBBox transformedMeshBoundingBox = m_MeshBoundingBox;
    transformedMeshBoundingBox.scaleFromOrigin(rad);
    transformedMeshBoundingBox.translate(pos);

    m_BoundingBox.expandToInclude(transformedMeshBoundingBox);

	  rad *= 0.5f;

    m_BoundingBox.expandToInclude(pos+rad);
    m_BoundingBox.expandToInclude(pos-rad);

    // make sure that this box remains valid until the next update
    float dt = m_spDescriptor->m_fDynamicInflateInterval;
    hkvVec3 lastPos(pos.x + p->velocity[0] * dt, pos.y + p->velocity[1] * dt, pos.z + p->velocity[2] * dt);
    m_BoundingBox.expandToInclude(lastPos+rad);
    m_BoundingBox.expandToInclude(lastPos-rad);

    if (m_cUseDistortion)
    {
      pos.x += p->distortion[0];
      pos.y += p->distortion[1];
      pos.z += p->distortion[2];
      m_BoundingBox.expandToInclude(pos+rad);
      m_BoundingBox.expandToInclude(pos-rad);
    }
    bAnyValid = true;
  }

  if (!bAnyValid && bForceValid)
  {
    if(m_spEmitter->GetType()==EMITTER_TYPE_MESH && m_pEmitterMeshEntity!=NULL)
    {
      m_BoundingBox.expandToInclude( m_pEmitterMeshEntity->GetBoundingBox());
      //add boundary of the initial bounding box
      hkvVec3 boundary(m_spDescriptor->m_BoundingBox.getSizeX()*0.5f,m_spDescriptor->m_BoundingBox.getSizeY()*0.5f,m_spDescriptor->m_BoundingBox.getSizeZ()*0.5f);
      m_BoundingBox.addBoundary(boundary);
    }
    else
    {
      hkvVec3 vCenter;
      if (!GetUseLocalSpaceMatrix())
        vCenter = GetPosition();

      // create a default box around the origin
      m_BoundingBox.expandToInclude(vCenter);
      m_BoundingBox.addBoundary(hkvVec3 (Vision::World.GetGlobalUnitScaling()*10.f)); // need enough pixels for HOQ    
    }
    bAnyValid = true;
  }

  m_bBBoxValid |= bAnyValid;
}


int ParticleGroupBase_cl::AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence)
{
  EnsureUpdaterTaskFinished();
  VASSERT(pSrcList);
  int i;
  const int iCount = pSrcList->GetConstraintCount();
  int iFound = 0;
  if (!m_BoundingBox.isValid())
    bCheckInfluence = false;

  for (i=0;i<iCount;i++)
  {
    VisParticleConstraint_cl *pConstraint = pSrcList->GetConstraint(i);
    if (!pConstraint) continue;
    if (bCheckInfluence && !pConstraint->Influences(m_BoundingBox)) continue;
    if (!AddConstraint(pConstraint,false)) continue; // influence already checked
    iFound++;
  }
  return iFound;
}


bool ParticleGroupBase_cl::AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence)
{
  EnsureUpdaterTaskFinished();
  VASSERT(pConstraint);
  if (bCheckInfluence && m_BoundingBox.isValid() && !pConstraint->Influences(m_BoundingBox))
    return false;
  m_Constraints.AddConstraint(pConstraint);

  // add also to the child group (without checking)
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->AddConstraint(pConstraint,false);

  return true;
}


void ParticleGroupBase_cl::HandleAllConstraints(float dtime) 
{
  // local list of constraints
  m_Constraints.HandleParticles(this,dtime,m_iConstraintAffectBitMask);

  // handle global list. Note that HandleParticles causes problems in multithreaded mode. Removing dead constraints is not necessary here
  VisParticleGroupManager_cl &manager( VisParticleGroupManager_cl::GlobalManager());
  manager.GlobalConstraints().HandleParticlesNoRemove(this,dtime,m_iConstraintAffectBitMask);
}



V_IMPLEMENT_SERIALX( ParticleGroupBase_cl);
void ParticleGroupBase_cl::SerializeX( VArchive &ar )
{
  // serialize base class
  VisParticleGroup_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    int iVersion;
    float fScaling;

    // version
    ar >> iVersion;

    // particle group data
    ar >> fScaling; SetScaling(fScaling);
    if (iVersion>=PARTICLEGROUPBASE_VERSION_002)
    {
      ar >> m_AmbientColor; // version 2
    }
    if (iVersion>=PARTICLEGROUPBASE_VERSION_003)
    {
      unsigned int iMask = 0xffffffff;
      ar >> iMask; // version 3
      SetVisibleBitmask(iMask);
    }

    if (iVersion>=PARTICLEGROUPBASE_VERSION_004)
      m_vWindSpeed.SerializeAsVisVector (ar);// version 4
    if (iVersion>=PARTICLEGROUPBASE_VERSION_005)
      ar >> m_bWindInLocalSpace; // version 5
    if (iVersion>=PARTICLEGROUPBASE_VERSION_006)
      ar >> m_bApplyTimeOfDayLight; // version 6
    if (iVersion>=PARTICLEGROUPBASE_VERSION_008)
      ar >> GetEmitter()->m_fIntensity; // version 8

    if (iVersion>=PARTICLEGROUPBASE_VERSION_007)
      ar >> m_pEmitterMeshEntity;

    // constraints
    m_Constraints.SerializeX(ar);

    ParticleGroupBase_cl * pOnDestroyGroup = m_spOnDestroyCreateGroup;

    while(pOnDestroyGroup!=NULL && pOnDestroyGroup!=this /*avoid loops*/)
    {
      pOnDestroyGroup->m_Constraints.AddFrom(&m_Constraints);
      pOnDestroyGroup = pOnDestroyGroup->m_spOnDestroyCreateGroup; //continue with next group
    }

    // use white as a default value, because one cannot sample the light-grid during loading
    m_InstanceColor.SetRGB (255, 255, 255);

    // Evaluate the real color next frame, see EvaluateSceneBrightness
    m_bEvaluateBrightnessNextFrame = true;
  } 
  else
  {
    // version number
    ar << (int)PARTICLEGROUPBASE_VERSION_CURRENT; 

    // particle group data
    ar << GetScaling();
    ar << m_AmbientColor; // version 2
    ar << GetVisibleBitmask(); // version 3
    m_vWindSpeed.SerializeAsVisVector (ar);// version 4
    ar << m_bWindInLocalSpace; // version 5
    ar << m_bApplyTimeOfDayLight; // version 6
    ar << GetEmitter()->m_fIntensity; // version 8

    VisBaseEntity_cl *pEmitterMesh = NULL;
    if (GetEmitter()->m_wpEntityMeshEmitter!=NULL)
      pEmitterMesh = GetEmitter()->m_wpEntityMeshEmitter.GetPtr();
    ar << pEmitterMesh; // version 7

    // constraints
    m_Constraints.SerializeX(ar);
  }
}

void ParticleGroupBase_cl::OnDeserializationCallback(const VSerializationContext &context)
{
  VisParticleGroup_cl::OnDeserializationCallback(context);

  GetEmitter()->SetMeshEmitterEntity(m_pEmitterMeshEntity);
  m_pEmitterMeshEntity = NULL;
  OnObject3DChanged( VIS_OBJECT3D_POSCHANGED | VIS_OBJECT3D_ORICHANGED);
  RespawnAllParticles(false);
}

bool ParticleGroupBase_cl::SetMeshEmitterEntity(VisBaseEntity_cl* pEntity)
{
	// allows chaning the mesh emitter before 
	m_pEmitterMeshEntity = pEntity;
	VisParticleEmitter_cl *pEmitter = GetEmitter();
	if (pEmitter->GetType()==EMITTER_TYPE_MESH)
	{
		pEmitter->SetMeshEmitterEntity(pEntity);
		return pEmitter->m_spEmitterMesh!=NULL;
	} 
	else
	{
		pEmitter->SetMeshEmitterEntity(NULL);
		return false;
	}
}

// Virtual function that gets called whenever the position or orientation of the Object3D is changed
void ParticleGroupBase_cl::OnObject3DChanged(int iO3DFlags)
{
  VisParticleGroup_cl::OnObject3DChanged(iO3DFlags);

  if ( iO3DFlags & VIS_OBJECT3D_POSCHANGED )
  {
    EnsureUpdaterTaskFinished(); // whenever bounding boxes are tested, this is necessary
    if (VisParticleGroup_cl::GetVisibilityObject() && m_BoundingBox.isValid())
    {
      // make sure the emitter is inside the bounding box
      m_BoundingBox.expandToInclude(GetPosition()); // this is the emitter's position
      VisParticleGroup_cl::SetBoundingBox(m_BoundingBox);
    }

    m_bBBoxValid = false;
    m_bVisibilityUpdate = true;
    m_bEvaluateBrightnessNextFrame |= m_spDescriptor->m_fApplySceneBrightness > 0.0f;

    hkvVec3 vDelta = GetPosition() - m_vOldPos;

  #ifdef WIN32
    if ( Vision::Editor.IsInEditor() && !Vision::Editor.IsAnimatingOrPlaying() && !GetUseLocalSpaceMatrix())
    {
      MoveParticles( vDelta );

      // also move child group
      if (m_spOnDestroyCreateGroup)
        m_spOnDestroyCreateGroup->IncPosition(vDelta);
      vDelta.setZero(); // already handled
    }
  #endif
    m_vGroupMoveDeltaAccum += vDelta;
  }
  if ( iO3DFlags & VIS_OBJECT3D_ORICHANGED )
  {
    m_bBBoxValid = false;
    m_bVisibilityUpdate = true;
    hkvMat3 rotMat = GetRotationMatrix();
    hkvVec3 t1, t2, t3;
    rotMat.getAxisXYZ(&t1, &t2, &t3);
    m_vDirection = t1; 
    m_vRightDir = t2; 
    m_vUpDir = t3;;
  }

  m_vOldPos = GetPosition();

  // remove only the "change" flags
  ClearO3DChangeFlags();
}


void ParticleGroupBase_cl::SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace)
{
  m_vWindSpeed = vWind;
  m_bWindInLocalSpace = bApplyInLocalSpace;
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->SetWindSpeed(vWind);
}


void ParticleGroupBase_cl::SetVisibleBitmask(unsigned int iMask)
{
  VisParticleGroup_cl::SetVisibleBitmask(iMask);
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->SetVisibleBitmask(iMask);
}



////////////////////////////////////////////////////////////////////////////
// handle the particle effect instance
////////////////////////////////////////////////////////////////////////////


// macro to iterate through all valid groups
#define FOR_ALL_GROUPS \
  for (int i=0;i<m_iGroupCount;i++)\
  {\
    ParticleGroupBase_cl *pGroup = m_spGroups[i];\
    if (!pGroup) continue;

VisParticleEffect_cl::VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, unsigned int uiRandomSeed) :
  m_uiRandomBaseSeed(uiRandomSeed)
{
  VASSERT(pEffect);
  m_bUseLightgrid = false;
  m_spGroups = NULL;
  m_iGroupCount = 0;
  m_bPaused = m_bHalted = false;
  m_bVisible = true;
  m_bRemoveDeadLayers = true;
  m_vPosition = vPos;
  m_vOrientation = vOri;
  m_bAnyMeshEmitter = false;

  pEffect->InitParticleEffectInstance(this,vPos,vOri, 1.0f);
  //SetPosition( vPos );
  //SetOrientation( vOri );
//  m_fSpeedMeasurePos = 0.f;
 // m_vOldPos = vPos;
}

VisParticleEffect_cl::VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, unsigned int uiRandomSeed) :
  m_uiRandomBaseSeed(uiRandomSeed)
{
  VASSERT(pEffect);
  m_bUseLightgrid = false;
  m_spGroups = NULL;
  m_iGroupCount = 0;
  m_bPaused = m_bHalted = false;
  m_bVisible = true;
  m_bRemoveDeadLayers = true;
  m_vPosition = vPos;
  m_vOrientation = vOri;
  m_bAnyMeshEmitter = false;

  pEffect->InitParticleEffectInstance(this,vPos,vOri, fScaling);
  //SetPosition( vPos );
  //SetOrientation( vOri );
  //m_fSpeedMeasurePos = 0.f;
  //m_vOldPos = vPos;
}

VisParticleEffect_cl::VisParticleEffect_cl() :
  m_uiRandomBaseSeed(0)
{
  m_bRemoveDeadLayers = true;
  m_bUseLightgrid = false;
  m_spGroups = NULL;
  m_iGroupCount = 0;
  m_bPaused = m_bHalted = false;
  m_bVisible = true;
  m_bAnyMeshEmitter = false;
//  m_fSpeedMeasurePos = 0.f;
}


void VisParticleEffect_cl::CreateFromDescriptors(VisParticleGroupDescriptor_cl **pDescList,int iDescCount)
{
  V_SAFE_DELETE_ARRAY(m_spGroups);
  m_iGroupCount = iDescCount;
  m_spSourceFXFile = NULL;

  if (!iDescCount)
    return;
  
  // allocate the particle group pointers and fill them
  m_spGroups = new ParticleGroupBasePtr[iDescCount];
  for (int i=0;i<m_iGroupCount;i++)
  {
    ParticleGroupBase_cl *pLayer = new ParticleGroupBase_cl(pDescList[i],NULL, GetPosition(), GetOrientation(), true, GetRandomBaseSeed());
    m_spGroups[i] = pLayer;
    pLayer->m_pParentEffect = this;
    pLayer->m_iChildIndex = i;
    pLayer->AttachToParent(this);
    pLayer->Finalize();
  }
}


VisParticleEffect_cl::~VisParticleEffect_cl()
{
  V_SAFE_DELETE_ARRAY(m_spGroups);
}


void VisParticleEffect_cl::Tick(float fTimeDiff)
{
  FOR_ALL_GROUPS
    if (pGroup->m_bHasTransformationCurves || pGroup->m_bHasEvents)
    {
      float &x(pGroup->m_fTransformationCurveTime);
      const float fDelta = pGroup->m_fTransformationCurveSpeed*fTimeDiff*pGroup->m_fTimeScale;
      if (pGroup->m_spDescriptor->m_spPositionCurve!=NULL)
      {
        hkvVec3 vPos = pGroup->m_spDescriptor->m_spPositionCurve->GetValueFast(x) * pGroup->m_fScaling;
        pGroup->SetLocalPosition(vPos);
      }
      if (pGroup->m_spDescriptor->m_spOrientationCurve!=NULL)
      {
        hkvVec3 vOri = pGroup->m_spDescriptor->m_spOrientationCurve->GetValueFast(x);
        vOri *= 360.f;
        pGroup->SetLocalOrientation(vOri);
      }
      if (pGroup->m_bHasEvents)
      {
        VisAnimEventList_cl &list(pGroup->m_spDescriptor->m_EventList);
        list.PrepareCurrentEventSet(x,x+fDelta,true); // no fmod here. note this method is not thread safe
        VisAnimEvent_cl *pEvent = list.GetNextEvent();
        while (pEvent!=NULL)
        {
          VisParticleLayerEventData_cl data(pGroup,pEvent);
          data.Trigger();
          pEvent = list.GetNextEvent();
        }
      }
      x = hkvMath::mod (x + fDelta,1.f);
    }
    if (m_bAnyMeshEmitter)
    {
      pGroup->GetEmitter()->UpdateMeshEmitterEntity(); // must be called from the main thread

      //Use this line to debug the bounding box of the emitter
      //Vision::Game.DrawBoundingBox(pGroup->m_BoundingBox, V_RGBA_CYAN);    
    }

    pGroup->HandleParticles(fTimeDiff);
  }
}

// Virtual function that gets called whenever the position or orientation of the Object3D is changed
void VisParticleEffect_cl::OnObject3DChanged(int iO3DFlags)
{
  // the binding takes care of moving the child emitter (with local space positions)
  VisObject3D_cl::OnObject3DChanged(iO3DFlags);

  if ( iO3DFlags & VIS_OBJECT3D_POSCHANGED )
  {
    UpdateLightGrid();
  }

  // remove only the "change" flags
  ClearO3DChangeFlags();
}


static int PARTICLETRIGGER_ID_PAUSE = -1;
static int PARTICLETRIGGER_ID_RESUME = -1;
static int PARTICLETRIGGER_ID_RESTART = -1;

void VisParticleEffect_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VisObject3D_cl::MessageFunction(iID,iParamA,iParamB);
  if (iID==VIS_MSG_TRIGGER)
  {
    EnsureComponentIDsRegistered();

    // the trigger components are added by vForge and serialized if used
    // See EngineInstanceParticleGroup::c'tor
    VisTriggerTargetComponent_cl *pTarget = (VisTriggerTargetComponent_cl *)iParamB;
    if (pTarget->m_iComponentID==PARTICLETRIGGER_ID_PAUSE)
      SetPause(true);
    else if (pTarget->m_iComponentID==PARTICLETRIGGER_ID_RESUME)
      SetPause(false);
    else if (pTarget->m_iComponentID==PARTICLETRIGGER_ID_RESTART)
      if (!GetRemoveWhenFinished())
        Restart();
    
    return;
  }
}

void VisParticleEffect_cl::CheckForRestartComponent()
{
  // check whether any trigger component is of type trigger->restart. In that case the effect won't be removed automatically
  for (int i=0;i<Components().Count();i++)
  {
    IVObjectComponent *pComponent = Components().GetAt(i);
    if (pComponent==NULL || !pComponent->IsOfType(V_RUNTIME_CLASS(VisTriggerTargetComponent_cl)))
      continue;
    EnsureComponentIDsRegistered();
    if (pComponent->m_iComponentID==PARTICLETRIGGER_ID_RESTART)
      SetRemoveWhenFinished(false);
  }
}

void VisParticleEffect_cl::EnsureComponentIDsRegistered()
{
  // cache IDs to avoid string comparisons
  if (VISION_UNLIKELY(PARTICLETRIGGER_ID_PAUSE==-1))
  {
    PARTICLETRIGGER_ID_PAUSE = IVObjectComponent::RegisterStringID(PARTICLETRIGGER_PAUSE);
    PARTICLETRIGGER_ID_RESUME = IVObjectComponent::RegisterStringID(PARTICLETRIGGER_RESUME);
    PARTICLETRIGGER_ID_RESTART = IVObjectComponent::RegisterStringID(PARTICLETRIGGER_RESTART);
  }
}


BOOL VisParticleEffect_cl::AddComponent(IVObjectComponent *pComponent)
{
  if (!VisObject3D_cl::AddComponent(pComponent))
    return FALSE;
  CheckForRestartComponent();
  return TRUE;
}

void VisParticleEffect_cl::IncPosition(const hkvVec3& vDelta, bool bMoveParticles)
{
  VisObject3D_cl::IncPosition(vDelta);

  if ( bMoveParticles)
  {
    // do not manually update particles in vForge edit mode
    // (happens automatically in OnObject3DChanged function)
    if ( Vision::Editor.IsInEditor() && !Vision::Editor.IsAnimatingOrPlaying() )
      return;

//    m_vOldPos = GetPosition();
    FOR_ALL_GROUPS
      if (!pGroup->GetUseLocalSpaceMatrix())
        pGroup->MoveParticles( vDelta );
    }
  }
}



void VisParticleEffect_cl::SetScaling(float fScale)
{
  FOR_ALL_GROUPS
    pGroup->SetScaling(fScale);
  }
}

void VisParticleEffect_cl::SetVisible(bool bStatus)
{
  if (m_bVisible==bStatus) return;
  m_bVisible = bStatus;
  FOR_ALL_GROUPS
    pGroup->SetVisible(bStatus);
  }
}

void VisParticleEffect_cl::SetAmbientColor(VColorRef iColor)
{
  FOR_ALL_GROUPS
    pGroup->SetAmbientColor(iColor);
  }
}

void VisParticleEffect_cl::EvaluateSceneBrightness()
{
  FOR_ALL_GROUPS
    pGroup->EvaluateSceneBrightness();
    pGroup->HandleParticles(0.f);
  }
}

void VisParticleEffect_cl::SetVisibleBitmask(unsigned int iMask)
{
  FOR_ALL_GROUPS
    pGroup->SetVisibleBitmask(iMask & pGroup->m_spDescriptor->m_iVisibleBitmask);
  }
}

void VisParticleEffect_cl::UpdateVisibilityBoundingBox()
{
  FOR_ALL_GROUPS
    pGroup->UpdateVisibilityBoundingBox(); // halts the updater task
    pGroup->UpdateVisibilityObject();
  }
}

void VisParticleEffect_cl::SetPause(bool bStatus)
{
  m_bPaused = bStatus;
  FOR_ALL_GROUPS
    pGroup->SetPause(bStatus);
  }
}

void VisParticleEffect_cl::SetHalted(bool bStatus)
{
  m_bHalted = bStatus;
  FOR_ALL_GROUPS
    pGroup->SetHalted(bStatus);
  }
}

void VisParticleEffect_cl::SetApplyTimeOfDayLight(bool bApply)
{
  FOR_ALL_GROUPS
    pGroup->SetApplyTimeOfDayLight(bApply);
  }
}

bool VisParticleEffect_cl::GetApplyTimeOfDayLight(void) const
{
  FOR_ALL_GROUPS
    if (pGroup->GetApplyTimeOfDayLight ())
      return (true);
  }

  return (false);
}

void VisParticleEffect_cl::SetFinished()
{
  FOR_ALL_GROUPS
    pGroup->SetFinished();
  }
}

void VisParticleEffect_cl::SetHandleWhenVisible(bool bStatus)
{
  FOR_ALL_GROUPS
    pGroup->SetHandleWhenVisible(bStatus);
  }
}

bool VisParticleEffect_cl::IsUpdatedOnlyWhenVisible()
{
  FOR_ALL_GROUPS
    if (!pGroup->GetHandleWhenVisible() || pGroup->m_bUpdateLifetimeIfInvisible) return false;
  }
  return true;
}

bool VisParticleEffect_cl::WasRecentlyRendered()
{ 
  FOR_ALL_GROUPS
    if (pGroup->WasRecentlyRendered()) return true;
  }
  return false;
}

void VisParticleEffect_cl::Restart()
{
#ifdef WIN32
  VASSERT((!m_bRemoveDeadLayers || VisParticleGroupManager_cl::g_bLoopAllEffects) && "When calling Restart, this instance must be flagged as SetRemoveWhenFinished(false)");
#else
  VASSERT(!m_bRemoveDeadLayers && "When calling Restart, this instance must be flagged as SetRemoveWhenFinished(false)");
#endif
  FOR_ALL_GROUPS
    pGroup->m_bIsDead = false;
    pGroup->InitGroup(true);
    pGroup->m_iChildIndex = i;
  }
}


void VisParticleEffect_cl::DisposeObject()
{
  // dispose all layers as well
  FOR_ALL_GROUPS
    pGroup->DisposeObject();
  }
  V_SAFE_DELETE_ARRAY(m_spGroups);
  m_iGroupCount = 0;

  VisObject3D_cl::DisposeObject();
  if (!m_spSourceFXFile || !m_spSourceFXFile->GetParentManager())
    return;
  m_spSourceFXFile->GetParentManager()->Instances().SafeRemove(this);
}

void VisParticleEffect_cl::AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence)
{
  FOR_ALL_GROUPS
    pGroup->AddRelevantConstraints(pSrcList,bCheckInfluence);
  }
}

void VisParticleEffect_cl::AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence)
{
  FOR_ALL_GROUPS
    pGroup->AddConstraint(pConstraint,bCheckInfluence);
  }
}

void VisParticleEffect_cl::SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace)
{
  FOR_ALL_GROUPS
    pGroup->SetWindSpeed(vWind, bApplyInLocalSpace);
  }
}

void VisParticleEffect_cl::SetMeshEmitterEntity(VisBaseEntity_cl *pEntity)
{
  m_bAnyMeshEmitter = false;
  FOR_ALL_GROUPS
	  m_bAnyMeshEmitter |= pGroup->SetMeshEmitterEntity(pEntity);
  }
}


void VisParticleEffect_cl::RemoveConstraint(VisParticleConstraint_cl *pConstraint)
{
  FOR_ALL_GROUPS
    pGroup->RemoveConstraint(pConstraint);
  }
}

void VisParticleEffect_cl::RemoveAllConstraints()
{
  FOR_ALL_GROUPS
    pGroup->RemoveAllConstraints();
  }
}

void VisParticleEffect_cl::SetIntensity(float fIntensity)
{
  FOR_ALL_GROUPS
    pGroup->GetEmitter()->SetIntensity(fIntensity);
  }
}


void VisParticleEffect_cl::RespawnAllParticles(bool bUseOldCount)
{
  FOR_ALL_GROUPS
    pGroup->RespawnAllParticles(bUseOldCount);
  }
}

void VisParticleEffect_cl::TeleportSpawnPosition()
{
  FOR_ALL_GROUPS
    pGroup->TeleportSpawnPosition();
  }
}

void VisParticleEffect_cl::RenderParticleBoundingBoxes()
{
  FOR_ALL_GROUPS
    pGroup->RenderParticleBoundingBoxes();
  }
}

bool VisParticleEffect_cl::GetLocalBoundingBox(hkvAlignedBBox &destBox) const
{
  hkvAlignedBBox tempBox;
  destBox.setInvalid();
  FOR_ALL_GROUPS
    if (!pGroup->GetDescriptor()->GetBoundingBox(tempBox)) continue;
    destBox.expandToInclude(tempBox);
  }
  return destBox.isValid();
}


bool VisParticleEffect_cl::GetCurrentBoundingBox(hkvAlignedBBox &destBox) const
{
  destBox.setInvalid();
  FOR_ALL_GROUPS
    destBox.expandToInclude(*pGroup->GetCurrentBoundingBox());
  }
  return destBox.isValid();
}


void VisParticleEffect_cl::OnSingleGroupFinished(ParticleGroupBase_cl *pFinishedGroup)
{

#ifdef WIN32
  if (VisParticleGroupManager_cl::g_bLoopAllEffects) // this feature is only relevant inside the editor
  {

    bool bAllDead = true;
    FOR_ALL_GROUPS
      if (!pGroup->m_bIsDead)
      {
        bAllDead = false;
        break;
      }
    }
    if (bAllDead)
    {
      Restart();
    }
    return;
  }
#endif

  if (!m_bRemoveDeadLayers)
    return;

  int iMaxValid = 0;
  FOR_ALL_GROUPS
    if (pGroup==pFinishedGroup)
    {
      pGroup->AttachToParent(NULL);
      m_spGroups[i] = NULL; // remove from list, pFinishedGroup might not be valid anymore afterwards
      continue;
    }
    iMaxValid = i+1;
  }
  m_iGroupCount = iMaxValid; // new upper limit. If 0, group is "dead"
}


const char SERIALMODE_FILENAME = 0; // serializes the file name of the effect file and goes through the manager (XML parsing)
const char SERIALMODE_FULL     = 1; // binary serializes the effects into the file

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=178
#endif

#if defined(__ghs__)
#pragma ghs nowarning 177
#endif

const int PARTICLEEFFECT_VERSION_0  = 0;
const int PARTICLEEFFECT_VERSION_1  = 1; // effect key, serialize mode
const int PARTICLEEFFECT_VERSION_2  = 2; // group count bugfix
const int PARTICLEEFFECT_VERSION_3  = 3; // serialize paused & halted
const int PARTICLEEFFECT_VERSION_4  = 4; // add m_uiRandomBaseSeed
#define PARTICLEEFFECT_CURRENTVERSION  PARTICLEEFFECT_VERSION_4

#if defined(__SNC__)
#pragma diag_pop
#endif

#if defined(__ghs__)
#pragma ghs endnowarning
#endif


V_IMPLEMENT_SERIAL( VisParticleEffect_cl, VisObject3D_cl, 0, &g_VisionEngineModule );
void VisParticleEffect_cl::Serialize( VArchive &ar )
{
  VisObject3D_cl::Serialize(ar);
  char iSerializeMode = SERIALMODE_FILENAME;
  int iVersion = PARTICLEEFFECT_CURRENTVERSION;

  if (ar.IsLoading())
  {
    ar >> iVersion; VASSERT(iVersion>=0 && iVersion<=PARTICLEEFFECT_CURRENTVERSION);
    if (iVersion>=PARTICLEEFFECT_VERSION_1)
    {
      ar >> iSerializeMode;
      VASSERT(iSerializeMode==SERIALMODE_FILENAME || iSerializeMode==SERIALMODE_FULL);
    }

    if (iVersion>=PARTICLEEFFECT_VERSION_4)
      ar >> m_uiRandomBaseSeed;

    if (iSerializeMode==SERIALMODE_FILENAME)
    {
      // read effect filename
      char szFXFilename[FS_MAX_PATH+1];
      ar.ReadStringBinary(szFXFilename,FS_MAX_PATH);
      m_spSourceFXFile = VisParticleGroupManager_cl::GlobalManager().LoadFromFile(szFXFilename);
      VASSERT(m_spSourceFXFile);
    }
    else 
    if (iSerializeMode==SERIALMODE_FULL)
    {
      VASSERT(!"Not supported yet");
      VisParticleEffectFile_cl *pFX = NULL;
      ar >> pFX;
      m_spSourceFXFile = pFX;
    }
    m_spSourceFXFile->InitParticleEffectInstance(this);

    if (iVersion>=PARTICLEEFFECT_VERSION_1)
    {
      // OLD key, now in object3d
      if (ar.GetLoadingVersion()<VISION_ARCHIVE_VERSION_025)
        ar >> m_sObjectKey;
    }

    if (iVersion>=PARTICLEEFFECT_VERSION_3)
    {
      ar >> m_bPaused >> m_bHalted;
    }

    // read properties for each particle group. Read count must match actual count from the effect!
    int iGroupCount;
    ar >> iGroupCount;

    m_bAnyMeshEmitter = false;
    VASSERT(iGroupCount<=m_iGroupCount);
    for (int i=0;i<iGroupCount;i++)
    {
      bool bPresent = true;
      if (iVersion>=PARTICLEEFFECT_VERSION_2)
        ar >> bPresent;
      if (bPresent)
      {
        m_spGroups[i]->SerializeX(ar);
        m_bAnyMeshEmitter |= m_spGroups[i]->GetEmitter()->m_spEmitterMesh!=NULL;
      }
      else
        m_spGroups[i] = NULL;
    }
//    m_vOldPos = GetPosition();
  } else
  {
    ar << iVersion;
    ar << iSerializeMode;
    ar << m_uiRandomBaseSeed;
    VASSERT(m_spSourceFXFile);
    if (iSerializeMode==SERIALMODE_FILENAME)
    {
      ar << m_spSourceFXFile->GetFilename();
    }
    else
    if (iSerializeMode==SERIALMODE_FULL)
    {
      ar << m_spSourceFXFile;
    }

    //ar << m_sEffectKey; now in object 3d
    ar << m_bPaused << m_bHalted;

    // save the properties (position, constraints,..) of each particle group separately
    ar << m_iGroupCount;
    for (int i=0;i<m_iGroupCount;i++)
    {
      if (m_spGroups[i])
      {
        ar << (bool)true;
        m_spGroups[i]->SerializeX(ar);
      }
      else
        ar << (bool)false;
    }
  }
}


void VisParticleEffect_cl::OnDeserializationCallback(const VSerializationContext &context)
{
  VisObject3D_cl::OnDeserializationCallback(context);

  for (int i=0;i<m_iGroupCount;i++)
  {
    if (m_spGroups[i] != NULL )
    {
      m_spGroups[i]->OnDeserializationCallback(context);
      m_bAnyMeshEmitter |= m_spGroups[i]->GetEmitter()->m_spEmitterMesh!=NULL;
    }
  }
  UpdateLightGrid();
  SetPause(m_bPaused);
  SetHalted(m_bHalted);
}



void VisParticleEffect_cl::OnSerialized(VArchive &ar)
{
  VisObject3D_cl::OnSerialized(ar);
  CheckForRestartComponent();
}


#ifdef SUPPORTS_SNAPSHOT_CREATION

void VisParticleEffect_cl::GetDependencies(VResourceSnapshot &snapshot)
{
  VisObject3D_cl::GetDependencies(snapshot);

  if (m_spSourceFXFile)
    m_spSourceFXFile->GetDependencies(snapshot);

  FOR_ALL_GROUPS
    pGroup->GetDependencies(snapshot);
  }
}
#endif


int VisParticleEffect_cl::GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList)
{
  int iCount = VisObject3D_cl::GetSynchronizationGroupList(context, pDestList);
  if (context.m_bSupportsInterpolation)
    pDestList[iCount++].Set(this, &VNetworkParticleEffectGroup::g_Instance);
  else
    pDestList[iCount++].Set(this, &VNetworkParticleEffectGroup::g_Instance);
  return iCount;
}

void VisParticleEffect_cl::OnReposition(const VisZoneRepositionInfo_t &info, const hkvVec3 &vLocalPos)
{
  // don't call base implementation here
  hkvVec3 vAbsPos(hkvNoInitialization);
  info.Helper_MakeAbsolute(vAbsPos, vLocalPos, GetParentZone());
  IncPosition(vAbsPos - GetPosition(), true);
}



////////////////////////////////////////////////////////////////////////////
// handle the particle effect instance collection
////////////////////////////////////////////////////////////////////////////

#define FOR_ALL_INSTANCES \
  int iCount = Count();\
  for (int i=0;i<iCount;i++)\
  {\
    VisParticleEffect_cl *pInstance = GetAt(i);\
    VASSERT(pInstance!=NULL);

#define FOR_ALL_INSTANCES_REVERSE \
  int iCount = Count();\
  for (int i=iCount-1;i>=0;i--)\
  {\
  VisParticleEffect_cl *pInstance = GetAt(i);\
  VASSERT(pInstance!=NULL);

void VisParticleEffectCollection_cl::Tick(float fTimeDelta, bool bPurgeDead)
{
  FOR_ALL_INSTANCES_REVERSE
    if (pInstance->IsUpdatedOnlyWhenVisible() && !pInstance->WasRecentlyRendered()) continue;
    pInstance->Tick(fTimeDelta);
    if (bPurgeDead && pInstance->IsDead())
    {
      pInstance->DisposeObject(); // this removes it from this collection
    }
  }
}

void VisParticleEffectCollection_cl::EvaluateSceneBrightness()
{
  FOR_ALL_INSTANCES
    pInstance->EvaluateSceneBrightness();
  }
}

void VisParticleEffectCollection_cl::Purge(bool bDeadOnly)
{
  FOR_ALL_INSTANCES_REVERSE
    if ((!bDeadOnly && pInstance->GetRefCount()==1) || pInstance->IsDead()) 
      RemoveAt(i);
  }
}

void VisParticleEffectCollection_cl::ReassignShader(bool bRecreateFX)
{
  FOR_ALL_INSTANCES
    const int iGroupCount = pInstance->GetParticleGroupCount();
    for(int group=0;group<iGroupCount;++group)
    {
      ParticleGroupBase_cl *pGroup = pInstance->GetParticleGroup(group);
      if (pGroup)
        pGroup->ReassignShader(bRecreateFX);
    }
  }
}

VisParticleEffect_cl *VisParticleEffectCollection_cl::FindByKey(const char *szKey, DynArray_cl<VisParticleEffect_cl *> *pStoreArray) const
{
  VisParticleEffect_cl *pFound = NULL;
  int iFound = 0;
  FOR_ALL_INSTANCES
    const char *szOtherKey = pInstance->GetEffectKey();
    if (szKey==szOtherKey || (szKey != NULL && szOtherKey != NULL && !_stricmp(szKey,szOtherKey))) // szKey can be NULL to find all without a key
    {
      if (!pStoreArray)
        return pInstance;
      if (!pFound) pFound = pInstance;
      (*pStoreArray)[iFound++] = pInstance;
    }
  }
  return pFound;
}



////////////////////////////////////////////////////////////////////////////////////
// class VNetworkParticleEffectGroupI
////////////////////////////////////////////////////////////////////////////////////

VNetworkParticleEffectGroupI VNetworkParticleEffectGroupI::g_Instance;

bool VNetworkParticleEffectGroupI::QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  VHistoryDataParticleEffect* pData = (VHistoryDataParticleEffect*) instanceInfo.m_pCustomData;
  
  BYTE iMask = 0;
  iMask |= pParticleEffect->IsPaused() * VCF_PAUSED;
  iMask |= pParticleEffect->IsHalted() * VCF_HALTED;

  BYTE iInterpolatedMask;
  pData->m_particleFlagHistory.Interpolate (&iInterpolatedMask, context.m_iCurrentServerTimeMS);
  // Every change in the flags is reliably transmitted
  if (iInterpolatedMask != iMask)
  {
    out_paketSettings.SetReliability (VMR_Reliable_Ordered);
    out_paketSettings.SetPriority (VMP_HighPriority);
    out_paketSettings.SetOrderingChannel (0);
    return true;
  }
  return false;
}

void VNetworkParticleEffectGroupI::Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  VHistoryDataParticleEffect* pData = (VHistoryDataParticleEffect*) instanceInfo.m_pCustomData;
  BYTE iMask = 0;

  if (ar.IsLoading())
  {
    __int64 iTimeMS;
    ar >> iTimeMS;
    ar >> iMask;
    pData->m_particleFlagHistory.Write (iMask, iTimeMS);
  }
  else
  {
    __int64 iTimeMS = context.m_iCurrentServerTimeMS;
    ar << iTimeMS;
    iMask |= pParticleEffect->IsPaused() * VCF_PAUSED;
    iMask |= pParticleEffect->IsHalted() * VCF_HALTED;
    ar << iMask;
    pData->m_particleFlagHistory.Write (iMask, iTimeMS);
  }
}

void VNetworkParticleEffectGroupI::TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta)
{
  // The server does not use the tick function, calling the base implementation will disable the tick function altogether.
  if (context.m_eNetType == VNT_Server)
  {
    IVNetworkSynchronizationGroup::TickFunction (context, instanceInfo, fTimeDelta);
    return;
  }

  VSystemGUID iOwnerID = instanceInfo.m_pComponent->GetNetworkOwnerID();
  // For clients and peers the group should only interpolate and set the values, if the owner is not set to themselves.
  if (iOwnerID != context.m_iUserID)
  {
    VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
    VHistoryDataParticleEffect* pData = (VHistoryDataParticleEffect*) instanceInfo.m_pCustomData;

    BYTE iMask = 0;
    VReadResult_e res = pData->m_particleFlagHistory.Interpolate (&iMask, context.m_iCurrentServerTimeMS - context.m_iInterpolationDelayMS);
    if (res != VRR_None)
    {
      pParticleEffect->SetPause  ((iMask & VCF_PAUSED) != 0);
      pParticleEffect->SetHalted ((iMask & VCF_HALTED) != 0);
    }
  }
}



////////////////////////////////////////////////////////////////////////////////////
// class VNetworkParticleEffectGroup
////////////////////////////////////////////////////////////////////////////////////

VNetworkParticleEffectGroup VNetworkParticleEffectGroup::g_Instance;

bool VNetworkParticleEffectGroup::QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  __int64 iNewHash = 0x0000000100000000;
  if (pParticleEffect->IsPaused()) iNewHash |= VCF_PAUSED;
  if (pParticleEffect->IsHalted()) iNewHash |= VCF_HALTED;

  // same transformation as last frame?
  if (iNewHash==instanceInfo.m_iLastUpdateDataHash)
    return false;

  instanceInfo.m_iLastUpdateDataHash = iNewHash;
  return true;
}

void VNetworkParticleEffectGroup::Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  BYTE iMask = 0;

  if (ar.IsLoading())
  {
    ar >> iMask;
    pParticleEffect->SetPause  ((iMask & VCF_PAUSED) != 0);
    pParticleEffect->SetHalted ((iMask & VCF_HALTED) != 0);
  }
  else
  {
    if (pParticleEffect->IsPaused()) iMask |= VCF_PAUSED;
    if (pParticleEffect->IsHalted()) iMask |= VCF_HALTED;
    ar << iMask;
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
