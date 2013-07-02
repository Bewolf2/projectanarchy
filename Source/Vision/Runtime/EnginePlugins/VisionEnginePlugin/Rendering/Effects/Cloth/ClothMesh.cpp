/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/ClothMesh.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#pragma warning(push)  
#pragma warning(disable:4996)   //Disable deprecation warnings

extern VModule g_VisionEngineModule;

// helper function
inline float Helper_GetDist(float *f1, float *f2)
{
  float diff[3] = {f2[0]-f1[0],f2[1]-f1[1],f2[2]-f1[2]};
  return hkvMath::sqrt (diff[0]*diff[0]+diff[1]*diff[1]+diff[2]*diff[2]);
}

// vertex adjacency structure for spring generation
struct VertexAdjacencyInfo_t
{
  VertexAdjacencyInfo_t() {iNeighbourCount=0;}
  inline bool Add(unsigned short v) // adds a neighbour vertex to the list (if not already in it)
  {
    int i;
    for (i=0;i<iNeighbourCount;i++) // already there?
      if (iNeighbourIndex[i]==v)
        return false;
    if (iNeighbourCount>=8)
    {
      VASSERT(iNeighbourCount<8);
      return false;
    }
    iNeighbourIndex[iNeighbourCount++] = v;
    return true;
  }
  short iNeighbourCount; // [0..7]
  unsigned short iNeighbourIndex[8];
};


V_IMPLEMENT_DYNCREATE( ClothMeshPhysicsTask_cl, VThreadedTask, &g_VisionEngineModule );


ClothMeshPhysicsTask_cl::ClothMeshPhysicsTask_cl(VClothMesh *pMesh) : VThreadedTask()
{
  VASSERT(pMesh != NULL);
  m_pMesh = pMesh;
  m_iTickCount = 0;
}


void ClothMeshPhysicsTask_cl::Run(VManagedThread *pThread)
{
  int i;
  for (i=0; i<m_iTickCount; i++)
    m_pMesh->HandleMeshPhysics(m_fDeltaTime, m_fGravity);
  m_pMesh->CalcBoundingBox();
}

VString VClothMesh::g_sLastError;

VClothMesh::VClothMesh()
{
  m_iSpringCount = 0;
  m_pSpring = NULL;
  m_iVertexCount = 0;
  m_pVertexDelta = NULL;
  m_pParticle = NULL;
  m_bBoxValid = false;
  m_pLocalSpacePos = NULL;
}

VClothMesh::~VClothMesh()
{
  FreeMesh();
}


void VClothMesh::FreeMesh()
{
  FreeSprings();
  m_iVertexCount = 0;
  V_SAFE_DELETE_ARRAY(m_pVertexDelta);
  V_SAFE_DELETE_ARRAY(m_pParticle);
  V_SAFE_DELETE_ARRAY(m_pLocalSpacePos);
}



// handle the springs as stick constraints
void VClothMesh::HandleSpringPhysics(float dtime, float fGravity)
{
  int i;

  VisObjectVertexDelta_t *pDelta  = m_pVertexDelta;
  ClothParticle_t *p = m_pParticle;

  for (i=0;i<m_iVertexCount;i++, pDelta++, p++)
  {
    // this way the pos in the vertex delta is one iteration delayed, but we
    // save one copy operation. The constraints work on the particle structure only
    float *fOldPos = pDelta->delta;
    float *fNewPos = p->pos;
    hkvVec3 vOld(fNewPos[0], fNewPos[1], fNewPos[2]);
    fNewPos[0]=1.999f*fNewPos[0]-0.999f*fOldPos[0] + dtime*p->velocity[0];
    fNewPos[1]=1.999f*fNewPos[1]-0.999f*fOldPos[1] + dtime*p->velocity[1];
    fNewPos[2]=1.999f*fNewPos[2]-0.999f*fOldPos[2] + dtime*p->velocity[2];
    fOldPos[0] = vOld.x;
    fOldPos[1] = vOld.y;
    fOldPos[2] = vOld.z;

    p->velocity[0] = p->velocity[1] = 0.f;
    p->velocity[2] = -fGravity*dtime;
  }

  float fraction = 0.2f;

  {
    Spring_t *spr = m_pSpring;
    for (i=0;i<m_iSpringCount;i++,spr++)
    {
      float *v1 = m_pParticle[spr->iVertexIndex[0]].pos;
      float *v2 = m_pParticle[spr->iVertexIndex[1]].pos;
      hkvVec3 diff(v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]);
      float len = diff.getLength();
      if (len<=0.00001f) continue; 
      float scale = fraction*(len-spr->fDefaultLength)/len;
      diff *= scale;

      v1[0] += diff.x;
      v1[1] += diff.y;
      v1[2] += diff.z;

      v2[0] -= diff.x;
      v2[1] -= diff.y;
      v2[2] -= diff.z;
    }
  }
}



