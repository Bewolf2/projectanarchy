/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VWallmarkManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// single particle functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VParticleWallmark::TickFunction(float dtime)
{
  float& fLifeTime(velocity[0]);
  float& fFadeOutTime(velocity[1]);
  if (fLifeTime>0.f) // otherwise infinite lifetime...
  {
    fLifeTime -= dtime;
    if (fLifeTime<=0.f)
    {
      Remove();
      return;
    }

    if (fLifeTime<fFadeOutTime)
    {
      float fBlend = fLifeTime/fFadeOutTime;
      float& fOriginalAlpha(velocity[2]);
      color.a = (UBYTE)(fOriginalAlpha*fBlend);
    }
  }
}

void VParticleWallmark::Remove()
{
  VASSERT(m_pOwner);
  m_pOwner->RemoveWallmark(this);
}


void VParticleWallmark::InflateBoundingBox(hkvAlignedBBox &bbox)
{
  hkvVec3 vDist(distortion[0],distortion[1],distortion[2]);
  float fRadius = vDist.getLength();
  fRadius = hkvMath::Max(fRadius,size);
  hkvAlignedBBox thisBox(hkvVec3 (pos[0]-fRadius,pos[1]-fRadius,pos[2]-fRadius), hkvVec3 (pos[0]+fRadius,pos[1]+fRadius,pos[2]+fRadius));
  bbox.expandToInclude(thisBox);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// group functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


VParticleWallmarkGroup::VParticleWallmarkGroup(int iCount, VTextureObject *pTexture, VIS_TransparencyType eBlending, bool bApplyDeferredLighting)
{
  m_iCacheCount = hkvMath::Min(iCount,MAX_WALLMARK_CACHE_SIZE);
  for (int i=0;i<m_iCacheCount;i++)
    m_iCacheIndex[i] = m_iCacheCount-1-i;

  m_bApplyDeferredLighting = bApplyDeferredLighting;
  m_bHasFreeParticles = true;
  m_bBBoxValid = true;

  SetParticleStride(sizeof(VParticleWallmark),0);
  Init( 0, iCount);
  SetTextureObject(pTexture);
  SetTransparencyType(eBlending);
  SetUseNormals(true);
  SetUseDistortion(true);
  SetParticleCenter(0.5f,0.5f); // centered

  if (m_bApplyDeferredLighting)
  {
#ifdef HK_DEBUG
    VType *pDeferredType = Vision::GetTypeManager()->GetType("VDeferredRenderingSystem");
    VASSERT_MSG(pDeferredType!=NULL && Vision::Renderer.IsRendererNodeOfType(pDeferredType),"bApplyDeferredLighting=true may only be passed when a deferred renderer is set");
#endif
    VShaderEffectLib *pLib = Vision::GetApplication()->GetShaderProvider()->LoadDefaultParticleShaderLib();
    VCompiledTechnique *pTech = Vision::Shaders.CreateTechnique("DeferredLitParticles",NULL,NULL,EFFECTCREATEFLAG_NONE, pLib);
    SetRenderOrder(VRH_PRE_OCCLUSION_TESTS);  // rendered as part of g-buffer geometry
    this->SetTechnique(pTech);
  } else
  {
    SetRenderOrder(VRH_DECALS);  // wallmarks must be rendered before other particles
  }
    
  SetObjectFlag(VObjectFlag_AutoDispose);
}


VParticleWallmark* VParticleWallmarkGroup::TryGetFreeParticle()
{
  if (!m_bHasFreeParticles) // early out
    return NULL;
  VParticleWallmark* pArray = (VParticleWallmark *)GetParticles();
  while (m_iCacheCount>0)
  {
    m_iCacheCount--;
    int iIndex = m_iCacheIndex[m_iCacheCount];
    VParticleWallmark& p(pArray[iIndex]);
    if (p.IsValid())
      continue;
    m_iHighWaterMark = hkvMath::Max(m_iHighWaterMark,iIndex+1);
    m_bBBoxValid = false;
    p.valid = true;
    p.m_pOwner = this;
    return &p;
  }

  FillCache();
  return TryGetFreeParticle(); // can return NULL
}


void VParticleWallmarkGroup::FillCache()
{
  VParticleWallmark* p = (VParticleWallmark *)GetParticles();
  const int iCount = GetNumOfParticles();
  for (int i=0;i<iCount && m_iCacheCount<MAX_WALLMARK_CACHE_SIZE;i++,p++)
    if (!p->IsValid())
      m_iCacheIndex[m_iCacheCount++] = i;

  m_bHasFreeParticles = m_iCacheCount>0;
}


void VParticleWallmarkGroup::RemoveWallmark(VParticleWallmark *p)
{
  m_bHasFreeParticles = true;
  m_bBBoxValid = false;
  p->valid = 0;
  VParticleWallmark* pFirst = (VParticleWallmark *)GetParticles();
  int iIndex = (int)(p - pFirst);
  VASSERT(iIndex>=0 && iIndex<GetNumOfParticles());
  if (m_iCacheCount<MAX_WALLMARK_CACHE_SIZE)
    m_iCacheIndex[m_iCacheCount++] = iIndex;
}

void VParticleWallmarkGroup::TickFunction(float dtime)
{
  VParticleWallmark* p = (VParticleWallmark *)GetParticles();
  const int iCount = GetNumOfParticles();
  int iHighWater = -1;
  for (int i=0;i<iCount;i++,p++)
    if (p->IsValid())
    {
      p->TickFunction(dtime);
      iHighWater = i;
    }
  m_iHighWaterMark = iHighWater+1;
  if (!m_bBBoxValid)
    RecomputeBoundingBox();
/* DEBUG rendering:
  if (m_BoundingBox.IsValid())
    Vision::Game.DrawBoundingBox(m_BoundingBox, VColorRef(100,200,255,255));
    */
}

void VParticleWallmarkGroup::RecomputeBoundingBox()
{
  VParticleWallmark* p = (VParticleWallmark *)GetParticles();
  hkvAlignedBBox oldBBox = m_BoundingBox; 
  m_BoundingBox.setInvalid();
  for (int i=0;i<m_iHighWaterMark;i++,p++)
    if (p->IsValid())
      p->InflateBoundingBox(m_BoundingBox);

  if (m_BoundingBox.isValid())
  {
    // actually all other tests are too error-prone for frequently resizing boxes
    if (m_spVisObj==NULL)
      m_spVisObj = new VisVisibilityObjectAABox_cl(VISTESTFLAGS_FRUSTUMTEST|VISTESTFLAGS_ACTIVE);
    m_spVisObj->SetActivate(TRUE);

    // build visibility assignment on our own. automatic update is too error prone by going through portals
    if (oldBBox!=m_BoundingBox)
    {
      m_spVisObj->SetWorldSpaceBoundingBox(m_BoundingBox);
      VisObject3DVisData_cl *pVisData = m_spVisObj->GetVisData();
      pVisData->SetAutomaticUpdate(false);
      VisVisibilityZone_cl *pZones[4096];
      const int iVisCount = Vision::GetSceneManager()->FindVisibilityZones(m_BoundingBox,pZones,4096);
      pVisData->UpdateNodeList(iVisCount,pZones);
    }
    SetVisibilityObject(m_spVisObj);
    SetVisibleBitmask(0xffffffff);
  }
  else
  {
    if (m_spVisObj)
      m_spVisObj->SetActivate(FALSE);
    SetVisibleBitmask(0);
  }
  m_bBBoxValid = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// manager functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


VWallmarkManager VWallmarkManager::g_WallmarkManager;

int VWallmarkManager::PROFILING_WALLMARK_CREATION = 0;
int VWallmarkManager::PROFILING_WALLMARK_SIMULATION = 0;
int VWallmarkManager::PROFILING_WALLMARK_RENDERING = 0;
int VWallmarkManager::PROFILING_WALLMARK_CLEANUP = 0;

VWallmarkManager::VWallmarkManager()
  : m_TempGeoInstanceCollection(0), m_TempEntityCollection(0)
  , m_iPrimaryOpaquePassRenderOrder(VRH_PRE_OCCLUSION_TESTS)
  , m_iSecondaryOpaquePassRenderOrder(0)
  , m_iTransparentPassRenderOrder(VRH_POST_OCCLUSION_TESTS)
{
  m_eProjectedWMPassTypes = VPT_Undefined;
  m_iGeomRefHashMask = 0;
  m_bSimulationCallbackRegistered = m_bRenderCallbackRegistered = m_bUnloadWorldCallbackRegistered = false;
}

void VWallmarkManager::OneTimeInit()
{
  // callbacks see EnsureXYZ function

#ifdef PROFILING
  if (PROFILING_WALLMARK_CREATION==0)
  {
    PROFILING_WALLMARK_CREATION  = Vision::Profiling.GetFreeElementID();
    PROFILING_WALLMARK_SIMULATION  = Vision::Profiling.GetFreeElementID();
    PROFILING_WALLMARK_RENDERING  = Vision::Profiling.GetFreeElementID();
    PROFILING_WALLMARK_CLEANUP  = Vision::Profiling.GetFreeElementID();
    VProfilingNode* pGroup = Vision::Profiling.AddGroup("Wallmarks");
      Vision::Profiling.AddElement(PROFILING_WALLMARK_CREATION,   "Wallmark creation",TRUE,pGroup);
      Vision::Profiling.AddElement(PROFILING_WALLMARK_SIMULATION, "Wallmark simulation",TRUE,pGroup);
      Vision::Profiling.AddElement(PROFILING_WALLMARK_RENDERING,  "Projected wallmark rendering",TRUE,pGroup);
      Vision::Profiling.AddElement(PROFILING_WALLMARK_CLEANUP,  "Projected wallmark geometry cleanup",TRUE,pGroup);
  }
#endif

  // vForge and application lifetime management
  Vision::Callbacks.OnReassignShaders += this;
}

void VWallmarkManager::OneTimeDeInit()
{
  if (m_bSimulationCallbackRegistered)
    Vision::Callbacks.OnUpdateSceneFinished -= this;
  if (m_bUnloadWorldCallbackRegistered)
    Vision::Callbacks.OnWorldDeInit -= this;  
  if (m_bRenderCallbackRegistered)
  {
    Vision::Callbacks.OnRenderHook -= this;
    VisZoneResourceManager_cl::GlobalManager().OnResourceChanged -= this;
    VisStaticGeometryInstance_cl::OnStaticGeometryInstanceDestroyed -= this;
  }
  m_bSimulationCallbackRegistered = m_bRenderCallbackRegistered = m_bUnloadWorldCallbackRegistered = false;

  Vision::Callbacks.OnReassignShaders -= this;
}


VWallmarkManager& VWallmarkManager::GlobalManager()
{
  return g_WallmarkManager;
}

void VWallmarkManager::OnZoneLoaded(VisZoneResource_cl *pZone)
{
  // new zone is loaded: update all wallmarks that intersect with that zone
  const hkvAlignedBBox &zoneBox = pZone->m_BoundingBox;
  const int iWallmarkCount = m_AllProjectedWallmarks.Count();
  for (int i=0;i<iWallmarkCount;i++)
  {
    VProjectedWallmark *pProjWallmark = m_AllProjectedWallmarks.GetAt(i);
    if (zoneBox.overlaps(pProjWallmark->GetBoundingBox()))
      pProjWallmark->m_bPrimDirty = true;
  }
}

void VWallmarkManager::OnZoneUnloading(VisZoneResource_cl *pZone)
{
  // zone is to be unloaded: filter out primitives with that zone reference
  const int iWallmarkCount = m_AllProjectedWallmarks.Count();
  for (int i=0;i<iWallmarkCount;i++)
  {
    VProjectedWallmark *pProjWallmark = m_AllProjectedWallmarks.GetAt(i);
    VisStaticGeometryInstanceCollection_cl &wmGiList = pProjWallmark->m_GeoInstanceList;  
    const int iNum = wmGiList.GetNumEntries();
    for (int j=0;j<iNum;j++)
    {
      VisStaticGeometryInstance_cl *pInst = wmGiList.GetEntry(j);
      if (pInst->GetParentZone()==pZone)
        wmGiList.FlagForRemoval(j);
    }
    wmGiList.RemoveFlagged();
  }
}

  
void VWallmarkManager::OnStaticGeometryDeleted(VisStaticGeometryInstance_cl *pGeom)
{
  const int iWallmarkCount = m_AllProjectedWallmarks.Count();
  for (int i=0;i<iWallmarkCount;i++)
  {
    VProjectedWallmark *pProjWallmark = m_AllProjectedWallmarks.GetAt(i);
    VisStaticGeometryInstanceCollection_cl &wmGiList = pProjWallmark->m_GeoInstanceList;  
    const int iNum = wmGiList.GetNumEntries();
    for (int j=0;j<iNum;j++)
      if (wmGiList.GetEntry(j)==pGeom)
        wmGiList.FlagForRemoval(j);
    wmGiList.RemoveFlagged();
  }
}



void VWallmarkManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnRenderHook)
  {
    VISION_PROFILE_FUNCTION(PROFILING_WALLMARK_RENDERING);
    // render projected wallmarks
    int iEntry = ((VisRenderHookDataObject_cl *)pData)->m_iEntryConst;
    if (iEntry == m_iPrimaryOpaquePassRenderOrder && ((m_eProjectedWMPassTypes & VPT_PrimaryOpaquePass) != 0))
      RenderProjectedWallmarks(VPT_PrimaryOpaquePass);
    else if(iEntry == m_iSecondaryOpaquePassRenderOrder && ((m_eProjectedWMPassTypes & VPT_SecondaryOpaquePass) != 0))
      RenderProjectedWallmarks(VPT_SecondaryOpaquePass);
    else if (iEntry == m_iTransparentPassRenderOrder && ((m_eProjectedWMPassTypes & VPT_TransparentPass)!=0))
      RenderProjectedWallmarks(VPT_TransparentPass);
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    VISION_PROFILE_FUNCTION(PROFILING_WALLMARK_SIMULATION);
    // simulate particle wallmarks
    int iCount = m_AllWallmarkGroups.Count();
    for (int i=0;i<iCount;i++)
      m_AllWallmarkGroups.GetAt(i)->TickFunction(Vision::GetTimer()->GetTimeDifference());

    // simulate projected wallmarks
    iCount = m_FadingProjectedWallmarks.Count();
    for (int i=0;i<iCount;i++)
      if (m_FadingProjectedWallmarks.GetAt(i)->TickFunction(Vision::GetTimer()->GetTimeDifference()))
      {
        m_FadingProjectedWallmarks.GetAt(i)->DisposeObject();
        i--;iCount--;
      }      
    return;
  }
  // respond to some zone streaming events (clean/rebuild wallmark primitive references) 
  if (pData->m_pSender==&VisZoneResourceManager_cl::GlobalManager().OnResourceChanged)
  {
    VISION_PROFILE_FUNCTION(PROFILING_WALLMARK_CLEANUP);
    VisResourceInfoDataObject_cl *pResData = (VisResourceInfoDataObject_cl *)pData;
    if (pResData->m_iAction==VRESOURCECHANGEDFLAG_LOADEDRESOURCE)
      OnZoneLoaded((VisZoneResource_cl *)pResData->m_pResource);
    else if (pResData->m_iAction==VRESOURCECHANGEDFLAG_UNLOADINGRESOURCE)
      OnZoneUnloading((VisZoneResource_cl *)pResData->m_pResource);
    return;
  }
  // respond to deletion of static geometry instances (that are not part of zones)
  if (pData->m_pSender==&VisStaticGeometryInstance_cl::OnStaticGeometryInstanceDestroyed)
  {
    VISION_PROFILE_FUNCTION(PROFILING_WALLMARK_CLEANUP);
    VisStaticGeometryInstance_cl *pInst =((VisStaticGeometryInstanceObject_cl *)pData)->m_pInstance;
    if ((m_iGeomRefHashMask & GetGeomHashMask(pInst))!=0) // effective early out
      OnStaticGeometryDeleted(pInst);
    return;
  }
  // OnReassignShaders
  if (pData->m_pSender==&Vision::Callbacks.OnReassignShaders)
  {
    // Shaders will be re-created the next time they're used.
    DeleteWallmarkShaders();

    // Re-create shaders for wallmarks with custom effects.
    const int iWallmarkCount = m_AllProjectedWallmarks.Count();
    for (int i = 0; i < iWallmarkCount; i++)
    {
      VProjectedWallmark *pProjWallmark = m_AllProjectedWallmarks.GetAt(i);
      VCompiledEffectPtr spEffect = pProjWallmark->m_spCustomEffect;

      if (spEffect != NULL && spEffect->GetSourceEffect() != NULL)
      {
        const char *szFXName  = spEffect->GetSourceEffect()->GetName();
        const char *szParam   = spEffect->m_sParamStr;
        int iFlags            = spEffect->m_iCreationFlags;

        VCompiledEffect *pFX = Vision::Shaders.CreateEffect(szFXName,szParam,iFlags);

        pProjWallmark->SetCustomShaderEffect(pFX);
      }
    }
    return;
  }
  if (pData->m_pSender == &Vision::Callbacks.OnWorldDeInit)
  {
    // clean-up
    DeleteWallmarkShaders();

    m_AllWallmarkGroups.Clear();
    m_AllProjectedWallmarks.Clear();
    m_FadingProjectedWallmarks.Clear();
    m_iGeomRefHashMask = 0;
    m_eProjectedWMPassTypes = VPT_Undefined;
    return;
  }
}


