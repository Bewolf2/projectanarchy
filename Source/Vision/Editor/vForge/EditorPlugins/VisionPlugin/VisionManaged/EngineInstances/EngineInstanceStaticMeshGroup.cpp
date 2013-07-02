/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceStaticMeshGroup.hpp>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceStaticMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>
#include <Vision/Runtime/Base/Container/VBitField.hpp>
//#include "ConversionUtils.hpp"

#ifdef VGEOM2_LIB
#include <Vision/Tools/Libraries/Geom2/vGeom2.hpp>
#endif

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;

/*
Still TODO:
 - custom shader assignment conversion: when custom texture parameters are used, the filename must be re-directed using ConvertResourceFilename
 - GetDependencies works but not for "Show assets"
 - dynamic light/adjacency
 - visibility object binding
*/

namespace VisionManaged
{

  EngineInstanceStaticMeshGroup::EngineInstanceStaticMeshGroup()
  {
    _x = _y = _z = 0.0f;
    m_pMeshInst = nullptr;
    _uniqueID = NULL;
    fCombinedFarClipDist = -1.f;
    iCombinedVisibleBitmask = 0;
    iCombinedCollisionBitmask = 0;
    bCombinedStaticShadows = false;
    bCombinedDynamicShadows = false;
    pParentZone = nullptr;
  }

  void EngineInstanceStaticMeshGroup::DisposeObject()
  {
  }

  void EngineInstanceStaticMeshGroup::SetObjectKey(String ^key)
  {
    _objectKey = key; // set later
  }


  void EngineInstanceStaticMeshGroup::SetParentZone(Zone ^zone)
  {
    pParentZone = zone; // set later
  }

  void EngineInstanceStaticMeshGroup::OnRecomputeVisibility()
  {
  }

  void EngineInstanceStaticMeshGroup::RenderShape(VisionViewBase ^view, ShapeRenderMode mode, BoundingBox ^pAbsBox)
  {
    if (Vision::Editor.IsAnimatingOrPlaying() || pAbsBox==nullptr || !pAbsBox->Valid)
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
    VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
    
    hkvAlignedBBox bbox(
      hkvVec3 (pAbsBox->X1,pAbsBox->Y1,pAbsBox->Z1),
      hkvVec3 (pAbsBox->X2,pAbsBox->Y2,pAbsBox->Z2));
    pRenderer->RenderAABox(bbox,VColorRef(100,100,255,50),state);
  }


  bool EngineInstanceStaticMeshGroup::OnExport(SceneExportInfo ^info) 
  {
    if (!m_pMeshInst)
      return true;
    m_pMeshInst->AssignToVisibilityZones();
    m_pMeshInst->ReComputeBoundingBoxes(true); // accurate vertex bboxes
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pMeshInst;
    return true;
  }

  void EngineInstanceStaticMeshGroup::GetDependencies(ResourceSnapshotInfo ^info)
  {
    if (!m_pMeshInst)
      return;

    VResourceSnapshot *snapshot = (VResourceSnapshot *)info->NativeResourceSnapshotPtr.ToPointer();
    m_pMeshInst->GetDependencies(*snapshot);
  }

  void EngineInstanceStaticMeshGroup::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
  }

  bool EngineInstanceStaticMeshGroup::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    return false;
  }

