/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/Engine.hpp> 
#include <Vision/Runtime/Engine/SceneManagement/VisionSceneManager.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityZone.hpp>

// *******************************************************************************
// *  This file is Vision's default Scene Manager implementation and is
// *  provided in full source code with the Vision SDK to serve as a reference
// *  for custom implementations. Note that you will have to alter class names
// *  and replace the precompiled header reference (Engine.hpp) with custom
// *  #includes in order to get this code to compile.
// *******************************************************************************


static VisionResourceCreator_cl g_ResourceCreator;
VisCallback_cl IVisSceneManager_cl::OnReposition(VIS_PROFILE_REPOSITION_CUSTOMOBJECTS);

#define FOR_ALL_VISIBILITY_ZONES \
  const int iNumZones = m_VisibilityZones.Count();\
  for (int iZone=0; iZone<iNumZones; iZone++) \
  {\
    VisVisibilityZone_cl *pZone = m_VisibilityZones.GetAt(iZone);
    
#define FOR_ALL_GEOMETRY_INSTANCES \
  const int iCount = VisStaticGeometryInstance_cl::ElementManagerGetSize();\
  for (int iInst=0; iInst<iCount; iInst++) \
  {\
    VisStaticGeometryInstance_cl *pInst = VisStaticGeometryInstance_cl::ElementManagerGet(iInst);\
    if (!pInst) continue;
    

IVisSceneManager_cl::IVisSceneManager_cl()
{
  m_pStreamingReferenceObject = NULL;
  m_pLastTestedZone = NULL;
}

IVisSceneManager_cl::~IVisSceneManager_cl() 
{
}

VisVisibilityZone_cl *IVisSceneManager_cl::VisibilityZoneForUID(__int64 uid) const
{
  // test cache
  if (m_pLastTestedZone && m_pLastTestedZone->GetUniqueID()==uid)
    return m_pLastTestedZone;

  FOR_ALL_VISIBILITY_ZONES
    if (pZone->GetUniqueID() == uid)
    {
      m_pLastTestedZone = pZone;
      return pZone;
    }
  }
  return NULL;
}

void IVisSceneManager_cl::AddVisibilityZone(VisVisibilityZone_cl *pZone)
{
  int iIndex = m_VisibilityZones.Count();
  m_VisibilityZones.Add(pZone);
  pZone->SetIndex(iIndex);
  VASSERT(m_VisibilityZones.GetAt(iIndex)==pZone);
  pZone->OnAddedToSceneManager(this);
}

void IVisSceneManager_cl::RemoveVisibilityZone(VisVisibilityZone_cl *pZone)
{
  if (pZone==m_pLastTestedZone) // reset cached zone
    m_pLastTestedZone = NULL;

  pZone->OnRemovedFromSceneManager(this);
  pZone->SetIndex(-1);
  m_VisibilityZones.Remove(pZone);

  // enumerate
  for (int i=0;i<m_VisibilityZones.Count();i++)
    m_VisibilityZones.GetAt(i)->SetIndex(i);
}

void IVisSceneManager_cl::RemoveAllVisibilityZones()
{
  m_pLastTestedZone = NULL; // reset cached zone

  // do this in separate loops to avoid assertions
  {
    FOR_ALL_VISIBILITY_ZONES
      pZone->OnRemovedFromSceneManager(this);
    }
  }
  {
    FOR_ALL_VISIBILITY_ZONES
      pZone->SetIndex(-1);
    }
  }

  m_VisibilityZones.Clear();

/*
  // also clear the resource caching queue
  m_FullResQueue.Clear();
  m_spLoadingTask = NULL;
*/
}


void IVisSceneManager_cl::SetStreamingReference(VisObject3D_cl *pRefObject)
{
  m_pStreamingReferenceObject = pRefObject;
}

