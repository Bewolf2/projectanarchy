/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/FmodEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


// -------------------------------------------------------------------------- //
// Constructor/ Destructor                                                 
// -------------------------------------------------------------------------- //
VFmodCollisionMeshInstance::VFmodCollisionMeshInstance(VFmodManager *pManager)
{
  if (!pManager)
    pManager = &VFmodManager::GlobalManager();
  m_pManager = pManager;

  m_bActive = true;
  m_bDoubleSided = false;
  m_vScale.set(1.0f, 1.0f, 1.0f);
  m_fDirectOcclusion = m_fReverbOcclusion = 1.0f;

  m_pGeometry = NULL;

  m_pManager->CollisionMeshes().Add(this);
}

VFmodCollisionMeshInstance::~VFmodCollisionMeshInstance()
{
  if (m_pGeometry)
  {
    m_pGeometry->release();
    m_pGeometry = NULL;
  }
}

void VFmodCollisionMeshInstance::DisposeObject()
{
  VisObject3D_cl::DisposeObject();
  m_pManager->CollisionMeshes().SafeRemove(this);
}


// -------------------------------------------------------------------------- //
// Overridden functions                                                 
// -------------------------------------------------------------------------- //
void VFmodCollisionMeshInstance::OnObject3DChanged(int iO3DFlags)
{
  VisObject3D_cl::OnObject3DChanged(iO3DFlags);

  if (!m_pManager->IsInitialized())
    return; 

  if (m_pGeometry)
  {
    // update the transformation
    const hkvVec3 &vPos = GetPosition();
    hkvMat3 rotMat(hkvNoInitialization);
    GetRotationMatrix(rotMat);
    rotMat.transpose();
    hkvVec3 vDir,vRight,vUp;
    rotMat.getAxisXYZ(&vDir, &vRight, &vUp);

    m_pGeometry->setRotation((FMOD_VECTOR*)&vUp, (FMOD_VECTOR*)&vRight);
    m_pGeometry->setPosition((FMOD_VECTOR*)&vPos);
    m_pGeometry->setScale((FMOD_VECTOR*)&GetScale());
  }
}


#ifdef SUPPORTS_SNAPSHOT_CREATION

void VFmodCollisionMeshInstance::GetDependencies(VResourceSnapshot &snapshot)
{
  VisObject3D_cl::GetDependencies(snapshot);

  if (m_spMesh)
    m_spMesh->GetDependencies(snapshot);
}

#endif


// -------------------------------------------------------------------------- //
// Serialization                                                
// -------------------------------------------------------------------------- //
V_IMPLEMENT_SERIAL( VFmodCollisionMeshInstance, VisObject3D_cl, 0, &g_FmodModule );

void VFmodCollisionMeshInstance::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    char iVersion = -1;
    bool bActive;
    int iMeshType;
    ar >> iVersion; 
    VASSERT(iVersion >= V_FMODCOLLISIONMESHINSTANCE_VERSION_0 && iVersion <= V_FMODCOLLISIONMESHINSTANCE_VERSION_CURRENT);

    VisObject3D_cl::Serialize(ar);
    ar >> bActive;
    m_vScale.SerializeAsVec3(ar);
    ar >> m_fDirectOcclusion >> m_fReverbOcclusion;
    ar >> iMeshType; // mesh type; 0==VBaseMesh

    if (iMeshType==0)
    {
      VBaseMesh *pMesh = (VBaseMesh *)ar.ReadProxyObject();
      SetCollisionMesh(pMesh, m_fDirectOcclusion, m_fReverbOcclusion);
    } 
    else
    {
      VASSERT(!"Yet unsupported mesh type for sound collision geometry");
    }

    SetActive(bActive);
  } 
  else
  {
    ar << (char)V_FMODCOLLISIONMESHINSTANCE_VERSION_CURRENT; 
    VisObject3D_cl::Serialize(ar);
    ar << m_bActive;
    m_vScale.SerializeAsVec3(ar);
    ar << m_fDirectOcclusion << m_fReverbOcclusion;
    ar << (int)0; // mesh type; 0==VBaseMesh
    ar.WriteProxyObject(m_spMesh);
  }
}


// -------------------------------------------------------------------------- //
// Mesh definition                                                
// -------------------------------------------------------------------------- //
bool VFmodCollisionMeshInstance::SetCollisionMesh(VSimpleCollisionMesh *pMesh, float fDirectOcclusion, float fReverbOcclusion, int iMeshFlags)
{
  if (pMesh)
    return SetCollisionMesh(pMesh->GetVertexCount(), pMesh->GetPrimitiveCount(), pMesh->GetVertexPtr(), pMesh->GetIndexPtr(), fDirectOcclusion, fReverbOcclusion, iMeshFlags, pMesh->m_pIndex32 != NULL ? VIS_INDEXFORMAT_32 : VIS_INDEXFORMAT_16);
  RemoveCollisionMesh();
  return true;
}

