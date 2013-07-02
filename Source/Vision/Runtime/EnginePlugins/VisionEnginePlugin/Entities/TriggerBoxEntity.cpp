/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

extern VModule g_VisionEngineModule;

// static collection
VisEntityCollection_cl TriggerBoxEntity_cl::g_ObservedEntities(0,256);
bool TriggerBoxEntity_cl::g_bObservedEntityListNeedsSorting = false;

TriggerBoxEntity_cl::TriggerBoxEntity_cl() : m_EntitiesInside(0,64)
{
  m_bEnabled = true;
  m_fSphereRadius = 0.0f;
  m_bCameraIsInside = false;
  m_bDestroyOnTrigger = false;
  m_bIsChanged = false;
  m_iTriggerObserverFlags = 0;
  m_pCurrentEntity = NULL;
  m_bIsSphere = m_bIsOriented = false;
  SetUseEulerAngles(FALSE);
}

TriggerBoxEntity_cl::~TriggerBoxEntity_cl()
{
  SetObserverFlags(0);
}

void TriggerBoxEntity_cl::InitFunction()
{
  VisBaseEntity_cl::InitFunction();

  m_Components.EnsureCapacity(m_Components.Count()+4);

  // note that this is only called from inside vForge. Serialization reads these components from the archive
  m_spOnCameraEnter = new VisTriggerSourceComponent_cl(TRIGGERBOX_ONCAMERAENTER, VIS_OBJECTCOMPONENTFLAG_NONE);
  AddComponent(m_spOnCameraEnter);

  m_spOnCameraLeave = new VisTriggerSourceComponent_cl(TRIGGERBOX_ONCAMERALEAVE, VIS_OBJECTCOMPONENTFLAG_NONE);
  AddComponent(m_spOnCameraLeave);

  m_spOnObjectEnter = new VisTriggerSourceComponent_cl(TRIGGERBOX_ONOBJECTENTER, VIS_OBJECTCOMPONENTFLAG_NONE);
  AddComponent(m_spOnObjectEnter);

  m_spOnObjectLeave = new VisTriggerSourceComponent_cl(TRIGGERBOX_ONOBJECTLEAVE, VIS_OBJECTCOMPONENTFLAG_NONE);
  AddComponent(m_spOnObjectLeave);
}

void TriggerBoxEntity_cl::DeInitFunction()
{
  VisBaseEntity_cl::DeInitFunction();
  m_spOnCameraEnter = NULL;
  m_spOnCameraLeave = NULL;
  m_spOnObjectEnter = NULL;
  m_spOnObjectLeave = NULL;
}

V_IMPLEMENT_SERIAL( TriggerBoxEntity_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule );
void TriggerBoxEntity_cl::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  char iLocalVersion = TRIGGERBOX_CURRENT_VERSION;
  hkvVec4 vSphere;
  if (ar.IsLoading())
  {
    m_bIsOriented = false;
    int iObserverFlags;
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= TRIGGERBOX_CURRENT_VERSION, "invalid version number");

    if (iLocalVersion >= TRIGGERBOX_VERSION_4)
    {
      ar >> m_bEnabled;
    }

    ar >> iObserverFlags;
    ar >> m_bDestroyOnTrigger;
    if (iLocalVersion == TRIGGERBOX_VERSION_1)
    {
      ar >> m_bIsSphere;
      ar >> vSphere;
      SetSphere(vSphere);
    }
    m_AbsBoundingBox.SerializeAs_VisBoundingBox (ar);
    if (iLocalVersion == TRIGGERBOX_VERSION_2)
    {
      ar >> m_bIsOriented >> m_bIsSphere;
      VASSERT(!m_bIsSphere || !m_bIsOriented);
      if (m_bIsOriented)
        m_LocalBoundingBox.SerializeAs_VisBoundingBox (ar);
      else if (m_bIsSphere)
      {
        ar >> vSphere;
        SetSphere(vSphere);
      }
    }

    if(iLocalVersion >= TRIGGERBOX_VERSION_3)
    {
      ar >> m_bIsOriented >> m_bIsSphere;
      VASSERT(!m_bIsSphere || !m_bIsOriented);
      m_LocalBoundingBox.SerializeAs_VisBoundingBox (ar);
      if (m_bIsSphere)
      {
        ar >> vSphere;
        SetSphere(vSphere);			
      }
    }

    VisTriggerSourceComponent_cl *pComp = NULL;
    ar >> pComp;m_spOnCameraEnter = pComp;
    ar >> pComp;m_spOnCameraLeave = pComp;
    ar >> pComp;m_spOnObjectEnter = pComp;
    ar >> pComp;m_spOnObjectLeave = pComp;

    ar >> m_bCameraIsInside;
    m_EntitiesInside.SerializeX(ar);
    m_EntitiesInside.SortByPointer(); // keep this list sorted
    SetObserverFlags(iObserverFlags); // this function sets additional flags
  } 
  else
  {
    ar << iLocalVersion;

    ar << m_bEnabled;         // Version 4

    ar << m_iTriggerObserverFlags;
    ar << m_bDestroyOnTrigger;
    m_AbsBoundingBox.SerializeAs_VisBoundingBox (ar);
    ar << m_bIsOriented << m_bIsSphere;
    m_LocalBoundingBox.SerializeAs_VisBoundingBox (ar);

    if (m_bIsSphere)
    {
      vSphere = GetSphere();
      ar << vSphere;      
    }

    ar << m_spOnCameraEnter;
    ar << m_spOnCameraLeave;
    ar << m_spOnObjectEnter;
    ar << m_spOnObjectLeave;

    ar << m_bCameraIsInside;
    m_EntitiesInside.SerializeX(ar);
  }
}