VisObject3D_cl* IVisSceneManager_cl::GetStreamingReferenceSafe() const 
{
  return (m_pStreamingReferenceObject!=NULL) ? m_pStreamingReferenceObject : Vision::Camera.GetMainCamera();
}



void IVisSceneManager_cl::GatherStaticGeometryInBoundingBox(const hkvAlignedBBox &bbox, VisStaticGeometryInstanceCollection_cl &destList)
{
  VisVisibilityZone_cl *tempNodeList[256];
  int iNumNodes = FindVisibilityZones(bbox, tempNodeList, 256);
//  int iNumFoundGIs = 0;
  VisStaticGeometryInstance_cl::ResetTags();

  for (int i=0; i<iNumNodes; i++) 
  {
    VisVisibilityZone_cl *pNode = tempNodeList[i];
    hkvAlignedBBox nodeBox = pNode->GetBoundingBox();
    if (!nodeBox.overlaps(bbox))
      continue;

    const VisStaticGeometryInstanceCollection_cl *pGIs = pNode->GetStaticGeometryInstances();
    VisStaticGeometryInstance_cl **pGeoInstanceList = (VisStaticGeometryInstance_cl **)pGIs->GetDataPtr();
    int iNumGIs = pGIs->GetNumEntries();
    if (iNumGIs < 1)
      continue;
    for (int j=0; j<iNumGIs; j++, pGeoInstanceList++)
    {
      if ((*pGeoInstanceList)->IsTagged())
        continue;
      (*pGeoInstanceList)->Tag();
      if (!(*pGeoInstanceList)->GetBoundingBox().overlaps(bbox))
        continue;
      destList.AppendEntry(*pGeoInstanceList);
    }
  }
}



void IVisSceneManager_cl::GatherEntitiesInBoundingBox(const hkvAlignedBBox &bbox, VisEntityCollection_cl &destList)
{
  VisVisibilityZone_cl *tempNodeList[256];
  int iNumNodes = FindVisibilityZones(bbox, tempNodeList, 256);
  VisBaseEntity_cl::ResetTags();

  for (int i=0; i<iNumNodes; i++) 
  {
    VisVisibilityZone_cl *pNode = tempNodeList[i];
    hkvAlignedBBox nodeBox = pNode->GetBoundingBox();
    if (!nodeBox.overlaps(bbox))
      continue;

    const VisEntityCollection_cl *pEntities = pNode->GetEntities();
    VisBaseEntity_cl **pEntityList = (VisBaseEntity_cl **)pEntities->GetDataPtr();
    const int iEntities = pEntities->GetNumEntries();
    if (iEntities < 1)
      continue;
    for (int j=0; j<iEntities; j++, pEntityList++)
    {
      if ((*pEntityList)->IsTagged())
        continue;
      (*pEntityList)->Tag();
      if (!(*pEntityList)->GetCurrentVisBoundingBoxPtr()->overlaps(bbox))
        continue;
      destList.AppendEntry(*pEntityList);
    }
  }
}


void IVisSceneManager_cl::RepositionAllZones()
{
  const int iZoneCount = VisZoneResourceManager_cl::GlobalManager().GetResourceCount();

  for (int i=0;i<iZoneCount;++i)
  {
    VisZoneResource_cl *pZone = (VisZoneResource_cl *)VisZoneResourceManager_cl::GlobalManager().GetResourceByIndex(i);

    if (!pZone)
      continue;

    pZone->OnReposition(m_RepositionInfo);
  }

  // trigger callback for custom scene elements
  VisZoneRepositionDataObject_cl data(&OnReposition, m_RepositionInfo);
  data.Trigger();
}

