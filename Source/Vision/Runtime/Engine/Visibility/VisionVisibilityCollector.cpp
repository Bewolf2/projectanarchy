/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/Engine.hpp> 
#include <Vision/Runtime/Engine/Visibility/VisionVisibilityCollector.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityLODHysteresis.hpp>

// *******************************************************************************
// *  This file is Vision's default Visibility Collector implementation and is
// *  provided in full source code with the Vision SDK to serve as a reference
// *  for custom implementations. Note that you will have to alter class names
// *  and replace the precompiled header reference (Engine.hpp) with custom
// *  #includes in order to get this code to compile.
// *******************************************************************************
#ifdef SUPPORTS_MULTITHREADING

#ifdef _VISION_PS3
extern char _binary_spu_SPUVisibilityJob_bin_start[];
extern char _binary_spu_SPUVisibilityJob_bin_size[];
bool VisionVisibilityCollector_cl::s_bMultithreaded = false;
bool VisionVisibilityCollector_cl::s_bUseWorkflow = true;
#else
bool VisionVisibilityCollector_cl::s_bMultithreaded = true;
bool VisionVisibilityCollector_cl::s_bUseWorkflow = false;
#endif

#else

bool VisionVisibilityCollector_cl::s_bMultithreaded = false;
bool VisionVisibilityCollector_cl::s_bUseWorkflow = false;

#endif


VisCallback_cl VISION_ALIGN(16) IVisVisibilityCollector_cl::OnVisibilityCollectorCreated;
VisCallback_cl VISION_ALIGN(16) IVisVisibilityCollector_cl::OnVisibilityCollectorDestroyed;


V_IMPLEMENT_DYNAMIC(IVisVisibilityCollector_cl, VisTypedEngineObject_cl, Vision::GetEngineModule());

IVisVisibilityCollector_cl::IVisVisibilityCollector_cl()
{
  m_pLODContext = NULL;
  m_pOQContext = NULL;
  m_iFilterBitmask = 0xffffffff;

  VisVisibilityCollectorDataObject_cl cbdata(&OnVisibilityCollectorCreated,this);
  cbdata.m_pSender->TriggerCallbacks(&cbdata);
}

IVisVisibilityCollector_cl::~IVisVisibilityCollector_cl()
{ 
  VisVisibilityCollectorDataObject_cl cbdata(&OnVisibilityCollectorDestroyed,this);
  cbdata.m_pSender->TriggerCallbacks(&cbdata);
}


const VisFrustum_cl *IVisVisibilityCollector_cl::GetBaseFrustum()
{
  return NULL;
}


V_IMPLEMENT_DYNAMIC(VisionVisibilityCollector_cl, IVisVisibilityCollector_cl, Vision::GetEngineModule());

VisVisibilityStreamConfig_t defaultVisStreamConfig;

VisionVisibilityCollector_cl::VisionVisibilityCollector_cl(VisSceneElementTypes_e eSceneElementTypes)
  : IVisVisibilityCollector_cl(), m_VisibleVisibilityZones(64,256), m_EntityFlags(256, 0), m_VisObjectFlags(256, 0), m_LightFlags(64, 0), m_VisibilityZoneVisitedFlags(32,0), m_VisibilityZoneFlags(32,0),
  m_TraversalProtocol(64, 128),
  m_StreamConfigs(1, defaultVisStreamConfig), targetPortal((hkvVec3*) &tempMem1[0],(hkvPlane*) &tempMem2[0], sizeof(tempMem1)/sizeof(hkvPlane))
{
  m_pVisibleEntities = new VisEntityCollection_cl(256, 256);
  m_pVisiblePrimaryOpaquePassEntities = new VisEntityCollection_cl(256, 256);
  m_pVisibleSecondaryOpaquePassEntities = new VisEntityCollection_cl(64, 64);
  m_pVisibleTransparentPassEntities = new VisEntityCollection_cl(64, 64);
  m_pVisibleForeGroundEntities = new VisEntityCollection_cl(0, 16);
  m_pVisibleVisObjects = new VisVisibilityObjectCollection_cl(256, 256);
  m_pVisibleLights = new VisLightSrcCollection_cl(16, 16);
  m_pVisibleStaticGeometryInstances = new VisStaticGeometryInstanceCollection_cl(1024, 1024);
  m_pVisiblePrimaryOpaquePassStaticGeometryInstances = new VisStaticGeometryInstanceCollection_cl(1024, 1024);
  m_pVisibleSecondaryOpaquePassStaticGeometryInstances = new VisStaticGeometryInstanceCollection_cl(256, 256);
  m_pVisibleTransparentPassStaticGeometryInstances = new VisStaticGeometryInstanceCollection_cl(256, 256);

  m_pSourceObject = NULL;
  m_fFov[0] = m_fFov[1] = 90.f;
  m_fClipPlanes[0] = 5.f;
  m_fClipPlanes[1] = 30000.f;
  m_eProjectionType = VIS_PROJECTIONTYPE_PERSPECTIVE;
  m_eContextHint = VIS_CONTEXTUSAGE_NONE;
  m_fOrthographicSize[0] = m_fOrthographicSize[1] = 512.f;

  m_iBehaviorFlags = VIS_VISCOLLECTOR_DEFAULTS_CAMERA;
  m_eSceneElementFlags = eSceneElementTypes;
  m_iContextRenderFlags = 0;

  m_FrustumStack.EnsureSize(16);
  m_iFrustumStackDepth = 0;
  m_iOcclusionQueryMinTriangles = 50;

  m_bTraceAllTerrainZones = false;

  m_pOverrideFrustum = NULL;

  m_eStatus = VIS_VISIBILITYSTATUS_READY;
  m_bUseCameraBBox = false;
  m_bUsedWorkflow = false;

  m_pStartZone = NULL;

  m_pTask = new VisVisibilityCollectorTask_cl(this);
  //  VASSERT(VStreamProcessor::IsInitialized() && "Stream processor needs to be initialized!");
  m_pWorkflow = VStreamProcessor::CreateWorkflow(8, 0);

#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
  m_pLODHysteresisManager = new VLODHysteresisManager();
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
}

VisionVisibilityCollector_cl::~VisionVisibilityCollector_cl()
{
  if (m_pTask && !( m_pTask->GetState()==TASKSTATE_FINISHED || m_pTask->GetState()==TASKSTATE_UNASSIGNED ))
    Vision::GetThreadManager()->WaitForTask(m_pTask, true);

  V_SAFE_DELETE(m_pVisibleEntities);
  V_SAFE_DELETE(m_pVisiblePrimaryOpaquePassEntities);
  V_SAFE_DELETE(m_pVisibleSecondaryOpaquePassEntities);
  V_SAFE_DELETE(m_pVisibleTransparentPassEntities);
  V_SAFE_DELETE(m_pVisibleForeGroundEntities);
  V_SAFE_DELETE(m_pVisibleVisObjects);
  V_SAFE_DELETE(m_pVisibleLights);
  V_SAFE_DELETE(m_pVisibleStaticGeometryInstances);
  V_SAFE_DELETE(m_pVisiblePrimaryOpaquePassStaticGeometryInstances);
  V_SAFE_DELETE(m_pVisibleSecondaryOpaquePassStaticGeometryInstances);
  V_SAFE_DELETE(m_pVisibleTransparentPassStaticGeometryInstances);

  V_SAFE_DELETE(m_pTask);
  if (m_pWorkflow)
    VStreamProcessor::DestroyWorkflow(m_pWorkflow);

#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
  V_SAFE_DELETE(m_pLODHysteresisManager);
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
}

const VisStaticGeometryInstanceCollection_cl* VisionVisibilityCollector_cl::GetVisibleStaticGeometryInstances() const
{
  return m_pVisibleStaticGeometryInstances;
}

const VisStaticGeometryInstanceCollection_cl* VisionVisibilityCollector_cl::GetVisibleStaticGeometryInstancesForPass(VPassType_e ePassType) const
{
  switch(ePassType)
  {
  case VPT_PrimaryOpaquePass:
    return m_pVisiblePrimaryOpaquePassStaticGeometryInstances;
  case VPT_SecondaryOpaquePass:
    return m_pVisibleSecondaryOpaquePassStaticGeometryInstances;
  case VPT_TransparentPass:
    return m_pVisibleTransparentPassStaticGeometryInstances;
  default:
    VASSERT_MSG(0, "Invalid pass type");
  }
  return NULL;
}


const VisEntityCollection_cl* VisionVisibilityCollector_cl::GetVisibleEntities() const
{
  return m_pVisibleEntities;
}

const VisEntityCollection_cl* VisionVisibilityCollector_cl::GetVisibleEntitiesForPass(VPassType_e ePassType) const
{
  switch(ePassType)
  {
  case VPT_PrimaryOpaquePass:
    return m_pVisiblePrimaryOpaquePassEntities;
  case VPT_SecondaryOpaquePass:
    return m_pVisibleSecondaryOpaquePassEntities;
  case VPT_TransparentPass:
    return m_pVisibleTransparentPassEntities;
  default:
    VASSERT_MSG(0, "Invalid pass type");
  }
  return NULL;
}

const VisEntityCollection_cl* VisionVisibilityCollector_cl::GetVisibleForeGroundEntities() const
{
  return m_pVisibleForeGroundEntities;
}


const VisVisibilityObjectCollection_cl* VisionVisibilityCollector_cl::GetVisibleVisObjects() const
{
  return m_pVisibleVisObjects;
}

const VisLightSrcCollection_cl* VisionVisibilityCollector_cl::GetVisibleLights() const 
{
  return m_pVisibleLights;
}

VisVisibilityZoneCollection_cl* VisionVisibilityCollector_cl::GetVisibleVisibilityZones()
{
  return &m_VisibleVisibilityZones;
}


void VisionVisibilityCollector_cl::ClearVisibilityData()
{
  m_pVisibleEntities->Clear();
  m_pVisiblePrimaryOpaquePassEntities->Clear();
  m_pVisibleSecondaryOpaquePassEntities->Clear();
  m_pVisibleTransparentPassEntities->Clear();
  m_pVisibleForeGroundEntities->Clear();
  m_pVisibleVisObjects->Clear();
  m_pVisibleLights->Clear();
  m_pVisibleStaticGeometryInstances->Clear();
  m_pVisiblePrimaryOpaquePassStaticGeometryInstances->Clear();
  m_pVisibleSecondaryOpaquePassStaticGeometryInstances->Clear();
  m_pVisibleTransparentPassStaticGeometryInstances->Clear();
}



