/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationModel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Manager functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////


VTerrainDecorationModelManager VTerrainDecorationModelManager::g_GlobalManager;


VTerrainDecorationModelManager &VTerrainDecorationModelManager::GlobalManager()
{
  return g_GlobalManager;
}


void VTerrainDecorationModelManager::OneTimeInit()
{
  Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(30,70,0));
#ifdef WIN32
  if (Vision::Editor.IsInEditor())
  {
    VisSurface_cl::OnSurfaceShaderChanged += this;
    Vision::Callbacks.OnRendererNodeChanged += this;
  }
#endif
  Vision::Callbacks.OnWorldDeInit += this;
}

void VTerrainDecorationModelManager::OneTimeDeInit()
{
  Vision::ResourceSystem.UnregisterResourceManager(this);
#ifdef WIN32
  if (Vision::Editor.IsInEditor())
  {
    VisSurface_cl::OnSurfaceShaderChanged -= this;
    Vision::Callbacks.OnRendererNodeChanged -= this;
  }
#endif
  Vision::Callbacks.OnWorldDeInit -= this;
}


void VTerrainDecorationModelManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
#ifdef WIN32
  if (pData->m_pSender==&VisSurface_cl::OnSurfaceShaderChanged)
  {
    VisSurface_cl::VisSurfaceDataObject_cl &data(*((VisSurface_cl::VisSurfaceDataObject_cl *)pData));
    TriggerOnEditorEvent(IVTerrainDecorationModel::SurfaceShaderChanged,NULL,data.m_pSurface,NULL);
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnRendererNodeChanged)
  {
    TriggerOnEditorEvent(IVTerrainDecorationModel::RendererNodeChanged,NULL,NULL,NULL);
    return;
  }
#endif
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    m_spInfraredLib = NULL;
    m_spInstanceBuffer = NULL;
    return;
  }

  VASSERT(FALSE);
}


IVTerrainDecorationModel *VTerrainDecorationModelManager::CreateModel(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t &props, bool bApplyProperties, IVTerrainDecorationModel::ModelType_e eForceType)
{
  // if we find a model with the same ID, then we apply the changes to that model
  IVTerrainDecorationModel *pRes = NULL;
  if (props.m_iModelID==-1) // reserved for search for exact match of all properties
  {
    bApplyProperties = true;
    char canonicalPath[FS_MAX_PATH];
    VPathHelper::CanonicalizePath(szFilename, canonicalPath);
    for (int i=0;i<GetResourceCount();i++)
    {
      IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)GetResourceByIndex(i);
      if (!pModel || VStringHelper::SafeCompare(pModel->GetOriginalFilename(),canonicalPath)!=0 || props!=pModel->m_Properties)
        continue;
      pRes = pModel;
      break;
    }
  } 
  else if (props.m_iModelID==0)
    pRes = (IVTerrainDecorationModel *)GetResourceByName(szFilename);
  else
    pRes = FindModelByID(props.m_iModelID);

  // Create a new resource with specified properties
  if (!pRes)
  {
    for (int i=0;i<m_iModelFactoryCount;i++)
    {
      IVTerrainDecorationModel::ModelType_e eThisType = m_pModelFactory[i]->m_eType;
      if (eForceType!=IVTerrainDecorationModel::AnyType && eForceType!=eThisType)
        continue;
      pRes = m_pModelFactory[i]->CreateModelWithFilename(szFilename,&props);
      if (pRes)
        break;
    }
  }

  if (pRes)
  {
    if (bApplyProperties)
    {
      // if the filename changes, reload the resource. Changing class type is not handled here
      if (_stricmp(pRes->GetOriginalFilename(),szFilename))
      {
//        BOOL bOldFlag = pRes->IsResourceFlagSet(VRESOURCEFLAG_ALLOWUNLOAD);
//        pRes->SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
        pRes->EnsureUnloaded();
      pRes->SetFilename(szFilename);
        pRes->EnsureLoaded();
//        if (!bOldFlag)
//          pRes->RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
      }
      int iOldID = pRes->m_Properties.m_iModelID;
      pRes->m_Properties = props;
      if (pRes->m_Properties.m_iModelID==0)
        pRes->m_Properties.m_iModelID = iOldID; // retain old ID

      // get additional properties from editor
      VTerrainDecorationModelDataObject data(&OnGatherModelProperties, pRes);
      OnGatherModelProperties.TriggerCallbacks(&data);

      pRes->UpdateParameter();
    }
  }

  return pRes;
}