VParticleWallmark* VWallmarkManager::CreateParticle(VTextureObject *pTexture, VIS_TransparencyType eBlending, bool bApplyDeferredLighting, const hkvVec3& vCenter, bool bCheckBest)
{
  EnsureSimulationCallbackRegistered();
  const int iCount = m_AllWallmarkGroups.Count();
  int iNewInitCount = INITIAL_WALLMARKS_PERGROUP; // default count
  VParticleWallmarkGroup *pBestGroup = NULL;
  float fBestVal = FLT_MAX;

  for (int i=0;i<iCount;i++)
  {
    VParticleWallmarkGroup *pGroup = m_AllWallmarkGroups.GetAt(i);
    if (pGroup->GetTextureObject()!=pTexture || pGroup->GetTransparencyType()!=eBlending || bApplyDeferredLighting!=pGroup->m_bApplyDeferredLighting)
      continue;
    if (bCheckBest)
    {
      if (!pGroup->HasFreeParticles())
        continue;
      hkvVec3 vDiff = pGroup->m_BoundingBox.getCenter() - vCenter; // prefer boxes that contain the particle
      float fThisVal = vDiff.getLengthSquared ();
      if (fThisVal<fBestVal || pBestGroup==NULL)
      {
        fBestVal = fThisVal;
        pBestGroup = pGroup;
      }
    } else
    {
      VParticleWallmark* pParticle = pGroup->TryGetFreeParticle();
      if (pParticle)
        return pParticle;
    }
    iNewInitCount *= 2; // previous group is correct type, but full, so next one should be larger
  }

  if (pBestGroup)
  {
    VParticleWallmark* pParticle = pBestGroup->TryGetFreeParticle();
    if (pParticle)
      return pParticle;
    return CreateParticle(pTexture,eBlending,bApplyDeferredLighting,vCenter,false); // fall back
  }
    
  VParticleWallmarkGroup *pNewGroup = new VParticleWallmarkGroup(hkvMath::Min(iNewInitCount,16*1024), pTexture, eBlending, bApplyDeferredLighting);
  m_AllWallmarkGroups.Add(pNewGroup);
  return pNewGroup->TryGetFreeParticle(); // must be !=NULL
}