void VisionVisibilityCollector_cl::OnDoVisibilityDetermination(int iFilterBitmask)
{
  if (m_eStatus != VIS_VISIBILITYSTATUS_READY)
    return;

  if (m_pTask && !( m_pTask->GetState()==TASKSTATE_FINISHED || m_pTask->GetState()==TASKSTATE_UNASSIGNED ))
    Vision::GetThreadManager()->WaitForTask(m_pTask, true);

  m_eStatus = VIS_VISIBILITYSTATUS_VISIBILITYDETERMINATION;
  m_iFrustumStackDepth = 0;
  ClearVisibilityData();

  { // trigger callbacks on all attached components: 
    VisVisibilityCollectorDataObject_cl cbdata(&Vision::Callbacks.OnStartVisibilityDetermination,this);
    cbdata.m_pSender->TriggerCallbacks(&cbdata);
    const int iComponents = Components().Count();
    for (int i=0;i<iComponents;i++)
    {
      if(IVisVisibilityCollectorComponent_cl* pComponent = vdynamic_cast<IVisVisibilityCollectorComponent_cl*>(Components().GetAt(i)))
      {
        pComponent->BeginVisibility(this);
      }
    }
  }

#ifdef SUPPORTS_MULTITHREADING
  if (s_bUseWorkflow)
  {
    m_pWorkflow->ResetStatus();
    m_pWorkflow->ResetTasks();
    m_iStreamConfigCount = 0;
    m_iFrustumCount = 0;
  }
#endif

  if (s_bMultithreaded && m_pTask!=NULL)
  {
    m_pTask->SetFilterBitmask(iFilterBitmask);
    VASSERT(m_pTask->GetState()!=TASKSTATE_EXECUTING && m_pTask->GetState()!=TASKSTATE_PENDING);
    Vision::GetThreadManager()->ScheduleTask(m_pTask, 1);
  }
  else
  {
    PerformVisibilityDetermination(iFilterBitmask);
  }
}

static void ExpandAABBbyFrustum (hkvAlignedBBox& bbox, const hkvVec3& vOrigin, const hkvVec3& vDir, const hkvVec3& vRight, const hkvVec3& vUp, float fFOVX, float fFOVY, float fNearClip, float fFarClip)
{
  float fRadX = hkvMath::tanRad (fFOVX * hkvMath::pi () / 360.f);
  float fRadY = hkvMath::tanRad (fFOVY * hkvMath::pi () / 360.f);

  // build 8 corners of frustum
  bbox.expandToInclude(vOrigin + vRight*fRadX*fNearClip + vUp*fRadY*fNearClip + vDir*fNearClip);
  bbox.expandToInclude(vOrigin - vRight*fRadX*fNearClip + vUp*fRadY*fNearClip + vDir*fNearClip);
  bbox.expandToInclude(vOrigin + vRight*fRadX*fNearClip - vUp*fRadY*fNearClip + vDir*fNearClip);
  bbox.expandToInclude(vOrigin - vRight*fRadX*fNearClip - vUp*fRadY*fNearClip + vDir*fNearClip);
  bbox.expandToInclude(vOrigin + vRight*fRadX*fFarClip + vUp*fRadY*fFarClip + vDir*fFarClip);
  bbox.expandToInclude(vOrigin - vRight*fRadX*fFarClip + vUp*fRadY*fFarClip + vDir*fFarClip);
  bbox.expandToInclude(vOrigin + vRight*fRadX*fFarClip - vUp*fRadY*fFarClip + vDir*fFarClip);
  bbox.expandToInclude(vOrigin - vRight*fRadX*fFarClip - vUp*fRadY*fFarClip + vDir*fFarClip);
}


void VisionVisibilityCollector_cl::PerformVisibilityDetermination(int iFilterBitmask)
{
  if (m_eSceneElementFlags == 0 || Vision::GetSceneManager()->GetNumVisibilityZones() == 0)
    return;

  m_VisibleVisibilityZones.Clear();
  EnsureVisibilityZoneFlagsSize();
  ClearVisibilityZoneFlags();

  m_iFilterBitmask = iFilterBitmask;
  m_bUseCameraBBox = false;

  if (m_pOverrideFrustum)
  {
    // If we are simply starting from a given base frustum, make a copy of it
    m_FrustumStack[m_iFrustumStackDepth++].CopyFrom(*m_pOverrideFrustum);
  }
  else if ((m_iBehaviorFlags & VIS_VISCOLLECTOR_USEFOV) != 0)
  {
    // Scene traversal starts with a given position and a fov which we use to construct a
    // frustum. Note that it is also possible to start traversal with no frustum at all (e.g. for
    // testing visible scene elements from a point in space).
    hkvVec3 vPos(hkvNoInitialization);
    hkvMat3 mOrientation(hkvNoInitialization);
    m_pSourceObject->GetRotationMatrix(mOrientation);
    vPos = m_pSourceObject->GetPosition();


#if defined (_VISION_PS3)
#pragma diag_push
#pragma diag_suppress=552
#endif

    bool bFlipFrustum = (m_pOQContext != NULL) && ((m_pOQContext->GetRenderFlags() & VIS_RENDERCONTEXT_FLAG_REVERSE_CULLMODE) != 0);
    bool bRes;
    if (m_eProjectionType == VIS_PROJECTIONTYPE_ORTHOGRAPHIC)
    {
      bRes = m_FrustumStack[m_iFrustumStackDepth].SetOrthographic(vPos, mOrientation, m_fOrthographicSize[0], m_fOrthographicSize[1], m_fClipPlanes[0], m_fClipPlanes[1], bFlipFrustum);
    }
    else if (m_eProjectionType == VIS_PROJECTIONTYPE_PERSPECTIVE_AUTOFRUSTUM)
    {
      VASSERT(m_pOQContext != NULL);

      bRes = m_FrustumStack[m_iFrustumStackDepth].set(vPos, mOrientation, m_pOQContext->GetViewProperties()->getProjectionMatrix(hkvClipSpaceYRange::MinusOneToOne), bFlipFrustum);
    }
    else
    {
      bRes = m_FrustumStack[m_iFrustumStackDepth].Set(vPos, mOrientation, m_fFov[0], m_fFov[1], m_fClipPlanes[0], m_fClipPlanes[1], bFlipFrustum);
      m_CameraBBox.setInvalid();
      ExpandAABBbyFrustum (m_CameraBBox, vPos,mOrientation.getAxis(0),mOrientation.getAxis(1),mOrientation.getAxis(2),m_fFov[0], m_fFov[1], m_fClipPlanes[0], m_fClipPlanes[1]);
      m_bUseCameraBBox = true;
    }

    VASSERT(bRes);
    m_iFrustumStackDepth++;

#ifdef _VISION_PS3
#pragma diag_pop
#endif


  }

  // Get the visibility zone the origin of our source object is currently in (note that we always
  // need a source object, since otherwise, we would have to dynamically determine the starting
  // visibility zone, which can take a lot of time in complex scenes). If we find that the source
  // object is outside of the vis zone, we use the override vis zone.
  VASSERT(m_pSourceObject->GetVisData()!=NULL);
  VisVisibilityZone_cl *pStartZone = m_pSourceObject->GetVisData()->GetVisibilityZoneOfOrigin();

  if (!pStartZone)
    return;

  if (!pStartZone->GetBoundingBox().contains(m_pSourceObject->GetPosition()))
  {
    hkvVec3 vPos = m_pSourceObject->GetPosition();
    hkvVec3 vDir = m_pSourceObject->GetDirection();
    // we need to start tracing where the near clip plane starts. This might be significant for shadow contexts
    vPos += vDir * m_fClipPlanes[0];
    vDir *= (m_fClipPlanes[1] - m_fClipPlanes[0]); // set length of dir
    pStartZone = Vision::GetSceneManager()->TraceIntoZone(vPos, vDir);
    if (!pStartZone)
    {
      hkvAlignedBBox refBox(m_pSourceObject->GetPosition(), m_pSourceObject->GetPosition());
      pStartZone = Vision::GetSceneManager()->FindClosestVisibilityZone(refBox);
      VASSERT(pStartZone != NULL);
    }
  }

  if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY && m_pOQContext != NULL)
    m_iTraversalProtocolSize = 0;

  // Perform (recursive) scene traversal
  TraverseScene(pStartZone);

#ifdef SUPPORTS_MULTITHREADING
  if (s_bUseWorkflow)
  {
    KickoffVisibilityWorkflow();
  }
#endif
}

inline bool VisionVisibilityCollector_cl::IsInVisibleZone(VisObject3D_cl *pObject3D)
{
  VisObject3DVisData_cl *pVisData = pObject3D->GetVisData();
  VASSERT(pVisData != NULL);
  VisVisibilityZone_cl **pZoneList;
  int iNumZones = pVisData->GetVisibilityZonesPtr(&pZoneList);
  for (int i=0; i<iNumZones; i++)
  {
    int iZoneIndex = pZoneList[i]->GetIndex();
    if (m_VisibilityZoneFlags[iZoneIndex>>3]&(char)(1<<(iZoneIndex&7)))
      return true;
  }
  return false;
}

#define MAX_PORTAL_STACK_DEPTH              1024
#define MAX_ZONES_PASSING_OCCLUSIONTEST    16384