void IVisSceneManager_cl::SetGlobalPivot(const hkvVec3d& vPivot )
{
  // check for repositioning:
  VASSERT(m_RepositionInfo.SupportsRepositioning());

  VisObject3D_cl *pCamera = Vision::GetSceneManager()->GetStreamingReferenceSafe();
  m_RepositionInfo.SetGlobalPivotPos(vPivot);
  if (m_RepositionInfo.HandleRepositioning(pCamera))
    RepositionAllZones();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VisionSceneManager_cl
///////////////////////////////////////////////////////////////////////////////////////////////////////////

VisionSceneManager_cl::VisionSceneManager_cl() : m_ChangingZones(0, 0)
{
  m_bAnyZoneUnloaded = m_bAnyZoneCaching = false;
  m_fStreamingTimeBudget = 10.f; // 10ms
  m_bPreferUnload = false;
  m_iLastManagerIndex = 0;

  m_SnapshotQueue.m_pCreator = &g_ResourceCreator;
  m_SnapshotQueue.m_pLoader = &Vision::File.GetMemoryStreamManager();
  m_SnapshotQueue.m_pFileManager = Vision::File.GetManager();
  m_SnapshotQueue.m_pLog = NULL;

}


VisionSceneManager_cl::~VisionSceneManager_cl() 
{
}

unsigned int VisionSceneManager_cl::FindVisibilityZones(const hkvAlignedBBox &bbox, VisVisibilityZone_cl **pNodes, unsigned int iMaxNodes)
{
  int iNumRelevantZones = 0;

  FOR_ALL_VISIBILITY_ZONES
    if (bbox.overlaps(pZone->GetAssignmentBoundingBox()))
    {
      pNodes[iNumRelevantZones++] = pZone;
      if (iNumRelevantZones >= iMaxNodes) {
        Vision::Error.Warning("Entity overlapped more than %d visibility zones - bounding box may be incorrect or too large.", iMaxNodes);
        break;
      }
    }
  }
  return iNumRelevantZones;
}

VisVisibilityZone_cl *VisionSceneManager_cl::FindClosestVisibilityZoneSimple(const hkvAlignedBBox &bbox) const
{
  VASSERT(bbox.isValid());
  hkvVec3 vCenter = bbox.getCenter();
  float fMinDist = FLT_MAX;
  VisVisibilityZone_cl *pBestZone = NULL;

  FOR_ALL_VISIBILITY_ZONES
    #pragma warning(suppress:6246)
    const hkvAlignedBBox &bbox = pZone->GetAssignmentBoundingBox();
    float fDist = bbox.getDistanceToSquared(vCenter);
    if (bbox.contains(vCenter) && !(pZone->GetZoneFlags()&VIS_VISIBILITYZONEFLAGS_FALLBACKZONE))
      fDist = bbox.getMaxExtent(); // smaller bboxes are better
    else
      fDist += 1000000.f; // extra penalty for not being in the box

    if (fDist < fMinDist)
    {
      fMinDist = fDist;
      pBestZone = pZone;
    }
  }

  return pBestZone;
}