void VClothMesh::HandleMeshPhysics(float dtime, float fGravity)
{
  m_bBoxValid = false;
  HandleSpringPhysics(dtime,fGravity);

  int iCount,i;

  // handle the constraints
  m_Constraints.HandleParticles(this,dtime);

  // finally, handle the point constraints:
  iCount = m_PointConstraints.GetConstraintCount();
  for (i=0;i<iCount;i++)
  {
    VisParticleConstraintPoint_cl *pPoint = (VisParticleConstraintPoint_cl *)m_PointConstraints.GetConstraint(i);
    if (!pPoint) continue;
    int iVertex = m_PointConstraints.m_iVertex.GetDataPtr()[i];
    VASSERT(iVertex>=0 && iVertex<m_iVertexCount);
    // force cloth position to point position:
    float *pos = m_pParticle[iVertex].pos;
    hkvVec3 vPos = pPoint->GetPosition();
    pos[0] = vPos.x;
    pos[1] = vPos.y;
    pos[2] = vPos.z;
  }

  ComputeNormals();
}


// recalculate the normals from the mesh
void VClothMesh::ComputeNormals()
{
  int i;
  float crosslen;
  ClothParticle_t *p = m_pParticle;
  unsigned short v1,v2;
  for (i=0;i<m_iVertexCount;i++,p++)
  {
    p->GetNormalIndices(v1,v2);
    float dir[3] = {
      m_pVertexDelta[v1].delta[0]-m_pVertexDelta[i].delta[0],
      m_pVertexDelta[v1].delta[1]-m_pVertexDelta[i].delta[1],
      m_pVertexDelta[v1].delta[2]-m_pVertexDelta[i].delta[2]};

    float dir2[3] = {
      m_pVertexDelta[v2].delta[0]-m_pVertexDelta[i].delta[0],
      m_pVertexDelta[v2].delta[1]-m_pVertexDelta[i].delta[1],
      m_pVertexDelta[v2].delta[2]-m_pVertexDelta[i].delta[2]};

    float cross[4] = {
        dir[1]*dir2[2]-dir2[1]*dir[2],
        dir[2]*dir2[0]-dir2[2]*dir[0],
        dir[0]*dir2[1]-dir2[0]*dir[1],0.f};

    {
      crosslen = hkvMath::sqrt (cross[0]*cross[0]+cross[1]*cross[1]+cross[2]*cross[2]);
      if (crosslen<0.000001f) continue;
      crosslen=127.f/crosslen;
    }

    m_pVertexDelta[i].normal[0] = hkvMath::float2int(crosslen*cross[0]);
    m_pVertexDelta[i].normal[1] = hkvMath::float2int(crosslen*cross[1]);
    m_pVertexDelta[i].normal[2] = hkvMath::float2int(crosslen*cross[2]);
  }

}



void VClothMesh::Translate(const hkvVec3& vMoveDelta, bool bHandleConstraints)
{
  VisObjectVertexDelta_t *pDelta  = m_pVertexDelta;
  ClothParticle_t *p = m_pParticle;

  // increase both old and new pos
  for (int i=0;i<m_iVertexCount;i++,pDelta++,p++)
  {
    ((hkvVec3&) pDelta->delta[0]) += vMoveDelta;
    ((hkvVec3&) p->pos[0]) += vMoveDelta;
  }

  if (bHandleConstraints)
    m_Constraints.HandleParticles(this,0.f);
  m_bBoxValid = false;
}



