/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/MeshTrailEffectComponent.h>

#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>

// One global instance of our manager
RPG_MeshTrailEffectComponentManager RPG_MeshTrailEffectComponentManager::s_instance;

RPG_MeshTrailEffectComponent::RPG_MeshTrailEffectComponent(const char* textureFilename, const VColorRef color, const float lifetime, const int numSegments)
  : IVObjectComponent(0,0),
  m_textureFilename(textureFilename),
  m_color(color),
  m_lifetime(lifetime),
  m_numSegments(numSegments),
  m_enabled(false),
  m_historyPos(0),
  m_historyCount(0),  
  m_history(NULL),
  m_mesh(NULL),
  m_meshObj(NULL),
  m_startBoneName(),
  m_endBoneName()
{
}

RPG_MeshTrailEffectComponent::RPG_MeshTrailEffectComponent()
  : IVObjectComponent(),
  m_textureFilename(),
  m_color(),
  m_lifetime(0.0f),
  m_numSegments(0),
  m_enabled(false),
  m_historyPos(0),
  m_historyCount(0),  
  m_history(NULL),
  m_mesh(NULL),
  m_meshObj(NULL),
  m_startBoneName(),
  m_endBoneName()
{
}

RPG_MeshTrailEffectComponent::~RPG_MeshTrailEffectComponent()
{
  if (m_meshObj)
  {
    m_meshObj->DisposeObject();
  }

  V_SAFE_DELETE_ARRAY(m_history);
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_MeshTrailEffectComponent::GetDependencies(VResourceSnapshot &snapshot)
{
  VTypedObject::GetDependencies(snapshot);

  if(!m_textureFilename.IsEmpty())
  {
    RPG_DependencyHelper::AddTextureDependency(m_textureFilename, snapshot);
  }
}
#endif

void RPG_MeshTrailEffectComponent::SetEnabled(bool bStatus)
{
  if (m_enabled == bStatus)
  {
    return;
  }

  m_enabled = bStatus;
  m_meshObj->SetVisible(m_enabled);
  ResetTrail();

  if (m_enabled)
  {
    RPG_MeshTrailEffectComponentManager::s_instance.Instances().AddUnique(this);
  }
  else
  {
    RPG_MeshTrailEffectComponentManager::s_instance.Instances().Remove(this);
  }
}

void RPG_MeshTrailEffectComponent::ResetTrail()
{
  m_historyCount = 0;
}

void RPG_MeshTrailEffectComponent::PerFrameUpdate()
{
  float fTime = Vision::GetTimer()->GetTime();
  UpdateHistory(fTime);
  UpdateMesh(fTime);
}

void RPG_MeshTrailEffectComponent::UpdateHistory(float fAbsTime)
{
  m_historyCount = hkvMath::Min(m_historyCount + 1, m_numSegments);
  m_historyPos = (m_historyPos + 1) % m_numSegments;
  VTrailHistoryEntry &entry( m_history[m_historyPos] );

  VisObject3D_cl *pObj = (VisObject3D_cl *)GetOwner();
  hkvMat3 rotation = pObj->GetRotationMatrix();
  const hkvVec3 &vPos(pObj->GetPosition());

  entry.m_start = rotation.transformDirection(m_relStart);
  entry.m_end = rotation.transformDirection(m_relEnd);
  entry.m_start += vPos;
  entry.m_end += vPos;

  entry.m_time = fAbsTime;
}

void RPG_MeshTrailEffectComponent::UpdateMesh(float fAbsTime)
{
  int iIndex = m_historyPos + m_numSegments; // go back in history

  int iPrim = -1;
  VisMBSimpleVertex_t *pV = (VisMBSimpleVertex_t *) m_mesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
  for (int i = 0 ; i < m_historyCount; i++, iIndex--, pV += 2, iPrim++)
  {
    VASSERT(iIndex >= 0);
    VTrailHistoryEntry &entry( m_history[iIndex % m_numSegments] );

    // Intensity for alpha and V-texture coordinate (0..1 depending on lifetime)
    float fIntensity = (fAbsTime - entry.m_time) / m_lifetime;
    int iAlpha = 255 - (int) (fIntensity * 255.f);
    if (iAlpha < 0)
    {
      break;
    }

    VColorRef iColor(255, 255, 255, iAlpha);
    iColor *= m_color;
    pV[0].color = iColor.GetNative();
    pV[0].pos = entry.m_start;
    pV[0].texcoord0.set(0.f, fIntensity);
    pV[1].color = iColor.GetNative();
    pV[1].pos = entry.m_end;
    pV[1].texcoord0.set(1.f, fIntensity);
  }

  m_mesh->UnLockVertices();
  m_mesh->SetPrimitiveCount(hkvMath::Max(iPrim, 0) * 2);
}

BOOL RPG_MeshTrailEffectComponent::CanAttachToObject(VisTypedEngineObject_cl *object, VString& errorOut)
{
  if(!IVObjectComponent::CanAttachToObject(object, errorOut))
  {
    return FALSE;
  }

  bool const isBaseEntity = (object->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)) == TRUE);
  if(!isBaseEntity)
  {
    errorOut = "VisBaseEntity_cl (or derived) required";
    return FALSE;
  }

  bool const hasWeaponTrailEffect = (object->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_MeshTrailEffectComponent)) != NULL);
  if(hasWeaponTrailEffect)
  {
    errorOut = "RPG_MeshTrailEffectComponent (or derived) already attached";
    return FALSE;
  }

  return TRUE;
}