VisVisibilityZone_cl *VisionSceneManager_cl::FindClosestVisibilityZone(const hkvAlignedBBox &bbox, const hkvVec3* pOrigin)
{
  VASSERT(bbox.isValid());
  VisVisibilityZone_cl *pZones[256];
  unsigned int iNumRelevantNodes = FindVisibilityZones(bbox, pZones, 256);

  // No relevant nodes, so we determine the best node using a simple bbox/bbox proximity test
  if (iNumRelevantNodes == 0)
  {
    VISION_INC_PERFCOUNT(VIS_PROFCNT_RECOMPUTEVISIBILITYZONE);
    return FindClosestVisibilityZoneSimple(bbox);
  }

  // Only one relevant node
  if (iNumRelevantNodes == 1)
    return pZones[0];

  // Multiple relevant nodes, so we need to compare against the geometry in the nodes
  int i;
  float fMinDist = FLT_MAX;
//  int iMinPrim = -1;
  VisStaticGeometryInstance_cl *pMinGeoInstance = NULL;
  VisVisibilityZone_cl *pBestZone=NULL;
  
  VisStaticGeometryInstance_cl *pBestGeoInstance;
  
  bool bClosestPrimitiveContainedInMultipleNodes = false;
  for (i=0; i<iNumRelevantNodes; i++)
  {
    VisVisibilityZone_cl *pZone = pZones[i];
    if (pZone->GetStaticGeometryInstances()==NULL)
      continue;
    float fDist = pZone->GetDistanceFromGeometry(bbox, pBestGeoInstance);
    if (pOrigin && !pZone->GetAssignmentBoundingBox().contains(*pOrigin))
      fDist += 100000.f;
    if (pZone->GetZoneFlags()&VIS_VISIBILITYZONEFLAGS_FALLBACKZONE) // extra penalty for this zone
      fDist+=10000000.f;
    if (fDist < fMinDist)
    {
      pBestZone = pZones[i];
      fMinDist = fDist;

      // closest is static geo instance
      if (pBestGeoInstance!=NULL)
      {
//        iMinPrim = -1;
        pMinGeoInstance = pBestGeoInstance;
        if (pBestGeoInstance->GetVisibilityZoneAssignmentCount() > 1)
          bClosestPrimitiveContainedInMultipleNodes = true;
        else
          bClosestPrimitiveContainedInMultipleNodes = false;
      }
    }
  }

  // If iMinPrim still isn't set, the nodes did not contain any primitives, so we use only the bounding boxes to
  // compute the node assignment
  if (pMinGeoInstance==NULL)
    return FindClosestVisibilityZoneSimple(bbox);


  // Complex case: closest primitive is contained in multiple visibility zones 
  // -> we need to check which one is best by testing the center of our bounding box against
  // the visibility zones' portals.
  if (bClosestPrimitiveContainedInMultipleNodes)
  {
    hkvVec3 vCenter = bbox.getCenter();
    int iNumPortals = pBestZone->GetPortalCount();
    for (i=0; i<iNumPortals; i++)
    {
      VisPortal_cl &portal = pBestZone->GetPortal(i);
      VASSERT(portal.GetSource() == pBestZone);
      if (!portal.HasTarget())
        continue;
      VisVisibilityZone_cl *pNextNode = portal.GetTarget();
      if ((pMinGeoInstance!=NULL && pNextNode->GetStaticGeometryInstances()->Contains(pMinGeoInstance)))
      {
        if (portal.GetPlane().getDistanceTo (vCenter) >= 0.0f)
        {
          return pNextNode;
        }
      }
    }
  }

  return pBestZone;
}


void VisionSceneManager_cl::GetSceneExtents(hkvAlignedBBox& bbox) const
{
  bbox.setInvalid();
  FOR_ALL_GEOMETRY_INSTANCES
    bbox.expandToInclude(pInst->GetBoundingBox());
  }
}

VisVisibilityZone_cl *VisionSceneManager_cl::TraceIntoZone(const hkvVec3& vPos, const hkvVec3& vDir)
{
  hkvVec3 vEndPos = vPos + vDir; // length of dir defines the trace length

  hkvAlignedBBox traceBox;
  traceBox.expandToInclude(vPos);
  traceBox.expandToInclude(vEndPos);

  VisVisibilityZone_cl *pBestZone = NULL;
  float fMinDist = FLT_MAX;

  FOR_ALL_VISIBILITY_ZONES
    const hkvAlignedBBox &bbox = pZone->GetBoundingBox(); // GetAssignmentBoundingBox?
    if (bbox.overlaps(traceBox))
    {
      hkvVec3 vIntersection(hkvNoInitialization);
      if (bbox.contains(vPos))
        return pZone; // start position is inside...
      if (bbox.getLineSegmentIntersection (vPos, vEndPos, NULL, &vIntersection))
      {
        hkvVec3 vToIntersection((hkvVec3) vPos - vIntersection);
        float fDist = vToIntersection.getLength();
        if (fDist < fMinDist)
        {
          fMinDist = fDist;
          pBestZone = pZone;
        }
      }
    }
  }

  return pBestZone;
}