// Things that could still be improved:
// - Make sure that each portal is scheduled only once (currently, there are
//   situations in which portals will be scheduled for occlusion test more
//   than once).
// - Perform occlusion tests also for zones which are visited after the
//   first portal visit, but e.g. with scissoring (according to the current
//   portal sequence) enabled.
void VisionVisibilityCollector_cl::HandlePortalOcclusionQueryResults()
{
  if (m_iTraversalProtocolSize==0)
    return;

  int iProtocolIndex = 0;
  bool bVisStatus[MAX_PORTAL_STACK_DEPTH];
  int iPortalStackDepth = 0;
  bVisStatus[0] = true;
  char *pFlags = (char *)m_VisibilityZoneFlags.GetDataPtr();

  VisVisibilityZone_cl *pVisibleZones[MAX_ZONES_PASSING_OCCLUSIONTEST];
  int iNumVisibleZones = 0;

  // We need to keep track whether we came to the "current" visibility zone
  // in the protocol through a portal or directly through a neighboring node.
  // This is done using the following array.
  //bool bLastWasPortal[MAX_PORTAL_STACK_DEPTH];
  //bLastWasPortal[0] = false;

  // Go through the traversal protocol again. This step is necessary so we are able to correctly
  // classify portals and visibility zones as visible/invisible depending on their occlusion query
  // result. In order to do this correctly, we need to know the exact path through the portals and
  // visibility zones that led up to each individual portal/zone.
  do
  {
    int iOperation = (int)m_TraversalProtocol[iProtocolIndex++];
    switch (iOperation)
    {
    case VIS_TRAVERSALPROTOCOL_PORTAL:
      if (bVisStatus[iPortalStackDepth] == false)
      {
        VisPortal_cl *pPortal = (VisPortal_cl *)m_TraversalProtocol[iProtocolIndex++];
        iProtocolIndex++;
        iPortalStackDepth++;
        VASSERT(iPortalStackDepth < MAX_PORTAL_STACK_DEPTH);
        bVisStatus[iPortalStackDepth] = false;
        // Portal has passed frustum, but not occlusion test -> reschedule immediately
        if (pPortal->GetOccQueryObject()==NULL)
          continue;
        int iOQIndex = pPortal->GetOccQueryObject()->GetNumber();
        m_pOQContext->ScheduleOcclusionTest(iOQIndex);
      }
      else
      {
        VisPortal_cl *pPortal = (VisPortal_cl *)m_TraversalProtocol[iProtocolIndex++];
        iPortalStackDepth++;
        if (pPortal->GetOccQueryObject()==NULL)
          continue;
        int iOQIndex = pPortal->GetOccQueryObject()->GetNumber();
        if (m_pOQContext->HasOcclusionTestPassed(iOQIndex))
        {
          // Portal has passed frustum and occlusion test -> wait a while, then reschedule
          if (m_pOQContext->ShouldScheduleOcclusionTest(iOQIndex))
            m_pOQContext->ScheduleOcclusionTest(iOQIndex);
          bVisStatus[iPortalStackDepth] = true;
        }
        else
        {
          // Portal has passed frustum, but not occlusion test -> reschedule immediately
          m_pOQContext->ScheduleOcclusionTest(iOQIndex);
          bVisStatus[iPortalStackDepth] = false;
        }
      }
      break;
    case VIS_TRAVERSALPROTOCOL_REMOVEPORTAL:
      iPortalStackDepth--;
      break;
    case VIS_TRAVERSALPROTOCOL_ZONE:
      VisVisibilityZone_cl *pZone = (VisVisibilityZone_cl *)m_TraversalProtocol[iProtocolIndex++];
      int iOQIndex = pZone->GetOccQueryObject().GetNumber();
      int iZoneIndex = pZone->GetIndex();
      if (bVisStatus[iPortalStackDepth] == false)
      {
        pFlags[iZoneIndex>>3]&=~(char)(1<<(iZoneIndex&7));
      }
      else
      {
        if (m_pOQContext->HasOcclusionTestPassed(iOQIndex))
        {
          if (iPortalStackDepth==0 && m_pOQContext->ShouldScheduleOcclusionTest(iOQIndex))
            m_pOQContext->ScheduleOcclusionTest(iOQIndex);
          pVisibleZones[iNumVisibleZones++] = pZone;
        }
        else
        {
          m_pOQContext->ScheduleOcclusionTest(iOQIndex);
        }
        VASSERT(iNumVisibleZones < MAX_ZONES_PASSING_OCCLUSIONTEST);
      }
      break;
    }
  }
  while (iProtocolIndex < m_iTraversalProtocolSize);

  // Multiple portal chains may have led to the same zone, some of which were classified as visible, others as invisible.
  // The following block of code ensures that portals which have been classified as visible at least once actually remain
  // visible.
  for (int i=0; i<iNumVisibleZones; i++)
  {
    int iZoneIndex = pVisibleZones[i]->GetIndex();
    pFlags[iZoneIndex>>3]|=(char)(1<<(iZoneIndex&7));
  }

  VASSERT(iPortalStackDepth==0);
}