void RPG_MeshTrailEffectComponent::SetOwner(VisTypedEngineObject_cl *owner)
{
  IVObjectComponent::SetOwner(owner);

  if(owner)
  {
    // Get texture or use plain white
    VTextureObject *trailTexture = Vision::TextureManager.Load2DTexture(m_textureFilename);
    if (trailTexture == NULL)
      trailTexture = Vision::TextureManager.GetPlainWhiteTexture();

    // Get trail bones
    VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *) GetOwner();
    if(pEntity->GetMesh() == NULL)
    {
      return;
    }

    bool useFallback = false;

    if(pEntity->GetMesh()->GetSkeleton())
    {
      int startIndex = pEntity->GetMesh()->GetSkeleton()->GetBoneIndexByName(m_startBoneName);
      int endIndex = pEntity->GetMesh()->GetSkeleton()->GetBoneIndexByName(m_endBoneName);
      if(startIndex > -1 && endIndex > -1)
      {  
        // Use bones to define range
        hkvQuat rotation;
        pEntity->GetBoneCurrentLocalSpaceTransformation(startIndex, m_relStart, rotation);
        pEntity->GetBoneCurrentLocalSpaceTransformation(endIndex, m_relEnd, rotation);
      }
      else
      {
        useFallback = true;
      }
    }
    else
    {
      useFallback = true;
    }

    // use the fallback if there's no skeleton or if we don't have both of the bones that were specified
    if(useFallback)
    {
      // Fallback: Use bounding box along y axis to define range
      hkvAlignedBBox bbox = pEntity->GetMesh()->GetBoundingBox();
      m_relStart = hkvVec3(bbox.getCenter().x, bbox.m_vMin.y, bbox.getCenter().z);
      m_relEnd = hkvVec3(bbox.getCenter().x, bbox.m_vMax.y, bbox.getCenter().z);
    }

    if(m_history)
    {
      // cleanup existing history if we have one
      delete m_history;
      m_history = NULL;
    }

    m_history = new VTrailHistoryEntry[m_numSegments];

    m_mesh = new VisMeshBuffer_cl(VisMBSimpleVertex_t::VertexDescriptor, m_numSegments * 2, VisMeshBuffer_cl::MB_PRIMTYPE_TRISTRIP, 0, 0, VIS_MEMUSAGE_DYNAMIC);
    m_mesh->SetBaseTexture(trailTexture);

    VSimpleRenderState_t iState(VIS_TRANSP_ADDITIVE, RENDERSTATEFLAG_DOUBLESIDED | RENDERSTATEFLAG_FILTERING);
    m_mesh->SetDefaultRenderState(iState);
    m_meshObj = new VisMeshBufferObject_cl(m_mesh);

    m_enabled = true;

    RPG_MeshTrailEffectComponentManager::s_instance.Instances().AddUnique(this);
  }
  else
  {   
    RPG_MeshTrailEffectComponentManager::s_instance.Instances().SafeRemove(this);
  }
}

void RPG_MeshTrailEffectComponent::Serialize(VArchive& ar)
{
  char iLocalVersion = 0;
  IVObjectComponent::Serialize(ar);
  if(ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion == 0, "Invalid local version. Please re-export");

    ar >> m_textureFilename;
    ar >> m_color;
    ar >> m_lifetime;
    ar >> m_numSegments;
    ar >> m_startBoneName;
    ar >> m_endBoneName;
  }
  else
  {
    ar << iLocalVersion;

    ar << m_textureFilename;
    ar << m_color;
    ar << m_lifetime;
    ar << m_numSegments;
    ar << m_startBoneName;
    ar << m_endBoneName;
  }
}

void RPG_MeshTrailEffectComponentManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    // call update function on every component
    if (Vision::Editor.IsPlaying())
    {
      const int iCount = m_components.Count();
      for (int i=0;i<iCount;i++)
      {
        m_components.GetAt(i)->PerFrameUpdate();
      }
    }
  }
}

V_IMPLEMENT_SERIAL(RPG_MeshTrailEffectComponent, IVObjectComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_MeshTrailEffectComponent, IVObjectComponent, "(RPG) Mesh Trail", VVARIABLELIST_FLAGS_NONE, "(RPG) Mesh Trail")
  DEFINE_VAR_VSTRING_AND_NAME(RPG_MeshTrailEffectComponent, m_textureFilename, "Texture Filename", "The texture used for the trail effect", "", 0, 0, "filepicker(.TGA)");
  DEFINE_VAR_COLORREF_AND_NAME(RPG_MeshTrailEffectComponent, m_color, "Color", "Modulate color of the trail effect", "255/255/255/255", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_MeshTrailEffectComponent, m_lifetime, "Lifetime", "Lifetime of the trail effect in seconds (segment lifetime)", "1.0", 0, NULL);
  DEFINE_VAR_INT_AND_NAME(RPG_MeshTrailEffectComponent, m_numSegments, "Segment Count", "Number of segments of the trail mesh", "50", 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_MeshTrailEffectComponent, m_startBoneName, "Start Bone Name", "Name of the bone where the trail should start.", "trail_start", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_MeshTrailEffectComponent, m_endBoneName, "End Bone Name", "Name of the bone where the trail should end.", "trail_end", 0, 0, 0);
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
