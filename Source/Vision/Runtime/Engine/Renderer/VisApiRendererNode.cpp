/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiFrustumMeshHelper.hpp>

V_IMPLEMENT_SERIAL(IVRendererNode, VisTypedEngineObject_cl, 0, Vision::GetEngineModule());

void VRendererNodeHelper::Init(IVRendererNode *pRendererNode)
{
  m_pRendererNode = pRendererNode;
  m_spMeshBuffer = NULL;
  m_bFrustumMeshBufferDirty = true;
  m_pOverlayVertices = NULL;
  m_pOverlayVerticesHalfSize = NULL;
}

void VRendererNodeHelper::DeInit()
{
  m_spMeshBuffer = NULL;
  m_spSphereMeshBuffer = NULL;
  m_spConeMeshBuffer = NULL;

  V_SAFE_DELETE_ARRAY(m_pOverlayVertices);
  V_SAFE_DELETE_ARRAY(m_pOverlayVerticesHalfSize);
}

void IVRendererNode::Execute()
{
  if (!GetRenderingEnabled())
  {
    return;
  }

  VASSERT_MSG(IsInitialized(), "Renderer node must be initialized before rendering.");

  Vision::Renderer.SetCurrentRendererNode(this);
  VisRendererNodeDataObject_cl data(&Vision::Callbacks.OnRendererNodeSwitching, this);
  Vision::Callbacks.OnRendererNodeSwitching.TriggerCallbacks(&data);

  PerformVisibilityTests();
  RenderContexts(-FLT_MAX, FLT_MAX);
}

void VRendererNodeHelper::GetFrustumFarCorners(hkvVec3* pVectors)
{
  VisRenderContext_cl *pContext = m_pRendererNode->GetReferenceContext();

  hkvMat4 projMat = pContext->GetViewProperties()->getProjectionMatrix(hkvClipSpaceYRange::MinusOneToOne);
  projMat.invert ();
 
  // top left far corner
  pVectors[0].set(-1.0f,1.0f,1.0f);

  // bottom left far corner
  pVectors[1].set(-1.0f,-1.0f,1.0f);

  // bottom right far corner
  pVectors[2].set(1.0f,-1.0f,1.0f);

  // top right far corner
  pVectors[3].set(1.0f,1.0f,1.0f);

  for(int i=0;i<4;i++)
  { 
    hkvVec4 vTransformed = projMat.transform (pVectors[i].getAsPosition());
    pVectors[i] = vTransformed.getAsVec3 () / vTransformed.w;
  }
}

struct VScreenSpaceVertex_t
{
  float fPos[3];  // in this case only 2D is needed but must be DX11 compliant
  float fUV[3];   // must be 3D as well
};

VisMeshBuffer_cl *VRendererNodeHelper::GetFrustumMeshBuffer()
{
  if (!m_bFrustumMeshBufferDirty)
    return m_spMeshBuffer;

  m_bFrustumMeshBufferDirty = false;

  if(!m_spMeshBuffer)
  {
    m_spMeshBuffer = VFrustumMeshHelper::CreateMeshBuffer();
#ifdef HK_DEBUG
    m_spMeshBuffer->SetFilename("<DeferredShadingFrustumMesh>");
#endif
  }


  VFrustumMeshHelper::UpdateMeshBuffer(m_spMeshBuffer, m_pRendererNode->GetReferenceContext(), VFrustumMeshHelper::IN_VIEW_SPACE);

  return m_spMeshBuffer;
}



