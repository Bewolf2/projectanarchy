/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BillboardGroup.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderConstantHelper.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Class VBillboardStaticMesh
//////////////////////////////////////////////////////////////////////////////////////////////////////

VBillboardStaticMesh::VBillboardStaticMesh()
{
  FlagAsLoaded();
  RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
  SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  // a billboard mesh has only one surface
  AllocateSurfaces(1);
  Surface().SetLightingMode (VIS_LIGHTING_FULLBRIGHT);
  Surface().RaiseMaterialFlag (VisSurface_cl::VSF_NoDynamicLightShaders);
  Surface().SetGeometryTopology (VisSurface_cl::VGT_BillboardVerticalAligned);

}

void VBillboardStaticMesh::InitMesh(int iBillboardCount, VBillboardGroupInstance *pInstanceCreateDefaultFX)
{
  EnsureMeshCreated();
  VisMeshBuffer_cl *pMesh = GetMeshBuffer();

#ifdef HK_DEBUG
  pMesh->SetFilename("<BillBoardMesh>");
#endif

  iBillboardCount = hkvMath::Min(iBillboardCount,MAX_BILLBOARDS_PER_GROUP);

#ifdef BILLBOARDS_AS_CROSSES
  // allocate vertices
  VisMBVertexDescriptor_t desc;
  VBillboardCrossVertex_t::GetDesc(desc);
  pMesh->AllocateVertices(desc,iBillboardCount*8,VIS_MEMUSAGE_STATIC);

  // prepare indices
  pMesh->AllocateIndexList(iBillboardCount*12);
  if (iBillboardCount>0)
  {
    unsigned short *pInd = (unsigned short *)pMesh->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
    int iBase = 0;
    for (int i=0;i<iBillboardCount;i++,iBase+=8)
    {
      for (int iAlt=0;iAlt<2;iAlt++,pInd+=6)
      {
        pInd[0] = iBase+4+iAlt;
        pInd[1] = iBase+2+iAlt;
        pInd[2] = iBase+0+iAlt;
        pInd[3] = iBase+4+iAlt;
        pInd[4] = iBase+6+iAlt;
        pInd[5] = iBase+2+iAlt;
      }
    }
    pMesh->UnLockIndices();
  }
#else
  // allocate vertices
  VisMBVertexDescriptor_t desc;
  VBillboardVertex_t::GetDesc(desc);
  pMesh->AllocateVertices(desc,iBillboardCount*4,VIS_MEMUSAGE_STATIC);

  // prepare indices
  pMesh->AllocateIndexList(iBillboardCount*6);
  if (iBillboardCount>0)
  {
    unsigned short *pInd = (unsigned short *)pMesh->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
    int iBase = 0;
    for (int i=0;i<iBillboardCount;i++,pInd+=6,iBase+=4)
    {
      pInd[0] = iBase+2;
      pInd[1] = iBase+1;
      pInd[2] = iBase+0;
      pInd[3] = iBase+2;
      pInd[4] = iBase+3;
      pInd[5] = iBase+1;
    }
    pMesh->UnLockIndices();
  }
#endif

  pMesh->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
  UpdateStreamMasks();

  // surface & effect:
  if (pInstanceCreateDefaultFX!=NULL)
  {
    if (Surface().m_spCurrentEffect==NULL)
    {
      VCompiledEffect *pFX = CreateBillboardEffect(pInstanceCreateDefaultFX);
      VASSERT(pFX);
      Surface().SetEffect(pFX);
    }
    if (Surface().m_spDiffuseTexture==NULL)
      Surface().m_spDiffuseTexture = Vision::TextureManager.Load2DTexture("\\plainwhite.dds");
  }

  // submeshes:
  AllocateSubmeshes(1);
  VisStaticSubmesh_cl *pSubmesh = GetSubmesh(0);
  pSubmesh->SetRenderRange(0,0); // set later
  pSubmesh->SetRenderVertexRange(0, pMesh->GetVertexCount());
  pSubmesh->SetSurface(&Surface(),0);
  
  VBaseGeometryInfo &data = pSubmesh->GetGeometryInfo();   //edit the shared/default submeshdata of the mesh (since m_iGeometryInfoIndex==-1)
  data.m_iLightMask = 0;
  data.m_iTraceMask = 0;
  data.m_sFlags = 0;  //don't cast any shadows
}