void VisionVisibilityCollector_cl::PostProcessVisibilityResults()
{
  if (m_pTask && s_bMultithreaded && !( m_pTask->GetState()==TASKSTATE_FINISHED || m_pTask->GetState()==TASKSTATE_UNASSIGNED ))
    Vision::GetThreadManager()->WaitForTask(m_pTask, true);

#ifdef SUPPORTS_MULTITHREADING
  if (s_bUseWorkflow)
  {
    FinishVisibilityWorkflow();
  }
#endif

  if (m_eStatus != VIS_VISIBILITYSTATUS_VISIBILITYDETERMINATION)
    return;

  m_eStatus = VIS_VISIBILITYSTATUS_POSTPROCESS;

  ClearSceneElementFlags();

  // ------------------------------------------------------------------------------------------------------------
  // Add directional lights - they are visible everywhere in the scene!
  // ------------------------------------------------------------------------------------------------------------
  const VisLightSrcCollection_cl &directionalLightList(VisLightSource_cl::GetGlobalDirectionalLightList());
  const int iNumDirectionalLights = directionalLightList.GetNumEntries();
  for (int i=0; i<iNumDirectionalLights; i++)
  {
    VisLightSource_cl *pLight = directionalLightList.GetEntry(i);
    VASSERT(pLight!=NULL);
    if ((m_iFilterBitmask & pLight->GetVisibleBitmask())!=0)
      m_pVisibleLights->AppendEntry(pLight);
  }


  // ------------------------------------------------------------------------------------------------------------
  // Handle occlusion query results for portals and visibility zones
  // ------------------------------------------------------------------------------------------------------------

  bool bOcclusionResultsFetched = false;
  int i;
  if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY && 
    m_pOQContext != NULL && 
    (m_pOQContext->GetRenderFlags()&VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY) && 
    Vision::Renderer.GetUseHardwareOcclusion() == VIS_ENABLED)
  {
    m_pOQContext->FetchOcclusionTestResults();
    bOcclusionResultsFetched = true;

    HandlePortalOcclusionQueryResults();

    char *pFlags = (char *)m_VisibilityZoneFlags.GetDataPtr();
    int iNumZones = m_VisibleVisibilityZones.GetNumEntries();
    VisVisibilityZone_cl **pZones = m_VisibleVisibilityZones.GetDataPtr();
    for (i=0; i<iNumZones; i++, pZones++)
    {
      int iZoneIndex = (*pZones)->GetIndex();
      int iOQIndex = (*pZones)->GetOccQueryObject().GetNumber();

      if (!m_pOQContext->HasOcclusionTestPassed(iOQIndex) || !(pFlags[iZoneIndex>>3]&(unsigned char)(1<<(iZoneIndex&7))) )
      {
        pFlags[iZoneIndex>>3] &= ~(char)(1<<(iZoneIndex&7));
      }
      else
      {
        char *pStaticGeoFlags = (char *)m_StaticGeometryInstanceFlags.GetDataPtr();
        const VisStaticGeometryInstanceCollection_cl *pStaticGeo = (*pZones)->GetStaticGeometryInstances();
        int iNumStaticGeoInst = pStaticGeo->GetNumEntries();
        VisStaticGeometryInstance_cl **pGI = pStaticGeo->GetDataPtr();
        for (int j=0; j<iNumStaticGeoInst; j++, pGI++)
        {
          int iNum = (*pGI)->GetNumber();
          pStaticGeoFlags[iNum>>3] |= (char)(1<<(iNum&7));
        }
      }
    }

  }


  // ------------------------------------------------------------------------------------------------------------
  // Filter out duplicate scene elements and scene elements in visibility zones which have failed occlusion query
  // ------------------------------------------------------------------------------------------------------------

  int iNumGeomInstances = m_pVisibleStaticGeometryInstances->GetNumEntries();

  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_WORLDGEOMETRY) && (iNumGeomInstances>0) )
  {

    // If a visibility zone was classified as invisible due to occlusion query results, we can
    // remove all instances which are ONLY in this zone from the list of visible instances.
    if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY && Vision::Renderer.GetUseHardwareOcclusion() == VIS_ENABLED)
    {
      char *pFlags = (char *)m_StaticGeometryInstanceFlags.GetDataPtr();
      VisStaticGeometryInstance_cl **pGeomInstances = (VisStaticGeometryInstance_cl**)m_pVisibleStaticGeometryInstances->GetDataPtr();
      for (i=0; i<iNumGeomInstances; i++, pGeomInstances++)
      {
        int iGeomInstanceIndex = (*pGeomInstances)->GetNumber();
        if (!(pFlags[iGeomInstanceIndex>>3]&(char)(1<<(iGeomInstanceIndex&7))))
          m_pVisibleStaticGeometryInstances->FlagForRemoval(i);
      }

      // TODO: Maybe we can speed this up
      int iRequiredSize = (VisStaticGeometryInstance_cl::ElementManagerGetSize() >> 3) + 1;
      memset(m_StaticGeometryInstanceFlags.GetDataPtr(), 0, iRequiredSize); 
    }
  }

  char *pFlags = (char *)m_EntityFlags.GetDataPtr();
  int iNumEntities = m_pVisibleEntities->GetNumEntries();
  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_ENTITIES) && (iNumEntities>0) )
  {
    VisBaseEntity_cl **pEntities = m_pVisibleEntities->GetDataPtr();

    // Remove duplicates / invalid entities
    for (i=0; i<iNumEntities; i++, pEntities++)
    {
      int iEntityIndex = (*pEntities)->GetNumber();
      if (!(pFlags[iEntityIndex>>3]&(char)(1<<(iEntityIndex&7))))
        pFlags[iEntityIndex>>3]|=(char)(1<<(iEntityIndex&7));
      else
        m_pVisibleEntities->FlagForRemoval(i);

      // Sanity check: Remove elements that are not registered with the element manager any more
      if(iEntityIndex == VIS_INVALID)
        m_pVisibleEntities->FlagForRemoval(i);

      // Sanity check: Remove elements that possibly have the dead flag
      if(*pEntities && (*pEntities)->IsRemoved())
        m_pVisibleEntities->FlagForRemoval(i); 
    }


    // If a visibility zone was classified as invisible due to occlusion query results, we can
    // remove all scene elements which are ONLY in this zone from the list of visible elements.
    if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY)
    {
#pragma warning(suppress:6246)
      VisBaseEntity_cl **pEntities = m_pVisibleEntities->GetDataPtr();
      for (i=0; i<iNumEntities; i++, pEntities++)
      {
        if (*pEntities!=NULL && !IsInVisibleZone(*pEntities))
        {
          int iEntity = (*pEntities)->GetNumber();
          pFlags[iEntity>>3] &= ~(char)(1<<(iEntity&7));
          m_pVisibleEntities->FlagForRemoval(i);
        }
      }
    }
  }

  pFlags = (char *)m_LightFlags.GetDataPtr();
  int iNumLights = m_pVisibleLights->GetNumEntries();
  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_LIGHTS) && (iNumLights>0) )
  {
    VisLightSource_cl **pLights = m_pVisibleLights->GetDataPtr();
    for (i=0; i<iNumLights; i++, pLights++)
    {
      int iLightIndex = (*pLights)->GetNumber();

      // Remove duplicates
      if (!(pFlags[iLightIndex>>3]&(char)(1<<(iLightIndex&7))))
      {
        pFlags[iLightIndex>>3]|=(char)(1<<(iLightIndex&7));
      }
      else
        m_pVisibleLights->FlagForRemoval(i);
    }

    // If a visibility zone was classified as invisible due to occlusion query results, we can
    // remove all scene elements which are ONLY in this zone from the list of visible elements.
    if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY && Vision::Renderer.GetUseHardwareOcclusion() == VIS_ENABLED)
    {
#pragma warning(suppress:6246)
      VisLightSource_cl **pLights = m_pVisibleLights->GetDataPtr();
      for (i=0; i<iNumLights; i++, pLights++)
        if (*pLights!=NULL && !IsInVisibleZone(*pLights) && (*pLights)->GetType() != VIS_LIGHT_DIRECTED)
        {
          int iLight = (*pLights)->GetNumber();
          pFlags[iLight>>3] &= ~(char)(1<<(iLight&7));
          m_pVisibleLights->FlagForRemoval(i);
        }
    }

  }

  pFlags = (char *)m_VisObjectFlags.GetDataPtr();
  int iNumVisObjects = m_pVisibleVisObjects->GetNumEntries();
  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_VISOBJECTS) && (iNumVisObjects>0) )
  {
    VisVisibilityObject_cl **pVisObjects = m_pVisibleVisObjects->GetDataPtr();

    // Remove duplicates
    for (i=0; i<iNumVisObjects; i++, pVisObjects++)
    {
      int iVisObjIndex = (*pVisObjects)->GetNumber();
      if (!(pFlags[iVisObjIndex>>3]&(char)(1<<(iVisObjIndex&7))))
        pFlags[iVisObjIndex>>3]|=(char)(1<<(iVisObjIndex&7));
      else
        m_pVisibleVisObjects->FlagForRemoval(i);
    }

    // If a visibility zone was classified as invisible due to occlusion query results, we can
    // remove all scene elements which are ONLY in this zone from the list of visible elements.
    if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY && Vision::Renderer.GetUseHardwareOcclusion() == VIS_ENABLED)
    {
#pragma warning(suppress:6246)
      VisVisibilityObject_cl **pVisObjects = m_pVisibleVisObjects->GetDataPtr();
      for (i=0; i<iNumVisObjects; i++, pVisObjects++)
      {
        if (*pVisObjects!=NULL && !IsInVisibleZone(*pVisObjects))
        {
          int iVisObj = (*pVisObjects)->GetNumber();
          pFlags[iVisObj>>3] &= ~(char)(1<<(iVisObj&7));
          m_pVisibleVisObjects->FlagForRemoval(i);
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------------------------
  // Filter out scene elements (entities / visibility objects) which failed hw occlusion test
  // ------------------------------------------------------------------------------------------------------------

  // We use index values instead of pointers here (for performance reasons)
  if (m_pOQContext != NULL && (m_pOQContext->GetRenderFlags()&VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY) && Vision::Renderer.GetUseHardwareOcclusion() == VIS_ENABLED)
  {
    if (!bOcclusionResultsFetched)
      m_pOQContext->FetchOcclusionTestResults();

    if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_ENTITIES) && (iNumEntities>0) )
    {
      pFlags = (char *)m_EntityFlags.GetDataPtr();

      iNumEntities = m_pVisibleEntities->GetNumEntries();
      VisBaseEntity_cl **pEntities = m_pVisibleEntities->GetDataPtr();
      for (i=0; i<iNumEntities; i++, pEntities++)
      {
        // collection may now contain null-entries (due to duplicate removal, handling of occlusion query results, etc.)
        if (*pEntities == NULL)
          continue;

        // never schedule entities which are excluded from visibility testing
        if ((*pEntities)->GetExcludeFromVisTest() || (*pEntities)->GetExcludeFromOcclusionQueryTest())
          continue;

        // occlusion query does not pay off for entities which take very little time to render
        if ((*pEntities)->GetMesh()==NULL || ( (*pEntities)->GetMesh()->GetNumOfVertices() < m_iOcclusionQueryMinTriangles && (*pEntities)->GetAnimConfig() == NULL ) )
          continue;

        int iEntityIndex = (*pEntities)->GetNumber();
        int iOccQueryIndex = (*pEntities)->GetOccQueryObject().GetNumber();

        // entity has passed both portal and occlusion test. We reschedule it if it is time to schedule this occlusion query again.
        if (m_pOQContext->HasOcclusionTestPassed(iOccQueryIndex))
        {
          if (m_pOQContext->ShouldScheduleOcclusionTest(iOccQueryIndex))
            m_pOQContext->ScheduleOcclusionTest(iOccQueryIndex);
        }

        // entity has passed the portal test, but not the occlusion test. We reschedule it.
        else
        {
          pFlags[iEntityIndex>>3] &= ~(char)(1<<(iEntityIndex&7));
          m_pOQContext->ScheduleOcclusionTest(iOccQueryIndex);
          m_pVisibleEntities->FlagForRemoval(i);
        }
      }
    }

    if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_VISOBJECTS) && (iNumVisObjects>0) )
    {
      pFlags = (char *)m_VisObjectFlags.GetDataPtr();

      iNumVisObjects = m_pVisibleVisObjects->GetNumEntries();
      VisVisibilityObject_cl **pVisObjects = m_pVisibleVisObjects->GetDataPtr();
      for (i=0; i<iNumVisObjects; i++, pVisObjects++)
      {
        // collection may now contain null-entries (due to duplicate removal, handling of occlusion query results, etc.)
        if (*pVisObjects == NULL)
          continue;

        const int iFlags = (*pVisObjects)->GetVisTestFlags();

        // this test has not been performed in the workflow version
        if (m_bUsedWorkflow && !(iFlags&VISTESTFLAGS_TESTVISIBLE_NOT_OVERRIDDEN))
          if (!(*pVisObjects)->OnTestVisible(this, GetBaseFrustum()))
          {
            m_pVisibleVisObjects->FlagForRemoval(i);
            continue;
          }

          if (!(iFlags&VISTESTFLAGS_HARDWAREOCCLUSIONQUERY))
            continue;

          int iVisObjectIndex = (*pVisObjects)->GetNumber();
          int iOccQueryIndex = (*pVisObjects)->GetOccQueryObject().GetNumber();

          // visobject has passed both portal and occlusion test. We reschedule it if it is time to schedule this occlusion query again.
          if (m_pOQContext->HasOcclusionTestPassed(iOccQueryIndex))
          {
            if (m_pOQContext->ShouldScheduleOcclusionTest(iOccQueryIndex, (*pVisObjects)->GetRescheduleMask()))
              m_pOQContext->ScheduleOcclusionTest(iOccQueryIndex);
          }
          // visobject has passed the portal test, but not the occlusion test. We reschedule it.
          else
          {
            pFlags[iVisObjectIndex>>3] &= ~(char)(1<<(iVisObjectIndex&7));
            m_pOQContext->ScheduleOcclusionTest(iOccQueryIndex);
            m_pVisibleVisObjects->FlagForRemoval(i);
          }
      }
    }
  }
  else
  {
    //NOTE: this the same loop as above, needs to be executed even without hw occlusion
    iNumVisObjects = m_pVisibleVisObjects->GetNumEntries();
    VisVisibilityObject_cl **pVisObjects = m_pVisibleVisObjects->GetDataPtr();
    for (i=0; i<iNumVisObjects; i++, pVisObjects++)
    {
      // collection may now contain null-entries (due to duplicate removal, handling of occlusion query results, etc.)
      if (*pVisObjects == NULL)
        continue;

      const int iFlags = (*pVisObjects)->GetVisTestFlags();

      // this test has not been performed in the workflow version
      if (m_bUsedWorkflow && !(iFlags&VISTESTFLAGS_TESTVISIBLE_NOT_OVERRIDDEN))
      {
        if (!(*pVisObjects)->OnTestVisible(this, GetBaseFrustum()))
        {
          m_pVisibleVisObjects->FlagForRemoval(i);
          continue;
        }
      }
    }
  }

  // remove duplicates and filter
  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_WORLDGEOMETRY) && (iNumGeomInstances>0) )
  {
    const bool bConsiderVisObj = (m_eSceneElementFlags&VIS_SCENEELEMENT_VISOBJECTS)>0;
#pragma warning(suppress:6246)
    char *pFlags = (char *)m_StaticGeometryInstanceFlags.GetDataPtr();
    VisStaticGeometryInstance_cl **pGeomInstances = (VisStaticGeometryInstance_cl**)m_pVisibleStaticGeometryInstances->GetDataPtr();
    for (i=0; i<iNumGeomInstances; i++, pGeomInstances++)
    {
      VisStaticGeometryInstance_cl *pInst = (*pGeomInstances);
      // filter out those with a visibility object.  Visibility object results must be valid at this point
      if (pInst==NULL || pInst->GetSurface()==NULL || (bConsiderVisObj && pInst->GetVisibilityObject()!=NULL && !IsVisObjectVisible(pInst->GetVisibilityObject())))
      {
        m_pVisibleStaticGeometryInstances->FlagForRemoval(i);
        continue;
      }
      int iGeomInstanceIndex = pInst->GetNumber();

      // Initially 'pFlags' is nulled. If an instance is encountered for the first time,
      // its flag is thus 'false' and it will be added to the appropriate array and flagged as 'true', ...
      if (!(pFlags[iGeomInstanceIndex>>3]&(char)(1<<(iGeomInstanceIndex&7))))
      {
        pFlags[iGeomInstanceIndex>>3]|=(char)(1<<(iGeomInstanceIndex&7));
        VisSurface_cl* pSurface = pInst->GetSurface();
        VASSERT_MSG(pSurface != NULL, "static geometry instance without surface");
        if(pSurface)
        {
          VPassType_e ePassType = pSurface->GetResolvedPassType();
          switch(ePassType)
          {
          case VPT_PrimaryOpaquePass:
            m_pVisiblePrimaryOpaquePassStaticGeometryInstances->AppendEntry(pInst);
            break;
          case VPT_SecondaryOpaquePass:
            m_pVisibleSecondaryOpaquePassStaticGeometryInstances->AppendEntry(pInst);
            break;
          case VPT_TransparentPass:
            m_pVisibleTransparentPassStaticGeometryInstances->AppendEntry(pInst);
            break;
          case VPT_AUTO:
            VASSERT_ONCE_MSG(0, "Pass type should already be resolved here.");
            break;
          default:
            VASSERT_MSG(0, "unhandled pass type");
          }
        }
      }
      //... in the case that its flag is 'true' we have encountered this instance already in the past and flagged it.
      // Therefore, this instance is a duplicate and will be removed.
      else
      {
        m_pVisibleStaticGeometryInstances->FlagForRemoval(i);
      }
    }

    m_pVisibleStaticGeometryInstances->RemoveFlagged();
  }

  // ------------------------------------------------------------------------------------------------------------
  // Add foreground entities
  // ------------------------------------------------------------------------------------------------------------

  const VisEntityCollection_cl &foregroundEntities = Vision::Game.GetForegroundEntities();
  unsigned int iFgEntCount = foregroundEntities.GetNumEntries();
  VisBaseEntity_cl **pFgEnt = foregroundEntities.GetDataPtr();
  m_pVisibleForeGroundEntities->EnsureSize(iFgEntCount); // usually they are all visible
  for (int iFgEnt = 0; iFgEnt < iFgEntCount; iFgEnt++, pFgEnt++)
  {
    if ((*pFgEnt)!=NULL)
    {
      if (!(*pFgEnt)->GetMesh())
        continue;

      if (! (m_iFilterBitmask & (*pFgEnt)->GetVisibleBitmask())) 
        continue;

      if ((*pFgEnt)->GetExcludeFromVisTest())
        continue;

      m_pVisibleForeGroundEntities->AppendEntryFast(*pFgEnt);
    }
  }

  // ------------------------------------------------------------------------------------------------------------
  // Remove scene elements previously flagged for removal
  // ------------------------------------------------------------------------------------------------------------

  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_ENTITIES) && (iNumEntities>0) )
  {
    m_pVisibleEntities->RemoveFlagged();

    // sort entities
    if (Vision::Renderer.GetEntitySortingCriterion() != 0)
      m_pVisibleEntities->Sort(Vision::Renderer.GetEntitySortingCriterion());

    iNumEntities = m_pVisibleEntities->GetNumEntries();
    VisBaseEntity_cl **pCurEntity = m_pVisibleEntities->GetDataPtr();
    VisBaseEntity_cl **pEnd = pCurEntity + iNumEntities;
    m_pVisiblePrimaryOpaquePassEntities->EnsureSize(iNumEntities);
    m_pVisibleSecondaryOpaquePassEntities->EnsureSize(iNumEntities);
    m_pVisibleTransparentPassEntities->EnsureSize(iNumEntities);

    for(; pCurEntity < pEnd; ++pCurEntity)
    {
      VisShaderSet_cl* pShaderSet = (*pCurEntity)->GetActiveShaderSet();
      unsigned char passTypes = pShaderSet->GetPassTypes();
      if(passTypes & VPT_PrimaryOpaquePass)
        m_pVisiblePrimaryOpaquePassEntities->AppendEntryFast(*pCurEntity);
      if(passTypes & VPT_SecondaryOpaquePass)
        m_pVisibleSecondaryOpaquePassEntities->AppendEntryFast(*pCurEntity);
      if(passTypes & VPT_TransparentPass)
        m_pVisibleTransparentPassEntities->AppendEntryFast(*pCurEntity);
      VASSERT_MSG((passTypes & ~(VPT_PrimaryOpaquePass | VPT_SecondaryOpaquePass | VPT_TransparentPass)) == 0, "Invalid pass type");
    }
  }

  // sort transparent entities
  if (Vision::Renderer.GetSortTranslucentPrimitives())
    m_pVisibleTransparentPassEntities->Sort(VIS_API_SORTING_DISTANCE);

  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_LIGHTS) && (iNumLights>0) )
    m_pVisibleLights->RemoveFlagged();

  if ( (m_eSceneElementFlags & VIS_SCENEELEMENT_VISOBJECTS) && (iNumVisObjects>0) )
    m_pVisibleVisObjects->RemoveFlagged();

  if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY)
    m_VisibleVisibilityZones.RemoveFlagged();


  // ------------------------------------------------------------------------------------------------------------
  // Sort scene elements
  // ------------------------------------------------------------------------------------------------------------
  if (Vision::Renderer.GetStaticGeometrySortingCriterion() != 0)
  {
    m_pVisiblePrimaryOpaquePassStaticGeometryInstances->Sort(Vision::Renderer.GetStaticGeometrySortingCriterion());
    m_pVisibleSecondaryOpaquePassStaticGeometryInstances->Sort(Vision::Renderer.GetStaticGeometrySortingCriterion());
    m_pVisibleStaticGeometryInstances->Sort(Vision::Renderer.GetStaticGeometrySortingCriterion());
  }

  if (Vision::Renderer.GetSortTranslucentPrimitives())
    m_pVisibleTransparentPassStaticGeometryInstances->Sort(VIS_API_SORTING_DISTANCE);

  // ------------------------------------------------------------------------------------------------------------
  // Trigger callbacks on all attached components
  // ------------------------------------------------------------------------------------------------------------
  { 
    VisVisibilityCollectorDataObject_cl cbdata(&Vision::Callbacks.OnFinishVisibilityDetermination,this);
    cbdata.m_pSender->TriggerCallbacks(&cbdata);
    const int iComponents = Components().Count();
    for (int j=0;j<iComponents;j++)
    {
      if(IVisVisibilityCollectorComponent_cl* pComponent = vdynamic_cast<IVisVisibilityCollectorComponent_cl*>(Components().GetAt(j)))
      {
        pComponent->EndVisibility(this);
      }
    }
  }

  m_eStatus = VIS_VISIBILITYSTATUS_READY;
}


