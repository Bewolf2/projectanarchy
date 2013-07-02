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
using namespace ManagedFramework;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::Serialization;

class VTerrainDecorationGroup;
class VTerrainDecorationGroupCollection;
class IVTerrainDecorationModel;
class VGroupInstance;

using namespace System::Collections;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;

namespace VisionManaged
{

  //public ref class DecorationInstanceShape;

  [Serializable]
  public ref class DecorationInstanceShape : public Shape3D
  {
  public:
    DecorationInstanceShape(String ^name) : Shape3D(name)
    {
      _fRandomSeed = 0.0f;
      m_pGroupEntry = nullptr;
      _fCachedRadius = 1.f;
      _fCachedLocalMinX = _fCachedLocalMinY =_fCachedLocalMinZ = -1.f;
      _fCachedLocalMaxX = _fCachedLocalMaxY =_fCachedLocalMaxZ = 1.f;
      _iTintColor = 0xffffffff;
    }

    [SortedCategory("Seed", Shape3D::LAST_CATEGORY_ORDER_ID+1), PropertyOrder(1)]
    [Description("Random value to pick a model from the model group")]
    property float RandomSeed
    {
      float get()
      {
        return _fRandomSeed;
      }

      void set(float value)
      {
        _fRandomSeed=value;
        if (_fRandomSeed<0.f) _fRandomSeed = 0.f;
        else if (_fRandomSeed>1.f) _fRandomSeed = 1.f;
        TriggerSeedUpdate();
      }
    }

    VOVERRIDE void OnAddedToScene() override
    {
      Shape3D::OnAddedToScene();
      RemoveHint(HintFlags_e::OnlyYawRotation);
    }

    VOVERRIDE void GetObjectData(SerializationInfo ^info, StreamingContext context) override 
    {
      Shape3D::GetObjectData(info,context);

      info->AddValue("_fRandomSeed", _fRandomSeed);
      if (_iTintColor!=0xffffffff)
        info->AddValue("_iTintColor", _iTintColor);
     }

    float _fRandomSeed;
    ModelGroup::GroupEntry ^m_pGroupEntry;

    // cache during updating the mesh (model specific)
    float _fCachedRadius;
    float _fCachedLocalMinX, _fCachedLocalMinY, _fCachedLocalMinZ;
    float _fCachedLocalMaxX, _fCachedLocalMaxY, _fCachedLocalMaxZ;
    unsigned int _iTintColor;

  protected: 
    DecorationInstanceShape(SerializationInfo ^info, StreamingContext context)
      : Shape3D(info, context)
    {
      _fRandomSeed = info->GetSingle("_fRandomSeed");
      if (SerializationHelper::HasElement(info,"_iTintColor"))
        _iTintColor = info->GetUInt32("_iTintColor");
      else
        _iTintColor = 0xffffffff;
   }

    // implemented in DecorationInstanceProxyShape
    virtual void TriggerGroupUpdate()
    {
    }
    // implemented in DecorationInstanceProxyShape
    virtual void TriggerSeedUpdate()
    {
    }
  };

  public interface class IGroupHelper
  {
  public:
    ModelGroup::GroupEntry^ GetModel(float fSeed);
    DecorationInstanceShape ^CreateNewInstanceShape(String ^name);
  };


  public ref class EngineInstanceDecorationGroup : public IEngineShapeInstance
  {
  public:     

    EngineInstanceDecorationGroup();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override {return System::IntPtr((void*)NULL);}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override {m_bVisible=bStatus;UpdateVisibleStatus();}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float x,float y, float z) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
    VOVERRIDE void SetUniqueID(unsigned __int64 iID) override;
    VOVERRIDE void GetDependencies(ResourceSnapshotInfo ^info) override;
    VOVERRIDE void OnPostEngineInstanceCreation() override;
    VOVERRIDE void OnBeforeExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnAfterExport(SceneExportInfo ^info) override;

    // update mesh:
    void SetInstances(array<DecorationInstanceShape ^> ^ pData, array<ArrayList ^> ^ pInstancesSortedByModel) 
    {
      m_pInstances=pData;
      m_pInstancesSortedByModel = pInstancesSortedByModel;
      //UpdateInstances();
    }
    void UpdateInstances();

    String^ BakeInstances(String ^filename);
    String^ CreateFromBinaryFile(String ^filename, IGroupHelper ^atlas);
    String^ UnbakeInstances(String ^filename, IGroupHelper ^creator, ShapeCollection ^instances);
    IVTerrainDecorationModel* Helper_LoadNativeModel(ModelGroup::GroupEntry ^pGroup);
    void Helper_Assign(VTerrainDecorationInstance &dest, const VGroupInstance &src, float fExternalScale, IVTerrainDecorationModel* pModel);

    void SetVisibleBitmask(unsigned int iMask) {m_iVisibleBitMask=iMask;UpdateVisibleStatus();}
    void SetFarClipDistance(float fDistance);

    void GetDropToFloorHeights(Shape3D ^parentShape, array<DecorationInstanceShape ^>^ pData, Shape3D::DropToFloorMode mode, Vector3F axis, ShapeCollection ^colliderShapes);
    DecorationInstanceShape ^ GetBestTraceHit(Shape3D ^parentShape, array<DecorationInstanceShape ^>^ pData, Vector3F rayStart, Vector3F rayEnd);
    //void GetTransformation(hkvMat4 &transform);

    void UpdateLightgridColors();
    void SetRelativeLightGridSamplePos(float x, float y, float z)
    {
      _lgx = x; _lgy = y; _lgz = z;
      UpdateLightgridColors();
    }
    void SetAmbientColor(unsigned int iAmbColor)
    {
      m_iAmbientColor = iAmbColor;
      UpdateLightgridColors();
    }
    void SetUseLightgrid(bool bStatus)
    {
      m_bUseLightgrid = bStatus;
      UpdateLightgridColors();
    }
    void SetCollisionCapsule(float fRadius);
    void SetCastLightmapShadows(bool bStatus);

    void UpdateVisibleStatus();
    void RenderCollisionCapsules();

	  IntPtr GetGroupsObject() {return System::IntPtr((void*)m_pGroups);}

    // helpers:
    //static void InstancesFromRawData(ArrayList *pDest, int iVersion, String ^rawData);
    //static String* RawDataFromInstances(DecorationInstanceShape ^ pData[]);
    //static int GetCurrentRawDataVersion() {return 0;}
    static void RelaxPositions(array<DecorationInstanceShape ^>^ pData, array<Vector3F >^ pNewPos, int iIterations);

    float _x,_y,_z,_yaw,_pitch,_roll, m_fScaling, _lgx,_lgy,_lgz;
    array<DecorationInstanceShape ^>^ m_pInstances;
    array<ArrayList ^>^ m_pInstancesSortedByModel;

    VTerrainDecorationGroupCollection *m_pGroups;
    bool m_bVisible, m_bUseLightgrid, m_bUseCollisions;
    unsigned int m_iVisibleBitMask;
    unsigned int m_iAmbientColor;
    float m_fFarClip, m_fCollisionCapsuleRadius;
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