void VBillboardGroupInstance::SetTransparency(VIS_TransparencyType transp, bool bDeferredUseLighting)
{
  VBillboardStaticMesh *pSM = (VBillboardStaticMesh *)GetMesh();
  pSM->Surface().SetTransparencyType (transp);
  pSM->Surface().SetUserFlags (bDeferredUseLighting ? 0 : MATERIALFLAG_NODEFERREDLIGHTING);

  // re-assign effect because of alpha test and blend mode:
  pSM->ReassignShader(this);

 #if !defined(_VR_GLES2) && !defined( HK_ANARCHY )
  if (Vision::Renderer.IsRendererNodeOfType(V_RUNTIME_CLASS(VDeferredRenderingSystem)))
    pSM->Surface().SetPassType(bDeferredUseLighting ? VPT_AUTO : VPT_TransparentPass);
 #endif 
}

VCompiledEffect *VBillboardStaticMesh::CreateBillboardEffect(VBillboardGroupInstance *pInstance)
{
  const bool bForceNoDeferred = Surface().IsUserFlagSet (MATERIALFLAG_NODEFERREDLIGHTING);

#if defined(_VR_GLES2) || defined( HK_ANARCHY )
  const bool bDeferredRenderer = false;
#else

  const bool bDeferredRenderer = Vision::Renderer.IsRendererNodeOfType(V_RUNTIME_CLASS(VDeferredRenderingSystem))==TRUE;

#endif


  bool bUseDeferred = bDeferredRenderer && !bForceNoDeferred;
  VShaderEffectLib* pFXLib = Vision::Shaders.LoadShaderLibrary ((bUseDeferred) ? "\\Shaders\\DeferredShadingBillboards.ShaderLib" : "\\Shaders\\Billboards.ShaderLib", SHADERLIBFLAG_HIDDEN);
  char szFXName[128];
#ifdef BILLBOARDS_AS_CROSSES
  strcpy(szFXName, bUseDeferred ? "DeferredBillboard_Crosses" : "Billboard_Crosses");
#else
  strcpy(szFXName, bUseDeferred ? "DeferredBillboards" : "Billboards");
#endif

  const char *szDepthWrite = "DepthWrite=true";
  const char *szPassType = VPassTypeToString(VPT_PrimaryOpaquePass);

  // most real transparent modes work better without z-writing
  VIS_TransparencyType transp = Surface().GetTransparencyType();
  bool bNoDepthWrite = transp!=VIS_TRANSP_NONE && transp!=VIS_TRANSP_ALPHA && transp!=VIS_TRANSP_ALPHATEST;
  if (bNoDepthWrite)
    szDepthWrite = "DepthWrite=false";
  if ((bDeferredRenderer && bForceNoDeferred) || bNoDepthWrite)
    szPassType = VPassTypeToString(VPT_TransparentPass);

  const hkvVec2 &vClipDist(pInstance->m_vClipDist);
  char szParam[256];
  float fFar = vClipDist.y>0.f ? vClipDist.y : 999999999999.f;
  sprintf(szParam,"ClipDistances=%.3f,%.3f;%s;PassType=%s",vClipDist.x,fFar,szDepthWrite,szPassType);

  return Vision::Shaders.CreateEffect(szFXName,szParam,EFFECTCREATEFLAG_NONE,pFXLib);
}

