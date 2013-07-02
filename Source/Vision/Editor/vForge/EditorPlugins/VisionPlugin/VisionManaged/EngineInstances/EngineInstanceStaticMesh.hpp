/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceVisibilityObject.hpp>

namespace VisionManaged
{
  public enum class CollisionBehavior_e
  {
    ForceNoCollision = 0,
    FromFile = 1,
    Custom = 2
  };

  public enum class WeldingType_e
  {
    None = VIS_WELDING_TYPE_NONE,
    AntiClockwise = VIS_WELDING_TYPE_ANTICLOCKWISE,
    Clockwise = VIS_WELDING_TYPE_CLOCKWISE,
    TwoSided = VIS_WELDING_TYPE_TWO_SIDED
  };

  public ref class LODEntry
  {
  public:
    LODEntry()
    {
      _filename = nullptr;
      _clipDistance = _usedClipDistance = 0.0f;
      _uniqueID = 0;
    }

    LODEntry^ Clone()
    {
      LODEntry ^copy = (LODEntry ^)this->MemberwiseClone();
      return copy;
    }

    // persistent data
    String^ _filename;
    float _clipDistance;

    // temporary data
    unsigned __int64 _uniqueID;
    float _usedClipDistance;
  };

  public ref class EngineInstanceStaticMesh : public IEngineShapeInstance
  {
  public:

    EngineInstanceStaticMesh();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override  {return System::IntPtr((void*)GetLOD0());}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetParentZone(Zone ^zone) override;
    VOVERRIDE void SetVisible(bool bStatus) override {_bVisible=bStatus;UpdateVisibleStatus();}
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override {_yaw=yaw;_pitch=pitch;_roll=roll;UpdateTransformation();}
    VOVERRIDE void SetPosition(float x,float y,float z) override {_x=x;_y=y;_z=z;UpdateTransformation();}
    VOVERRIDE void SetScaling(float x,float y, float z) override {_sx=x;_sy=y;_sz=z;UpdateTransformation();}
    VOVERRIDE void SetUniqueID(unsigned __int64 iID) override;
    VOVERRIDE void SetObjectKey(String ^key) override;
    VOVERRIDE void OnRecomputeVisibility() override;
    VOVERRIDE void GetDependencies(ResourceSnapshotInfo ^info) override;

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnPostEngineInstanceCreation() override {ConversionUtils::CallOnDeserializationFunction(GetLOD0());}
    VOVERRIDE void OnBeforeExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnAfterExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnRenderHook(ShapeBase ^owner, int iConstant) override;

    // components:
    VOVERRIDE property bool SupportsComponents
    {
      bool get() override
      {
        return true;
      }
    }

    VOVERRIDE bool CanAttachComponent(ShapeComponent ^component, String ^%sError) override {return ConversionUtils::CanAttachComponent(GetLOD0(),component,sError);}
    VOVERRIDE void OnAttachComponent(ShapeComponent ^component) override {ConversionUtils::OnAttachComponent(GetLOD0(),component);}
    VOVERRIDE void OnRemoveComponent(ShapeComponent ^component) override {ConversionUtils::OnRemoveComponent(GetLOD0(),component);}

    // special
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    bool SetMeshFile(String ^filename, String ^filename_lowres, unsigned __int64 iIDLowRes, bool bNotifyPhysics);
    void SetLODChain(List<LODEntry^> ^meshlist);
    void SetFarClipDistance(List<LODEntry^> ^meshlist, bool bFromFile);
    void SetCustomMaterialSet(String ^variantName);

    void DisposeAll();
    VisStaticMeshInstance_cl *GetLOD0() {return m_pFirstValidLOD;}

    void AssignGeometryToVisibiltyZone(VisibilityZoneShape ^pZone);
    void AssignVisibilityObject(EngineInstanceVisibilityObject ^pVisObj);

    void SetVisibleBitmask(unsigned int iBits) {_iVisibleBitmask=iBits;UpdateVisibleStatus();}
    void SetLightInfluenceBitmask(unsigned int iBits);
    void SetCollisionBitmask(unsigned int iBits);
    void SetCollisionGroup(CollisionBehavior_e eMode, short iLayer, short iSystemId, short iSystemDontCollideWith, int iSystemGroup);
    void SetWeldingType(WeldingType_e eWeldingType);
    inline unsigned int GetCollisionFilter() {return _iCollisionBitmask;}

    void SetCastDynamicShadows(bool bStatus);
    void SetCastStaticShadows(bool bStatus);
    void SetLightgridSamplePosition(float x, float y, float z);
    void SetLightmapMultiplier(float fValue);
    void SetLightmapOverride(String ^filename, bool bForceSet);
    
    void SendPhysicsOnStaticMeshCreatedEvent();

    void CreateTransformation(hkvMat4 &dest);
    void UpdateTransformation();
    void UpdateVisibleStatus();
    void UpdateLightGridPos();
    void TriggerTransformationUpdated();

    // external use
    int GetNumSubMeshes();
    void RebuildSubMeshes();

    bool HasLightmaps();
    void ReloadLightmapFile(String ^litFile);
    String ^ GetLightmapPage( int _iIdx, int _iPage );
    void GetLightmapScaleOffset( System::Single* _fX, System::Single* _fY, System::Single* _fZ, System::Single* _fW, int _iIdx );

  protected:
    // Derived classes can overload this to provide specialized versions of static mesh instance.
    // The default implementation returns NULL, which implies the default VisStaticMeshInstance_cl
    // class.
    virtual void* NewStaticMeshInstance() { return NULL; }

  public:
    //VisStaticMeshInstance_cl *m_pMeshInst, *m_pMeshInstLowRes;
    unsigned __int64 _uniqueID;
    float _x,_y,_z, _yaw,_pitch,_roll, _sx,_sy,_sz, _lgx,_lgy,_lgz;
    unsigned int _iVisibleBitmask, _iCollisionBitmask;
    bool _bVisible, _bNoExport, _bDragging;
    VisZoneResource_cl *m_pParentZone;
    int m_iLODCount;

    VisStaticMeshInstance_cl *m_pFirstValidLOD;
    VSmartPtr<VisStaticMeshInstance_cl> *m_spLODList; // array of smart pointers
    VisStaticGeometryInstanceCollection_cl *m_pSubMeshes;
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