IVTerrainDecorationModel *VTerrainDecorationModelManager::FindModelByID(int iID) const
{
  const int iCount = GetResourceCount();
  IVTerrainDecorationModel *pRes;
  for (int i=0;i<iCount;i++)
  {
    pRes = (IVTerrainDecorationModel *)GetResourceByIndex(i);
    if (pRes!=NULL && pRes->GetID()==iID && !pRes->m_bIgnore)
      return pRes;
  }
  return NULL;
}



bool VTerrainDecorationModelManager::RegisterFactory(IVTerrainDecorationModelFactory *pFactory, bool bPrioritize)
{
  for (int i=0;i<m_iModelFactoryCount;i++)
    if (m_pModelFactory[i]==pFactory)
      return true; // already registered?

  if (m_iModelFactoryCount>=16)
    return false;

  VPointerArrayHelpers::InsertPointer((void **)m_pModelFactory, m_iModelFactoryCount, pFactory, bPrioritize ? 0 : -1);
  return true;
}

bool VTerrainDecorationModelManager::UnregisterFactory(IVTerrainDecorationModelFactory *pFactory)
{
  int iCount = m_iModelFactoryCount;
  m_iModelFactoryCount = 0;

  for (int i=0;i<iCount;i++)
    if (m_pModelFactory[i]!=pFactory)
      m_pModelFactory[m_iModelFactoryCount++] = m_pModelFactory[i];

  return m_iModelFactoryCount<iCount;
}

