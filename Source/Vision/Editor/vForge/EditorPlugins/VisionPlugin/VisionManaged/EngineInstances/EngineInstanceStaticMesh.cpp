/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceStaticMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/MeshPaintingComponent.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

#define FOREACH_MESH_LOD \
  for (int iLODIndex=0;iLODIndex<m_iLODCount;iLODIndex++)\
  {\
    VisStaticMeshInstance_cl *pInstance = m_spLODList[iLODIndex];\
    if (pInstance==NULL) continue;


  EngineInstanceStaticMesh::EngineInstanceStaticMesh()
  {
    _uniqueID = 0;
    _x = _y = _z = _yaw = _pitch = _roll = 0.f;
    _sx = _sy = _sz = 1.f;
    _lgx = _lgy = _lgz = 0.f;
    _bNoExport = false;
    _bDragging = false;
    _iVisibleBitmask = _iCollisionBitmask = 0xffffffff;
    _bVisible = true;

    m_pSubMeshes = new VisStaticGeometryInstanceCollection_cl(4);
    m_iLODCount = 0;
    m_spLODList = NULL;
    m_pFirstValidLOD = NULL;

    m_pParentZone = nullptr;
  }

  void EngineInstanceStaticMesh::DisposeObject()
  {
    DisposeAll();
    delete m_pSubMeshes;
  }

  void EngineInstanceStaticMesh::DisposeAll()
  {
    m_pSubMeshes->Clear();
    FOREACH_MESH_LOD    
      pInstance->DisposeObject();
    }
    V_SAFE_DELETE_ARRAY(m_spLODList);
    m_iLODCount =  0;
    m_pFirstValidLOD = NULL;
  }

 void EngineInstanceStaticMesh::SetObjectKey(String ^key)
  {
    // set key only on first instance, not on LODs?
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (!pMainInst)
      return;
    VString sKey;
    ConversionUtils::StringToVString(key,sKey);
    if (sKey.IsEmpty())
      pMainInst->SetObjectKey(NULL);
    else
      pMainInst->SetObjectKey(sKey);
  }


  void EngineInstanceStaticMesh::SetParentZone(Zone ^zone)
  {
    VisZoneResource_cl *pNativeZone = zone ? ((VisZoneResource_cl *)zone->NativeZonePtr.ToPointer()) : NULL;
    m_pParentZone = pNativeZone;
    FOREACH_MESH_LOD    
      pInstance->SetParentZone(pNativeZone);
    }
  }

  void EngineInstanceStaticMesh::CreateTransformation(hkvMat4 &dest)
  {
    hkvMat3 rotation (hkvNoInitialization);
    rotation.setFromEulerAngles (_roll, _pitch, _yaw);

    hkvMat3 scaling (hkvNoInitialization);
    scaling.setIdentity ();
    scaling.setDiagonal (hkvVec3(_sx, _sy, _sz));

    rotation = rotation.multiply (scaling);
    dest.setIdentity();
    dest.setTranslation(hkvVec3(_x,_y,_z));
    dest.setRotationalPart(rotation);
  }

  void EngineInstanceStaticMesh::UpdateTransformation()
  {
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (pMainInst==NULL)
      return;
    hkvMat4 transform;
    CreateTransformation(transform);
    FOREACH_MESH_LOD    
      pInstance->SetTransform(transform);
      pInstance->ReComputeBoundingBoxes();
      if (!EditorManager::VisibilityBuilder->UseInEngine) // don't reassign in this mode because we might be in the middle of an export process
        pInstance->AssignToVisibilityZones();
    }

    UpdateLightGridPos();

    IVisPhysicsModule_cl *pModule = Vision::GetApplication()->GetPhysicsModule();
    if (pModule)
      pModule->OnStaticMeshInstanceMoved(pMainInst);
  }
  

  void EngineInstanceStaticMesh::SetUniqueID(unsigned __int64 iID)
  {
    // not needed
    _uniqueID = iID;
    UpdateTransformation();
  }

  
  void EngineInstanceStaticMesh::GetDependencies(ResourceSnapshotInfo ^info)
  {
    if (_bNoExport)
      return;

    VResourceSnapshot *snapshot = (VResourceSnapshot *)info->NativeResourceSnapshotPtr.ToPointer();
    FOREACH_MESH_LOD
      pInstance->GetDependencies(*snapshot);
    }
  }


  void EngineInstanceStaticMesh::OnRecomputeVisibility()
  {
    FOREACH_MESH_LOD
      pInstance->AssignToVisibilityZones();
    }
  }


  void EngineInstanceStaticMesh::OnBeforeExport(SceneExportInfo ^info) 
  {
    FOREACH_MESH_LOD
      pInstance->SetMarkInsideSerializationSession(true);
    }
  }

  void EngineInstanceStaticMesh::OnAfterExport(SceneExportInfo ^info) 
  {
    FOREACH_MESH_LOD
      pInstance->SetMarkInsideSerializationSession(false);
    }
  }

  bool EngineInstanceStaticMesh::OnExport(SceneExportInfo ^info) 
  {
    if (_bNoExport)
      return true;
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    FOREACH_MESH_LOD
      pInstance->ReComputeBoundingBoxes(true); // accurate vertex bboxes
      ar << pInstance;
    }
    return true;
  }

  void EngineInstanceStaticMesh::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    if (m_pSubMeshes->GetNumEntries()<1)
      return;
    ShapeTraceResult ^pResult = result;
    static VisTraceLineInfo_t info;//@@@@
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
    if (Vision::CollisionToolkit.TraceLineStaticGeometry(vStart,vEnd,*m_pSubMeshes,1,&info))
      (*result).SetShapeHit_IfCloser(ownerShape,info.distance);
  }

  bool EngineInstanceStaticMesh::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (!pMainInst || !pMainInst->GetMesh())
      return false;
    hkvAlignedBBox temp = pMainInst->GetMesh()->GetBoundingBox();
    (*bbox).Set(temp.m_vMin.x,temp.m_vMin.y,temp.m_vMin.z, temp.m_vMax.x,temp.m_vMax.y,temp.m_vMax.z );
    return true;
  }

 
  void EngineInstanceStaticMesh::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      return;

  }

  void EngineInstanceStaticMesh::OnRenderHook(ShapeBase ^owner, int iConstant)
  {
    if (Vision::Editor.IsAnimatingOrPlaying() || m_pSubMeshes==NULL)
      return;

    ((VisionEngineManager ^)EditorManager::EngineManager)->RenderStaticMeshHighlighting(*m_pSubMeshes);
  }


  bool EngineInstanceStaticMesh::SetMeshFile(String ^filename, String ^filename_lowres, unsigned __int64 iIDLowRes, bool bNotifyPhysics)
  {
    return true;
/*
    // in some cases artists might choose the same file for low res. Ignore that case
    if (!String::IsNullOrEmpty(filename) && !String::IsNullOrEmpty(filename_lowres) && String::Compare(filename,filename_lowres,true)==0)
      filename_lowres = nullptr;

    VString sFilename,sFilenameLowRes;
    ConversionUtils::StringToVString(filename,sFilename);
    ConversionUtils::StringToVString(filename_lowres,sFilenameLowRes);
    m_pSubMeshes->Clear();

    //...load mesh
    V_SAFE_DISPOSEANDRELEASE(m_pMeshInst);
    if (!sFilename.IsEmpty())
    {
      VisStaticMesh_cl *pRes = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(sFilename);
      if (pRes)
      {
        hkvMat4 transform;//@@@@
        CreateTransformation(transform);

        m_pMeshInst = pRes->CreateInstance(transform, NULL, bNotifyPhysics);

        if (m_pMeshInst)
        {
          m_pMeshInst->AddRef();
          m_pMeshInst->SetParentZone(m_pParentZone);

          // cache the collection of submeshes
          m_pSubMeshes->EnsureSize(m_pMeshInst->GetSubmeshInstanceCount());
          for (int i=0;i<m_pMeshInst->GetSubmeshInstanceCount();i++)
            m_pSubMeshes->AppendEntryFast(m_pMeshInst->GetSubmeshInstance(i));
        }
      }
    }

    V_SAFE_DISPOSEANDRELEASE(m_pMeshInstLowRes);
    if (!sFilenameLowRes.IsEmpty())
    {
      VisStaticMesh_cl *pRes = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(sFilenameLowRes);
      if (pRes)
      {
        hkvMat4 transform;//@@@@
        CreateTransformation(transform);
        m_pMeshInstLowRes = pRes->CreateInstance(transform, NULL, false); // do not notify physics for low-res meshes
        m_pMeshInstLowRes->AddRef();
        m_pMeshInstLowRes->SetParentZone(m_pParentZone);
        m_pMeshInstLowRes->SetUniqueID(iIDLowRes);
        m_pMeshInstLowRes->SetCastStaticShadows(FALSE);

        // Mark the low-res mesh as non-colliding and non-traceable. The user can modify
        // the bitmask for the high-res mesh, but not for this low-res one.
        m_pMeshInstLowRes->SetCollisionBitmask(0, VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FORCENOCOLLISION);
        for (int i=0;i<m_pMeshInstLowRes->GetSubmeshInstanceCount();i++)
          m_pMeshInstLowRes->GetSubmeshInstance(i)->SetTraceBitmask(0);
      }
    }

    UpdateTransformation();
    return true;
    */
  }

  void EngineInstanceStaticMesh::SetLODChain(List<LODEntry^> ^meshlist)
  {
    DisposeAll();
    if (meshlist==nullptr || meshlist->Count==0)
      return;

    m_iLODCount = meshlist->Count;
    m_spLODList = new VSmartPtr<VisStaticMeshInstance_cl>[m_iLODCount];
    bool bIsFirst = true;
    for (int i=0;i<m_iLODCount;i++)
    {
      m_spLODList[i]  = NULL;
      LODEntry ^lod = meshlist[i];
      if (String::IsNullOrEmpty(lod->_filename))
        continue;
      VString sFilename;
      ConversionUtils::StringToVString(lod->_filename,sFilename);

      VisStaticMesh_cl *pRes = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(sFilename);
      if (pRes==NULL)
        continue;
      hkvMat4 transform;
      CreateTransformation(transform);

      VisStaticMeshInstance_cl *pMeshInst = pRes->CreateInstance(transform, (VisStaticMeshInstance_cl*)NewStaticMeshInstance(), false); // physics is notified later
      if (pMeshInst==NULL)
        continue;

      m_spLODList[i] = pMeshInst; // it is intended that this list has NULL pointers to guarantee it is in sync with C# list
      pMeshInst->SetParentZone(m_pParentZone);
      pMeshInst->SetUniqueID(lod->_uniqueID);

      // set other properties:
      if (bIsFirst)
      {
        m_pFirstValidLOD = pMeshInst;
        bIsFirst = false;
      }
      else
      {
        // turn off features for lower LOD
        pMeshInst->SetCastStaticShadows(FALSE);
        pMeshInst->SetCollisionBitmask(0, VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FORCENOCOLLISION);
        for (int j=0;j<pMeshInst->GetSubmeshInstanceCount();j++)
          pMeshInst->GetSubmeshInstance(j)->SetTraceBitmask(0);
      }

      // cache the collection of submeshes
      for (int j=0;j<pMeshInst->GetSubmeshInstanceCount();j++)
        m_pSubMeshes->AppendEntry(pMeshInst->GetSubmeshInstance(j));
    }
    UpdateTransformation();
  }

  void EngineInstanceStaticMesh::SetCustomMaterialSet(String ^variantName)
  {
    FOREACH_MESH_LOD
      if (String::IsNullOrEmpty(variantName))
      {
        // take a short-cut here so we don't have to build the material provider list
        pInstance->SetCustomSurfaceSet(NULL);
        continue;
      }
      String ^filename = ConversionUtils::VStringToString(pInstance->GetMesh()->GetOriginalFilename());
      IMaterialProvider ^set = EditorManager::EngineManager->FindMaterialProvider(filename);
      VisSurfaceTextureSet_cl *pNativeSet = NULL;
      if (set!=nullptr)
      {
        CustomMaterialSetProvider ^variant = dynamic_cast<CustomMaterialSetProvider ^>(set->FindVariantByName(variantName));
        if (variant!=nullptr)
          pNativeSet = variant->GetTextureSet();
      }
      pInstance->SetCustomSurfaceSet(pNativeSet);

    }
  }

  void EngineInstanceStaticMesh::SetFarClipDistance(List<LODEntry^> ^meshlist, bool bFromFile)
  {
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (pMainInst==NULL)
      return;

    hkvVec3 vClipRef(_x,_y,_z);
    const hkvAlignedBBox &bbox(pMainInst->GetBoundingBox()); // world space bounding box
    if (!bFromFile && bbox.isValid())
      vClipRef = bbox.getCenter();

    float fNear = 0.f;
    FOREACH_MESH_LOD
      LODEntry^ lod = meshlist[iLODIndex]; // assume list have same index
      if (bFromFile)
      {
        pInstance->SetDefaultClipSettings(&vClipRef);
      } else
      {
        float fFar = lod->_usedClipDistance; // these values are properly set outside this function
        pInstance->SetClipSettings(fNear,fFar, &vClipRef);
        fNear = fFar;
      }
    }
  }



  void EngineInstanceStaticMesh::UpdateVisibleStatus()
  {
    // forward to all submeshes
    int iFlags = _bVisible ? _iVisibleBitmask : 0;
    FOREACH_MESH_LOD    
      pInstance->SetVisibleBitmask(iFlags);
    }
  }

  void EngineInstanceStaticMesh::SetLightInfluenceBitmask(unsigned int iBits)
  {
    FOREACH_MESH_LOD
      // forward to all submeshes
      for (int i=0;i<pInstance->GetSubmeshInstanceCount();i++)
      {
        unsigned int iFinal = pInstance->GetMesh()->GetSubmesh(i)->GetGeometryInfo().m_iLightMask & iBits;
        pInstance->GetSubmeshInstance(i)->SetLightInfluenceBitmask(iFinal);
      }
    }
  }

  void EngineInstanceStaticMesh::SetCollisionBitmask(unsigned int iBits)
  {
    _iCollisionBitmask = iBits;
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (!pMainInst)
      return;

    // Set the flags on the mesh instance
    pMainInst->SetCollisionBitmask(iBits);
  }

  void EngineInstanceStaticMesh::SetCollisionGroup(CollisionBehavior_e eMode, short iLayer, short iSystemId, short iSystemDontCollideWith, int iSystemGroup)
  {
    // build a 32bit integer from the passed values, currently the same as hkpGroupFilter::calcFilterInfo
	  unsigned int iCollisionFilter = 0;
    // store it in the member because the mesh group needs it
    _iCollisionBitmask = ((int)iSystemId<<5) | ((int)iSystemDontCollideWith<<10) | (iSystemGroup<<16) | iLayer;

    VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior = VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FROMFILE;
    switch (eMode)
    {
    case CollisionBehavior_e::ForceNoCollision: 
      eCollisionBehavior = VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FORCENOCOLLISION; 
      break;   
    case CollisionBehavior_e::FromFile:
      eCollisionBehavior = VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FROMFILE;
      break; 
    case CollisionBehavior_e::Custom:
      iCollisionFilter = _iCollisionBitmask;
      eCollisionBehavior = VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM;
      break;
    }

    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (pMainInst)
      pMainInst->SetCollisionBitmask(iCollisionFilter, eCollisionBehavior);
  }

  void EngineInstanceStaticMesh::SetWeldingType(WeldingType_e eWeldingType)
  {
    FOREACH_MESH_LOD
      pInstance->SetWeldingType((VisWeldingType_e)eWeldingType);
    }
  }

  void EngineInstanceStaticMesh::SetCastDynamicShadows(bool bStatus)
  {
    // forward to all submeshes
    FOREACH_MESH_LOD
      for (int i=0;i<pInstance->GetSubmeshInstanceCount();i++)
      {
        bool bFinal = pInstance->GetMesh()->GetSubmesh(i)->GetGeometryInfo().GetCastDynamicShadows() && bStatus;
        pInstance->GetSubmeshInstance(i)->SetCastDynamicShadows(bFinal);
      }
    }
  }

  void EngineInstanceStaticMesh::SetCastStaticShadows(bool bStatus)
  {
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (!pMainInst)
      return;
    pMainInst->SetCastStaticShadows(bStatus);
  }


  void EngineInstanceStaticMesh::AssignGeometryToVisibiltyZone(VisibilityZoneShape ^pZone)
  {
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (!pMainInst)
      return;

    VisVisibilityZone_cl *pNativeZone = (VisVisibilityZone_cl *)pZone->NativeZonePointer.ToPointer();
    if (!pNativeZone)
      return; // should not happen though

    // TODO: what about the LOD versions?
    if (pZone->SubMeshIndices==nullptr && !pZone->ForceGeometryAssignment) // This case is a heuristic that assumes that all geometry instances are assigned when the array is not specified
    {
      int iCount = pMainInst->GetSubmeshInstanceCount();
      for (int i=0;i<iCount;i++)
        pNativeZone->AddStaticGeometryInstance(pMainInst->GetSubmeshInstance(i));
    }
    else
    {
      int iCount = (pZone->SubMeshIndices!=nullptr) ? pZone->SubMeshIndices->Length : 0;
      for (int i=0;i<iCount;i++)
      {
        int iSubIndex = pZone->SubMeshIndices[i];
        bool bValid = iSubIndex>=0 && iSubIndex<pMainInst->GetSubmeshInstanceCount();
        if (!bValid)
        {
          VASSERT(!"submesh index must be in valid range");
          continue;
        }
        pNativeZone->AddStaticGeometryInstance(pMainInst->GetSubmeshInstance(iSubIndex));
      }
    }
  }

  void EngineInstanceStaticMesh::AssignVisibilityObject(EngineInstanceVisibilityObject ^pVisObj)
  {
    VisVisibilityObject_cl *pObj = nullptr;
    if (pVisObj!=nullptr)
      pObj = pVisObj->GetVisibilityObject();

    // assign to all:
    FOREACH_MESH_LOD
      int iCount = pInstance->GetSubmeshInstanceCount();
      for (int i=0;i<iCount;i++)
        pInstance->GetSubmeshInstance(i)->SetVisibilityObject(pObj);
    }
  }

    
  void EngineInstanceStaticMesh::UpdateLightGridPos()
  {
    hkvVec3 vPos(_x+_lgx,_y+_lgy,_z+_lgz);
    FOREACH_MESH_LOD
      pInstance->SetLightGridSamplePosition(vPos);
      pInstance->InvalidateLightGridInfo();
    }
  }


  void EngineInstanceStaticMesh::SetLightgridSamplePosition(float x, float y, float z) 
  {
    _lgx = x;
    _lgy = y;
    _lgz = z;
    UpdateLightGridPos();
  }

  void EngineInstanceStaticMesh::SetLightmapMultiplier(float fValue)
  {
    FOREACH_MESH_LOD
      pInstance->SetLightmapMultiplier(fValue);
    }
  }


  void EngineInstanceStaticMesh::SetLightmapOverride(String ^filename, bool bForceSet)
  {
    VTextureObject *pTex = NULL;
    if (!String::IsNullOrEmpty(filename))
    {
      VString sFilename;
      ConversionUtils::StringToVString(filename, sFilename);
      pTex = Vision::TextureManager.Load2DTexture(sFilename);
    }

    if (m_pSubMeshes!=NULL && (pTex!=NULL || bForceSet))
    {
      for (int i=0;i<(int)m_pSubMeshes->GetNumEntries();i++)
      {
        VisStaticGeometryInstance_cl *pGeom = m_pSubMeshes->GetEntry(i);
        pGeom->SetLightmapTexture(pTex,0);
        pGeom->SetLightmapTexture(NULL,1);
        pGeom->SetLightmapTexture(NULL,2);
        pGeom->SetLightmapTexture(NULL,3);
        pGeom->SetLightmapScaleOffset(hkvVec4(1.f,1.f,0.f,0.f));
      }
    }

  }

  String ^ EngineInstanceStaticMesh::GetLightmapPage( int _iIdx, int _iPage )
  {
    if ( !m_pSubMeshes )
      return nullptr;
    VTextureObject* pTexture = m_pSubMeshes->GetEntry( _iIdx )->GetLightmapTexture( _iPage );
    return pTexture ? ConversionUtils::VStringToString(pTexture->GetFilename()) : "";
  }

 

  void EngineInstanceStaticMesh::GetLightmapScaleOffset( System::Single* _fX, System::Single* _fY, System::Single* _fZ, System::Single* _fW, int _iIdx )
  {
    if ( !m_pSubMeshes )
      return;

    hkvVec4 vRes = m_pSubMeshes->GetEntry( _iIdx )->GetLightmapScaleOffset();
    *_fX = vRes.x;
    *_fY = vRes.y;
    *_fZ = vRes.z;
    *_fW = vRes.w;
  }

  bool EngineInstanceStaticMesh::HasLightmaps()
  {
    if (!m_pSubMeshes)
      return false;
    for (int i=0;i<(int)m_pSubMeshes->GetNumEntries();i++)
    {
      VisStaticGeometryInstance_cl *pGeom = m_pSubMeshes->GetEntry(i);
      for (int j=0;j<4;j++)
        if (pGeom->GetLightmapTexture(j)!=NULL)
          return true;
    }

    return false;
  }

  void EngineInstanceStaticMesh::ReloadLightmapFile(String ^litFile)
  {
    VString sFilename;
    ConversionUtils::StringToVString(litFile,sFilename);
    VLightmapSceneInfo lmInfo;
    if (!Vision::File.Exists(sFilename) || !lmInfo.LoadOutputFile(sFilename, Vision::File.GetManager(), &Vision::TextureManager.GetManager()))
      return;

    FOREACH_MESH_LOD
      VLightmapPrimitive *pPrim = lmInfo.GetPrimitiveByID(VLightmapPrimitive::OWNERTYPE_STATICMESH, pInstance->GetUniqueID());
      if (pPrim==NULL)
        continue;
      pInstance->ApplyLightmapInfo(lmInfo, pPrim);
    }
  }


  int EngineInstanceStaticMesh::GetNumSubMeshes()
  {
    if ( !m_pSubMeshes )
      return 0;
    return m_pSubMeshes->GetNumEntries();
  }

  void EngineInstanceStaticMesh::RebuildSubMeshes()
  {
    if (!m_pSubMeshes)
      return;

    m_pSubMeshes->Clear();

    for (int i=0;i<m_iLODCount;i++)
    {
      VisStaticMeshInstance_cl* pMeshInst = m_spLODList[i];
      for (int j=0;j<pMeshInst->GetSubmeshInstanceCount();j++)
      {
        m_pSubMeshes->AppendEntry(pMeshInst->GetSubmeshInstance(j));
      }
    }
  }

  void EngineInstanceStaticMesh::SendPhysicsOnStaticMeshCreatedEvent()
  {
    VisStaticMeshInstance_cl *pMainInst = GetLOD0();
    if (pMainInst)
      pMainInst->FirePhysicsStaticMeshInstanceCreatedEvent();
  }

  void EngineInstanceStaticMesh::TriggerTransformationUpdated()
  {
    // special purpose component handling to notify components when the position has changed through UI
    FOREACH_MESH_LOD
      VMeshPaintingComponent *pPaintingComp = pInstance->Components().GetComponentOfType<VMeshPaintingComponent>();
      if (pPaintingComp!=NULL)
        pPaintingComp->OnOwnerTransformationChanged();
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