void TriggerBoxEntity_cl::SetObserverFlags(int iFlags)
{
  SetThinkFunctionStatus(iFlags!=0);

  // only register to this callback if necessary
  if (m_iTriggerObserverFlags&TRIGGERBOXOBSERVERFLAGS_ALL_ENTITIES)
    VisObject3D_cl::OnObject3DDestroyed -= this;
  if (iFlags&TRIGGERBOXOBSERVERFLAGS_ALL_ENTITIES)
    VisObject3D_cl::OnObject3DDestroyed += this;

  m_iTriggerObserverFlags = iFlags;
}

// this version is optimized for one global observer list
bool TriggerBoxEntity_cl::HandleSortedEntityList(const VisEntityCollection_cl& observedEntities)
{
  int iCount = observedEntities.GetNumEntries();
  VisBaseEntity_cl **ppEnt = observedEntities.GetDataPtr();

  int iCurrentInsideCount = m_EntitiesInside.GetNumEntries();
  VisBaseEntity_cl **ppEntInside = m_EntitiesInside.GetDataPtr();
  int iCurrentInsidePos = 0;
  bool bInsideListNeedsResort = false;

  bool bHasBeenTriggered = false;

  for (int i=0;i<iCount;i++)
  {
    m_pCurrentEntity = ppEnt[i];
    if (!m_pCurrentEntity || !m_pCurrentEntity->HasMesh())
      continue;

    hkvAlignedBBox bbox(hkvNoInitialization);
    m_pCurrentEntity->GetCurrentVisBoundingBox(bbox);

    // remove objects that are in m_EntitiesInside but not in g_ObservedEntities
    while (iCurrentInsidePos<iCurrentInsideCount-1 && (INT_PTR)ppEntInside[iCurrentInsidePos+1]<=(INT_PTR)m_pCurrentEntity)
    {
      m_EntitiesInside.RemoveAt(iCurrentInsidePos);
      iCurrentInsideCount--;
    }

    BOOL bIsInside = TestInside(bbox);
    BOOL bLastInside = iCurrentInsidePos<iCurrentInsideCount && ppEntInside[iCurrentInsidePos]==m_pCurrentEntity;
    VASSERT(bLastInside == m_EntitiesInside.Contains(m_pCurrentEntity));

    if (VISION_UNLIKELY(bIsInside!=bLastInside))
    {
      if (bIsInside)
      {
        m_EntitiesInside.AppendEntry(m_pCurrentEntity);
        m_spOnObjectEnter->TriggerAllTargets();
        this->TriggerScriptEvent("OnObjectEnter", "*o", m_pCurrentEntity);
        bInsideListNeedsResort = true;
        ppEntInside = m_EntitiesInside.GetDataPtr();
      } else
      {
        m_EntitiesInside.RemoveAt(iCurrentInsidePos);
        m_spOnObjectLeave->TriggerAllTargets();
        this->TriggerScriptEvent("OnObjectLeave", "*o", m_pCurrentEntity);
        iCurrentInsidePos--;
        iCurrentInsideCount--;
      }
      bHasBeenTriggered = true;
    }
    if (bLastInside)
      iCurrentInsidePos++;

  }

  if (bInsideListNeedsResort)
    m_EntitiesInside.SortByPointer();

  return bHasBeenTriggered;
}