VisMeshBuffer_cl *VTerrainDecorationModelManager::GetInstanceBuffer(int &iInstanceCount, int &iStreamMask)
{
  if (VISION_UNLIKELY(m_spInstanceBuffer==NULL))
  {
    VisMBVertexDescriptor_t desc;
    VModelInstanceData_t::GetDesc(desc);
    m_spInstanceBuffer = new VisMeshBuffer_cl(desc, DECORATION_MAX_INSTANCE_COUNT, VisMeshBuffer_cl::MB_PRIMTYPE_TRILIST, 0, 1, VIS_MEMUSAGE_DYNAMIC, true, false);
    #ifdef HK_DEBUG
    m_spInstanceBuffer->SetFilename("<TerrainDecorationInstanceBuffer>");
    #endif
    m_iInstanceStreamMask = desc.GetStreamMask();
  }
  iInstanceCount = m_iInstancingBatchCount;
  iStreamMask = m_iInstanceStreamMask;
  return m_spInstanceBuffer;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model proxy functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////


class VTerrainDecorationModelProxy : public IVSerializationProxy
{
public:
  VTerrainDecorationModelProxy(IVTerrainDecorationModel *pModel=NULL) {m_spModel=pModel;}
  virtual IVSerializationProxyCreator *GetInstance() HKV_OVERRIDE {return m_spModel;}
  virtual void Serialize(VArchive &ar) HKV_OVERRIDE;
  IVTerrainDecorationModelPtr m_spModel;
  V_DECLARE_SERIAL_DLLEXP( VTerrainDecorationModelProxy, TERRAIN_IMPEXP )
};

V_IMPLEMENT_SERIAL( VTerrainDecorationModelProxy, VTypedObject, 0, &g_VisionEngineModule );

void VTerrainDecorationModelProxy::Serialize(VArchive &ar)
{
  char iCurrentVersion = 8;
  if (ar.IsLoading())
  {
#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=187
#endif

    char iVersion;
    ar >> iVersion; VASSERT(iVersion>=0 && iVersion<=iCurrentVersion);

#if defined(__SNC__)
#pragma diag_pop
#endif

    // load filename and properties
    IVTerrainDecorationModel::ModelType_e eForcedType = IVTerrainDecorationModel::AnyType;
    if (iVersion>=7)
      ar >> (int &)eForcedType;
    char szFilename[FS_MAX_PATH];
    ar.ReadStringBinary(szFilename,FS_MAX_PATH);
    IVTerrainDecorationModel::ModelProperties_t tempProp;
    tempProp.SerializeX(ar,iVersion);
    // take the model properties that are already in the manager (only these properties when not already loaded)
    m_spModel = VTerrainDecorationModelManager::GlobalManager().CreateModel(szFilename, tempProp, false, eForcedType);
    if (iVersion>=6)
    {
      // load the filename, but ignore it outside the editor. This is necessary for lightmap calculation of referenced terrains [#3401]
      VMemoryTempBuffer<FS_MAX_PATH> buffer;
      const char *szCasterFilename = buffer.ReadStringBinary(ar);
      if (szCasterFilename!=NULL && szCasterFilename[0] && Vision::Editor.IsInEditor())
        m_spModel->m_spCustomLightmapMesh = VMeshManager::GetMeshManager()->LoadDynamicMeshFile(szCasterFilename);
    }

  } else
  {
    VASSERT(m_spModel);
    ar << iCurrentVersion;
    ar << m_spModel->GetDecorationType(); // version 7
    ar.WriteStringBinary(m_spModel->GetFilename());
    m_spModel->m_Properties.SerializeX(ar,iCurrentVersion);
    // version 6: Save the filename of the lightmap caster mesh
    const char *szCasterFilename = (m_spModel->m_spCustomLightmapMesh!=NULL && m_spModel->m_spCustomLightmapMesh->IsLoaded()) ? m_spModel->m_spCustomLightmapMesh->GetOriginalFilename() : NULL;
    ar.WriteStringBinary(szCasterFilename);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model property functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

IVTerrainDecorationModel::ModelProperties_t::ModelProperties_t()
{
  m_iModelID = 0;
  m_fNearClip = 0.f;
  m_fFarClip = 1000.f;
  m_fRelFadeStart = 0.8f; // 80% of far clip
  m_fAverageScale = 1.0f;
  m_fScaleVariation = 0.f;
  m_fRelHeight = 0.f;
  m_eModelFlags = AlwaysVertical;
  m_fRelativeDensity = 0.8f;
  m_fRandomness = 1.0f;
  m_fApplyWind = m_fApplyConstraint = 0.f;
  m_iVisibleBitmask = 0xffffffff;
  m_fCollisionCapsuleRadius = 0.f;
//  m_fPivotHeight = 0.95f;
}


// load/save parameters that are relevant for realtime rendering. Called by VTerrainDecorationModelProxy::Serialize
void IVTerrainDecorationModel::ModelProperties_t::SerializeX(VArchive &ar, int iVersion)
{
  int iDummy = 0;
  if (ar.IsLoading())
  {
    ar >> m_iModelID;
    if (iVersion>=3)
      ar >> m_fNearClip;
    ar >> m_fFarClip;
    if (iVersion>=4)
      ar >> m_fRelFadeStart;
    ar >> iDummy;
    ar >> m_fApplyWind;
    ar >> m_fApplyConstraint;
    if (iVersion>0)
      m_eModelFlags = (ModelFlags_e)iDummy;
    if (iVersion>=2)
      ar >> m_iVisibleBitmask;
    if (iVersion>=8)
      ar >> m_fCollisionCapsuleRadius;
  } else
  {
    VASSERT(iVersion==8); // latest version!
    ar << m_iModelID;
    ar << m_fNearClip; // version 3
    ar << m_fFarClip;
    ar << m_fRelFadeStart;
    ar << (int)m_eModelFlags;
    ar << m_fApplyWind;
    ar << m_fApplyConstraint;
    ar << m_iVisibleBitmask;
    ar << m_fCollisionCapsuleRadius;
  }
}

// called by VDecorationDensityChannel::ChunkFileExchange
void IVTerrainDecorationModel::ModelProperties_t::ChunkFileExchange(VChunkFile &file, int iVersion)
{
  int iDummy = 0;
  if (file.IsLoading())
  {
    file.ReadInt(m_iModelID);
    if (iVersion>=3)
      file.ReadFloat(m_fNearClip);
    file.ReadFloat(m_fFarClip);
    if (iVersion>=4)
      file.ReadFloat(m_fRelFadeStart);
    file.ReadInt(iDummy);
    if (iVersion>=5)
      m_eModelFlags = (ModelFlags_e)iDummy;
    file.ReadFloat(m_fApplyWind);
    file.ReadFloat(m_fApplyConstraint);
    if (iVersion>=2)
      file.ReadInt((int &)m_iVisibleBitmask);
    if (iVersion>=8)
      file.ReadFloat(m_fCollisionCapsuleRadius);
  } else
  {
    VASSERT(iVersion==8); // latest version!
    file.WriteInt(m_iModelID);
    file.WriteFloat(m_fNearClip);
    file.WriteFloat(m_fFarClip);
    file.WriteFloat(m_fRelFadeStart);
    file.WriteInt((int)m_eModelFlags); // version 5
    file.WriteFloat(m_fApplyWind);
    file.WriteFloat(m_fApplyConstraint);
    file.WriteInt(m_iVisibleBitmask);
    file.WriteFloat(m_fCollisionCapsuleRadius);
  }
}


bool IVTerrainDecorationModel::ModelProperties_t::operator == (const ModelProperties_t &other) const
{
  return memcmp(this, &other, sizeof(ModelProperties_t))==0;
}
bool IVTerrainDecorationModel::ModelProperties_t::operator != (const ModelProperties_t &other) const
{
  return !(*this == other);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////


IVTerrainDecorationModel::IVTerrainDecorationModel(const char *szFilename, const ModelProperties_t *pProperties)
: VManagedResource(&VTerrainDecorationModelManager::GlobalManager())
{
  SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);

  m_bIgnore = false;
  m_pNextLOD = NULL;
  m_bTempFlag = false;
  m_bFullMatrixTransformationType = true;
  m_bValidState = m_bNeedsLightmap = false;
  SetFilename(szFilename);
  m_iLightmapSampler = -1;
  if (pProperties)
    m_Properties = *pProperties;
  m_iSupportedContextTypes = (1<<VIS_CONTEXTUSAGE_VIEWCONTEXT) | (1<<VIS_CONTEXTUSAGE_MIRROR);
}

IVSerializationProxy *IVTerrainDecorationModel::CreateProxy()
{
  return new VTerrainDecorationModelProxy(this);
}



void IVTerrainDecorationModel::UpdateParameter()
{
  //m_Properties.m_fRelHeight = m_LocalBBox.m_vMin.z*m_Properties.m_fPivotHeight; // 0=pivot

  float fFarClip = m_Properties.m_fFarClip * VTerrainSectorManager::g_fDecorationDistanceScaling;
  float fFadeStart = fFarClip*m_Properties.m_fRelFadeStart;

#if defined(_VR_DX11)
  VTerrainConstantBuffer_PerModel &data = m_PerModelConstantBuffer.BeginUpdate();
  hkvVec4& m_vDecorationParams = (hkvVec4&) data.VegetationParams;
#endif
  m_vDecorationParams.x = fFadeStart;
  m_vDecorationParams.y = (fFarClip>fFadeStart+HKVMATH_LARGE_EPSILON) ? 1.f/(fFarClip-fFadeStart) : 0.f;
  m_vDecorationParams.z = m_Properties.m_fApplyConstraint;
  m_vDecorationParams.w = m_Properties.m_fApplyWind;
#if defined(_VR_DX11)
  m_PerModelConstantBuffer.EndUpdate();
#endif

  m_iSupportedContextTypes = (1<<VIS_CONTEXTUSAGE_VIEWCONTEXT) | (1<<VIS_CONTEXTUSAGE_MIRROR);
  if (m_Properties.m_eModelFlags&CastDynamicShadows) 
    m_iSupportedContextTypes |= (1<<VIS_CONTEXTUSAGE_DEPTHSHADOW);
}




void IVTerrainDecorationModel::GatherLightmapInfo(VBaseMesh *pMesh, VLightmapSceneInfo &info, VTerrainDecorationInstance **pInst, int iCount)
{
#ifdef WIN32
  if (pMesh==NULL)
    return;
  if (pMesh->GetSurfaceCount()!=1)
  {
    Vision::Error.Warning("Lightmap shadow caster mesh '%s' must have only one material. Skipping lightmap shadows for this model", pMesh->GetFilename());
    return;
  }

  // shadows for decoration
  VLightmapPrimitive *pPrim = info.CreateNewPrimitive();
  pPrim->m_eOwnerType = VLightmapPrimitive::OWNERTYPE_OBJECT3D;
  pPrim->m_iUniqueID = 0;
  pPrim->m_eType = VLightmapPrimitive::INDEXED_MESH;
  pPrim->m_iFlags = PRIMITIVEFLAG_CASTSHADOWS;

  int iVertexCount = pMesh->GetNumOfVertices();
  int iIndexCount = pMesh->GetNumOfTriangles()*3;
  int iOverallVertices = iVertexCount*iCount;

  VisSurface_cl *pSurface = pMesh->GetSurface(0);

  pPrim->AllocateMaterials(1);
  pPrim->AllocateVertices(iOverallVertices);
  VLightmapPrimitive::MeshMaterial_t &material(pPrim->GetMaterial(0));
  pSurface->GetLightmapInfo(material);
  if (iOverallVertices>0xffff)
    material.AllocateIndices32(iIndexCount * iCount);
  else
    material.AllocateIndices(iIndexCount * iCount);

  VLightmapPrimitive::MeshVertex_t *pVertices = pPrim->GetVertices();
  unsigned short *pInd16 = material.GetIndexList();
  unsigned int *pInd32 = material.GetIndexList32();

  for (int i=0;i<iCount;i++)
  {
    hkvMat4 transform = pInst[i]->GetTransformation();
    pMesh->WriteLightmapPrimitivesToBuffer(transform, pVertices, i*iVertexCount, pInd32, pInd16);
    pVertices += iVertexCount;
    if (pInd32) pInd32+=iIndexCount;
    if (pInd16) pInd16+=iIndexCount;
  }
#endif
}

void IVTerrainDecorationModel::GatherLightmapInfo(VLightmapSceneInfo &info, VTerrainDecorationInstance **pInst, int iCount)
{
  GatherLightmapInfo(m_spCustomLightmapMesh, info,pInst,iCount);
}


void IVTerrainDecorationModel::GetDensitySampleIndicesAtPos(const VTerrainConfig &cfg, const VLargePosition &vPos, int &x, int &y) const
{
  VASSERT(vPos.IsValid(cfg));
  int iResX = cfg.m_iDensityMapSamplesPerSector[0];
  int iResY = cfg.m_iDensityMapSamplesPerSector[1];

  hkvVec2 vWorld2DensityMap(cfg.m_vInvSectorSize.x*(float)iResX,cfg.m_vInvSectorSize.y*(float)iResY);
  x = vPos.m_iSectorX*iResX + (int)(vPos.m_vSectorOfs.x*vWorld2DensityMap.x);
  y = vPos.m_iSectorY*iResY + (int)(vPos.m_vSectorOfs.y*vWorld2DensityMap.y);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// VDecorationCollisionPrimitive Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////


bool CheckRaySphereIntersection(const hkvVec3& vStart, const hkvVec3& vDir, const hkvVec3& vSphereCenter, float fRadius, float &t)
{
  float rsqr = fRadius*fRadius;
  hkvVec3 vDiff = vSphereCenter - vStart;
  float fCenterDistSqr = vDiff.getLengthSquared ();
  if (fCenterDistSqr<=rsqr) // inside sphere
  {
    t = 0.f;
    return true;
  }

  float lc = vDiff.dot (vDir);
  if (lc<0.0f) // sphere behind and not inside
    return false;
  float fInvDirLenSqr = 1.0f / vDir.getLengthSquared ();
  float rc = fCenterDistSqr - lc*lc*fInvDirLenSqr;
  if (rc>rsqr)
    return false;

  float dt = hkvMath::sqrt (rsqr - rc);

  fInvDirLenSqr = hkvMath::sqrt (fInvDirLenSqr);
  t = (lc*fInvDirLenSqr - dt)*fInvDirLenSqr;
  VASSERT(t>=0.f); // otherwise point would be inside sphere (tested earlier)
  return true;
}

bool VDecorationCollisionPrimitive::TraceDistance(const hkvVec3& startOfLine, const hkvVec3& vRayDir, float &fDist) const
{
  const float r = GetRadius();
  hkvVec3 AB = m_vDirection;
  hkvVec3 AO = (startOfLine - m_vStartPosition);
  hkvVec3 AOxAB = AO.cross(AB); // cross product
  hkvVec3 VxAB  = vRayDir.cross(AB); // cross product
  float  ab2   = AB.dot (AB); // dot product
  float a      = VxAB.dot (VxAB); // dot product
  float b      = 2 * VxAB.dot (AOxAB); // dot product
  float c      = AOxAB.dot (AOxAB) - (r*r * ab2);

  // solve second order equation : a*t^2 + b*t + c = 0
  if (hkvMath::Abs (a)<HKVMATH_LARGE_EPSILON)
    return false;
  float fInv = 1.f/a;
  float pHalf = b*-0.5f*fInv;
  float q = pHalf*pHalf - c*fInv;
  if (q<=0.f)
    return false;
  fDist = pHalf - hkvMath::sqrt (q); // always use smaller t-value

  hkvVec3 vTouchP = startOfLine+vRayDir*fDist;

  // special case handling for caps
  hkvVec3 vDiff = vTouchP - m_vStartPosition;
  float d1 = vDiff.dot (m_vDirection);
  if (d1<0.f)
  {
    if (!CheckRaySphereIntersection(startOfLine,vRayDir,m_vStartPosition,r,fDist))
      return false;
  }
  else if (d1>GetLength())
  {
    if (!CheckRaySphereIntersection(startOfLine,vRayDir,m_vStartPosition + m_vDirection * GetLength(),r,fDist))
      return false;
  }

  return true;
}



void VDecorationCollisionPrimitive::DebugRender(IVRenderInterface* pRI, const hkvMat4 &transform, float fScaling)
{
  hkvVec3 vStart = transform.transform(GetStart().getAsPosition()).getAsVec3();
  hkvVec3 vEnd = transform.transform(GetEnd().getAsPosition()).getAsVec3();

  hkvVec3 vDirection = vEnd-vStart;
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA);
  VColorRef iColor(50,255,255,100);
  float fRadius = GetRadius() * fScaling;
  pRI->RenderCapsule(vStart, vDirection, fRadius, iColor, state);
}




VRefCountedCollection<VBillboardDecorationSectorInstance> VBillboardDecorationSectorInstance::g_Instances;
V_IMPLEMENT_SERIAL( VBillboardDecorationSectorInstance, VBillboardGroupInstance, 0, &g_VisionEngineModule );


VBillboardDecorationSectorInstance::VBillboardDecorationSectorInstance(IVTerrainDecorationModel *pModel, int iBillboardCount, VTerrainSector *pOwner)
  : VBillboardGroupInstance(iBillboardCount, true)
{
  m_pSector = pOwner;
  m_iModelID = (pModel!=NULL) ? pModel->GetID() : 0;
  // We add these objects to a global collection because nothing else holds a reference to it 
  // and then it gets purged upon editor play mode
  VBillboardDecorationSectorInstance::g_Instances.Add(this);
}


void VBillboardDecorationSectorInstance::Serialize( VArchive &ar )
{
  VisVisibilityZoneSerializationMode_e eOldMode = VisVisibilityZone_cl::GetVisibilityZoneSerializationMode();
  if (ar.IsSaving())
  {
    VisVisibilityZone_cl::SetVisibilityZoneSerializationMode(VIS_SERIALIZE_AS_PROXIES);
    // this has to be done because the viszones are not persistent
    //for (int i=0;i<GetSubmeshInstanceCount();i++)
    //  GetSubmeshInstance(0)->RemoveFromAllVisibilityZones();
  }

  char iLocalVersion = 0;
  VBillboardGroupInstance::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version number");
    // load the model ID since the model pointer does not exist outside the editor. 
    // But we need the ID in the editor to cleanup (VTerrainDecorationBillboardMesh::RemoveSectorDecoration)
    ar >> m_iModelID;
  } else
  {
    ar << iLocalVersion;
    ar << m_iModelID;
  }

  // see above
  if (ar.IsSaving())
  {
    VisVisibilityZone_cl::SetVisibilityZoneSerializationMode(eOldMode);
  }
}

void VBillboardDecorationSectorInstance::DisposeObject()
{
  VBillboardGroupInstance::DisposeObject();
  VBillboardDecorationSectorInstance::g_Instances.Remove(this);
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