VParticleWallmark* VWallmarkManager::CreateWallmark(
    VTextureObject *pTexture, VIS_TransparencyType eBlending,
    const hkvVec3& vCenter, const hkvMat3 &alignment,
    VColorRef color, float fLifetime, float fFadeOutTime, bool bApplyDeferredLighting)
{
  VISION_PROFILE_FUNCTION(PROFILING_WALLMARK_CREATION);
  VParticleWallmark* p = CreateParticle(pTexture,eBlending,bApplyDeferredLighting,vCenter,true);
  p->pos[0] = vCenter.x;
  p->pos[1] = vCenter.y;
  p->pos[2] = vCenter.z;
  p->color = color;
  
  hkvVec3 normal = alignment.getAxis(0);
  hkvVec3 right = alignment.getAxis(1);
  hkvVec3 up = alignment.getAxis(2);
  p->size = up.getLength()*2.f;
  normal.normalizeIfNotZero();
  p->normal[0] = normal.x;
  p->normal[1] = normal.y;
  p->normal[2] = normal.z;
  p->distortion[0] = right.x*2.f;
  p->distortion[1] = right.y*2.f;
  p->distortion[2] = right.z*2.f;

  p->velocity[0] = fLifetime + fFadeOutTime;
  p->velocity[1] = fFadeOutTime;
  p->velocity[2] = (float)color.a; // original alpha (255 based)
  return p;
}