void VisionVisibilityCollector_cl::TraverseScene(VisVisibilityZone_cl *pZone, int iRecursionDepth)
{
  int i;
  const int iMaxRecursionDepth = 1024;
  if (iRecursionDepth > iMaxRecursionDepth) // stop recursive vis checks here
  {
    Vision::Error.Warning("Visibility zone traversal exceeded the maximum recursion depth of %d zones!", iMaxRecursionDepth);
    return;
  }

#ifdef HK_DEBUG_SLOW
  pZone->AssertValid();
#endif

  // If node is not yet registered with scene manager, we don't touch it.
  int iZoneIndex = pZone->GetIndex();
  if (iZoneIndex<0)
    return;

  bool bTagZoneAsVisible = true;

  int iOldFrustumStackDepth = m_iFrustumStackDepth;

  // Flag node, so we don't traverse it again later on
  m_VisibilityZoneVisitedFlags[iZoneIndex>>3]|=(char)(1<<(iZoneIndex&7));

  // test scene elements and add to collections
  if (m_iFrustumStackDepth > 0)
    CollectVisibleSceneElements(pZone, &(m_FrustumStack[m_iFrustumStackDepth-1]));
  else
    CollectVisibleSceneElements(pZone, NULL);

  if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY && m_pOQContext != NULL)
    ProtocolAddZone(pZone);

  // Check visibility zones connected without portals
  int iNumZones = pZone->GetVisibilityZoneCount();

  for (i=0; i<iNumZones; i++)
  {
    VisVisibilityZone_cl *pTargetNode = pZone->GetVisibilityZone(i);
    int iTargetNodeIndex = pTargetNode->GetIndex();
    if (iTargetNodeIndex<0)
      continue;

    // Avoid traversing the same node twice
    if (m_VisibilityZoneVisitedFlags[iTargetNodeIndex>>3]&(char)(1<<(iTargetNodeIndex&7)))
      continue;
    if (m_VisibilityZoneFlags[iTargetNodeIndex>>3]&(char)(1<<(iTargetNodeIndex&7)))
      continue;

    const hkvAlignedBBox &nodeBox = pTargetNode->GetBoundingBox();

    // early out when the zone is too far away (in some cases better than frustum clipping)
    if (m_bUseCameraBBox && !m_CameraBBox.overlaps(nodeBox))
      continue;

    // Test visibility zone against active frustum (if there is one)
    if (m_iFrustumStackDepth > 0 && !m_bTraceAllTerrainZones)
    {
      if (!m_FrustumStack[m_iFrustumStackDepth-1].Overlaps(nodeBox))
        continue;
    }

    TraverseScene(pTargetNode, iRecursionDepth+1);
  }

  if (!(m_VisibilityZoneFlags[iZoneIndex>>3]&(char)(1<<(iZoneIndex&7))))
  {
    m_VisibleVisibilityZones.AppendEntry(pZone);

    if (pZone->GetZoneFlags()&VIS_VISIBILITYZONEFLAGS_CALLVIRTUALTAGFUNCTION)
      if (bTagZoneAsVisible)
        pZone->OnTagVisible(this, &m_FrustumStack[m_iFrustumStackDepth-1]);

    m_VisibilityZoneFlags[iZoneIndex>>3]|=(char)(1<<(iZoneIndex&7));
  }

  vPos = m_pSourceObject->GetPosition();
  vDir = m_pSourceObject->GetDirection();

  // Check visibility zones connected through portals
  int iNumPortals = pZone->GetPortalCount();
  for (i=0; i<iNumPortals; i++)
  {
    VisPortal_cl &portal = pZone->GetPortal(i);
    if (!portal.IsActive()) // also checks for HasTarget()
      continue;

    VisVisibilityZone_cl *pTargetNode = portal.GetTarget();
    if (m_iBehaviorFlags & VIS_VISCOLLECTOR_USEPORTALS)
    {
      // viewpoint has to be on negative side of plane. Allow for some margin, though.
      if (portal.GetPlane().getDistanceTo(vPos)>= (0.00001f * Vision::World.GetGlobalUnitScaling()))
        continue;

      // Avoid traversing the same node twice
      int iTargetNodeIndex = pTargetNode->GetIndex();
      if ((iTargetNodeIndex < 0) || (m_VisibilityZoneVisitedFlags[iTargetNodeIndex>>3]&(char)(1<<(iTargetNodeIndex&7))))
        continue;

      if (m_iFrustumStackDepth > 0)
      {
        // targetPortal is now a member to reduce stack footprint. This works as long it is not recursively passed
        VisClippingResult_e eClipRes = portal.Clip(m_FrustumStack[m_iFrustumStackDepth-1], targetPortal);
        if (eClipRes == VIS_CLIPPINGRESULT_ALLCLIPPED)
        {
          continue;
        }
        else if (eClipRes == VIS_CLIPPINGRESULT_CLIPPED)
        {
          m_FrustumStack[m_iFrustumStackDepth].Set(*m_FrustumStack[m_iFrustumStackDepth-1].GetPlane(1), vPos, targetPortal);
          m_iFrustumStackDepth++;
        }
        else if (eClipRes == VIS_CLIPPINGRESULT_UNCHANGED)
        {
          m_FrustumStack[m_iFrustumStackDepth].Set(*m_FrustumStack[m_iFrustumStackDepth-1].GetPlane(1), vPos, portal);
          m_iFrustumStackDepth++;
        }
        // Fourth case: VIS_CLIPPINGRESULT_TOOCLOSE -> Continue traversal with unchanged frustum.
      }
      else
      {
        if (m_pOverrideFrustum!=NULL)
        {
          m_FrustumStack[0].CopyFrom(*m_pOverrideFrustum);
        }
        else
        {

#if defined (_VISION_PS3)
#pragma diag_push
#pragma diag_suppress=552
#endif


          //hkvMat3 mOrientation(false); // now a member
          m_pSourceObject->GetRotationMatrix(mOrientation);
          bool bRes;
          if (m_eProjectionType == VIS_PROJECTIONTYPE_ORTHOGRAPHIC)
            bRes = m_FrustumStack[0].SetOrthographic(vPos, mOrientation, m_fOrthographicSize[0], m_fOrthographicSize[1], m_fClipPlanes[0], m_fClipPlanes[1]);
          else
            bRes = m_FrustumStack[0].Set(vPos, mOrientation, m_fFov[0], m_fFov[1], m_fClipPlanes[0], m_fClipPlanes[1]);
          VASSERT(bRes);


#ifdef _VISION_PS3
#pragma diag_pop
#endif


        }
        m_FrustumStack[0].Set(*m_FrustumStack[0].GetPlane(1), vPos, portal);
        m_iFrustumStackDepth++;
      }
    }
    else
    {
      // When not using portals, the only reason not to visit a zone is that it has already been visited
      int iTargetNodeIndex = pTargetNode->GetIndex();
      if ((iTargetNodeIndex < 0) || (m_VisibilityZoneVisitedFlags[iTargetNodeIndex>>3]&(char)(1<<(iTargetNodeIndex&7))))
        continue;
    }

    bool bPortalAddedToProtocol = false;
    if (m_iBehaviorFlags&VIS_VISCOLLECTOR_USEZONEOCCLUSIONQUERY && m_pOQContext != NULL)
    {
      if (portal.GetOccQueryObject()==NULL)
        continue;
      int iOQIndex = portal.GetOccQueryObject()->GetNumber();
      ProtocolAddPortal(&portal);
      bPortalAddedToProtocol = true;
    }

    TraverseScene(pTargetNode, iRecursionDepth+1);

    if (bPortalAddedToProtocol)
      ProtocolRemovePortal(&portal);

    m_iFrustumStackDepth = iOldFrustumStackDepth;
  }

  // Unflag node
  m_VisibilityZoneVisitedFlags[iZoneIndex>>3]&=~(char)(1<<(iZoneIndex&7));
}