VisMeshBuffer_cl *VRendererNodeHelper::GetSphereMeshBuffer()
{
  if (m_spSphereMeshBuffer != NULL)
    return m_spSphereMeshBuffer;

  VDynamicMesh *pMesh = Vision::Game.LoadDynamicMesh("\\Models\\MagicBall.model", true, false);
  VASSERT(pMesh!=NULL);

  m_spSphereMeshBuffer = new VisMeshBuffer_cl();
  m_spSphereMeshBuffer->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
#ifdef HK_DEBUG
  m_spSphereMeshBuffer->SetFilename("<DeferredShadingSphereMesh>");
#endif

  VisMBVertexDescriptor_t descr;
  descr.Reset();
  descr.m_iPosOfs = 0 | VERTEXDESC_FORMAT_FLOAT3;
  descr.m_iStride = sizeof(float)*3;

  // get model properties
  int iVertexCount = pMesh->GetNumOfVertices();
  int iIndexCount = pMesh->GetNumOfTriangles() * 3;

  // copy vertex buffer, normalize vertex positions
  m_spSphereMeshBuffer->AllocateVertices(descr, iVertexCount); 
  float *pDestVerts = (float *)m_spSphereMeshBuffer->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
  pMesh->CopyMeshVertices(pDestVerts, descr, 0, iVertexCount);
  for (int i=0; i<iVertexCount*3; i+=3)
  {
    hkvVec3 vPos(pDestVerts[i], pDestVerts[i+1], pDestVerts[i+2]);
    vPos.normalizeIfNotZero();
    memcpy(&pDestVerts[i], vPos.data, 3*sizeof(float));
  }
  m_spSphereMeshBuffer->UnLockVertices();

  // copy index buffer
  m_spSphereMeshBuffer->AllocateIndexList(iIndexCount);
  unsigned short *pDestIndices = (unsigned short *)m_spSphereMeshBuffer->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
  pMesh->CopyMeshIndices(pDestIndices, (VisSurface_cl *)NULL);
  m_spSphereMeshBuffer->UnLockIndices();

  return m_spSphereMeshBuffer;
}

#define NUM_CONE_SURFACE_VERTICES 128


VisMeshBuffer_cl *VRendererNodeHelper::GetConeMeshBuffer()
{
  if (m_spConeMeshBuffer != NULL)
    return m_spConeMeshBuffer;

  m_spConeMeshBuffer = new VisMeshBuffer_cl();
  m_spConeMeshBuffer->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
#ifdef HK_DEBUG
  m_spConeMeshBuffer->SetFilename("<DeferredShadingConeMesh>");
#endif

  // describe the compact model vertex format
  VisMBVertexDescriptor_t descr;
  descr.Reset();
  descr.m_iPosOfs = 0 | VERTEXDESC_FORMAT_FLOAT3;
  descr.m_iStride = sizeof(float)*3;

  // set up vertex positions
  int iVertCount = NUM_CONE_SURFACE_VERTICES;
  m_spConeMeshBuffer->AllocateVertices(descr, iVertCount+2);
  int iIndexCount = (iVertCount*2-2)*3;
  m_spConeMeshBuffer->AllocateIndexList(iIndexCount);

  hkvVec3* pVertexBuf = (hkvVec3* )m_spConeMeshBuffer->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
  unsigned short *pIndexBuf = (unsigned short*)m_spConeMeshBuffer->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
  {
    pVertexBuf[0].set(0.0f, 0.0f, 0.0f);
    float fAngle = 0.0f;
    float fAngleStep = 1.0f / (float)iVertCount;
    for (int i=0; i<iVertCount; i++, fAngle += fAngleStep) 
    {
      float fRad = hkvMath::pi () * 2.0f * fAngle;
      float fX = hkvMath::sinRad (fRad);
      float fY = hkvMath::cosRad (fRad);

      pVertexBuf[i+1].set(1.0f, fX, fY);
    }
  }
  {
    int j=0;
    // Set surface indices
    for (int i=1; i<iVertCount; i++) 
    {
      pIndexBuf[j++] = 0;
      pIndexBuf[j++] = i;
      pIndexBuf[j++] = i+1;
    }
    pIndexBuf[j++] = 0;
    pIndexBuf[j++] = iVertCount;
    pIndexBuf[j++] = 1;

    // Set cap indices
    for (int i=3; i<=iVertCount; i++) 
    {
      pIndexBuf[j++] = i;
      pIndexBuf[j++] = i-1;
      pIndexBuf[j++] = 1;
    }

    VASSERT(j==iIndexCount);
  }
  m_spConeMeshBuffer->UnLockIndices();
  m_spConeMeshBuffer->UnLockVertices();

  m_spConeMeshBuffer->EnableDefaultZBufferWriting(FALSE);
  m_spConeMeshBuffer->SetDefaultTransparency(VIS_TRANSP_ADDITIVE);
  m_spConeMeshBuffer->SetDefaultCullMode(CULL_NONE);

  return m_spConeMeshBuffer;
}