BOOL VWallmarkManager::TryAlignWallmark(
    const hkvVec3& vCenter,
    const hkvVec3& vNormal,
    float fSize, float fRotation,
    hkvVec3& vNewCenter,
    hkvMat3 &alignment,
    float fEpsilon
    )
{
  VISION_PROFILE_FUNCTION(PROFILING_WALLMARK_CREATION);

  hkvVec3 vNewNormal(hkvNoInitialization);
  float fTraceRad = fSize;
  if (!IsTracePointOnPlane(vCenter,vNormal,fTraceRad,fEpsilon,vNewNormal))
    return false;

  hkvVec3 vRight(hkvNoInitialization),vUp(hkvNoInitialization),vRotRight(hkvNoInitialization),vRotUp(hkvNoInitialization), vDummy(hkvNoInitialization);
  if (hkvMath::Abs (vNewNormal.x)>0.5f)
    vRight.set(0.f,1.f,0.f);
  else
    vRight.set(1.f,0.f,0.f);
  vUp = vNewNormal.cross(vRight);
  vRight = vNewNormal.cross(vUp);

  float fSin = hkvMath::sinDeg (fRotation);
  float fCos = hkvMath::cosDeg (fRotation);

  vRotRight = vRight *  fCos + vUp * fSin;
  vRotUp    = vRight * -fSin + vUp * fCos;

  vRotRight.setLength(fSize*0.5f);
  vRotUp.setLength(fSize*0.5f);
  alignment.setAxisXYZ (vNewNormal,vRotRight,vRotUp);
  vNewCenter = vCenter + vNewNormal*fEpsilon;

  // check corners:
  if (!IsTracePointOnPlane(vCenter+vRotRight+vRotUp,vNewNormal,fTraceRad,fEpsilon,vDummy))
    return false;
  if (!IsTracePointOnPlane(vCenter+vRotRight-vRotUp,vNewNormal,fTraceRad,fEpsilon,vDummy))
    return false;
  if (!IsTracePointOnPlane(vCenter-vRotRight+vRotUp,vNewNormal,fTraceRad,fEpsilon,vDummy))
    return false;
  if (!IsTracePointOnPlane(vCenter-vRotRight-vRotUp,vNewNormal,fTraceRad,fEpsilon,vDummy))
    return false;

  return TRUE;
}