#ifdef SUPPORTS_MULTITHREADING

void VisionVisibilityCollector_cl::KickoffVisibilityWorkflow()
{
  int iRequiredEntities = 4;
  int iRequiredVisObjects = 4;
  int iRequiredGeoInstances = 4;
  for (int i=0; i<m_iStreamConfigCount; i++)
  {
    if (m_StreamConfigs[i].eSceneElements == (int)VIS_SCENEELEMENT_WORLDGEOMETRY)
      iRequiredGeoInstances += m_StreamConfigs[i].pVisZone->GetStaticGeometryInstances()->GetNumEntries();
    else if (m_StreamConfigs[i].eSceneElements == (int)VIS_SCENEELEMENT_ENTITIES)
      iRequiredEntities += m_StreamConfigs[i].pVisZone->GetEntities()->GetNumEntries();
    else if (m_StreamConfigs[i].eSceneElements == (int)VIS_SCENEELEMENT_VISOBJECTS)
      iRequiredVisObjects += m_StreamConfigs[i].pVisZone->GetVisObjects()->GetNumEntries();
  }

  m_pVisibleStaticGeometryInstances->EnsureSize(iRequiredGeoInstances);

  m_pVisibleEntities->EnsureSize(iRequiredEntities);
  m_pVisibleVisObjects->EnsureSize(iRequiredVisObjects);

  unsigned int iGeoInstanceOffset=0;
  unsigned int iEntityOffset=0;
  unsigned int iVisObjectOffset=0;
  for (int i=0; i<m_iStreamConfigCount; i++)
  {
    // This requires the task to be exactly "parallel" with the stream configs...
    VStreamProcessingTask *pStreamTask = m_pWorkflow->GetTask(i);
    if (m_StreamConfigs[i].eSceneElements == (int)VIS_SCENEELEMENT_WORLDGEOMETRY)
    {
      unsigned int iCount = m_StreamConfigs[i].pVisZone->GetStaticGeometryInstances()->GetNumEntries();
      UINT_PTR iPtr = (UINT_PTR)m_pVisibleStaticGeometryInstances->GetDataPtr() + iGeoInstanceOffset*sizeof(void*);
      // The remaining streams can only be added here, since the dynarrays can reallocate memory during the scene traversal, which
      // would otherwise result in invalid pointers!
      VDataStream dataInStream(((void *)&(m_StreamConfigs[i])), sizeof(VisVisibilityStreamConfig_t));
      pStreamTask->AddInStream(dataInStream);
      VDataStream dataInStream2((void *)m_StreamConfigs[i].pVisZone->GetStaticGeometryInstances()->GetDataPtr(), iCount * sizeof(void*));
      pStreamTask->AddInStream(dataInStream2);
      VDataStream dataOutStream((void*)iPtr, iCount * sizeof(void *));
      pStreamTask->AddOutStream(dataOutStream);
      iGeoInstanceOffset += iCount;
    }
    else if (m_StreamConfigs[i].eSceneElements == (int)VIS_SCENEELEMENT_ENTITIES)
    {
      unsigned int iCount = m_StreamConfigs[i].pVisZone->GetEntities()->GetNumEntries();
      UINT_PTR iPtr = (UINT_PTR)m_pVisibleEntities->GetDataPtr() + iEntityOffset*sizeof(void*);
      VDataStream dataInStream(((void *)&(m_StreamConfigs[i])), sizeof(VisVisibilityStreamConfig_t));
      pStreamTask->AddInStream(dataInStream);
      VDataStream dataInStream2((void *)m_StreamConfigs[i].pVisZone->GetEntities()->GetDataPtr(), iCount * sizeof(void*));
      pStreamTask->AddInStream(dataInStream2);
      VDataStream dataOutStream((void*)iPtr, iCount * sizeof(void *));
      pStreamTask->AddOutStream(dataOutStream);
      iEntityOffset += iCount;


    }
    else if (m_StreamConfigs[i].eSceneElements == (int)VIS_SCENEELEMENT_VISOBJECTS)
    {
      unsigned int iCount = m_StreamConfigs[i].pVisZone->GetVisObjects()->GetNumEntries();
      UINT_PTR iPtr = (UINT_PTR)m_pVisibleVisObjects->GetDataPtr() + iVisObjectOffset*sizeof(void*);
      VDataStream dataInStream(((void *)&(m_StreamConfigs[i])), sizeof(VisVisibilityStreamConfig_t));
      pStreamTask->AddInStream(dataInStream);
      VDataStream dataInStream2((void *)m_StreamConfigs[i].pVisZone->GetVisObjects()->GetDataPtr(), iCount * sizeof(void*));
      pStreamTask->AddInStream(dataInStream2);
      VDataStream dataOutStream((void*)iPtr, iCount * sizeof(void *));
      pStreamTask->AddOutStream(dataOutStream);
      iVisObjectOffset += iCount;
    }
  }

  m_pVisibleStaticGeometryInstances->SetNumEntries(iGeoInstanceOffset);
  m_pVisibleEntities->SetNumEntries(iEntityOffset);
  m_pVisibleVisObjects->SetNumEntries(iVisObjectOffset);

  if (m_pWorkflow->GetTaskCount()>0)
    VStreamProcessor::KickWorkflow(m_pWorkflow);

}

void VisionVisibilityCollector_cl::FinishVisibilityWorkflow()
{
  if (m_pWorkflow->GetTaskCount()==0)
    return;

  m_pWorkflow->WaitUntilFinished();

  for (int i=0; i<m_iStreamConfigCount; i++)
  {
    // This requires the task to be exactly "parallel" with the stream configs...
    VStreamProcessingTask *pStreamTask = m_pWorkflow->GetTask(i);
    DeInitVisibilityTask(pStreamTask);
  }

  m_pVisibleStaticGeometryInstances->RemoveUnused();
  m_pVisibleEntities->RemoveUnused();
  m_pVisibleVisObjects->RemoveUnused();
}

#endif  // SUPPORTS_MULTITHREADING

void VisionVisibilityCollector_cl::DeInitVisibilityTask(VStreamProcessingTask *pTask)
{
#ifdef VSTREAMPROCESS_RUN_IN_THREADMANAGER
  VStreamProcessingKernel *pKernel = pTask->GetKernel();
  V_SAFE_DELETE(pKernel);
#endif
}

VStreamProcessingTask *VisionVisibilityCollector_cl::InitVisibilityTask()
{
  int iTaskIndex = m_pWorkflow->AddTask();
  VStreamProcessingTask *pTask = m_pWorkflow->GetTask(iTaskIndex);
  pTask->ResetStreams();
  pTask->SetScratchBufferSize(32768);
  pTask->SetOutputBufferSize(8192);
  pTask->SetStackSize(4096);

#ifdef VSTREAMPROCESS_RUN_IN_SPURS
  VSpuBinary kernel;
  VSpursHandler::GetEmbeddedSpuBinary(_binary_spu_SPUVisibilityJob_bin_start, _binary_spu_SPUVisibilityJob_bin_size, kernel);
  pTask->SetKernel(&kernel);
#else
  pTask->SetKernel(new VStreamProcessVisibilityJob());
#endif

  return pTask;
}


void VisionVisibilityCollector_cl::WaitForAllTasks()
{
  if (m_pTask)
    Vision::GetThreadManager()->WaitForTask(m_pTask,true);
}