void VClothMesh::Rotate(const hkvMat3 &rotMatrix, const hkvVec3& vCenter,bool bHandleConstraints)
{
  if (!m_pLocalSpacePos) // we need original local space position
    return;
  VisObjectVertexDelta_t *pDelta  = m_pVertexDelta;
  ClothParticle_t *p = m_pParticle;
  hkvVec3* pLocal = m_pLocalSpacePos;

  // increase both old and new pos
  for (int i=0;i<m_iVertexCount;i++,pLocal++,pDelta++,p++)
  {
    hkvVec3 vPos = *pLocal;
    vPos = rotMatrix * vPos;
    vPos += vCenter;
    memcpy(p->pos,&vPos,3*sizeof(float));
    memcpy(pDelta->delta,&vPos,3*sizeof(float));
  }

  m_bBoxValid = false;
  ComputeNormals();
  if (bHandleConstraints)
    m_Constraints.HandleParticles(this,0.f);
}



void VClothMesh::ResetForces()
{
  VisObjectVertexDelta_t *pDelta  = m_pVertexDelta;
  ClothParticle_t *p = m_pParticle;
  for (int i=0;i<m_iVertexCount;i++,pDelta++,p++)
  {
    hkvVec3* pOldPos = (hkvVec3* )pDelta->delta;
    hkvVec3* pNewPos = (hkvVec3* )p->pos;
    *pOldPos = *pNewPos;
  }
}


void VClothMesh::RenderVertices()
{
  IVRenderInterface *pRI = Vision::Contexts.GetCurrentContext()->GetRenderInterface();

  ClothParticle_t *p = m_pParticle;
  int i;

  unsigned short v1,v2;
  for (i=0;i<m_iVertexCount;i++,p++)
  {
    p->GetNormalIndices(v1,v2);
    hkvVec3 vPos(m_pVertexDelta[i].delta[0], m_pVertexDelta[i].delta[1], m_pVertexDelta[i].delta[2]);
    hkvVec3 vDir1(
      m_pVertexDelta[v1].delta[0]-m_pVertexDelta[i].delta[0],
      m_pVertexDelta[v1].delta[1]-m_pVertexDelta[i].delta[1],
      m_pVertexDelta[v1].delta[2]-m_pVertexDelta[i].delta[2]);

    hkvVec3 vDir2(
      m_pVertexDelta[v2].delta[0]-m_pVertexDelta[i].delta[0],
      m_pVertexDelta[v2].delta[1]-m_pVertexDelta[i].delta[1],
      m_pVertexDelta[v2].delta[2]-m_pVertexDelta[i].delta[2]);

    hkvVec3 vCross = vDir1.cross(vDir2);
    vCross.setLength(20.f);
    Vision::Game.DrawSingleLine(vPos,vPos+vCross, V_RGBA_GREEN);

    char szTxt[64];
    sprintf(szTxt,"%i",i);
    Vision::Game.DrawMessage3D(szTxt,vPos);
  }
  return;
}


struct ModelVertex_t
{
  hkvVec3 pos,normal;
};