bool VWallmarkManager::IsTracePointOnPlane(const hkvVec3& vPos, const hkvVec3& vNormal, float fTraceRad, float fEpsilon, hkvVec3& vNewNormal)
{
  hkvVec3 vStart = vPos + vNormal*fTraceRad;
  hkvVec3 vEnd = vPos - vNormal*fTraceRad;

#ifdef WALLMARKS_USE_VISION_TRACELINES
  VisTraceLineInfo_t result;
  if (Vision::CollisionToolkit.TraceLine(vStart,vEnd,(ULONG)VIS_TRACE_ALL,(ULONG)VIS_TRACE_NONE,NULL,NULL,&result))
    return false; // no hit

  // only allow wallmarks on static geometry and terrain
  if (result.hitType != VIS_TRACETYPE_STATICGEOMETRY  &&  result.hitType != VIS_TRACETYPE_TERRAIN)
    return false;

  if (hkvMath::Abs (result.distance - fTraceRad)>fEpsilon)
    return false;

  // too much deviation from plane normal
  vNewNormal = result.primitivePlane.m_vNormal;

  float fDot = vNewNormal.dot(vNormal);
  if (hkvMath::Abs (fDot-1.f)>fEpsilon)
    return false;
#else
  IVisPhysicsModule_cl *pPhysMod = Vision::GetApplication()->GetPhysicsModule();
  VisPhysicsHit_t result;
  if (pPhysMod==NULL || !pPhysMod->Raycast(vStart,vEnd,result))
    return false;

  // only allow wallmarks on static geometry and terrain
  if (result.eHitType != VIS_TRACETYPE_STATICGEOMETRY  &&  result.eHitType != VIS_TRACETYPE_TERRAIN)
    return false;

  if (hkvMath::Abs ((result.fHitFraction - 0.5f)*fTraceRad)>fEpsilon)
    return false;

  //NOTE: result.vImpactNormal is the interpolated vertex normal and thus not useful here
  vNewNormal = vNormal;
#endif

//  Vision::Game.DrawSingleLine(vPos,vPos+vNewNormal*20.f);

  return true;
}