void IVRendererNode::ReInitializeRenderer()
{
  if(!IsInitialized())
  {
    InitializeRenderer();
  }
  else
  {
    // Take care of deregistration and re-registration at the correct index
    VScopedRendererNodeDeinit lock(this);
  }
  m_RendererNodeHelper.InvalidateFrustum();
  Vision::Callbacks.OnReferenceContextChanged.TriggerCallbacks();
}

void IVRendererNode::BeginPropertyUpdate()
{
  VASSERT(m_iUpdateCounter>=0);
  m_iUpdateCounter++;
}

bool IVRendererNode::SetVariable(const char *szName, const char *szValue)
{
  bool bRes = VisTypedEngineObject_cl::SetVariable(szName, szValue);

  if (m_iUpdateCounter == 0)
  {
    m_bUpdateDirty = false;
    if(m_bIsInitialized)
    {
      ReInitializeRenderer();
    }
  }
  else
  {
    m_bUpdateDirty = true;
  }

  return bRes;
}


void IVRendererNode::EndPropertyUpdate()
{
  VASSERT(m_iUpdateCounter>0);
  m_iUpdateCounter--;
  if (m_iUpdateCounter == 0 && m_bUpdateDirty)
  {
    m_bUpdateDirty = false;
    if(m_bIsInitialized)
    {
      ReInitializeRenderer();
    }
  }
}


void VRendererNodeHelper::ComputeOverlayVertices(int iWidth, int iHeight, Overlay2DVertex_t *pVertices)
{
  // Create vertices for overlay rendering
  pVertices[0].Set(0.0f, 0.0f, 0.0f, 0.0f);
  pVertices[1].Set(0.0f, (float)iHeight, 0.0f, 1.0f);
  pVertices[2].Set((float)iWidth, 0.0f, 1.0f, 0.0f);
  pVertices[3].Set((float)iWidth, 0.0f, 1.0f, 0.0f);
  pVertices[4].Set(0.0f, (float)iHeight, 0.0f, 1.0f);
  pVertices[5].Set((float)iWidth, (float)iHeight, 1.0f, 1.0f);

#ifdef _VR_DX9
  int i;
  for (i=0; i<6; i++)
  {
    pVertices[i].texCoord.x += 0.5f/iWidth;
    pVertices[i].texCoord.y += 0.5f/iHeight;   
  }
#endif
}

Overlay2DVertex_t *VRendererNodeHelper::GetOverlayVertices()
{
  if (m_pOverlayVertices)
    return m_pOverlayVertices;

  m_pOverlayVertices = new Overlay2DVertex_t[6];
  VASSERT(m_pOverlayVertices!=NULL);

  int iWidth, iHeight;
  m_pRendererNode->GetReferenceContext()->GetSize(iWidth, iHeight);
  ComputeOverlayVertices(iWidth, iHeight, m_pOverlayVertices);

  return m_pOverlayVertices;
}

Overlay2DVertex_t *VRendererNodeHelper::GetOverlayVerticesHalfSize()
{
  if (m_pOverlayVerticesHalfSize)
    return m_pOverlayVerticesHalfSize;

  m_pOverlayVerticesHalfSize = new Overlay2DVertex_t[6];
  VASSERT(m_pOverlayVerticesHalfSize!=NULL);

  int iWidth, iHeight;
  m_pRendererNode->GetReferenceContext()->GetSize(iWidth, iHeight);
  ComputeOverlayVertices(iWidth/2, iHeight/2, m_pOverlayVerticesHalfSize);

  return m_pOverlayVerticesHalfSize;
}


void VRendererNodeHelper::InvalidateFrustum()
{
  V_SAFE_DELETE_ARRAY(m_pOverlayVertices);
  V_SAFE_DELETE_ARRAY(m_pOverlayVerticesHalfSize);

  m_bFrustumMeshBufferDirty = true;
}