// this version is optimized for a collection of entities that have changed since the last simulation tick
bool TriggerBoxEntity_cl::HandleUpdatedEntityList(const VisEntityCollection_cl& updatedEntities)
{
  int iCount = updatedEntities.GetNumEntries();
  if (iCount == 0)
    return false;

  bool bHasBeenTriggered = false;

  VisBaseEntity_cl **ppEnt = updatedEntities.GetDataPtr();
  Vision::RenderLoopHelper.ResetEntityTags();

  m_EntitiesInside.TagEntries();

  for (int i = 0; i < iCount; i++)
  {
    m_pCurrentEntity = ppEnt[i];

    if (!m_pCurrentEntity || !m_pCurrentEntity->HasMesh())
      continue;

    hkvAlignedBBox bbox(hkvNoInitialization);
    m_pCurrentEntity->GetCurrentVisBoundingBox(bbox);
    BOOL bIsInside = TestInside(bbox);
    BOOL bLastInside = Vision::RenderLoopHelper.IsEntityTagged(m_pCurrentEntity);
    if (VISION_UNLIKELY(bIsInside!=bLastInside))
    {
      if (bIsInside)
      {
        m_EntitiesInside.AppendEntry(m_pCurrentEntity);
        m_spOnObjectEnter->TriggerAllTargets();
        this->TriggerScriptEvent("OnObjectEnter", "*o", m_pCurrentEntity);
      } else
      {
        m_EntitiesInside.Remove(m_pCurrentEntity);
        m_spOnObjectLeave->TriggerAllTargets();
        this->TriggerScriptEvent("OnObjectLeave", "*o", m_pCurrentEntity);
      }
      bHasBeenTriggered = true;
    }
  }

  return bHasBeenTriggered;
}

// this version is used for checking ALL entities against a possibly changed trigger box
bool TriggerBoxEntity_cl::HandleUpdatedTriggerBox()
{
  const int iCount = VisBaseEntity_cl::ElementManagerGetSize();

  if (iCount==0)
    return false;

  bool bHasBeenTriggered = false;

  hkvAlignedBBox bbox(hkvNoInitialization);
  Vision::RenderLoopHelper.ResetEntityTags();
  m_EntitiesInside.TagEntries();

  for (int i=0;i<iCount;i++)
  {
    m_pCurrentEntity = VisBaseEntity_cl::ElementManagerGet(i);
    if (!m_pCurrentEntity || !m_pCurrentEntity->HasMesh()) continue;
    m_pCurrentEntity->GetCurrentVisBoundingBox(bbox);
    BOOL bIsInside = TestInside(bbox);
    BOOL bLastInside = Vision::RenderLoopHelper.IsEntityTagged(m_pCurrentEntity);
    if (VISION_UNLIKELY(bIsInside!=bLastInside))
    {
      if (bIsInside)
      {
        m_EntitiesInside.AppendEntry(m_pCurrentEntity);
        m_spOnObjectEnter->TriggerAllTargets();
        this->TriggerScriptEvent("OnObjectEnter", "*o", m_pCurrentEntity);
      } else
      {
        m_EntitiesInside.Remove(m_pCurrentEntity);
        m_spOnObjectLeave->TriggerAllTargets();
        this->TriggerScriptEvent("OnObjectLeave", "*o", m_pCurrentEntity);
      }
      bHasBeenTriggered = true;
    }
  }

  return bHasBeenTriggered;
}

void TriggerBoxEntity_cl::DebugRender(IVRenderInterface* pRenderer, VColorRef iColor)
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  if (m_bIsSphere)
  {
    const hkvVec4 sphere = GetSphere();
    pRenderer->RenderSphere (sphere.getAsVec3 (), sphere.w, iColor, state);
  } else
  {
    if (m_bIsOriented)
    {
      hkvMat4 mTransformation (GetRotationMatrix(), GetPosition());

      hkvVec3 vCorners[8];
      m_LocalBoundingBox.getCorners (vCorners);
      mTransformation.transformPositions (vCorners, 8);

      pRenderer->RenderBox (vCorners,sizeof(hkvVec3),iColor,state);
    } else
    {
      hkvAlignedBBox bbox((hkvVec3& )m_AbsBoundingBox.m_vMin,(hkvVec3& )m_AbsBoundingBox.m_vMax);
      pRenderer->RenderAABox(bbox,iColor,state);
    }
  }
}

