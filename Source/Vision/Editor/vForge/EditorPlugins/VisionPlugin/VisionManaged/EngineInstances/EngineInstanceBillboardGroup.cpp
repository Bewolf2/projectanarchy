/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceBillboardGroup.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

namespace VisionManaged
{

  EngineInstanceBillboardGroup::EngineInstanceBillboardGroup()
  {
    m_pInstances = nullptr;
    _x=_y=_z=_yaw=_pitch=_roll=0.f;
    m_iVisibleBitmask = 0xffffffff;
    m_iColor = 0xffffffff;
    m_bVisible=true;
    m_fBaseSizeX=10.f;
    m_fBaseSizeY=20.f;
    m_fRelCenterX = m_fRelCenterY = 0.5f;
    m_bUseBrightness = false;
    m_pMesh = new VBillboardGroupInstance(MAX_BILLBOARDS_PER_GROUP,true);
    m_pMesh->AddRef();
  }

  void EngineInstanceBillboardGroup::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pMesh);
  }


  void EngineInstanceBillboardGroup::UpdateTransformation()
  {
    hkvMat3 rotMat;
    hkvMat4 transform;
    ConversionUtils::CreateRotationMatrix (rotMat, _yaw,_pitch,_roll);
    transform.setRotationalPart(rotMat);
    transform.setTranslation(hkvVec3(_x,_y,_z));
    m_pMesh->SetTransform(transform);
    m_pMesh->ReComputeBoundingBoxes();
    m_pMesh->AssignToVisibilityZones();
  }

  void EngineInstanceBillboardGroup::SetParentZone(Zone ^zone)
  {
    if (!m_pMesh)
      return;

    VisZoneResource_cl *pNativeZone = zone ? ((VisZoneResource_cl *)zone->NativeZonePtr.ToPointer()) : NULL;
    m_pMesh->SetParentZone(pNativeZone);
  }


  void EngineInstanceBillboardGroup::SetPosition(float x,float y,float z)
  {
    _x=x; _y=y; _z=z;
    UpdateTransformation();
  }

  void EngineInstanceBillboardGroup::SetOrientation(float yaw,float pitch,float roll)
  {
    _yaw = yaw; _pitch=pitch; _roll=roll;
    UpdateTransformation();
  }

  void EngineInstanceBillboardGroup::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
  }

  void EngineInstanceBillboardGroup::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
  }



  bool EngineInstanceBillboardGroup::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    hkvAlignedBBox box = m_pMesh->GetMesh()->GetBoundingBox();
    (*bbox).Set(box.m_vMin.x,box.m_vMin.y,box.m_vMin.z, box.m_vMax.x,box.m_vMax.y,box.m_vMax.z);
    return true;
  }

  void EngineInstanceBillboardGroup::SetObjectKey(String ^key)
  {
     if (!m_pMesh)
      return;
    VString sKey;
    ConversionUtils::StringToVString(key,sKey);
    if (sKey.IsEmpty())
      m_pMesh->SetObjectKey(NULL);
    else
      m_pMesh->SetObjectKey(sKey);
  }

  void EngineInstanceBillboardGroup::OnRecomputeVisibility()
  {
    if (m_pMesh)
      m_pMesh->AssignToVisibilityZones();
  }

  void EngineInstanceBillboardGroup::GetDependencies(ResourceSnapshotInfo ^info)
  {
    if (!m_pMesh)
      return;

    VResourceSnapshot *snapshot = (VResourceSnapshot *)info->NativeResourceSnapshotPtr.ToPointer();
    m_pMesh->GetDependencies(*snapshot);
  }


  bool EngineInstanceBillboardGroup::OnExport(SceneExportInfo ^info) 
  {
    if (m_pInstances==nullptr || m_pInstances->Length==0)
      return true;

    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pMesh;
    return true;
  }

  void EngineInstanceBillboardGroup::SetTexture(String ^filename)
  {
    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    VTextureObject *pTex = nullptr;
    if (!sFilename.IsEmpty())
      pTex = Vision::TextureManager.Load2DTexture(sFilename, VTM_FLAG_DEFAULT_MIPMAPPED|VTM_FLAG_ASSUME_SRGB);
    m_pMesh->SetTexture(pTex);

  }

  void EngineInstanceBillboardGroup::SetBlendMode(TransparencyType transp, bool bDeferredLighting)
  {
    if (((int)transp)!=((int)(VIS_TRANSP_NONE)) && ((int)transp)!=((int)(VIS_TRANSP_ALPHATEST)))
      bDeferredLighting = false;
    if (m_pMesh)
      m_pMesh->SetTransparency((VIS_TransparencyType)transp, bDeferredLighting);
  }


  void EngineInstanceBillboardGroup::UpdateMesh()
  {

    VBillboardStaticMesh::VBillboardVertex_t *pV = m_pMesh->LockVertices();
    int iCount = m_pInstances ? m_pInstances->Length : 0;
    iCount = hkvMath::Min(iCount,MAX_BILLBOARDS_PER_GROUP);
    hkvAlignedBBox groupBox;
    float cx = -1.f + m_fRelCenterX;
    float cy = -1.f + m_fRelCenterY;
    float mx = hkvMath::Max(m_fRelCenterX,-cx);
    float my = hkvMath::Max(m_fRelCenterY,-cy);

    VColorRef iColor,iFinalColor;
    iColor.SetRGBA(m_iColor);
    hkvVec3 vBaseColor = iColor.ToFloat();
    iFinalColor = iColor;
    VLightGrid_cl *pGrid = m_pMesh->GetRelevantLightGrid();

    bool bUseLighting = m_bUseBrightness && pGrid!=nullptr;
    const hkvMat4 &transform = m_pMesh->GetTransform();
    for (int i=0;i<iCount;i++,pV+=4)
    {
      BillboardInstance ^inst = m_pInstances[i];
      TextureAtlas::AtlasEntry ^pEntry = m_pAtlas->GetRandomEntry(inst->_fAtlasValue);
      hkvAlignedBBox bbox;
      hkvVec3 vPivot(inst->X,inst->Y,inst->Z);
      hkvVec3 wsPos = vPivot;
      wsPos = transform.transformPosition(wsPos);
      if (bUseLighting)
      {
        hkvVec3 vCol;
        pGrid->GetAverageColorAtPositionI(wsPos,vCol);
        vCol.x *= vBaseColor.x;
        vCol.y *= vBaseColor.y;
        vCol.z *= vBaseColor.z;
        iFinalColor.FromFloat(vCol);
      }

      // cache the world space coordinates:
      inst->_wsX = wsPos.x;
      inst->_wsY = wsPos.y;
      inst->_wsZ = wsPos.z;

      iFinalColor.a = 255;
      VBillboardStaticMesh::VBillboardVertex_t pivot(vPivot,iFinalColor);
      float sx = m_fBaseSizeX*inst->ScaleX*pEntry->ResultingScaleX;
      float sy = m_fBaseSizeY*inst->ScaleY*pEntry->ResultingScaleY;
      inst->_fFinalScaleX = sx;
      inst->_fFinalScaleY = sy;

      pV[0].SetPivot(pivot, sx * m_fRelCenterX,sy * m_fRelCenterY,  pEntry->U1,pEntry->V1);
      pV[1].SetPivot(pivot, sx*cx,sy * m_fRelCenterY,               pEntry->U2,pEntry->V1);
      pV[2].SetPivot(pivot, sx * m_fRelCenterX, sy*cy,              pEntry->U1,pEntry->V2);
      pV[3].SetPivot(pivot, sx*cx, sy*cy,                           pEntry->U2,pEntry->V2);
      pivot.GetPivotBoundingBox(bbox,mx*sx,my*sy);
      groupBox.expandToInclude(bbox);
    }
    m_pMesh->UnLockVertices();
    m_pMesh->SetUsedBillboardCount(iCount,groupBox);
    VCompiledEffect *pFX = ConversionUtils::GetShaderEffect(m_pAtlas->ShaderEffect, EFFECTFLAGS_NONE, NULL);
    m_pMesh->SetCustomEffect(pFX);
  }

  BillboardInstance^ EngineInstanceBillboardGroup::GetBestTraceHit(Shape3D ^parentShape, array<BillboardInstance^>^ pData, Vector3F rayStart, Vector3F rayEnd)
  {
    float cx = -1.f + m_fRelCenterX;
    float cy = -1.f + m_fRelCenterY;

    BillboardInstance ^pBestInst = nullptr;
    hkvVec3 intersectionPos;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
    hkvVec3 vDir = vEnd-vStart;

    float fBestDist = 0.f;
    hkvVec3 vCamPos = Vision::Camera.GetMainCamera()->GetPosition();
    hkvVec3 vCamRight;// = Vision::Camera.GetMainCamera()->GetObjDir_Right();
    hkvVec3 vCamUp(0,0,1.f);// = //Vision::Camera.GetMainCamera()->GetObjDir_Up();

    VTriangle tri;

    for (int i=0;i<pData->Length;i++)
    {
      BillboardInstance ^inst = pData[i];

      float sx = inst->_fFinalScaleX;
      float sy = inst->_fFinalScaleY;

      // match the spanning in the vertex shader here
      hkvVec3 pivot(inst->_wsX,inst->_wsY,inst->_wsZ); // assume world space is cached
      hkvVec3 diff = pivot-vCamPos;
      vCamRight = vCamUp.cross (diff);
      vCamRight.normalizeIfNotZero();

      hkvVec3 v0 = pivot + vCamRight*sx*cx            + vCamUp*sy*cy;
      hkvVec3 v1 = pivot + vCamRight*sx*m_fRelCenterX + vCamUp*sy*cy;
      hkvVec3 v2 = pivot + vCamRight*sx*cx            + vCamUp*sy*m_fRelCenterY;
      hkvVec3 v3 = pivot + vCamRight*sx*m_fRelCenterX + vCamUp*sy*m_fRelCenterY;

      // check both triangles for hit
      tri.SetPoints((hkvVec3* )&v0,(hkvVec3* )&v1,(hkvVec3* )&v2);
      float fThisDist;
      if (!tri.GetTraceIntersection(vStart,vDir,true,fThisDist))
      {
        tri.SetPoints((hkvVec3* )&v1,(hkvVec3* )&v2,(hkvVec3* )&v3);
        if (!tri.GetTraceIntersection(vStart,vDir,true,fThisDist))
          continue;
      }
      
      if (pBestInst==nullptr || fThisDist<fBestDist)
      {
        fBestDist = fThisDist;
        pBestInst = inst;
      }
    }

    return pBestInst;
  }

  void EngineInstanceBillboardGroup::GetDropToFloorHeights(Shape3D ^parentShape, array<BillboardInstance^>^ pData, Shape3D::DropToFloorMode mode, Vector3F axis, ShapeCollection ^colliderShapes)
  {
    float fPickDist = EditorManager::Settings->MaxPickingDistance;
    VisTraceLineInfo_t* pTraceInfo = new VisTraceLineInfo_t();
    Vector3F shapepos = parentShape->Position;
    Matrix3F shaperot = parentShape->RotationMatrix;
    hkvVec3 vAxis(axis.X,axis.Y,axis.Z);

    for (int i=0;i<pData->Length;i++)
    {
      BillboardInstance ^inst = pData[i];
      if (mode == Shape3D::DropToFloorMode::Pivot)
      {
        // perform a traceline
        Vector3F localPos(inst->X,inst->Y,inst->Z);
        Vector3F wsPos = shaperot * localPos + shapepos;
        hkvVec3 vStart(wsPos.X,wsPos.Y,wsPos.Z);
        hkvVec3 vEnd = vStart + vAxis * fPickDist;
        EditorManager::EngineManager->TriggerTracelineEvent(Vector3F(vStart.x,vStart.y,vStart.z), Vector3F(vEnd.x,vEnd.y,vEnd.z));

        Vision::CollisionToolkit.TraceLine(vStart,vEnd,VIS_TRACE_ALL,VIS_TRACE_NONE,NULL,NULL,pTraceInfo);
        VisionEngineManager::TraceCustomMeshes(pTraceInfo,vStart,vEnd,colliderShapes);
        if (pTraceInfo->detected)
        {
          inst->X += axis.X * pTraceInfo->distance;
          inst->Y += axis.Y * pTraceInfo->distance;
          inst->Z += axis.Z * pTraceInfo->distance;
        }
      }
    }
    delete pTraceInfo;
  }



  float FromHexString(const char *str)
  {
    char hexstr[9];
    strncpy(hexstr,str,8);
    hexstr[8] = 0;

    unsigned int iHex = 0;
    if (sscanf(hexstr,"%08X",&iHex)==1)
      return *((float *)&iHex);
    return 0.f;
  }

  void ToHexString(float fVal, char *str)
  {
    unsigned int iHex = *((unsigned int *)&fVal);
    sprintf(str,"%08X",iHex);
  }


  void EngineInstanceBillboardGroup::InstancesFromRawData(ArrayList ^pDest, int iVersion, String ^rawData)
  {
    System::Diagnostics::Debug::Assert(iVersion==0 || iVersion==1);

    VString sData;
    ConversionUtils::StringToVString(rawData,sData);

    int iStride32 = 5;
    if (iVersion==1) iStride32 = 6; // added atlas

    int iLen = sData.GetSize(); // only ANSI characters
    System::Diagnostics::Debug::Assert((iLen%(iStride32*8+2))==0);
    int iCount = iLen/(iStride32*8+2);
    int iIndex = 0;
    const char *data = sData.AsChar();
    for (int i=0;i<iCount;i++,data+=(iStride32*8+2))
    {
      BillboardInstance ^inst = gcnew BillboardInstance();
      inst->X = FromHexString(&data[0*8]);
      inst->Y = FromHexString(&data[1*8]);
      inst->Z = FromHexString(&data[2*8]);
      inst->ScaleX = FromHexString(&data[3*8]);
      inst->ScaleY = FromHexString(&data[4*8]);
      if (iVersion>=1)
        inst->_fAtlasValue = FromHexString(&data[5*8]);
      pDest->Add(inst);
    }
  }

  String ^ EngineInstanceBillboardGroup::RawDataFromInstances(array<BillboardInstance^>^ pData)
  {
    char *data = new char[pData->Length*(6*8+2) + 1];
    data[0] = 0;

    char *pos = data;
    int iIndex = 0;
    for (int i=0;i<pData->Length;i++,pos+=(6*8+2))
    {
      BillboardInstance ^inst = pData[i];
      ToHexString(inst->X,&pos[0*8]);
      ToHexString(inst->Y,&pos[1*8]);
      ToHexString(inst->Z,&pos[2*8]);
      ToHexString(inst->ScaleX,&pos[3*8]);
      ToHexString(inst->ScaleY,&pos[4*8]);
      ToHexString(inst->_fAtlasValue,&pos[5*8]);
      strcpy(&pos[6*8+0],"  ");
    }

    String ^str = ConversionUtils::VStringToString(data);
    delete[] data;
    return str;
  }

  void EngineInstanceBillboardGroup::RelaxPositions(array<BillboardInstance^>^ pData, float fAverageRadX, int iIterations)
  {
    int iCount = pData->Length;
    hkvVec3* pForces = new hkvVec3[iCount];
    float fEps = 0.1f;
    float fInvDistScale = 1.f;
    const float fForceScale = fAverageRadX*m_fBaseSizeX;

    for (int iter=0;iter<iIterations;iter++)
    {
      // reset forces
      //for (int j=0;j<iCount;j++) pForces[j].Clear();

      for (int a=0;a<iCount;a++)
        for (int b=a+1;b<iCount;b++)
        {
          BillboardInstance ^instA = pData[a];
          BillboardInstance ^instB = pData[b];
          hkvVec3 vA(instA->X,instA->Y,instA->Z);
          hkvVec3 vB(instB->X,instB->Y,instB->Z);
          hkvVec3 vDiff = vB-vA;

          float rA = fAverageRadX * instA->_fFinalScaleX;
          float rB = fAverageRadX * instB->_fFinalScaleX;

          float fDistSqr = vDiff.getLengthSquared ()*fInvDistScale;
          float fDist = hkvMath::sqrt(fDistSqr);
          if (fDist>(rA+rB))
            continue;

          if (fDist<fEps)
          {
            vDiff.set(Vision::Game.GetFloatRandNeg(),Vision::Game.GetFloatRandNeg(),0.001f);
            vDiff.setLength(fAverageRadX);
          }
          else
          {
            if (fDistSqr<fForceScale)
              fDistSqr = fForceScale;
            vDiff *= (fForceScale/fDistSqr); // 1/r falloff
          }

          pForces[a] -= vDiff;
          pForces[b] += vDiff;
        }

      // apply forces and reset
      for (int j=0;j<iCount;j++)
      {
        hkvVec3& force(pForces[j]);
        BillboardInstance ^inst = pData[j];
        inst->X += force.x;
        inst->Y += force.y;
        //inst->_z += force.z; // do not move up or down
        force.setZero();
      }
    }
    delete[] pForces;
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