bool VClothMesh::CreateFromEntityModel(VisBaseEntity_cl *pEntity, const hkvVec3& vScaling)
{
  g_sLastError.Reset();
  int i;
  FreeMesh();
  VDynamicMeshPtr spModel = pEntity->GetMesh();
  if (!spModel || !spModel->IsLoaded())
  {
    g_sLastError = "No model file specified (or model file not found)";
    return false;
  }

  // to make sure model is loaded and doesn't get unloaded here
  V_LOCK_RESOURCE(spModel);

  m_bBoxValid = false;

  // access the model mesh
  m_iVertexCount = spModel->GetNumOfVertices();
  int iTriCount  = spModel->GetNumOfTriangles();

  if (!m_iVertexCount || !iTriCount)
  {
    g_sLastError = "Model has no vertices or triangles to convert";
    return false;
  }

  // create mesh data
  m_pVertexDelta = new VisObjectVertexDelta_t[m_iVertexCount];
  m_pParticle = new ClothParticle_t[m_iVertexCount];
  ModelVertex_t *pTempVert = new ModelVertex_t[m_iVertexCount];
  unsigned short *pTempTriangleIndices = new unsigned short[iTriCount*3];
  m_pLocalSpacePos = new hkvVec3[m_iVertexCount];

  // copy original vertices and triangles to buffer
  VisMBVertexDescriptor_t compactVertexDescr;
  compactVertexDescr.Reset();
  compactVertexDescr.m_iStride    = sizeof(ModelVertex_t);
  compactVertexDescr.m_iPosOfs    = offsetof(ModelVertex_t,pos);
  compactVertexDescr.m_iNormalOfs = offsetof(ModelVertex_t,normal);
  compactVertexDescr.SetFormatDefaults();

  spModel->CopyMeshVertices(pTempVert, compactVertexDescr);
  spModel->CopyMeshIndices(pTempTriangleIndices, (VisSurface_cl *)NULL);
  
  // the cloth calculation is performed in world space, thus transform vertices
  // to world space
  hkvMat3 rotMatrix = pEntity->GetRotationMatrix();
  hkvVec3 vEntityPos = pEntity->GetPosition();
  rotMatrix.transpose();
  pEntity->SetOrientation(0,0,0);
  //pEntity->SetPosition(0,0,0);

  // copy the mesh to delta buffer:
  VisObjectVertexDelta_t *pDelta = m_pVertexDelta;
  ModelVertex_t *pSrcVert = pTempVert;
  ClothParticle_t *p = m_pParticle;
  hkvVec3* pLocal = m_pLocalSpacePos;

  for (i=0;i<m_iVertexCount;i++,pDelta++,pSrcVert++,p++,pLocal++)
  {
    pDelta->vertex = i+1; // vertex number is 1-based

    // transform object space vertex to global space
    hkvVec3 vPos(pSrcVert->pos[0],pSrcVert->pos[1],pSrcVert->pos[2]);
    vPos.x *= vScaling.x;
    vPos.y *= vScaling.y;
    vPos.z *= vScaling.z;

    vPos = rotMatrix * vPos;
    //vPos+=vEntityPos;
    pLocal->x = p->pos[0] = pDelta->delta[0] = vPos.x;
    pLocal->y = p->pos[1] = pDelta->delta[1] = vPos.y;
    pLocal->z = p->pos[2] = pDelta->delta[2] = vPos.z;
    pDelta->normal[0] = (signed char)(127.f*pSrcVert->normal[0]);
    pDelta->normal[1] = (signed char)(127.f*pSrcVert->normal[1]);
    pDelta->normal[2] = (signed char)(127.f*pSrcVert->normal[2]);
  }

  // build references to vertices used for normal vector calculation
  if (!BuildVertexNormalReferences(pTempTriangleIndices,iTriCount)) goto failed;
  if (!GenerateSprings(pTempTriangleIndices,iTriCount)) goto failed;

  V_SAFE_DELETE_ARRAY(pTempVert);
  V_SAFE_DELETE_ARRAY(pTempTriangleIndices);
  ComputeNormals();
  return true;

failed:
  V_SAFE_DELETE_ARRAY(pTempVert);
  V_SAFE_DELETE_ARRAY(pTempTriangleIndices);
  return false;
}

bool VClothMesh::BuildVertexNormalReferences(unsigned short *pTriangleIndices, int iTriCount)
{
  VASSERT(m_pParticle); // must be allocated
  unsigned short *pTriangleIndex = pTriangleIndices;
  int i;
  for (i=0;i<iTriCount;i++,pTriangleIndex+=3)
  {
    m_pParticle[pTriangleIndex[0]].SetNormalIndices(pTriangleIndex[1],pTriangleIndex[2]);
  }

  // once again to fix unassigned tris
  pTriangleIndex = pTriangleIndices;
  for (i=0;i<iTriCount;i++,pTriangleIndex+=3)
  {
    ClothParticle_t *p1 = &m_pParticle[pTriangleIndex[1]];
    if (p1->GetNormalIndex(0)==INDEX_UNINITIALIZED)
      p1->SetNormalIndex(0,pTriangleIndex[2]);
    if (p1->GetNormalIndex(1)==INDEX_UNINITIALIZED)
      p1->SetNormalIndex(1,pTriangleIndex[0]);

    ClothParticle_t *p2 = &m_pParticle[pTriangleIndex[2]];
    if (p2->GetNormalIndex(0)==INDEX_UNINITIALIZED)
      p2->SetNormalIndex(0,pTriangleIndex[0]);
    if (p2->GetNormalIndex(1)==INDEX_UNINITIALIZED)
      p2->SetNormalIndex(1,pTriangleIndex[1]);
  }

  // check whether all are assigned
  for (i=0;i<m_iVertexCount;i++)
  {
    unsigned short v1,v2;
    m_pParticle[i].GetNormalIndices(v1,v2);
    if (v1==INDEX_UNINITIALIZED || v2==INDEX_UNINITIALIZED)
    {
      g_sLastError = "Some vertices have uninitialized normal references";
      return false;
    }
  }

  return true;
}