void IVRendererNode::SetRenderingEnabled(bool bStatus)
{
  m_bRenderingEnabled = bStatus;
}

void IVRendererNode::Serialize(VArchive &ar)
{
  VisTypedEngineObject_cl::Serialize(ar);
}

IVRendererNode::IVRendererNode(VisRenderContext_cl *pTargetContext) 
: IVRenderContextContainer()
, m_bRenderingEnabled(true)
, m_bIsInitialized(false)
, m_fPixelAspectRatio( Vision::Video.GetCurrentConfig()->GetPixelAspectRatio() )
, m_bUpdateDirty( false )
, m_iUpdateCounter( 0 )
{
  m_RendererNodeHelper.Init(this);
  m_spFinalTargetContext = pTargetContext;
}

IVRendererNode::~IVRendererNode()
{
  m_RendererNodeHelper.DeInit();
}

void IVRendererNode::OnViewPropertiesChanged()
{
  m_RendererNodeHelper.InvalidateFrustum();
  
  if(m_spFinalTargetContext)
  {
    int iStorageWidth;
    int iStorageHeight;
    m_spFinalTargetContext->GetSize(iStorageWidth, iStorageHeight);

    float fStorageAspectRatio = float(iStorageWidth) / float(iStorageHeight);
    float fDisplayAspectRatio = fStorageAspectRatio * m_fPixelAspectRatio;

    GetViewProperties()->setDisplayAspectRatio(fDisplayAspectRatio);
  }
}

VRendererNodeHelper *IVRendererNode::GetRendererNodeHelper()
{
  return &m_RendererNodeHelper;
}

IVRendererNode* IVRendererNode::ReadFromStream(IVFileInStream *pIn)
{
  VASSERT(pIn!=NULL);
  VArchive archive(NULL,pIn, Vision::GetTypeManager());
  int iVers;
  archive >> iVers;
  archive.SetLoadingVersion(iVers);
  IVRendererNode *pRenderer = (IVRendererNode *)archive.ReadObject(NULL);
  archive.Close();

  return pRenderer;
}

void IVRendererNode::WriteToStream(IVFileOutStream *pOut)
{
  VASSERT(pOut!=NULL);
  VArchive archive(NULL,pOut, Vision::GetTypeManager());
  archive << Vision::GetArchiveVersion();
  archive << this;
  archive.Close();
}

void IVRendererNode::SetPixelAspectRatio(float fPixelAspectRatio)
{
  m_fPixelAspectRatio = fPixelAspectRatio;
  OnViewPropertiesChanged();
}

float IVRendererNode::GetPixelAspectRatio()
{
  return m_fPixelAspectRatio;
}

void IVRendererNode::InitializeRenderer()
{
  VASSERT_MSG(GetFinalTargetContext() != NULL, "The final target context must be set before initialization.");
}

void IVRendererNode::DeInitializeRenderer()
{
  VASSERT_MSG(Vision::Renderer.GetRendererNodeIndex(this) == -1, "The renderer node must be deregistered before deinitialization!");
}

START_VAR_TABLE(IVRendererNode, VisTypedEngineObject_cl, "IVRendererNode", VFORGE_HIDECLASS, "")  
END_VAR_TABLE



VScopedRendererNodeDeinit::VScopedRendererNodeDeinit( IVRendererNode* pNode ) : m_spNode(pNode), m_bWasInitialized(false)
{
  if(m_spNode)
  {
    m_iNodeIdx = Vision::Renderer.GetRendererNodeIndex(m_spNode);

    if(m_iNodeIdx != -1)
    {
      Vision::Renderer.SetRendererNode(m_iNodeIdx, NULL);
    }

    if(m_spNode->IsInitialized())
    {
      m_bWasInitialized = true;
      m_spNode->DeInitializeRenderer();
    }
  }
}

VScopedRendererNodeDeinit::~VScopedRendererNodeDeinit()
{
  if(m_bWasInitialized)
  {
    m_spNode->InitializeRenderer();

    if(m_iNodeIdx != -1)
    {
      Vision::Renderer.SetRendererNode(m_iNodeIdx, m_spNode);
    }
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