VisZoneResource_cl *VisionSceneManager_cl::CreateZoneResource(VisZoneResourceManager_cl *pManager)
{
  if (m_RepositionInfo.SupportsRepositioning())
    return new VisRepositioningZoneResource_cl(pManager, this);
  return new VisZoneResource_cl(pManager, this);
}

// the numerical values here define the priority for processing (see SortZonesByActionPriority)
#define ACTION_CALL_ENSURELOADED    1
#define ACTION_CALL_LOADINGTICK     2
#define ACTION_CALL_SCHEDULELOADING 3
#define ACTION_CALL_UNLOADINGTICK   4

#define MARK_ZONE_AS_CHANGING \
  if (!bZoneAdded) {m_ChangingZones[iChangingZones++] = pZone;bZoneAdded=true;}

// qsort function
int SortZonesByActionPriority(const void *arg1, const void *arg2)
{
  VisZoneResource_cl *z1 = *((VisZoneResource_cl **)arg1);
  VisZoneResource_cl *z2 = *((VisZoneResource_cl **)arg2);
  int iDiff = (int)z1->m_iWantedAction - (int)z2->m_iWantedAction;
  if (iDiff!=0) return iDiff;
  float fDiff = z1->m_fCameraDistance - z2->m_fCameraDistance;
  if (fDiff>0.f) return 1;
  if (fDiff<0.f) return -1;
  return 0;
}



