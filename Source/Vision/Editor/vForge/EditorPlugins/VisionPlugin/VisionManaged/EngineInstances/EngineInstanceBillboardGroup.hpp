/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::View;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::Serialization;
using namespace System::Collections;
using namespace System::Runtime::Serialization;
using namespace ManagedFramework;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BillboardGroup.hpp>

namespace VisionManaged
{
 
  [Serializable]
  public ref class BillboardInstance : public ICloneable, public ISerializable
  {
  public:
    BillboardInstance()
    {
      X = 0.f;
      Y = 0.f;
      Z = 0.f;
      ScaleX = 1.0f;
      ScaleY = 1.0f;

      _fFinalScaleX = _fFinalScaleY = 1.0f;
      _wsX = _wsY = _wsZ = 0.0f;
      _fAtlasValue = 0.0f;

    }

    VOVERRIDE Object ^Clone() = ICloneable::Clone
    {
      BillboardInstance^ copy = static_cast<BillboardInstance ^>(this->MemberwiseClone());
      return copy;
    }
    
    BillboardInstance(SerializationInfo ^info, StreamingContext context)
    {
      X = info->GetSingle("_x");
      Y = info->GetSingle("_y");
      Z = info->GetSingle("_z");
      ScaleX = info->GetSingle("_sx");
      ScaleY = info->GetSingle("_sy");

      if (SerializationHelper::HasElement(info,"_av"))
        _fAtlasValue = info->GetSingle("_av");

	  }

    VOVERRIDE void GetObjectData(SerializationInfo ^info, StreamingContext context) = ISerializable::GetObjectData
    {
      info->AddValue("_x",X);
      info->AddValue("_y",Y);
      info->AddValue("_z",Z);
      info->AddValue("_sx",ScaleX);
      info->AddValue("_sy",ScaleY);
      info->AddValue("_av",_fAtlasValue);
    }

    // position
    property float X;
    property float Y;
    property float Z;

    // size
    property float ScaleX;
    property float ScaleY;

    property float UniformScale
    {
      void set(float value) {ScaleY = value; ScaleX = value;}
      float get() {return (ScaleX+ScaleY)*0.5f;}
    }


    float _fAtlasValue; // [0..1] random value

    // cached information
    float _wsX, _wsY, _wsZ; // world space coordinates
    float _fFinalScaleX, _fFinalScaleY; // cached final scale that uses atlas scaling as well
  };


  public ref class EngineInstanceBillboardGroup : public IEngineShapeInstance
  {
  public:     

    EngineInstanceBillboardGroup();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override {return System::IntPtr((void*)m_pMesh);}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetParentZone(Zone ^zone) override;
    VOVERRIDE void SetVisible(bool bStatus) override {m_bVisible=bStatus;UpdateVisibleStatus();}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float x,float y, float z) override {}
    VOVERRIDE void SetUniqueID(unsigned __int64 iID) override {if (m_pMesh) m_pMesh->SetUniqueID(iID);}
    VOVERRIDE void SetObjectKey(String ^key) override;
    VOVERRIDE void OnRecomputeVisibility() override;
    VOVERRIDE void GetDependencies(ResourceSnapshotInfo ^info) override;
    VOVERRIDE void OnPostEngineInstanceCreation() override {ConversionUtils::CallOnDeserializationFunction(m_pMesh);}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnBeforeExport(SceneExportInfo ^info) override {if (m_pMesh!=NULL) m_pMesh->SetMarkInsideSerializationSession(true);}
    VOVERRIDE void OnAfterExport(SceneExportInfo ^info) override {if (m_pMesh!=NULL) m_pMesh->SetMarkInsideSerializationSession(false);}

    // special functions
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    void UpdateTransformation();

    // base properties
    void SetVisibleBitmask(unsigned int iVisibleBitmask) {m_iVisibleBitmask=iVisibleBitmask;UpdateVisibleStatus();}
    void UpdateVisibleStatus() {m_pMesh->SetVisibleBitmask(m_bVisible ? m_iVisibleBitmask:0);}
    void SetBaseSize(float x, float y) {m_fBaseSizeX=x;m_fBaseSizeY=y;}
    void SetRelCenter(float x, float y) {m_fRelCenterX=x;m_fRelCenterY=y;}
    void SetTexture(String ^filename);
    void SetBlendMode(TransparencyType transp, bool bDeferredLighting);
    void SetColor(unsigned int iCol, bool bUseBrightness) {m_iColor = iCol;m_bUseBrightness=bUseBrightness;}
    void ReassignShader() {if (m_pMesh) m_pMesh->ReassignShader();}
    void SetClipDistances(float fNear, float fFar) {if (m_pMesh) m_pMesh->SetClipDistances(fNear,fFar);}
    void SetCastStaticShadows(bool bStatus) {if (m_pMesh) m_pMesh->SetCastStaticShadows(bStatus);}

    // update mesh:
    //void SetInstances(BillboardInstance^ pData[], TextureAtlas ^pAtlas) {m_pInstances=pData;m_pAtlas=pAtlas;UpdateMesh();}
    void SetInstances(array<BillboardInstance^>^ pData, TextureAtlas ^pAtlas) {m_pInstances=pData;m_pAtlas=pAtlas;UpdateMesh();}
    void UpdateMesh();
    //void GetDropToFloorHeights(Shape3D ^parentShape, BillboardInstance^ pData[], Shape3D::DropToFloorMode mode, Vector3F axis, ShapeCollection ^colliderShapes);
    void GetDropToFloorHeights(Shape3D ^parentShape, array<BillboardInstance^>^ pData, Shape3D::DropToFloorMode mode, Vector3F axis, ShapeCollection ^colliderShapes);
    //BillboardInstance^ GetBestTraceHit(Shape3D ^parentShape, BillboardInstance^ pData[], Vector3F rayStart, Vector3F rayEnd);
    BillboardInstance^ GetBestTraceHit(Shape3D ^parentShape, array<BillboardInstance^>^ pData, Vector3F rayStart, Vector3F rayEnd);

    // helpers:
//    static void InstancesFromRawData(ArrayList ^pDest, int iVersion, float src __gc[]);
//    static float RawDataFromInstances(BillboardInstance^ pData[]) __gc[];
    static void InstancesFromRawData(ArrayList ^pDest, int iVersion, String ^rawData);
    //static String ^ RawDataFromInstances(BillboardInstance^ pData[]);
    static String ^ RawDataFromInstances(array<BillboardInstance^>^ pData);
    static int GetCurrentRawDataVersion() {return 1;}
    //void RelaxPositions(BillboardInstance^ pData[], float fAverageRadX, int iIterations);
    void RelaxPositions(array<BillboardInstance^>^ pData, float fAverageRadX, int iIterations);

  private:
    float _x,_y,_z,_yaw,_pitch,_roll;
    float m_fBaseSizeX,m_fBaseSizeY; 
    float m_fRelCenterX,m_fRelCenterY; 
    bool m_bVisible, m_bUseBrightness;
    unsigned int m_iVisibleBitmask, m_iColor;
    VBillboardGroupInstance *m_pMesh;
    TextureAtlas ^ m_pAtlas;
    //BillboardInstance^ m_pInstances[];
    array<BillboardInstance ^>^ m_pInstances;
  };
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