void TriggerBoxEntity_cl::ThinkFunction()
{
  if(!m_bEnabled)
    return;

  bool bHasBeenTriggered = false;

  if (m_iTriggerObserverFlags&TRIGGERBOXOBSERVERFLAGS_MAINCAMERA)
  {
    bool bIsInside = TestInside(Vision::Camera.GetCurrentCameraPosition())==TRUE;
    if (VISION_UNLIKELY(bIsInside!=m_bCameraIsInside))
    {
      m_bCameraIsInside = bIsInside;
      if (bIsInside)
      {
        m_spOnCameraEnter->TriggerAllTargets();
        this->TriggerScriptEvent("OnCameraEnter", "*");
      }
      else
      {
        m_spOnCameraLeave->TriggerAllTargets();
        this->TriggerScriptEvent("OnCameraLeave", "*");
      }
      bHasBeenTriggered = true;
    }
  }

  if (m_iTriggerObserverFlags&TRIGGERBOXOBSERVERFLAGS_ALL_ENTITIES)
  {
    if(!m_bIsChanged)
    {	
      bHasBeenTriggered = HandleUpdatedEntityList(Vision::Game.GetUpdatedEntities()) || bHasBeenTriggered;
    } else 
    {
      bHasBeenTriggered = HandleUpdatedTriggerBox() || bHasBeenTriggered;
      m_bIsChanged = false;
    }    
  }
  // these two types are mutual exclusive
  else if (m_iTriggerObserverFlags&TRIGGERBOXOBSERVERFLAGS_ENTITYLIST)
  {
    if (VISION_UNLIKELY(g_bObservedEntityListNeedsSorting))
    {
      g_ObservedEntities.SortByPointer();
      g_bObservedEntityListNeedsSorting = false;
    }
    bHasBeenTriggered = HandleSortedEntityList(g_ObservedEntities) || bHasBeenTriggered;
  }

  if(bHasBeenTriggered && m_bDestroyOnTrigger)
    this->DisposeObject();
}


void TriggerBoxEntity_cl::AddObservedEntity(VisBaseEntity_cl *pEnt)
{
  VASSERT_MSG(!g_ObservedEntities.Contains(pEnt), "Collection already contains the entity");
  g_ObservedEntities.AppendEntry(pEnt);
  g_bObservedEntityListNeedsSorting = true;
}

void TriggerBoxEntity_cl::RemoveObservedEntity(VisBaseEntity_cl *pEnt)
{
  VASSERT_MSG(g_ObservedEntities.Contains(pEnt), "Collection does not contain the entity");
  g_ObservedEntities.Remove(pEnt);
}

#define PLANE_TEST(_a,_b,_c,_d) \
  plane.setFromPointAndNormal(_d, hkvVec3(_a,_b,_c));\
  plane.transform(rot);\
  bAnyPositive = false;\
  for (int i=0;i<8;i++)\
  {\
    float fDist = plane.getDistanceTo(vAACorners[i]);\
    if ( fDist>=0.f)\
    {\
      bAnyPositive = true;\
      break;\
    }\
  }\
  if (!bAnyPositive)\
    return FALSE;

BOOL TriggerBoxEntity_cl::TestOrientedBoxOverlap(const hkvAlignedBBox &aabox,const hkvAlignedBBox &obox, const hkvVec3& vPos, const hkvMat3 &rot)
{
  hkvPlane plane(hkvNoInitialization);
  hkvVec3 vAACorners[8];

  aabox.getCorners(vAACorners);
  for (int i=0;i<8;i++)
    vAACorners[i] -= vPos;

  bool bAnyPositive;
  PLANE_TEST(+1.f,0.f,0.f,obox.m_vMin);
  PLANE_TEST(-1.f,0.f,0.f,obox.m_vMax);
  PLANE_TEST(0.f,+1.f,0.f,obox.m_vMin);
  PLANE_TEST(0.f,-1.f,0.f,obox.m_vMax);
  PLANE_TEST(0.f,0.f,+1.f,obox.m_vMin);
  PLANE_TEST(0.f,0.f,-1.f,obox.m_vMax);

  return TRUE;
}

START_VAR_TABLE(TriggerBoxEntity_cl, VisBaseEntity_cl, "Trigger box entity", VFORGE_HIDECLASS, "")
END_VAR_TABLE

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