// algorithm to generate springs from an arbitrary mesh
bool VClothMesh::GenerateSprings(unsigned short *pTempTriangleIndices, int iTriCount)
{
  // create list of vertex adjacencies:
  VertexAdjacencyInfo_t *adj_info = new VertexAdjacencyInfo_t[m_iVertexCount];
  V_VERIFY_MALLOC(adj_info);

  int i,j;
  int numAdjacencies = 0;
  unsigned short *pTempTriangleIndex = pTempTriangleIndices;
  double sumMin = 0;
  double sumMax = 0;
  double sumMid = 0;
  for (i=0;i<iTriCount;i++,pTempTriangleIndex+=3)
  {
    if (adj_info[pTempTriangleIndex[0]].Add(pTempTriangleIndex[1])) numAdjacencies++;
    if (adj_info[pTempTriangleIndex[0]].Add(pTempTriangleIndex[2])) numAdjacencies++;
    if (adj_info[pTempTriangleIndex[1]].Add(pTempTriangleIndex[0])) numAdjacencies++;
    if (adj_info[pTempTriangleIndex[1]].Add(pTempTriangleIndex[2])) numAdjacencies++;
    if (adj_info[pTempTriangleIndex[2]].Add(pTempTriangleIndex[0])) numAdjacencies++;
    if (adj_info[pTempTriangleIndex[2]].Add(pTempTriangleIndex[1])) numAdjacencies++;
    float d0 = Helper_GetDist(&m_pVertexDelta[pTempTriangleIndex[0]].delta[0], &m_pVertexDelta[pTempTriangleIndex[1]].delta[0]);
    float d1 = Helper_GetDist(&m_pVertexDelta[pTempTriangleIndex[0]].delta[0], &m_pVertexDelta[pTempTriangleIndex[2]].delta[0]);
    float d2 = Helper_GetDist(&m_pVertexDelta[pTempTriangleIndex[1]].delta[0], &m_pVertexDelta[pTempTriangleIndex[2]].delta[0]);
    float min_d = hkvMath::Min(hkvMath::Min(d0,d1),d2);
    float max_d = hkvMath::Max(hkvMath::Max(d0,d1),d2);
    float mid_d = (d0+d1+d2)-min_d-max_d;
    sumMin += min_d;
    sumMax += max_d;
    sumMid += mid_d;
  } 

  // take the average of max distance and medium distance
  float fMaxSpringLen   = (float)((sumMax+sumMid)/(2.0*iTriCount));
  float fSpringLenScale = 1.f;
  const float epsilon = (float)(sumMin/(2.0*iTriCount)); // tweaked (half the average min dist)
  const float fEpsilonSqr = epsilon*epsilon;

  for (int action=0;action<2;action++) // perform the same operation two times; first only count springs
  {
    int iSpringCount = 0;
    VertexAdjacencyInfo_t *adj = adj_info;
    VASSERT(adj != NULL && adj_info != NULL);
    Spring_t *spr = m_pSpring;
    for (i=0;i<m_iVertexCount;i++,adj++)
      for (j=0;j<adj->iNeighbourCount;j++)
      {
        int index0 = i;
        int index1 = adj->iNeighbourIndex[j];
        if (index0>=index1) continue; // only VertexA -> VertexB
        hkvVec3 vDir = ((hkvVec3&) m_pVertexDelta[index1].delta) - ((hkvVec3&) m_pVertexDelta[index0].delta);
        float fVertexDist = vDir.getLength();
        if (fVertexDist>fMaxSpringLen) // do not connect diagonal
          continue;

        // connect next vertex also:
        VertexAdjacencyInfo_t *adj2 = &adj_info[index1];
        int k;
        for (k=0;k<adj2->iNeighbourCount;k++)
        {
          int index2 = adj2->iNeighbourIndex[k];
          hkvVec3 vDir2 = ((hkvVec3&) m_pVertexDelta[index2].delta) - ((hkvVec3&) m_pVertexDelta[index1].delta);
          float dotp = vDir2.dot (vDir);
          dotp = hkvMath::Abs (dotp - fVertexDist*fVertexDist);
          if (dotp > fEpsilonSqr)
            continue; // do they point into same direction?

          // another check must be performed to test whether they are parallel:
          hkvVec3 vCross = vDir2.cross(vDir);
          if (vCross.getLength() > epsilon) 
            continue;

          if (action==1) // add spring which spans over two vertices
          {
            spr->Set(index0,index2, fVertexDist*2.f*fSpringLenScale);
            spr++;
          }
          iSpringCount++;
        }

        if (action==1)
        {
          spr->Set(index0,index1, fVertexDist*fSpringLenScale);
          spr++;
        }
        iSpringCount++;
      }

    if (action==0) // after the first run allocate springs
      AllocateSprings(iSpringCount);
  }
  V_SAFE_DELETE_ARRAY(adj_info);
  return true;
}