// Returns the wallmark shader and modifies it according to the passed VProjectedWallmark object.
VProjectorShaderPass *VWallmarkManager::GetWallmarkShader(const VProjectedWallmark *pWallmark, VisStaticGeometryType_e eGeomType)
{
  VASSERT(eGeomType==STATIC_GEOMETRY_TYPE_MESHINSTANCE || eGeomType==STATIC_GEOMETRY_TYPE_TERRAIN);
  V_COMPILE_ASSERT(STATIC_GEOMETRY_TYPE_MESHINSTANCE<4 && STATIC_GEOMETRY_TYPE_TERRAIN<4 && STATIC_GEOMETRY_TYPE_FIRSTCUSTOM<4);

  VCompiledTechnique *pTech = pWallmark->m_spCustomTechnique[eGeomType]; // first try with wallmark's own technique
  if (pTech == NULL)
    pTech = m_spWallmarkTechnique[eGeomType][pWallmark->IsLightmapped() ? 1:0];
  if (pTech == NULL)
  {
    if (Vision::Shaders.LoadShaderLibrary("\\Shaders\\Projectors.ShaderLib", SHADERLIBFLAG_HIDDEN) == NULL)
    {
      VASSERT(!"failed to load 'Shaders\\Projectors.ShaderLib'");
      return NULL;
    }

  #if defined( SUPPORTS_BORDERCOLOR )
    VTechniqueConfig defaultConfig( "", NULL );
  #else
    VTechniqueConfig defaultConfig( "NO_BORDERCOLOR", NULL );
  #endif

    m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_MESHINSTANCE][0] = Vision::Shaders.CreateTechnique("ProjectorFullbright", NULL, &defaultConfig);
    m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_MESHINSTANCE][1] = Vision::Shaders.CreateTechnique("ProjectorLightmapped", NULL, &defaultConfig);
    VASSERT_MSG(m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_MESHINSTANCE][0] && m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_MESHINSTANCE][1],"failed to create shaders for projectors");
  #if !defined(_VISION_IOS) 
    #if defined( SUPPORTS_BORDERCOLOR )
      VTechniqueConfig terrainConf("VTerrain",NULL);
    #else // defined( SUPPORTS_BORDERCOLOR )
      VTechniqueConfig terrainConf("NO_BORDERCOLOR;VTerrain",NULL);
    #endif  // defined( SUPPORTS_BORDERCOLOR )
    m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_TERRAIN][0] = Vision::Shaders.CreateTechnique("ProjectorFullbright", NULL, &terrainConf);
    m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_TERRAIN][1] = Vision::Shaders.CreateTechnique("ProjectorLightmapped", NULL, &terrainConf);
    VASSERT_MSG(m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_TERRAIN][0] && m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_TERRAIN][1],"failed to create shaders for terrain projectors. Please use latest Projectors.ShaderLib");
  #endif  // !defined(_VISION_IOS) 
    pTech = m_spWallmarkTechnique[eGeomType][pWallmark->IsLightmapped() ? 1:0];
  }
  VASSERT(pTech->GetShaderCount()==1 && "Invalid shader pass count for projectors");
  VProjectorShaderPass *pWallmarkShader = (VProjectorShaderPass *)pTech->GetShader(0);
  VASSERT(pWallmarkShader->IsOfType(VProjectorShaderPass::GetClassTypeId()));

  // update the shader registers
  pWallmarkShader->SetProperties(pWallmark);
  return pWallmarkShader;
}