void VisionSceneManager_cl::HandleZones(float fTimeDelta)
{
#ifdef WIN32
  // don't handle zones inside the editor (but only test on PC platform as console clients should execute anyway)
  if (Vision::Editor.IsInEditor())
    return;
#endif

  VISION_PROFILE_FUNCTION(VIS_PROFILE_STREAMING_OVERALL);
  m_bAnyZoneCaching = false;
  VisZoneResourceManager_cl &zones = VisZoneResourceManager_cl::GlobalManager();

  VisObject3D_cl *pCamera = Vision::GetSceneManager()->GetStreamingReferenceSafe();
  const hkvVec3 vCamPos = pCamera->GetPosition();

  // check for repositioning:
  if (m_RepositionInfo.SupportsRepositioning())
  {
    if (m_RepositionInfo.HandleRepositioning(pCamera))
      RepositionAllZones();
  }


  const int iCount = zones.GetResourceCount();
  int iChangingZones = 0;

  // Loop 1. Evaluate what to do with any of the zones
  for (int i=0;i<iCount;i++)
  {
    VISION_PROFILE_FUNCTION(VIS_PROFILE_STREAMING_HANDLE_ZONES);
    VisZoneResource_cl *pZone = zones.GetZoneByIndex(i);
    if (!pZone || !pZone->m_bHandleZone || pZone->IsMissing())
      continue;
    float fDist = pZone->m_BoundingBox.getDistanceTo(vCamPos);
    pZone->m_fCameraDistance = fDist;
    bool bZoneAdded = false;
    UBYTE &action(pZone->m_iWantedAction);

    if (pZone->IsStreaming()) // this zone is busy loading
    {
      m_bAnyZoneCaching = true;
      action = ACTION_CALL_LOADINGTICK;
      MARK_ZONE_AS_CHANGING;
    } 
    else if (pZone->IsDestroyingInstances())
    {
      action = ACTION_CALL_UNLOADINGTICK;
      MARK_ZONE_AS_CHANGING;
    }

    if (!pZone->IsLoaded() || pZone->IsDestroyingInstances())
    {
      if (fDist<pZone->m_fLoadedDistance)
      {
        action = ACTION_CALL_ENSURELOADED;
        MARK_ZONE_AS_CHANGING;
        continue;
      }
      if (!pZone->IsStreaming() && fDist<pZone->m_fCacheDistance)
      {
        action = ACTION_CALL_SCHEDULELOADING;
        MARK_ZONE_AS_CHANGING;
        continue;
      }
    }
    else
    {
      if (fDist>pZone->m_fCacheOutDistance) // uncache distance
      {
        action = ACTION_CALL_UNLOADINGTICK;
        MARK_ZONE_AS_CHANGING;
      }
    }
  }

  const double dTimerRes = (double)VGLGetTimerResolution()/1000.0;
  __int64 iStartTime = VGLGetTimer();
  const __int64 iEndTime = iStartTime + (__int64)(dTimerRes*m_fStreamingTimeBudget);

  // handle pending snapshots
  {
    VISION_PROFILE_FUNCTION(VIS_PROFILE_STREAMING_SNAPSHOT_TICK);
  #ifdef HK_DEBUG
    Vision::File.SetWarnOnUncachedFiles(true);
  #endif
    m_SnapshotQueue.TickFunction(fTimeDelta);
    m_bAnyZoneCaching |= m_SnapshotQueue.IsBusy();
  #ifdef HK_DEBUG
    Vision::File.SetWarnOnUncachedFiles(false);
  #endif
  }

  // process all changing zones as long as we are under time budget
  if (iChangingZones>0 && VGLGetTimer()<iEndTime)
  {
    VisZoneResource_cl **ppZones = m_ChangingZones.GetDataPtr();
    qsort(ppZones,iChangingZones,sizeof(VisZoneResource_cl *),SortZonesByActionPriority);
    bool bAbortLoop = false;
    int iIteration = 0; 
#ifdef PROFILING
    int yk = 30;
#endif
    while (!bAbortLoop)
    {
      iIteration++;
      bAbortLoop = true;
      for (int i=0;i<iChangingZones;i++)
      {
        VISION_PROFILE_FUNCTION(VIS_PROFILE_STREAMING_HANDLE_ZONES);
        VisZoneResource_cl *pZone = ppZones[i];
        int iAction = pZone->m_iWantedAction;
        bool bCheckTime = true;
        switch (iAction)
        {
          case ACTION_CALL_LOADINGTICK:
            if (pZone->LoadingTick(fTimeDelta) || pZone->IsLoaded())
              pZone->m_iWantedAction=0;
            break;
          case ACTION_CALL_ENSURELOADED:
            m_bAnyZoneCaching = true;
            if (pZone->IsDestroyingInstances())
              pZone->EnsureUnloaded();
            pZone->EnsureLoaded();
            pZone->m_iWantedAction=0;
            bCheckTime = bAbortLoop = false;
            break;
          case ACTION_CALL_SCHEDULELOADING:
            if (pZone->IsDestroyingInstances())
              pZone->EnsureUnloaded();
            pZone->ScheduleLoading();
            pZone->m_iWantedAction=0;
            break;
          case  ACTION_CALL_UNLOADINGTICK:
            if (pZone->UnLoadingTick(fTimeDelta) || !pZone->IsLoaded())
            {
              pZone->m_iWantedAction=0;
              m_bAnyZoneUnloaded = true;
            }
            break;
        }

        if (bCheckTime)
        {
          __int64 iTimeNow = VGLGetTimer();
#ifdef PROFILING
          if ((Vision::Profiling.GetDebugRenderFlags()&DEBUGRENDERFLAG_ZONES) && iAction>0)
          {
            char szName[1024];
            pZone->GetZoneName(szName);
            float fDiffMS = (float)((iTimeNow-iStartTime)/dTimerRes);
            Vision::Message.Print(1,700,yk,"%i: Zone '%s', action %i, time %2fms",iIteration,szName,iAction,fDiffMS);yk+=12;
          }
#endif
          if (iTimeNow>iEndTime)
          {
            bAbortLoop = true;
            break;
          }
          else 
            if (pZone->m_iWantedAction!=0)  // still anything to do
              bAbortLoop = false;
        }
      }
    }
  }


  // some additional handling that should not intercept with pre-caching
  if (!m_bAnyZoneCaching)
  {
    // run some cleanup
    if (m_bAnyZoneUnloaded && m_FullResQueue.Count()==0)
    {
      if (PurgeResources(iEndTime))
        m_bAnyZoneUnloaded = false;
    }
    HandleFullResLoadingQueue();
  }

#ifdef PROFILING
  if (Vision::Profiling.GetDebugRenderFlags()&DEBUGRENDERFLAG_ZONES)
    zones.DebugRender();
#endif
}