/*
void VBillboardStaticMesh::AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
{
  VisStaticMesh_cl::AccumulateMemoryFootprint(iUniqueSys,iUniqueGPU,iDependentSys,iDependentGPU);
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Class VBillboardGroupInstance
//////////////////////////////////////////////////////////////////////////////////////////////////////

VBillboardGroupInstance::VBillboardGroupInstance(int iBillboardCount,bool bCreateDefaultFX)
{
  m_vClipDist.x = 0.f;
  m_vClipDist.y = -1.f;
  m_iCollisionMask = 0;
  VBillboardStaticMesh *pSM = new VBillboardStaticMesh();
  pSM->InitMesh(iBillboardCount,bCreateDefaultFX ? this : (VBillboardGroupInstance *)NULL);
  pSM->CreateInstance(hkvMat4::IdentityMatrix (), this, false);

#ifdef HK_DEBUG
  pSM->SetFilename("<BillBoardGroup>");
  VisStaticSubmeshInstance_cl *pSMI = GetSubmeshInstance(0);  
  VASSERT(!pSMI->GetCastDynamicShadows());
  VASSERT(!GetCastStaticShadows());
  VASSERT(!GetCollisionBitmask());
#endif
}


void VBillboardGroupInstance::SetUsedBillboardCount(int iBillboardCount, const hkvAlignedBBox &groupbox)
{
  VBillboardStaticMesh *pSM = (VBillboardStaticMesh *)GetMesh();
  VASSERT(pSM != NULL && pSM->GetMeshBuffer());
  VisStaticSubmesh_cl *pSubmesh = pSM->GetSubmesh(0);
    
  
  pSubmesh->SetRenderRange(0,iBillboardCount*6);
  pSM->SetBoundingBox(groupbox);
  pSubmesh->SetBoundingBox(groupbox);

  ReComputeBoundingBoxes();
  AssignToVisibilityZones();
  UpdateMeshClipDistances();
}




V_IMPLEMENT_SERIAL( VBillboardGroupInstance, VisStaticMeshInstance_cl, 0, &g_VisionEngineModule );
void VBillboardGroupInstance::Serialize( VArchive &ar )
{
  char iLocalVersion = 2;
  if (ar.IsLoading())
  {
    int iBillboardCount;
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=2,"Invalid version number");
    if (iLocalVersion>=2)
      ar >> m_vClipDist; // version 2

    // create static mesh
    VBillboardStaticMesh *pSM = new VBillboardStaticMesh();

    // surface
    VisSurface_cl &srf(pSM->Surface());
    VMemoryTempBuffer<FS_MAX_PATH> sDiffuse;
    const char *szDiffuse = sDiffuse.ReadStringBinary(ar);
    if (szDiffuse != NULL && szDiffuse[0])
      srf.m_spDiffuseTexture = Vision::TextureManager.Load2DTexture(szDiffuse);
    int iTransp;
    ar >> iTransp;
    srf.SetTransparencyType ((VIS_TransparencyType) iTransp);
    if (iLocalVersion>=1)
    {
      int flags;
      ar >> flags;
      srf.SetUserFlags (flags);
    }

    VisEffectConfig_cl fx;
    fx.SerializeX(ar);

#ifndef BILLBOARDS_AS_CROSSES
    if (fx.HasEffects())
      srf.SetEffect(fx.GetEffect()); // otherwise gets an effect in Init
#endif
    // mesh data
    ar >> iBillboardCount;
    pSM->InitMesh(iBillboardCount,this);
    pSM->CreateInstance(hkvMat4::IdentityMatrix (),this, false);

    VASSERT(pSM && pSM->GetMeshBuffer());
    VisStaticSubmesh_cl *pSubmesh = pSM->GetSubmesh(0);

#ifdef BILLBOARDS_AS_CROSSES
    pSubmesh->SetRenderRange(0,iBillboardCount*12);
    pSM->Surface().SetDoubleSided (true);
    VBillboardStaticMesh::VBillboardCrossVertex_t *pV = LockVertices();
    VBillboardStaticMesh::VBillboardVertex_t srcVert[4];

    // convert billboard-wise
    float fSeed = 0.f;
    const int iVertCount = 4;
    for (int i=0;i<iBillboardCount;i++,pV+=8, fSeed+=4.11f)
    {
      ar.Read(&srcVert,iVertCount*sizeof(VBillboardStaticMesh::VBillboardVertex_t),VBillboardStaticMesh::VBillboardVertex_t::GetFormatString(),iVertCount);
      
      // remap to cross
      VBillboardStaticMesh::VBillboardCrossVertex_t::Create2CrossingVertices(&pV[0],srcVert[0],fSeed);
      VBillboardStaticMesh::VBillboardCrossVertex_t::Create2CrossingVertices(&pV[2],srcVert[1],fSeed);
      VBillboardStaticMesh::VBillboardCrossVertex_t::Create2CrossingVertices(&pV[4],srcVert[2],fSeed);
      VBillboardStaticMesh::VBillboardCrossVertex_t::Create2CrossingVertices(&pV[6],srcVert[3],fSeed);
    }
#else
    // load vertex data as one block
    pSubmesh->SetRenderRange(0,iBillboardCount*6);
    VBillboardStaticMesh::VBillboardVertex_t *pV = LockVertices();
    int iVertCount = iBillboardCount*4;
    ar.Read(pV,iVertCount*sizeof(VBillboardStaticMesh::VBillboardVertex_t),VBillboardStaticMesh::VBillboardVertex_t::GetFormatString(),iVertCount);

#endif

	// Set the color components in the correct place.
#if (defined (WIN32) && defined(_VR_DX11))
    for(int i = 0; i < iVertCount; ++i)
    {
      VColorRef* pRGBA = &(pV+i)->iColor;
      pRGBA->SetRGBA(pRGBA->b, pRGBA->g, pRGBA->r, pRGBA->a);
    }
          
#elif defined (_VISION_PS3) 
    for(int i = 0; i < iVertCount; ++i)
    {
      VColorRef* pRGBA = &(pV+i)->iColor;
      pRGBA->SetRGBA(pRGBA->g, pRGBA->b, pRGBA->a, pRGBA->r);
    }
    
#endif

    UnLockVertices();

    // base serialization
    VisStaticMeshInstance_cl::Serialize(ar);
  } else
  {
    ar << iLocalVersion;
    ar << m_vClipDist; // version 2

    // save surface information
    VBillboardStaticMesh *pSM = (VBillboardStaticMesh *)GetMesh();
    VisSurface_cl &srf(pSM->Surface());
    ar.WriteStringBinary(srf.GetBaseTexture());
    ar << (int)srf.GetTransparencyType ();
    ar << srf.GetUserFlags (); // version 1
    VisEffectConfig_cl fx;
    if (srf.m_spCurrentEffect && srf.m_spCurrentEffectLib)
      fx.AddEffect(srf.m_spCurrentEffect);
    fx.SerializeX(ar);

    // save mesh information
    int iFirst,iBillboardCount;
    VASSERT(pSM && pSM->GetMeshBuffer());
    VisStaticSubmesh_cl *pSubmesh = pSM->GetSubmesh(0);
    pSubmesh->GetRenderRange(iFirst,iBillboardCount);
    iBillboardCount/=6;

    // save the raw vertex data here rather than mesh buffer serialization to a) reduce overhead and b) make the data reusable for other platforms
    ar << iBillboardCount;
    int iVertCount = iBillboardCount*4;
    VBillboardStaticMesh::VBillboardVertex_t *pV = (VBillboardStaticMesh::VBillboardVertex_t *)pSM->GetMeshBuffer()->LockVertices(VIS_LOCKFLAG_READONLY);

	// Because the color doesn't get stored in a VColorRef object we need to always save
	// the color components in the same way. For DX11 we need to change the color components
	// in order gets saved in the same order as DX9 and then restore it to its previous order.
#if defined (WIN32) && defined(_VR_DX11)
    for(int i = 0; i < iVertCount; ++i)
    {
      VColorRef* pRGBA = &(pV+i)->iColor;
      pRGBA->SetRGBA(pRGBA->b, pRGBA->g, pRGBA->r, pRGBA->a);
    }
#endif

    ar.Write(pV,iVertCount*sizeof(VBillboardStaticMesh::VBillboardVertex_t),VBillboardStaticMesh::VBillboardVertex_t::GetFormatString(),iVertCount);

#if defined (WIN32) && defined(_VR_DX11)
    for(int i = 0; i < iVertCount; ++i)
    {
      VColorRef* pRGBA = &(pV+i)->iColor;
      pRGBA->SetRGBA(pRGBA->b, pRGBA->g, pRGBA->r, pRGBA->a);
    }
#endif

    pSM->GetMeshBuffer()->UnLockVertices();

    // use default functionality to save the mesh instance
    VisStaticMeshPtr spTemp = GetMesh();
    m_spMesh = NULL;
    VisStaticMeshInstance_cl::Serialize(ar);
    m_spMesh = spTemp;
  }
}


void VBillboardGroupInstance::UpdateMeshClipDistances()
{
  const hkvAlignedBBox &bbox(GetBoundingBox());
  if (!bbox.isValid() || (m_vClipDist.x<=0 && m_vClipDist.y<=0))
  {
    SetClipSettings(0.f,-1.f,NULL); // disable
    return;
  }

  // compensate for the mesh using a point distance to clip
  float fMeshNear = (m_vClipDist.x>0.f) ? m_vClipDist.x : 0.f;
  float fMeshFar = (m_vClipDist.y>0.f) ? m_vClipDist.y : 0.f;
  SetClipSettings(fMeshNear, fMeshFar, NULL);
  if (fMeshFar>fMeshNear)
    SetClipMode(VIS_LOD_TEST_BOUNDINGBOX|VIS_LOD_TEST_APPLYLODSCALING);
}

void VBillboardGroupInstance::SetClipDistances(float fNear, float fFar)
{
  m_vClipDist.x = fNear;
  m_vClipDist.y = fFar;
  UpdateMeshClipDistances();
  ReassignShader();
}

#ifdef SUPPORTS_LIT_FILE_LOADING

bool VBillboardGroupInstance::GatherLightmapInfo(VLightmapSceneInfo &sceneInfo)
{
#ifdef BILLBOARDS_AS_CROSSES
  return VisStaticMeshInstance_cl::GatherLightmapInfo(sceneInfo); // treated as a standard mesh
#else
  int iFirst,iBillboardCount;
  VBillboardStaticMesh *pSM = (VBillboardStaticMesh *)GetMesh();
  VisSurface_cl &srf(pSM->Surface());
  VASSERT(pSM && pSM->GetMeshBuffer());
  VisStaticSubmesh_cl *pSubmesh = pSM->GetSubmesh(0);
  pSubmesh->GetRenderRange(iFirst,iBillboardCount);
  iBillboardCount/=6;
  bool bIsCaster = m_bCastStaticShadows;  
  bIsCaster &= sceneInfo.IsUsedCasterID(GetUniqueID());

  if (!bIsCaster || iBillboardCount==0)
    return false;

  VLightmapPrimitive *pPrim = sceneInfo.CreateNewPrimitive();
//  pPrim->AllocateVertices(iVertexCount);
  pPrim->m_eType = VLightmapPrimitive::INDEXED_MESH;
  pPrim->m_eOwnerType = VLightmapPrimitive::OWNERTYPE_STATICMESH;
  pPrim->m_iUniqueID = GetUniqueID();
  pPrim->m_iFlags = PRIMITIVEFLAG_CASTSHADOWS;

  pPrim->AllocateMaterials(1);
  VLightmapPrimitive::MeshMaterial_t &mat = pPrim->GetMaterial(0);
  srf.GetLightmapInfo(mat); // material base properties
  mat.m_sMaterialName = "Billboard Group";

  const hkvVec3 vSpanUp(0,0,1); // does not change
  hkvVec3 vSpanRight(1,0,0);
  hkvVec3 vSpanDir(0,1,0);

  // find most significant directional light and take orientation from it
  const int iLightCnt = VisLightSource_cl::ElementManagerGetSize();
  float fMaxMult = 0.f;
  for (int i=0;i<iLightCnt;i++) 
  {
    VisLightSource_cl *pLight = VisLightSource_cl::ElementManagerGet(i);
    if (pLight==NULL || pLight->GetType()!=VIS_LIGHT_DIRECTED || pLight->GetMultiplier()<fMaxMult)
      continue;
    fMaxMult = pLight->GetMultiplier();
    vSpanDir = pLight->GetObjDir();
    vSpanRight = vSpanUp;
    vSpanRight.cross(vSpanDir);
    vSpanDir = vSpanRight; 
    vSpanDir.cross(vSpanUp);
    vSpanRight.normalizeIfNotZero();
    vSpanDir.normalizeIfNotZero();
  }



  int iVertexCount = iBillboardCount*4;
  pPrim->AllocateVertices(iVertexCount);
  mat.AllocateIndices(iBillboardCount*6);

  // vertices
  const hkvMat4& transform(this->GetTransform());
  VLightmapPrimitive::MeshVertex_t *pDestV = pPrim->GetVertices();
  BILLBOARD_TARGET_VERTEX *pSrcV = (BILLBOARD_TARGET_VERTEX*) pSM->GetMeshBuffer()->LockVertices(VIS_LOCKFLAG_READONLY);
  for (int i=0;i<iVertexCount;i++, pDestV++, pSrcV++)
  {
    hkvVec3 vPos = pSrcV->vPivot;
    vPos = transform * vPos;
    pDestV->vPos = (hkvVec3) vPos + vSpanRight * pSrcV->vCorner.x + vSpanUp * pSrcV->vCorner.y;
    pDestV->vBaseUV = pSrcV->vTexCoord;
    pDestV->vNormal = vSpanDir;
    pDestV->vTangent = vSpanRight;
    pDestV->vBiTangent = vSpanUp;
  }

  pSM->GetMeshBuffer()->UnLockVertices();

  // copy over indices
  unsigned short *pSrcI = (unsigned short*) pSM->GetMeshBuffer()->LockIndices(VIS_LOCKFLAG_READONLY);
  unsigned short *pDestI = mat.GetIndexList();
  memcpy(pDestI,pSrcI,iBillboardCount*6*sizeof(short));
  pSM->GetMeshBuffer()->UnLockIndices();

  return true;
#endif // BILLBOARDS_AS_CROSSES
}

#endif // SUPPORTS_LIT_FILE_LOADING

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