void VisionVisibilityCollector_cl::CollectVisibleSceneElements(VisVisibilityZone_cl *pZone, VisFrustum_cl *pFrustum)
{
  int iPlaneFlags = 0;

  VisClippingResult_e eClipResult;
  if (pFrustum == NULL)
    eClipResult = VIS_CLIPPINGRESULT_UNCHANGED;
  else
    eClipResult = pFrustum->ClassifyPlanes(pZone->GetBoundingBox(), iPlaneFlags);

  if (eClipResult == VIS_CLIPPINGRESULT_ALLCLIPPED)
    return;

  hkvVec3 vCameraPos(hkvNoInitialization), vCameraDir(hkvNoInitialization);
  if (!(m_eContextHint & VIS_CONTEXTUSAGE_VIEWCONTEXT) && m_pLODContext != NULL)
  {
    vCameraPos = m_pLODContext->GetCamera()->GetPosition();
    vCameraDir = m_pLODContext->GetCamera()->GetDirection();
  }
  else
  {
    vCameraPos = m_pSourceObject->GetPosition();
    vCameraDir = m_pSourceObject->GetDirection();
  }
  const bool bConsiderFarClip = (m_iContextRenderFlags&VIS_RENDERCONTEXT_FLAG_NO_FARCLIP)==0;
  float fLODScaleSqr = bConsiderFarClip ? 1.f : -1.f;  // fLODScaleSqr<0.f turns off testing in IsClipped
  if (bConsiderFarClip && m_pLODContext!=NULL)
    fLODScaleSqr = m_pLODContext->GetLODDistanceScaling() * m_pLODContext->GetLODDistanceScaling();

  int i;

  if (m_eSceneElementFlags & VIS_SCENEELEMENT_LIGHTS)
  {
    int iNumLightsInNode = pZone->GetLightSources()->GetNumEntries();
    m_pVisibleLights->EnsureSize(m_pVisibleLights->GetNumEntries() + iNumLightsInNode);
    VisLightSource_cl **pDataInNode = pZone->GetLightSources()->GetDataPtr();

    for (i=0; i<iNumLightsInNode; i++, pDataInNode++)
    {
      VisLightSource_cl *pLight = *pDataInNode;
      if (! (m_iFilterBitmask & pLight->GetVisibleBitmask()) || !pLight->GetTriggered()) 
        continue;
      //if (bConsiderFarClip && pLight->IsFarClipped(vCameraPos)) continue; // coronas should not be clipped / faded out

      if (eClipResult != VIS_CLIPPINGRESULT_UNCHANGED)
      {
        hkvBoundingSphere sphere;
        hkvVec3 vPos = pLight->GetPosition();
        sphere.m_vCenter = vPos;
        sphere.m_fRadius = pLight->GetRadius();
        if (!pFrustum->Overlaps(sphere)) continue;
      }
      m_pVisibleLights->AppendEntryFast(*pDataInNode);
    }
  }

  int iNumGeomInstancesInZone = pZone->GetStaticGeometryInstances()->GetNumEntries();
  int iNumEntitiesInNode = pZone->GetEntities()->GetNumEntries();
  int iNumVisObjectsInNode = pZone->GetVisObjects()->GetNumEntries();
  m_bUsedWorkflow = false;

#ifdef SUPPORTS_MULTITHREADING
  if (s_bUseWorkflow && pFrustum != NULL)
  {
    m_bUsedWorkflow = true;

    // Add Stream Config to list
    m_Frusta[m_iFrustumCount] = *pFrustum;

    VisVisibilityStreamConfig_t streamConfig;
    streamConfig.cameraPos = vCameraPos;
    streamConfig.cameraDir = vCameraDir;
    streamConfig.fLODScaleSqr = fLODScaleSqr;
    streamConfig.iFilterMask = m_iFilterBitmask;
    streamConfig.pVisZone = pZone;
    streamConfig.iPlaneFlags = iPlaneFlags;

    if (m_eSceneElementFlags & VIS_SCENEELEMENT_WORLDGEOMETRY && iNumGeomInstancesInZone > 0)
    {
      streamConfig.iVisDataBlockOffset = VisStaticGeometryInstance_cl::GetVisDataOffset();
      streamConfig.eSceneElements = (int)VIS_SCENEELEMENT_WORLDGEOMETRY;
      m_StreamConfigs[m_iStreamConfigCount] = streamConfig;
      VStreamProcessingTask *pTask = InitVisibilityTask();
      pTask->SetUserData((void*)m_iStreamConfigCount);
      VDataStream dataInStream(((void *)&(m_Frusta[m_iFrustumCount])), sizeof(VisFrustum_cl));
      pTask->AddInStream(dataInStream);
      m_iStreamConfigCount++;
    }

    if (m_eSceneElementFlags & VIS_SCENEELEMENT_ENTITIES && iNumEntitiesInNode > 0)
    {
      streamConfig.iVisDataBlockOffset = VisBaseEntity_cl::GetVisDataOffset();
      streamConfig.eSceneElements = (int)VIS_SCENEELEMENT_ENTITIES;
      m_StreamConfigs[m_iStreamConfigCount] = streamConfig;
      VStreamProcessingTask *pTask = InitVisibilityTask();
      pTask->SetUserData((void*)m_iStreamConfigCount);
      VDataStream dataInStream(((void *)&(m_Frusta[m_iFrustumCount])), sizeof(VisFrustum_cl));
      pTask->AddInStream(dataInStream);
      m_iStreamConfigCount++;
    }

    if (m_eSceneElementFlags & VIS_SCENEELEMENT_VISOBJECTS && iNumVisObjectsInNode > 0)
    {
      streamConfig.iVisDataBlockOffset = VisVisibilityObject_cl::GetVisDataOffset();
      streamConfig.eSceneElements = (int)VIS_SCENEELEMENT_VISOBJECTS;
      m_StreamConfigs[m_iStreamConfigCount] = streamConfig;
      VStreamProcessingTask *pTask = InitVisibilityTask();
      pTask->SetUserData((void*)m_iStreamConfigCount);
      VDataStream dataInStream(((void *)&(m_Frusta[m_iFrustumCount])), sizeof(VisFrustum_cl));
      pTask->AddInStream(dataInStream);
      m_iStreamConfigCount++;
    }

    m_iFrustumCount++;
  }
  else

#endif  // SUPPORTS_MULTITHREADING
  {
    if (m_eSceneElementFlags & VIS_SCENEELEMENT_WORLDGEOMETRY)
    {
#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
      if (m_pLODHysteresisManager->GetThreshold(VLHT_WORLDGEOMETRY) > 0.0f)
      {
        CollectWorldGeometry_LODHysteresis(pZone, iNumGeomInstancesInZone, pFrustum, iPlaneFlags, vCameraPos, fLODScaleSqr, eClipResult);
      }
      else
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
      {
        CollectWorldGeometry(pZone, iNumGeomInstancesInZone, pFrustum, iPlaneFlags, vCameraPos, fLODScaleSqr, eClipResult);
      }
    }

    if (m_eSceneElementFlags & VIS_SCENEELEMENT_ENTITIES)
    {
#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
      if (m_pLODHysteresisManager->GetThreshold(VLHT_ENTITIES) > 0.0f)
      {
        CollectEntities_LODHysteresis(pZone, iNumEntitiesInNode, pFrustum, iPlaneFlags, vCameraPos, fLODScaleSqr, eClipResult);
      }
      else
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
      {
        CollectEntities(pZone, iNumEntitiesInNode, pFrustum, iPlaneFlags, vCameraPos, fLODScaleSqr, eClipResult);
      }
    }

    if (m_eSceneElementFlags & VIS_SCENEELEMENT_VISOBJECTS)
    {
      CollectVisElements(pZone, iNumVisObjectsInNode, pFrustum, iPlaneFlags, vCameraPos, fLODScaleSqr, eClipResult);
    }
  }
}

void VisionVisibilityCollector_cl::CollectWorldGeometry(VisVisibilityZone_cl* pZone, int iNumGeomInstancesInZone, VisFrustum_cl* pFrustum, int iPlaneFlags,
  const hkvVec3& vCameraPos, float fLODScaleSqr, VisClippingResult_e eClipResult)
{
  if (iNumGeomInstancesInZone == 0)
    return;

  m_pVisibleStaticGeometryInstances->EnsureSize(m_pVisibleStaticGeometryInstances->GetNumEntries() + iNumGeomInstancesInZone);
  VisStaticGeometryInstance_cl **pDataInZone = pZone->GetStaticGeometryInstances()->GetDataPtr();
  VASSERT(pDataInZone != NULL);

  // Prefetch first 2 instances (we look ahead 2 in the loop below).  For each instance do 2 prefetches to make sure
  // sure we get all the data referenced in case the object straddles a cache line.  m_BoundingBox is the first piece
  // of data referenced in the instance and m_fNearClipDistance will ensure we get the rest of the object.
  V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_BoundingBox), *(pDataInZone + 0));
  V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_fNearClipDistance), *(pDataInZone + 0));
  if (iNumGeomInstancesInZone > 1)
  {
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_BoundingBox), *(pDataInZone + 1));
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_fNearClipDistance), *(pDataInZone + 1));
  }

  int i = 0;
  for (; i < iNumGeomInstancesInZone-2; i++, pDataInZone++)
  {
    // Prefetch instance 2 ahead.  Use same offset as explained above.
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl, m_BoundingBox), *(pDataInZone + 2));
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl, m_fNearClipDistance), *(pDataInZone + 2));

    VisStaticGeometryInstance_cl *pGeoInstance = *pDataInZone;
    if (pGeoInstance->IsClipped(m_iFilterBitmask, vCameraPos, fLODScaleSqr))
      continue;

    if (eClipResult != VIS_CLIPPINGRESULT_UNCHANGED &&
      !pFrustum->Overlaps(pGeoInstance->GetBoundingBox(), iPlaneFlags)) 
    {
      continue;
    }
    m_pVisibleStaticGeometryInstances->AppendEntryFast(pGeoInstance);
  }
  // last 2 elements
  for (; i < iNumGeomInstancesInZone; i++, pDataInZone++)
  {
    VisStaticGeometryInstance_cl *pGeoInstance = *pDataInZone;
    if (pGeoInstance->IsClipped(m_iFilterBitmask, vCameraPos, fLODScaleSqr))
      continue;

    if (eClipResult != VIS_CLIPPINGRESULT_UNCHANGED &&
      !pFrustum->Overlaps(pGeoInstance->GetBoundingBox(), iPlaneFlags)) 
    {
      continue;
    }
    m_pVisibleStaticGeometryInstances->AppendEntryFast(pGeoInstance);
  }
}

void VisionVisibilityCollector_cl::CollectEntities(VisVisibilityZone_cl* pZone, int iNumEntitiesInNode, VisFrustum_cl* pFrustum, int iPlaneFlags,
  const hkvVec3& vCameraPos, float fLODScaleSqr, VisClippingResult_e eClipResult)
{
  m_pVisibleEntities->EnsureSize(m_pVisibleEntities->GetNumEntries() + iNumEntitiesInNode);
  VisBaseEntity_cl **pDataInNode = pZone->GetEntities()->GetDataPtr();

  for (int i=0; i<iNumEntitiesInNode; i++, pDataInNode++)
  {
    VisBaseEntity_cl *pEntity = *pDataInNode;

    if (!pEntity->GetMesh())
      continue;

    if (pEntity->IsClipped(m_iFilterBitmask, vCameraPos, fLODScaleSqr)) 
      continue;

    if (eClipResult != VIS_CLIPPINGRESULT_UNCHANGED)
    {
      const hkvAlignedBBox *pBox = pEntity->GetCurrentVisBoundingBoxPtr();
      if (!pFrustum->Overlaps(*pBox, iPlaneFlags)) 
        continue;
    }
    m_pVisibleEntities->AppendEntryFast((VisBaseEntity_cl *)pEntity);
  }
}

void VisionVisibilityCollector_cl::CollectVisElements(VisVisibilityZone_cl* pZone, int iNumVisObjectsInNode, VisFrustum_cl* pFrustum, int iPlaneFlags,
  const hkvVec3& vCameraPos, float fLODScaleSqr, VisClippingResult_e eClipResult)
{
  m_pVisibleVisObjects->EnsureSize(m_pVisibleVisObjects->GetNumEntries() + iNumVisObjectsInNode);
  VisVisibilityObject_cl **pDataInNode = pZone->GetVisObjects()->GetDataPtr();

  for (int i=0; i<iNumVisObjectsInNode; i++, pDataInNode++)
  {
    VisVisibilityObject_cl *pVisObject = *pDataInNode;
    if (pVisObject->IsClipped(m_iFilterBitmask, vCameraPos, fLODScaleSqr))
      continue;

    const int iFlags = pVisObject->GetVisTestFlags();
    if ((eClipResult != VIS_CLIPPINGRESULT_UNCHANGED) && (iFlags&(VISTESTFLAGS_PORTALTEST|VISTESTFLAGS_FRUSTUMTEST)))
    {
      const hkvAlignedBBox *pBbox = pVisObject->GetWorldSpaceBoundingBoxPtr();
      if (!pFrustum->Overlaps(*pBbox, iPlaneFlags)) 
        continue;
    }

    if (!(iFlags&VISTESTFLAGS_TESTVISIBLE_NOT_OVERRIDDEN))
      if (!pVisObject->OnTestVisible(this, pFrustum))
        continue;

    m_pVisibleVisObjects->AppendEntryFast(pVisObject);
  }
}