bool VClothMesh::AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence)
{
  // a point constraint is additionally assigned to a vertex ID in the mesh, so this is
  // stored in a separate list
  if (pConstraint->IsOfType(VisParticleConstraintPoint_cl::GetClassTypeId()))
    return AddPointConstraint((VisParticleConstraintPoint_cl *)pConstraint);

  VASSERT(pConstraint)
  if (bCheckInfluence && !pConstraint->Influences(m_BoundingBox))
    return false;
  m_Constraints.AddConstraint(pConstraint);
  return true;
}

bool VClothMesh::AddPointConstraint(VisParticleConstraintPoint_cl *pPoint, int iVertex)
{
  if (iVertex<0) // find closest vertex
  {
    float fMinDist = pPoint->GetRadius(); // ignore vertices further away
    VisObjectVertexDelta_t *pDelta  = m_pVertexDelta;
    for (int i=0;i<m_iVertexCount;i++, pDelta++)
    {
      hkvVec3 vDiff = pPoint->GetPosition() - ((hkvVec3&) pDelta->delta);
      float fLen = vDiff.getLength();
      if (fLen<fMinDist)
      {
        iVertex=i;
        fMinDist=fLen;
      }
    }
    if (iVertex<0) // no vertex found
      return false;
  }

  m_PointConstraints.AddPointConstraint(pPoint,iVertex);
  return true;
}

bool VClothMesh::RemoveConstraint(VisParticleConstraint_cl *pConstraint)
{
  if (m_Constraints.RemoveConstraint(pConstraint)) return true;
  if (m_PointConstraints.RemoveConstraint(pConstraint)) return true;
  return false;
}


V_IMPLEMENT_SERIALX( VisPointConstraintList_cl);
void VisPointConstraintList_cl::SerializeX( VArchive &ar )
{
  VisParticleConstraintList_cl::SerializeX(ar); // first call base func, and then load vertices
  if (ar.IsLoading())
  {
    if (m_iConstraintCount>0)
    {
      m_iVertex[m_iConstraintCount-1]=0; // resize array
      for (int i=0;i<m_iConstraintCount;i++)
        ar >> m_iVertex.GetDataPtr()[i];
    }
  }
  else
  {
    for (int i=0;i<m_iConstraintCount;i++)
      ar << m_iVertex.GetDataPtr()[i];
  }
}


V_IMPLEMENT_SERIALX( VClothMesh);
void VClothMesh::SerializeX( VArchive &ar )
{
  if (ar.IsLoading())
  {
    m_Constraints.SerializeX(ar);
    m_PointConstraints.SerializeX(ar);
  }
  else
  {
    m_Constraints.SerializeX(ar);
    m_PointConstraints.SerializeX(ar);
  }
}

#pragma warning(pop)

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
