/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/FmodManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/EngineInstances/EngineInstanceFmodCollisionMesh.hpp>

#using <mscorlib.dll>

namespace FmodManaged
{

  EngineInstanceFmodCollisionMesh::EngineInstanceFmodCollisionMesh()
  {
    m_bIsVisible = true;
    m_bSelected = false;  
    m_pEntity = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0.0f,0.0f,0.0f), NULL);
    m_pStaticMeshInst = NULL;
    m_pMeshInstance = new VFmodCollisionMeshInstance();
    m_pEntity->AttachToParent(m_pMeshInstance); // move with it
  }

  void EngineInstanceFmodCollisionMesh::DisposeObject()
  {
    V_SAFE_REMOVE(m_pEntity);
    if(m_pStaticMeshInst)
      m_pStaticMeshInst->DisposeObject();
    m_pStaticMeshInst = NULL;    
    if (m_pMeshInstance)
      m_pMeshInstance->DisposeObject(); // remove from manager
    m_pMeshInstance = NULL;
  }

  void EngineInstanceFmodCollisionMesh::SetVisible(bool bStatus) 
  {
    m_bIsVisible = bStatus;
    UpdateVisibleStatus();
  }  

  void EngineInstanceFmodCollisionMesh::SetPosition(float x, float y, float z)
  {
    if (!m_pEntity)
      return;

    if (m_pStaticMeshInst)
    {
      hkvMat4 worldMatrix;
      m_pEntity->GetWorldMatrix(worldMatrix);
      m_pStaticMeshInst->SetTransform(worldMatrix);
      m_pStaticMeshInst->ReComputeBoundingBoxes();
    }

    if (m_pMeshInstance) 
      m_pMeshInstance->SetPosition(x, y, z);
  }

  void EngineInstanceFmodCollisionMesh::SetOrientation(float yaw, float pitch, float roll)  
  {
    if (!m_pEntity) 
      return;
   
    if (m_pStaticMeshInst)
    {
      hkvMat4 worldMatrix;
      m_pEntity->GetWorldMatrix(worldMatrix);
      m_pStaticMeshInst->SetTransform(worldMatrix);
      m_pStaticMeshInst->ReComputeBoundingBoxes();
    }

    if (m_pMeshInstance) 
      m_pMeshInstance->SetOrientation(yaw, pitch, roll);
  }

  void EngineInstanceFmodCollisionMesh::SetScaling(float x, float y, float z)
  {
    if (!m_pEntity) 
      return;

    hkvVec3 scale(x, y, z);
    m_pEntity->SetScaling(scale); 
    
    if (m_pStaticMeshInst)
    {
      hkvMat4 worldMatrix;
      m_pEntity->GetWorldMatrix(worldMatrix);
      m_pStaticMeshInst->SetTransform(worldMatrix);
      m_pStaticMeshInst->ReComputeBoundingBoxes();
    }

    if (m_pMeshInstance) 
      m_pMeshInstance->SetScaling(scale);
  }

  bool EngineInstanceFmodCollisionMesh::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    if (!m_pEntity)
      return false;

    hkvAlignedBBox meshBox;
    if (m_pStaticMeshInst)
      meshBox = m_pStaticMeshInst->GetMesh()->GetBoundingBox();
    else if (m_pEntity->GetMesh())
      m_pEntity->GetMesh()->GetVisibilityBoundingBox(meshBox);
    else
      return false;

    (*bbox).Set(meshBox.m_vMin.x,meshBox.m_vMin.y,meshBox.m_vMin.z, meshBox.m_vMax.x,meshBox.m_vMax.y,meshBox.m_vMax.z);
    return true;
  }

  void EngineInstanceFmodCollisionMesh::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    // don't do anything because the shape code already does the picking
  }

  bool EngineInstanceFmodCollisionMesh::OnExport(SceneExportInfo ^info) 
  {
    if (!m_pMeshInstance)
      return true;

    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pMeshInstance);
    return true;
  }

  void EngineInstanceFmodCollisionMesh::UpdateVisibleStatus()
  {
    if (!m_pEntity) 
      return;
    m_pEntity->SetVisibleBitmask((m_bIsVisible&&m_bSelected) ? 0xffffffff:0);

    if (m_pStaticMeshInst)
      m_pStaticMeshInst->SetVisibleBitmask((m_bIsVisible&&m_bSelected) ? 0xffffffff:0); 

    if (m_pMeshInstance)
      m_pMeshInstance->SetActive(m_bIsVisible);
  }

  void EngineInstanceFmodCollisionMesh::SetCollisionMesh(String ^filename, float fDirectOcclusion, float fReverbOcclusion)
  {
    VString sFilename;
    ConversionUtils::StringToVString(filename, sFilename);
    if (sFilename.IsEmpty())
      return;

    const char *szFileExt = VPathHelper::GetExtension(sFilename);
    VASSERT(szFileExt != NULL);

    VASSERT(m_pEntity != NULL);

    VString sFileExt(szFileExt);
    if (sFileExt.CompareI("model")==0) 
    {
      if(m_pStaticMeshInst)
        m_pStaticMeshInst->DisposeObject();
      m_pStaticMeshInst = NULL;    
      VDynamicMesh *pModel = Vision::Game.LoadDynamicMesh(sFilename, true);
      m_pEntity->SetMesh(pModel);
      m_pMeshInstance->SetCollisionMesh(pModel, fDirectOcclusion, fReverbOcclusion);
    }
    else
    {
      if (m_pEntity->GetMesh())
        m_pEntity->SetMesh((VDynamicMesh*)NULL);
      VisStaticMesh_cl *pStaticMesh = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(sFilename);
      if (pStaticMesh)
      {
        hkvMat4 worldMatrix;
        m_pEntity->GetWorldMatrix(worldMatrix);
        if(m_pStaticMeshInst)
          m_pStaticMeshInst->DisposeObject();
        m_pStaticMeshInst = pStaticMesh->CreateInstance(worldMatrix);
        VASSERT(m_pStaticMeshInst != NULL);
        m_pStaticMeshInst->AssignToVisibilityZones();
      }
      m_pMeshInstance->SetCollisionMesh(pStaticMesh, fDirectOcclusion, fReverbOcclusion);
    }
  }

  void EngineInstanceFmodCollisionMesh::SetSelected(bool bStatus) 
  {
    m_bSelected = bStatus;
    UpdateVisibleStatus();
  }

  void EngineInstanceFmodCollisionMesh::SetOcclusion(float fDirectOcclusion, float fReverbOcclusion)
  {
    if (m_pMeshInstance)
      m_pMeshInstance->SetOcclusion(fDirectOcclusion, fReverbOcclusion);
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