void VWallmarkManager::DeleteWallmarkShaders()
{
  for (int i = 0; i < STATIC_GEOMETRY_TYPE_FIRSTCUSTOM; i++)
    m_spWallmarkTechnique[i][0] = m_spWallmarkTechnique[i][1] = NULL;
}

// renders visible wallmarks of specified pass type (pre or post, which is relevant in deferred context)
void VWallmarkManager::RenderProjectedWallmarks(VPassType_e ePassType)
{
  INSERT_PERF_MARKER_SCOPE("Wallmark Rendering (VWallmarkManager::RenderProjectedWallmarks)");

  const int iWallmarkCount = m_AllProjectedWallmarks.Count();
  IVisVisibilityCollector_cl *pVisCollector = Vision::Contexts.GetCurrentContext()->GetVisibilityCollector();
  if (!pVisCollector || !iWallmarkCount)
    return;

  const VisStaticGeometryInstanceCollection_cl *pGeoInstances = pVisCollector->GetVisibleStaticGeometryInstances();

  VisStaticGeometryInstance_cl::ResetTags();
  pGeoInstances->TagEntries();
  VisStaticGeometryInstanceCollection_cl &targetGiCollection = m_TempGeoInstanceCollection;

  VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();
  VisRenderContext_cl *pLODContext = pContext->GetLODReferenceContext();
  hkvVec3 vLODPos = pLODContext ? pLODContext->GetCamera()->GetPosition() : pContext->GetCamera()->GetPosition();

  unsigned int iContextFilter = pContext->GetRenderFilterMask();
  const VisFrustum_cl *pFrustum = pVisCollector->GetBaseFrustum();

  int i;
  for (i=0;i<iWallmarkCount;i++)
  {
    VProjectedWallmark *pProjWallmark = m_AllProjectedWallmarks.GetAt(i);
    if ((pProjWallmark->GetVisibleBitmask() & iContextFilter)==0 || (ePassType & pProjWallmark->m_ePassType) == 0)
      continue;
    pProjWallmark->PrepareForRendering();
    const VisStaticGeometryInstanceCollection_cl &wmGiList = pProjWallmark->GetStaticGeometryCollection();  

#ifdef HK_DEBUG
    const int iNum = wmGiList.GetNumEntries();
    for (int j=0;j<iNum;j++)
    {
      VisStaticGeometryInstance_cl *pInst = wmGiList.GetEntry(j);
      VASSERT_MSG(pInst && (pInst->GetGeometryType()==STATIC_GEOMETRY_TYPE_MESHINSTANCE || pInst->GetGeometryType()==STATIC_GEOMETRY_TYPE_TERRAIN), "The wallmark conains invalid primitive references")
    }
#endif

    // clip against its bounding box (primitive visibility might overestimate visible parts)
    const hkvAlignedBBox &bbox = pProjWallmark->GetBoundingBox();
    if (pProjWallmark->m_fFarClipDistance>0.f && pProjWallmark->m_fFarClipDistance<bbox.getDistanceTo(vLODPos))
      continue;
    if (pFrustum && !pFrustum->Overlaps(bbox))
      continue;

    const int iGeomFilter = pProjWallmark->GetGeometryTypeFilterMask();
    if (iGeomFilter&PROJECTOR_AFFECTS_STATICMESHES)
    {
      // standard geometry
      targetGiCollection.Clear();
      wmGiList.GetTaggedEntriesOfType(targetGiCollection,STATIC_GEOMETRY_TYPE_MESHINSTANCE);
      if (targetGiCollection.GetNumEntries())
      {
        // render the static geometry instances using lightmapped or non-lightmapped shader
        VProjectorShaderPass *pShader = GetWallmarkShader(pProjWallmark,STATIC_GEOMETRY_TYPE_MESHINSTANCE);
        Vision::RenderLoopHelper.RenderStaticGeometryWithShader(targetGiCollection, *pShader);
      }
    }

    if (iGeomFilter&PROJECTOR_AFFECTS_TERRAIN)
    {
      // terrain geometry (different shader)
      targetGiCollection.Clear();
      wmGiList.GetTaggedEntriesOfType(targetGiCollection,STATIC_GEOMETRY_TYPE_TERRAIN);
      if (targetGiCollection.GetNumEntries()>0)
      {
        // render the static geometry instances using lightmapped or non-lightmapped shader
        VProjectorShaderPass *pShader = GetWallmarkShader(pProjWallmark,STATIC_GEOMETRY_TYPE_TERRAIN);
        if (pShader)
          Vision::RenderLoopHelper.RenderStaticGeometryWithShader(targetGiCollection, *pShader);
      }
    }

    // entities
    if (iGeomFilter&PROJECTOR_AFFECTS_ENTITIES)
    {
      const VisEntityCollection_cl *pVisibleEntities = pVisCollector->GetVisibleEntities();
      const unsigned int iInfluenceMask = pProjWallmark->GetInfluenceBitmask();
      m_TempEntityCollection.Clear();
      const int iEntCount = pVisibleEntities->GetNumEntries();
      for (int j=0;j<iEntCount;j++)
      {
        VisBaseEntity_cl *pEntity = pVisibleEntities->GetEntry(j);
        if (pEntity==NULL || (pEntity->GetVisibleBitmask()&iInfluenceMask)==0)
          continue;
        const hkvAlignedBBox &entityBox(*pEntity->GetCurrentVisBoundingBoxPtr());
        if (!entityBox.overlaps(bbox))
          continue;
        m_TempEntityCollection.AppendEntry(pEntity);
      }
      if (m_TempEntityCollection.GetNumEntries()>0)
      {
        VProjectorShaderPass *pShader = GetWallmarkShader(pProjWallmark,STATIC_GEOMETRY_TYPE_MESHINSTANCE); // we can use this shader - VS skinning is used implicitly
        Vision::RenderLoopHelper.RenderEntitiesWithShader(m_TempEntityCollection, *pShader);
      }
    }

  }
}