#ifdef VGEOM2_LIB


  struct MaterialMappingEntry_t
  {
    MaterialMappingEntry_t()
    {
      m_pMaterial = nullptr;
    }
    ~MaterialMappingEntry_t()
    {
    }

    void Set(VisStaticSubmeshInstance_cl *pSubMeshInst, VGMaterial *pMaterial)
    {
      m_pMaterial = pMaterial;
      m_pSubMeshInst = pSubMeshInst;
    }


    VGMaterial *m_pMaterial;
    VisStaticSubmeshInstance_cl *m_pSubMeshInst;
  };

  // helper class to look up material that must be unique per surface and used light map page
  class MaterialMappingTable
  {
  public:
    MaterialMappingTable()
    {
      m_iEntryCount = 0;
    }
    ~MaterialMappingTable()
    {
      // cleanup
      for (int i=0;i<m_iEntryCount;i++)
        V_SAFE_DELETE(m_List.GetDataPtr()[i].m_pMaterial);
    }


    VGMaterial *FindCompatible(VisStaticSubmeshInstance_cl *pSubMeshInst, int* index=NULL) const
    {
      MaterialMappingEntry_t *pList = m_List.GetDataPtr();
      for (int i=0;i<m_iEntryCount;i++,pList++)
      {
        if (pList->m_pSubMeshInst->GetSurface()==pSubMeshInst->GetSurface() && pList->m_pSubMeshInst->GetLightmapTexture(0)==pSubMeshInst->GetLightmapTexture(0))
        {
          if(index)
            *index = i;
          return pList->m_pMaterial;
        }
      }
      return NULL;
    }
    int Add(VisStaticSubmeshInstance_cl *pSubMeshInst, VGMaterial *pMaterial)
    {
      MaterialMappingEntry_t &entry(m_List[m_iEntryCount]);
      entry.Set(pSubMeshInst, pMaterial);
      return m_iEntryCount++;
    }

    VisStaticSubmeshInstance_cl *GetSubMeshInst(int iIndex) const
    {
      VASSERT(iIndex>=0 && iIndex<m_iEntryCount);
      return m_List.GetDataPtr()[iIndex].m_pSubMeshInst;
    }

    /*
    int GetLightmapID(VTextureObject *pLightmapPage)
    {
      if (pLightmapPage==nullptr)
        return -1;
      int iPos = m_UniqueLightmaps.GetElementPos(pLightmapPage);
      if (iPos<0)
      {
        iPos = m_UniqueLightmaps.GetFreePos();
        m_UniqueLightmaps[iPos] = pLightmapPage;
      }
      return iPos;
    }
*/

    int m_iEntryCount;
    DynObjArray_cl<MaterialMappingEntry_t> m_List;
    //DynArray_cl<VTextureObject *> m_UniqueLightmaps;
  };


  void ConvertGeomInfo(VGGeometryInfo destInfo, const VBaseGeometryInfo& srcInfo)
  {
    destInfo.SetName(srcInfo.m_name);
    destInfo.SetTag(srcInfo.m_iTag);
    destInfo.SetUserFlags(srcInfo.m_sUserFlags);
/* 
These parameters should not contribute to uniqueness of the info, because they are merged across the whole instance
The asssignment would be more complicated, i.e. check per submesh which settings are overridden by the shape
    destInfo.SetVisibilityMask(srcInfo.m_iVisibleMask);
    destInfo.SetLightMask(srcInfo.m_iLightMask);
    destInfo.SetTraceMask(srcInfo.m_iTraceMask);
    destInfo.SetGeometryFlags((VGGeometryInfo::GeometryFlags)srcInfo.m_sFlags);
    destInfo.SetClipDistances(srcInfo.m_fNearClipDistance,srcInfo.m_fFarClipDistance);
    destInfo.SetClipReference(srcInfo.m_vClipReference);
  */
  }

  // check whether geom info with same properties exists
  int RegisterGeomInfo(VGScene &scene, const VGGeometryInfo &info)
  {
    int iCount = scene.GetNumGeometryInfos();
    for (int i=0;i<iCount;i++)
      if (scene.GetGeometryInfo(i)==info)
        return i;
    scene.AddGeometryInfo(info);
    return iCount;
  }


  VGMesh* ConvertStaticMesh(VisStaticMeshInstance_cl *pSrcInst, VGScene &scene, MaterialMappingTable &MaterialLookup)
  {
    VGVertex::VertexMask vm = VGVertex::VertexMask(VGVertex::VGVM_POSITION|VGVertex::VGVM_NORMAL|VGVertex::VGVM_TANGENT|VGVertex::VGVM_TEXCOORD_01|VGVertex::VGVM_TEXCOORD_02|VGVertex::VGVM_COLOR_1);
    VGMesh *pDest = &scene.CreateMesh(vm);
    VisStaticMesh_cl *pSrc = pSrcInst->GetMesh();

    VGVertex tempV(vm);
    VisMeshBuffer_cl *pSrcMesh = pSrc->GetMeshBuffer();
    VisMeshBuffer_cl::MB_PrimitiveType_e primtype = pSrcMesh->GetPrimitiveType();
    int iVertexCount = pSrcMesh->GetVertexCount();
    VisMBVertexDescriptor_t descr;
    pSrcMesh->GetVertexDescriptor(descr);

    // convert vertices
    char *pBuffer = (char *)pSrcMesh->LockVertices(VIS_LOCKFLAG_READONLY);
    for (int i=0;i<iVertexCount;i++, pBuffer+= descr.m_iStride)
    {
      if (descr.HasPosition())
      {
        const hkvVec3* pPos = (const hkvVec3* )&pBuffer[VisMBVertexDescriptor_t::GetComponentOffset(descr.m_iPosOfs)];
        tempV.SetPosition(*pPos);
      }
      if (descr.HasNormal())
      {
        const hkvVec3* pNrml = (const hkvVec3* )&pBuffer[VisMBVertexDescriptor_t::GetComponentOffset(descr.m_iNormalOfs)];
        tempV.SetNormal(*pNrml);
      }
      if (descr.HasTexCoord(2)) // tangent = vec3
      {
        const hkvVec3* pTangent = (const hkvVec3* )&pBuffer[VisMBVertexDescriptor_t::GetComponentOffset(descr.m_iTexCoordOfs[2])];
        tempV.SetTangent(hkvVec4(pTangent->x,pTangent->y,pTangent->z,0.f));
      }
      for (int iTex=0;iTex<2;iTex++) if (descr.HasTexCoord(iTex))
      {
        const hkvVec2 *pTex = (const hkvVec2 *)&pBuffer[VisMBVertexDescriptor_t::GetComponentOffset(descr.m_iTexCoordOfs[iTex])];
        tempV.SetTexCoord(iTex,*pTex);
      }
      if (descr.HasColor())
      {
        const VColorRef *pColor = (const VColorRef *)&pBuffer[VisMBVertexDescriptor_t::GetComponentOffset(descr.m_iColorOfs)];
        hkvVec4 color;
        VColorRef::RGBA_To_Float(*pColor, color);
        tempV.SetColor(0,color);
      }

      pDest->GetVertexList().AddVertex(tempV);
    }
    pSrcMesh->UnLockVertices();

    VTBitfield<16*1024> lightmapAlreadyRemapped(iVertexCount);

    // convert triangles
    pBuffer = (char *)pSrcMesh->LockIndices(VIS_LOCKFLAG_READONLY);
    int *pInd32 = (int *)pBuffer;
    unsigned short *pInd16 = (unsigned short *)pBuffer;;
    bool bIs32Bit = pSrcMesh->GetIndexType()==VIS_INDEXFORMAT_32;

    for (int iSubMesh=0;iSubMesh<pSrc->GetSubmeshCount();iSubMesh++)
    {
      VisStaticSubmesh_cl *pSubMesh = pSrc->GetSubmesh(iSubMesh);
      VisStaticSubmeshInstance_cl *pSubMeshInst = pSrcInst->GetSubmeshInstance(iSubMesh);

      int iFirstIndex, iNumIndices;
      pSubMesh->GetRenderRange(iFirstIndex, iNumIndices);

      int iFirstTri = VisMeshBuffer_cl::GetCalcPrimitiveCount(primtype,iFirstIndex,iFirstIndex); // TODO: the ranges should be available in this format already
      int iNumTris = VisMeshBuffer_cl::GetCalcPrimitiveCount(primtype,iNumIndices,iNumIndices);
      if (iNumTris<=0) // should not happen though (but important to check because it will confuse lightmapping order otherwise [#2290])
        continue; 
      VisSurface_cl *pSrf = pSubMeshInst->GetSurface();
      VTextureObject *pLightmap = pSubMeshInst->GetLightmapTexture(0);
      hkvVec4 vLMScaleOfs = pSubMeshInst->GetLightmapScaleOffset();
      int matIndex;
      VGMaterial *pMaterial = MaterialLookup.FindCompatible(pSubMeshInst, &matIndex);
      if (!pMaterial)
      {
        pMaterial = new VGMaterial();

        const int iLightmapID = pSrf->GetLightMapPageID ();
        pSrf->SetLightMapPageID (0);
        ConversionUtils::ConvertSurfaceToVGMaterial (*pSrf, *pMaterial, false);
        pSrf->SetLightMapPageID (iLightmapID);

        matIndex = MaterialLookup.Add(pSubMeshInst,pMaterial);
      }

      // geometry info group
      int iGeomIndex = -1;
      if (pSubMesh->GetGeometryInfoIndex()>=0) // non-default? Otherwise leave to default in target as well
      {
        VGGeometryInfo geomInfo;
        VBaseGeometryInfo& srcInfo(pSubMesh->GetGeometryInfo());
        ConvertGeomInfo(geomInfo,srcInfo);
        iGeomIndex = RegisterGeomInfo(scene, geomInfo); // get unique index
      }

      for (int iTri=0;iTri<iNumTris;iTri++,iFirstIndex+=3)
      {
        VGTriangleList::Triangle tri;
        tri.ti[0] = bIs32Bit ? pInd32[iFirstIndex+0] : pInd16[iFirstIndex+0];
        tri.ti[1] = bIs32Bit ? pInd32[iFirstIndex+1] : pInd16[iFirstIndex+1];
        tri.ti[2] = bIs32Bit ? pInd32[iFirstIndex+2] : pInd16[iFirstIndex+2];
        tri.tp.triangleFlags = VGTriangleList::TriangleFlags(VGTriangleList::VGTF_SHADOWCASTER|VGTriangleList::VGTF_DEFAULT);
        tri.tp.materialIndex = matIndex;
        tri.tp.geomInfoIndex = iGeomIndex;
        tri.tp.groupIndex    = -1;
        tri.tp.visibilityID  = VGPortal::NO_VIS_ID_ASSIGNED;
        tri.tp.physicsInfoIndex = -1;
        pDest->GetTriangleList().AddTriangle(tri);

        if (!pLightmap)
          continue;

        // manually un-roll lightmap UV:
        // (does this work for overlapping index ranges?)
        for (int iV=0;iV<3;iV++)
        {
          int v = tri.ti[iV];
          if (lightmapAlreadyRemapped.IsBitSet(v))
            continue;

          lightmapAlreadyRemapped.SetBit(v);
          hkvVec2 vLightmapUV = pDest->GetVertexList().GetTexCoord(1, v);
          vLightmapUV.x = vLightmapUV.x * vLMScaleOfs.x + vLMScaleOfs.z;
          vLightmapUV.y = vLightmapUV.y * vLMScaleOfs.y + vLMScaleOfs.w;
          pDest->GetVertexList().SetTexCoord(1, v, vLightmapUV);
        }
      }
    }

    pSrcMesh->UnLockIndices();

    return pDest;
  }

  void EngineInstanceStaticMeshGroup::BeginExport(ShapeCollection ^meshShapes, String ^relFilename)
  {
    try
    {
      relevantShapes = meshShapes;
      VString sFilename;
      ConversionUtils::StringToVString(relFilename,sFilename);
      MaterialMappingTable m_MaterialLookup;

      hkvVec3 vPivot(_x,_y,_z);
      hkvVec3 vLGPivot;
      int iValidCount = 0;

      VRefCountedCollection<VisVisibilityObject_cl> linkedVisObj;
      bool bAllSubmeshesHaveVisObj = true;


      VGScene scene;

      for (int i=0;i<meshShapes->Count;i++)
      {
        EngineInstanceStaticMesh ^pMeshEngineInst = safe_cast<EngineInstanceStaticMesh ^>(meshShapes->GetAt(i)->_engineInstance);

        // How about LOD here?
        if (!pMeshEngineInst || !pMeshEngineInst->GetLOD0() || !pMeshEngineInst->GetLOD0()->GetMesh())
          continue;

        VisStaticMeshInstance_cl *pInst = pMeshEngineInst->GetLOD0();
        VGMesh* pMesh = ConvertStaticMesh(pInst,scene,m_MaterialLookup);
        if (!pMesh)
          continue;

        iValidCount++;

        // collect all visibility objects that are linked to any submesh
        int iSumMeshCount = pInst->GetSubmeshInstanceCount();
        for (int iSubMesh=0;iSubMesh<iSumMeshCount && bAllSubmeshesHaveVisObj;iSubMesh++)
        {
          VisVisibilityObject_cl *pVisObj = pInst->GetSubmeshInstance(iSubMesh)->GetVisibilityObject();
          if (pVisObj==NULL)
          {
            bAllSubmeshesHaveVisObj = false;
            break;
          }
          linkedVisObj.AddUnique(pVisObj);
        }

        vLGPivot += pInst->GetLightGridSamplePosition();

        // Copy over transform, from editor to vGeom2.
        const hkvMat4 &transform = pInst->GetTransform();
        pMesh->VGTransformable::SetTransformation(transform);

        // handle collision meshes, whether they are external or internal
        if (pMeshEngineInst->_iCollisionBitmask!=0)
        {
          // from vcolmesh file? ...
          char szColMeshFile[FS_MAX_PATH];
          VFileHelper::AddExtension(szColMeshFile, pInst->GetMesh()->GetFilename(), "vcolmesh");
          IVFileInStream *pIn = Vision::File.Open(szColMeshFile);
          if (pIn!=NULL)
          {
            VGProcessor_VisionImporter colmeshLoader;
            colmeshLoader.SetDataFormat(VGVisionImporterExporter::VDF_COLLISION_MESH);
            colmeshLoader.SetInStream(pIn);
            if (colmeshLoader.Process(scene)) // Load into scene, directly.
            {
              // A .vcolmesh file only has a single mesh, so we can safely assume
              // it's the last one in the list - set transformation.
              scene.GetMesh(scene.GetNumMeshes()-1).VGTransformable::SetTransformation(transform);
              }
            pIn->Close();
          }
        }

        pMeshEngineInst->_bNoExport = true;
      }

      // Copy materials over to vGeom2 scene, so that they get written out.
      for(int j=0; j<m_MaterialLookup.m_iEntryCount; ++j)
      {
        scene.AddMaterial(*m_MaterialLookup.m_List[j].m_pMaterial);
      }

      if (!iValidCount)
        return;

      IVFileOutStream* pOutStream = Vision::File.Create(sFilename); 
      if (!pOutStream)
        return;

      char szColMeshFile[FS_MAX_PATH];
      VFileHelper::AddExtension(szColMeshFile, sFilename, "vcolmesh");
      IVFileOutStream* pCollOutStream = Vision::File.Create(szColMeshFile); 
      bool bHasCollisionFile = pCollOutStream!=NULL;

      {
        // Generate tangents, merge materials, write mesh and collision mesh.
        VGBackend meshBackend;
        VGProcessor_TangentGenerator tg;  // @@@ wrong - shouldn't enforce tangents that maybe weren't even there, or modify existing ones.
        VGProcessor_MaterialMerger   mm;
        VGProcessor_Baker            bk;
        VGProcessor_Recenterer       rc;
        VGProcessor_VisionExporter   ve;
        VGProcessor_VisionExporter   vc;

        mm.SetMergeScheme(VGProcessor_MaterialMerger::VGMS_NOTHING); // material merging was done outside

        bk.SetFlags(VGProcessor_Baker::VGBF_TRANSFORMS);

        hkvMat4 customCenter;
        customCenter.setTranslation(vPivot);
        customCenter.invert();
        customCenter.setRow (3, hkvVec4 (0, 0, 0, 1));
        rc.SetRecenteringMode(VGProcessor_Recenterer::VGRF_CUSTOM);
        rc.SetCustomTransformation(customCenter);

        ve.SetDataFormat(VGVisionImporterExporter::VDF_MESH);
        ve.SetOutStream(pOutStream);

        vc.SetDataFormat(VGVisionImporterExporter::VDF_COLLISION_MESH);
        vc.SetOutStream(pCollOutStream);

        meshBackend.AddProcessor(&tg);
        meshBackend.AddProcessor(&mm);
        meshBackend.AddProcessor(&bk);
        meshBackend.AddProcessor(&rc);
        meshBackend.AddProcessor(&ve);
        if (pCollOutStream)
          meshBackend.AddProcessor(&vc);

        if(meshBackend.RunProcessors(scene))
        {
          // delete the physX file (hack, should go through a physics module function)
          char szCookedMeshFile[FS_MAX_PATH];
          sprintf(szCookedMeshFile,"%s_data\\cookedMesh_1.00_1.00_1.00.pc.physx",pOutStream->GetFileName());
          VFileHelper::Delete(szCookedMeshFile);
          sprintf(szCookedMeshFile,"%s_data\\cookedMesh_1.00_1.00_1.00.xbox360.physx",pOutStream->GetFileName());
          VFileHelper::Delete(szCookedMeshFile);
          sprintf(szCookedMeshFile,"%s_data\\cookedMesh_1.00_1.00_1.00.ps3.physx",pOutStream->GetFileName());
          VFileHelper::Delete(szCookedMeshFile);
          sprintf(szCookedMeshFile,"%s_data\\cookedMesh_1.00_1.00_1.00.wii.physx",pOutStream->GetFileName());
          VFileHelper::Delete(szCookedMeshFile);
        }
      }

      if (pCollOutStream)
        pCollOutStream->Close();

      pOutStream->Close();


      // prepare the mesh instance:
      VisStaticMesh_cl *pRes = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(sFilename);
      if (!pRes)
        return;
      pRes->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      VisZoneResource_cl *pNativeZone = pParentZone ? ((VisZoneResource_cl *)pParentZone->NativeZonePtr.ToPointer()) : NULL;
      VString sObjectKey;
      ConversionUtils::StringToVString(_objectKey,sObjectKey);
      hkvMat4 transform;
      transform.setTranslation(vPivot);
      m_pMeshInst = pRes->CreateInstance(transform, NULL, false); // no physics notification
      m_pMeshInst->SetParentZone(pNativeZone);
      m_pMeshInst->ReComputeBoundingBoxes();
      m_pMeshInst->AssignToVisibilityZones();
      m_pMeshInst->SetUniqueID(_uniqueID);
      m_pMeshInst->SetClipSettings(0.f,fCombinedFarClipDist, (fCombinedFarClipDist>0.f) ? (hkvVec3* )&vPivot : NULL);
      m_pMeshInst->SetCollisionBitmask(iCombinedCollisionBitmask);
      m_pMeshInst->SetCollisionBehavior(bHasCollisionFile ? VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FROMFILE : VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM);
      m_pMeshInst->SetCastStaticShadows(bCombinedStaticShadows);
      m_pMeshInst->SetObjectKey(sObjectKey);

      // average light grid pivot
      vLGPivot *= (1.f/(float)iValidCount);
      m_pMeshInst->SetLightGridSamplePosition(vLGPivot);

      // generate a merged visibility object:
      VisVisibilityObjectPtr spMergedVisObj;
      if (bAllSubmeshesHaveVisObj)
      {
        VASSERT(linkedVisObj.Count()>0);
        if (linkedVisObj.Count()==1)
          spMergedVisObj = linkedVisObj.GetAt(0); // take the reference directly
        else
        {
          // make a union of all visibility objects
          hkvAlignedBBox bbox;
          int iTestFlags = 0;
          int iContextMask = 0;
          float fFarClip = 0.f;
          for (int i=0;i<linkedVisObj.Count();i++)
          {
            bbox.expandToInclude(linkedVisObj.GetAt(i)->GetWorldSpaceBoundingBox());
            iTestFlags |= linkedVisObj.GetAt(i)->GetVisTestFlags();
            iContextMask |= linkedVisObj.GetAt(i)->GetVisibleBitmask();
            float fFar = linkedVisObj.GetAt(i)->GetFarClipDistance();
            if (fFarClip>0.f || i==0)
            {
              if (fFar<=0.f) 
                fFarClip = 0.f; // from now on, disable farclip
              else
                fFarClip = hkvMath::Max(fFarClip,fFar);
            }
          }
          VASSERT(bbox.isValid());
          if (fFarClip>0.f)
            iTestFlags |= VISTESTFLAGS_FARCLIPTEST;
          else
            iTestFlags &= (~VISTESTFLAGS_FARCLIPTEST);

          // create temporary new vis obj. Since the temp mesh instance will hold the only reference to it, it will be deleted afer export
          spMergedVisObj = new VisVisibilityObjectAABox_cl(iTestFlags);
          spMergedVisObj->SetWorldSpaceBoundingBox(bbox);
          spMergedVisObj->SetVisibleBitmask(iContextMask);
          spMergedVisObj->SetFarClipDistance(fFarClip);
        }
      }

      // per submesh properties
      for (int i=0;i<m_pMeshInst->GetSubmeshInstanceCount();i++)
      {
        VisStaticSubmesh_cl *pSubMesh = m_pMeshInst->GetMesh()->GetSubmesh(i);
        VisStaticSubmeshInstance_cl *pSubMeshInst = m_pMeshInst->GetSubmeshInstance(i);
        bool bFinal = pSubMesh->GetGeometryInfo().GetCastDynamicShadows() && bCombinedDynamicShadows;
        pSubMeshInst->SetCastDynamicShadows(bFinal);

        // copy lightmap references
        int iSrfIndex = pSubMesh->GetSurfaceIndex(); // relies on materials not re-sorted, duplicated or collapsed
        VisStaticSubmeshInstance_cl *pOrigSubMeshInst = m_MaterialLookup.GetSubMeshInst(iSrfIndex);
        for (int j=0;j<4;j++)
        {
          VTextureObject *pLightmap = pOrigSubMeshInst->GetLightmapTexture(j);
          pSubMeshInst->SetLightmapTexture(pLightmap,j);
        }

        pSubMeshInst->SetLightmapScaleOffset(hkvVec4(1.f,1.f,0.f,0.f)); // identity - scaled during vertex conversion
        //pSubMeshInst->SetLightmapTexture();
        pSubMeshInst->SetVisibilityObject(spMergedVisObj);
      }
      m_pMeshInst->AddRef();
      m_pMeshInst->SetObjectFlag(VObjectFlag_AutoDispose);
    }
    catch (Exception ^ex)
    {
      EditorManager::DumpException( ex );
    }

  }

  void EngineInstanceStaticMeshGroup::EndExport()
  {
    V_SAFE_RELEASE(m_pMeshInst);

    if (relevantShapes == nullptr)
      return;

    // flag back instances
    for (int i=0;i<relevantShapes->Count;i++)
    {
      EngineInstanceStaticMesh ^pMeshEngineInst = safe_cast<EngineInstanceStaticMesh ^>(relevantShapes->GetAt(i)->_engineInstance);
      if (pMeshEngineInst)
        pMeshEngineInst->_bNoExport = false;
    }
    relevantShapes = nullptr;
  }

#else

  void EngineInstanceStaticMeshGroup::BeginExport(ShapeCollection ^meshShapes, String ^relFilename)
  {
  }

  void EngineInstanceStaticMeshGroup::EndExport()
  {
  }

#endif

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