#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
void VisionVisibilityCollector_cl::CollectWorldGeometry_LODHysteresis(VisVisibilityZone_cl* pZone, int iNumGeomInstancesInZone, VisFrustum_cl* pFrustum, int iPlaneFlags,
  const hkvVec3& vCameraPos, float fLODScaleSqr, VisClippingResult_e eClipResult)
{
  if (iNumGeomInstancesInZone == 0)
    return;

  m_pVisibleStaticGeometryInstances->EnsureSize(m_pVisibleStaticGeometryInstances->GetNumEntries() + iNumGeomInstancesInZone);
  VisStaticGeometryInstance_cl **pDataInZone = pZone->GetStaticGeometryInstances()->GetDataPtr();
  VASSERT(pDataInZone != NULL);

  // Prefetch first 2 instances (we look ahead 2 in the loop below).  For each instance do 2 prefetches to make sure
  // sure we get all the data referenced in case the object straddles a cache line.  m_BoundingBox is the first piece
  // of data referenced in the instance and m_fNearClipDistance will ensure we get the rest of the object.
  V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_BoundingBox), *(pDataInZone + 0));
  V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_fNearClipDistance), *(pDataInZone + 0));
  if (iNumGeomInstancesInZone > 1)
  {
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_BoundingBox), *(pDataInZone + 1));
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_fNearClipDistance), *(pDataInZone + 1));
  }

  int i = 0;
  for (; i < iNumGeomInstancesInZone-2; i++, pDataInZone++)
  {
    // Prefetch instance 2 ahead.  Use same offset as explained above.
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_BoundingBox), *(pDataInZone + 2));
    V_PRECACHE(offsetof(VisStaticGeometryInstance_cl,m_fNearClipDistance), *(pDataInZone + 2));

    VisStaticGeometryInstance_cl *pGeoInstance = *pDataInZone;
    if (m_pLODHysteresisManager->IsClipped(VLHT_WORLDGEOMETRY, pGeoInstance->GetNumber(), 0, pGeoInstance, m_iFilterBitmask, vCameraPos, fLODScaleSqr))
      continue;

    if (eClipResult != VIS_CLIPPINGRESULT_UNCHANGED &&
      !pFrustum->Overlaps(pGeoInstance->GetBoundingBox(), iPlaneFlags)) 
    {
      continue;
    }
    m_pVisibleStaticGeometryInstances->AppendEntryFast(pGeoInstance);
  }

  // last 2 elements
  for (; i < iNumGeomInstancesInZone; i++, pDataInZone++)
  {
    VisStaticGeometryInstance_cl *pGeoInstance = *pDataInZone;
    if (m_pLODHysteresisManager->IsClipped(VLHT_WORLDGEOMETRY, pGeoInstance->GetNumber(), 0, pGeoInstance, m_iFilterBitmask, vCameraPos, fLODScaleSqr))
      continue;

    if (eClipResult != VIS_CLIPPINGRESULT_UNCHANGED &&
      !pFrustum->Overlaps(pGeoInstance->GetBoundingBox(), iPlaneFlags)) 
    {
      continue;
    }
    m_pVisibleStaticGeometryInstances->AppendEntryFast(pGeoInstance);
  }
}

void VisionVisibilityCollector_cl::CollectEntities_LODHysteresis(VisVisibilityZone_cl* pZone, int iNumEntitiesInNode, VisFrustum_cl* pFrustum, int iPlaneFlags,
  const hkvVec3& vCameraPos, float fLODScaleSqr, VisClippingResult_e eClipResult)
{
  m_pVisibleEntities->EnsureSize(m_pVisibleEntities->GetNumEntries() + iNumEntitiesInNode);
  VisBaseEntity_cl **pDataInNode = pZone->GetEntities()->GetDataPtr();

  for (int i=0; i<iNumEntitiesInNode; i++, pDataInNode++)
  {
    VisBaseEntity_cl *pEntity = *pDataInNode;

    if (!pEntity->GetMesh())
      continue;

    IVLODHysteresisComponent* pComp = (IVLODHysteresisComponent*)pEntity->Components().GetComponentOfBaseType(IVLODHysteresisComponent::GetClassTypeId());
    int iLODLevel = (pComp != NULL)? pComp->GetLODLevel() : 0;
    if (m_pLODHysteresisManager->IsClipped(VLHT_ENTITIES, pEntity->GetNumber(), iLODLevel, pEntity, m_iFilterBitmask, vCameraPos, fLODScaleSqr))
      continue;

    if (eClipResult != VIS_CLIPPINGRESULT_UNCHANGED)
    {
      const hkvAlignedBBox *pBox = pEntity->GetCurrentVisBoundingBoxPtr();
      if (!pFrustum->Overlaps(*pBox, iPlaneFlags)) 
        continue;
    }
    m_pVisibleEntities->AppendEntryFast((VisBaseEntity_cl *)pEntity);
  }
}
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING

void VisionVisibilityCollector_cl::EnsureVisibilityZoneFlagsSize()
{
  int iNumBytes = (Vision::GetSceneManager()->GetNumVisibilityZones() >> 3) + 1;
  m_VisibilityZoneFlags.EnsureSize(iNumBytes);
  m_VisibilityZoneVisitedFlags.EnsureSize(iNumBytes);
}

void VisionVisibilityCollector_cl::ClearVisibilityZoneFlags()
{
  int iNumBytes = (Vision::GetSceneManager()->GetNumVisibilityZones() >> 3) + 1;
  VASSERT(iNumBytes <= m_VisibilityZoneFlags.GetSize());
  memset(m_VisibilityZoneFlags.GetDataPtr(), 0, iNumBytes);
  memset(m_VisibilityZoneVisitedFlags.GetDataPtr(), 0, iNumBytes);
}



void VisionVisibilityCollector_cl::ClearSceneElementFlags()
{
  int iRequiredSize = (VisBaseEntity_cl::ElementManagerGetSize() >> 3) + 1;
  m_EntityFlags.EnsureSize(iRequiredSize);
  memset(m_EntityFlags.GetDataPtr(), 0, iRequiredSize); 

  iRequiredSize = (VisLightSource_cl::ElementManagerGetSize() >> 3) + 1;
  m_LightFlags.EnsureSize(iRequiredSize);
  memset(m_LightFlags.GetDataPtr(), 0, iRequiredSize); 

  iRequiredSize = (VisVisibilityObject_cl::ElementManagerGetSize() >> 3) + 1;
  m_VisObjectFlags.EnsureSize(iRequiredSize);
  memset(m_VisObjectFlags.GetDataPtr(), 0, iRequiredSize); 

  iRequiredSize = (VisStaticGeometryInstance_cl::ElementManagerGetSize() >> 3) + 1;
  m_StaticGeometryInstanceFlags.EnsureSize(iRequiredSize);
  memset(m_StaticGeometryInstanceFlags.GetDataPtr(), 0, iRequiredSize); 
}

bool VisionVisibilityCollector_cl::IsStaticGeometryInstanceVisible(VisStaticGeometryInstance_cl *pGeomInstance)
{
  int iNum = pGeomInstance->GetNumber();
  return (m_StaticGeometryInstanceFlags[iNum>>3]&(char)(1<<(iNum&7)))?true:false;
}

bool VisionVisibilityCollector_cl::IsEntityVisible(const VisBaseEntity_cl *pEntity)
{
  int iNum = pEntity->GetNumber();
  return (m_EntityFlags[iNum>>3]&(char)(1<<(iNum&7)))?true:false;
}

bool VisionVisibilityCollector_cl::IsVisObjectVisible(const VisVisibilityObject_cl *pVisObject)
{
  int iNum = pVisObject->GetNumber();
  return (m_VisObjectFlags[iNum>>3]&(char)(1<<(iNum&7)))?true:false;
}

bool VisionVisibilityCollector_cl::IsLightVisible(const VisLightSource_cl *pLight)
{
  int iNum = pLight->GetNumber();
  return (m_LightFlags[iNum>>3]&(char)(1<<(iNum&7)))?true:false;
}

bool VisionVisibilityCollector_cl::IsVisibilityZoneVisible(const VisVisibilityZone_cl *pZone)
{
  int iNum = pZone->GetIndex();
  return (m_VisibilityZoneFlags[iNum>>3]&(char)(1<<(iNum&7)))?true:false;
}


void VisionVisibilityCollector_cl::SetPropertiesFromRenderContext(VisRenderContext_cl *pContext)
{
  if (m_eStatus != VIS_VISIBILITYSTATUS_READY)
    return;

  int iRenderFlags = pContext->GetRenderFlags();
  m_iContextRenderFlags = iRenderFlags;
  int eSceneElements = VIS_SCENEELEMENT_ALL;
  if (iRenderFlags & VIS_RENDERCONTEXT_FLAG_NO_WORLDGEOM)
    eSceneElements &= ~VIS_SCENEELEMENT_WORLDGEOMETRY;
  if (iRenderFlags & VIS_RENDERCONTEXT_FLAG_NO_ENTITIES)
    eSceneElements &= ~VIS_SCENEELEMENT_ENTITIES;
  if (iRenderFlags & VIS_RENDERCONTEXT_FLAG_NO_LIGHTS)
    eSceneElements &= ~VIS_SCENEELEMENT_LIGHTS;
  if (iRenderFlags & VIS_RENDERCONTEXT_FLAG_NO_VISOBJECTS)
    eSceneElements &= ~VIS_SCENEELEMENT_VISOBJECTS;
  float fFov[2];
  float fClipPlanes[2];
  m_eContextHint = pContext->GetUsageHint();;
  m_eProjectionType = pContext->GetProjectionType();
  pContext->GetFinalFOV(fFov[0], fFov[1]);
  pContext->GetClipPlanes(fClipPlanes[0], fClipPlanes[1]);
  pContext->GetOrthographicSize(m_fOrthographicSize[0],m_fOrthographicSize[1]);
  SetClipPlanes(fClipPlanes[0], fClipPlanes[1]);
  SetFOV(fFov[0], fFov[1]);
  SetSceneElementFlags((VisSceneElementTypes_e)eSceneElements);

  if (pContext->GetLODReferenceContext() != NULL)
    m_pLODContext = pContext->GetLODReferenceContext();
  else
    m_pLODContext = pContext;
}

const VisFrustum_cl *VisionVisibilityCollector_cl::GetBaseFrustum()
{
  return (const VisFrustum_cl *)&(m_FrustumStack[0]);
}


V_IMPLEMENT_DYNAMIC(VisVisibilityCollectorTask_cl,VThreadedTask,Vision::GetEngineModule());

VisVisibilityCollectorTask_cl::VisVisibilityCollectorTask_cl(VisionVisibilityCollector_cl *pVisibilityCollector)
{
  m_pVisibilityCollector = pVisibilityCollector;
  m_iFilterBitmask = 0xFFFFFFFF;
}

void VisVisibilityCollectorTask_cl::Run(VManagedThread *pThread)
{
  m_pVisibilityCollector->PerformVisibilityDetermination(m_iFilterBitmask);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IVisVisibilityCollectorComponent_cl
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// register the class
V_IMPLEMENT_DYNAMIC(IVisVisibilityCollectorComponent_cl,IVObjectComponent,Vision::GetEngineModule());

IVisVisibilityCollectorComponent_cl::IVisVisibilityCollectorComponent_cl()
{
}

IVisVisibilityCollectorComponent_cl::~IVisVisibilityCollectorComponent_cl()
{
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