VProjectedWallmark *VWallmarkManager::CreateProjectedWallmark(const hkvVec3& vDestPos, const hkvVec3& vOrigin, float radius, float depth, VTextureObject* pTexture, VIS_TransparencyType drawtype, VColorRef iColor, float rotation, float fLifeTime, float fFadeOutTime)
{
  hkvVec3 dir = vDestPos-vOrigin;
  float fDistance = dir.getLength();
  if (hkvMath::isZero (fDistance)) 
    return NULL;

  dir.normalizeIfNotZero();

  hkvVec3 upDir(0,0,-1.f);
  hkvVec3 rightDir;
  
  if (dir.z >= 0.999f || dir.z <= -0.999f)
  {
    upDir = hkvVec3(1,0,0);
  }
  rightDir = dir.cross(upDir);
  rightDir.normalizeIfNotZero();
  if (rightDir.x==0.f && rightDir.y==0.f && rightDir.z==0.f)
    rightDir.set(0,0,1.f);  

  upDir = rightDir.cross(dir);

  // rotate with angle
  float sina = hkvMath::sinDeg (rotation);
  float cosa = hkvMath::cosDeg (rotation);

  hkvVec3 up1     =  upDir*cosa + rightDir*sina;
  hkvVec3 right1  =  upDir*sina - rightDir*cosa;

  float fConeFactor = fDistance / (2.f * radius);

  VProjectedWallmark* pProjectedWM = new VProjectedWallmark(
    vOrigin,vDestPos,up1,right1,fConeFactor,fConeFactor,depth,pTexture,NULL,
    drawtype,iColor,fLifeTime,fFadeOutTime);
  V_VERIFY_MALLOC(pProjectedWM);

  return pProjectedWM;

}



VRefCountedCollection<VParticleWallmarkGroup>& VWallmarkManager::ParticleGroupInstances()
{
  return m_AllWallmarkGroups;
}

VRefCountedCollection<VProjectedWallmark>& VWallmarkManager::ProjectedWallmarkInstances()
{
  return m_AllProjectedWallmarks;
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