bool VFmodCollisionMeshInstance::SetCollisionMesh(VBaseMesh *pMesh, float fDirectOcclusion, float fReverbOcclusion)
{
  m_spMesh = pMesh;
  if (!pMesh)
  {
    RemoveCollisionMesh();
    return true;
  }

  pMesh->EnsureLoaded();
  if (!pMesh->IsLoaded())
  {
    RemoveCollisionMesh();
    return false;
  }

  hkvVec3* pVertexList = NULL; 
  unsigned short* pIndexList = NULL;
  void *pIndexListVoid;
  IVCollisionMesh *pColMesh = pMesh->GetCollisionMesh(true, true);
  const int iVertexCount = pColMesh->GetVertexList(pVertexList);
  int iIndexType;
  const int iIndexCount = pColMesh->GetIndexList(pIndexListVoid, iIndexType);
  pIndexList = (unsigned short*)pIndexListVoid;
  return SetCollisionMesh(iVertexCount, iIndexCount/3, pVertexList, pIndexList, fDirectOcclusion, fReverbOcclusion, TRACEFLAG_NONE, iIndexType);
}

bool VFmodCollisionMeshInstance::SetCollisionMesh(int iVertexCount, int iTriangleCount, hkvVec3 *pVertices, void *pIndices, float fDirectOcclusion, float fReverbOcclusion, int iMeshFlags, int iIndexType)
{
  m_fDirectOcclusion = fDirectOcclusion;
  m_fReverbOcclusion = fReverbOcclusion;

  if (m_pManager->IsInitialized())
  {
    // release old mesh
    if (m_pGeometry)
    {
      m_pGeometry->release();
      m_pGeometry = NULL;
    }

    if (iTriangleCount==0)
      return true;

    // create new geometry instance
    if (!m_pManager->GetFmodSystem())
      return false;
    FMOD_WARNINGCHECK(m_pManager->GetFmodSystem()->createGeometry(iTriangleCount,iTriangleCount*3,&m_pGeometry));
    if (!m_pGeometry)
      return false;

    // now do the conversion
    VTriangle tri;
    const bool bHasIndexList = pIndices!=NULL;

    unsigned int *pIndices32 = (unsigned int*)pIndices;
    unsigned short *pIndices16 = (unsigned short*)pIndices;

    m_bDoubleSided = (iMeshFlags & TRACEFLAG_DOUBLESIDED)!=0;

    for (int i=0;i<iTriangleCount;i++,pIndices16+=3,pIndices32+=3)
    {
      if (bHasIndexList)
      {
        if (iIndexType == VIS_INDEXFORMAT_32)
          tri.SetPoints(&pVertices[pIndices32[0]], &pVertices[pIndices32[1]], &pVertices[pIndices32[2]]);
        else
          tri.SetPoints(&pVertices[pIndices16[0]], &pVertices[pIndices16[1]], &pVertices[pIndices16[2]]);
      }
      else
      {
        tri.SetPoints(&pVertices[i*3], &pVertices[i*3+1], &pVertices[i*3+2]);
      }
      VASSERT(tri.IsValid());

      int iPolyIndex = i;
      FMOD_VECTOR vertices[3];
      VFMOD_ASSIGN_VECTOR(vertices[0], tri.GetVertex0());
      VFMOD_ASSIGN_VECTOR(vertices[1], tri.GetVertex1());
      VFMOD_ASSIGN_VECTOR(vertices[2], tri.GetVertex2());

      FMOD_RESULT result = m_pGeometry->addPolygon(fDirectOcclusion, fReverbOcclusion, m_bDoubleSided, 3, vertices, &iPolyIndex);
      if (result!=FMOD_OK)
      {
        FMOD_WARNINGCHECK(result);
        OnObject3DChanged(VIS_OBJECT3D_ALLCHANGED); // set new position etc. on new geometry object
        return false;
      }
    }
  }

  OnObject3DChanged(VIS_OBJECT3D_ALLCHANGED); // set new position etc. on new geometry object
  return true;
}


// -------------------------------------------------------------------------- //
// CollisionMeshInstance property functions                                              
// -------------------------------------------------------------------------- //
void VFmodCollisionMeshInstance::SetActive(bool bStatus)
{
  m_bActive = bStatus;
  
  if (m_pGeometry)
    m_pGeometry->setActive(bStatus);
}

void VFmodCollisionMeshInstance::SetScaling(const hkvVec3 &vScale) 
{ 
  m_vScale = vScale; 
  OnObject3DChanged(VIS_OBJECT3D_ALLCHANGED);
}

void VFmodCollisionMeshInstance::SetOcclusion(float fDirectOcclusion, float fReverbOcclusion)
{
  m_fDirectOcclusion = fDirectOcclusion;
  m_fReverbOcclusion = fReverbOcclusion;

  if (!m_pGeometry)
    return;
  
  int iNumPolygons = 0;
  m_pGeometry->getNumPolygons(&iNumPolygons);
  for (int i=0;i<iNumPolygons;i++)
    m_pGeometry->setPolygonAttributes(i, m_fDirectOcclusion, m_fReverbOcclusion, m_bDoubleSided);
}


VFmodCollisionMeshInstance* VFmodCollisionMeshInstanceCollection::SearchCollisionMesh(const char* szName) const
{
  int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    VFmodCollisionMeshInstance* pCollisionMeshInstance = GetAt(i);
    if(pCollisionMeshInstance->HasObjectKey(szName))
      return pCollisionMeshInstance;
  }

  return NULL;
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