void VisionSceneManager_cl::HandleFullResLoadingQueue()
{
  int iCount = m_FullResQueue.Count();
  if (iCount==0)
  {
    m_spLoadingTask = NULL;
    return;
  }

  if (m_spLoadingTask!=NULL && !m_spLoadingTask->IsLoaded())
    return; // wait...


  // the next one is finished
  if (m_spLoadingTask!=NULL)
  {
    VManagedResource *pRes = m_FullResQueue.GetAt(0); // process the next one
    if (!pRes->IsLoaded() || pRes->IsReplacement())
    {
      pRes->EnsureUnloaded();
      pRes->EnsureLoaded(); // reload real resource
    }
    m_FullResQueue.RemoveAt(0);
  }

  // TODO: sort by relevance here...
  iCount = m_FullResQueue.Count();

  m_spLoadingTask = NULL;
  while (iCount>0)
  {
    iCount--;
    VManagedResource *pRes = m_FullResQueue.GetAt(0);
    if (!pRes->IsLoaded() || pRes->IsReplacement())
    {
      m_spLoadingTask = Vision::File.PrecacheFile(pRes->GetFilename());
      if (m_spLoadingTask!=NULL)
        break; // remove entry #0 in the next tick
    }
    m_FullResQueue.RemoveAt(0);
  }
}


#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=112
#endif

bool VisionSceneManager_cl::PurgeResources(__int64 iEndTime)
{
  VISION_PROFILE_FUNCTION(VIS_PROFILE_STREAMING_PURGE_RESOURCES);

  const int iManagerCount = Vision::ResourceSystem.GetResourceManagerCount();
  const int iFirstIndex = m_iLastManagerIndex; // continue where we stopped last time
  const int iPurgeMask = VRESOURCEMANAGER_PURGEMASK_STREAMINGCLEANUP;
  while (true) // repeat iterations until everything is purged or time is over
  {
    int iPurgedCount = 0;
    for (int i=0;i<iManagerCount;i++)
    {
      m_iLastManagerIndex = (i+iFirstIndex+iManagerCount) % iManagerCount;
      VResourceManager *pManager = Vision::ResourceSystem.GetResourceManagerByIndex(m_iLastManagerIndex);
      if (pManager==NULL || (pManager->GetPurgeMask()&iPurgeMask)==0)
        continue;

      const int iResCount = pManager->GetResourceCount();
      for (int j=0;j<iResCount;j++)
      {
        VManagedResource *pResource = pManager->GetResourceByIndex(j);
        if (pResource==NULL || pResource->GetRefCount()>1 || pResource->IsResourceFlagSet(VRESOURCEFLAG_NOPURGING))
          continue;
        if (m_bPreferUnload)
        {
          if (pResource->IsLoaded() && pResource->CanUnload())
          {
            pResource->EnsureUnloaded();
            iPurgedCount++;
          }
        }
        else if (pResource->Purge())
          iPurgedCount++;

        __int64 iTimeNow = VGLGetTimer();
        if (iTimeNow>=iEndTime)
          return false;
      }

    }
    // now we looped through all managers. Nothing purged?
    if (iPurgedCount==0)
    {
      m_iLastManagerIndex = 0; // start with first manager next time
      return true;
    }
  }

  // PS3 does not like it if it is impossible to reach the statement
  //return false; // never gets here...
}

#if defined(__SNC__)
#pragma diag_pop
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
