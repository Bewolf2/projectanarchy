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
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;


namespace VisionManaged
{
  public ref class EngineInstanceStaticMeshGroup : public IEngineShapeInstance
  {
  public:

    EngineInstanceStaticMeshGroup();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override  {return System::IntPtr((void*)m_pMeshInst);}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetParentZone(Zone ^zone) override;
    VOVERRIDE void SetVisible(bool bStatus) override  {}
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override  {}
    VOVERRIDE void SetPosition(float x,float y,float z) override  {_x=x;_y=y;_z=z;}
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}
    VOVERRIDE void SetUniqueID(unsigned __int64 iID) override  {_uniqueID=iID;}
    VOVERRIDE void SetObjectKey(String ^key) override;
    VOVERRIDE void OnRecomputeVisibility() override;
    VOVERRIDE void RenderShape(VisionViewBase ^view, ShapeRenderMode mode, BoundingBox ^pAbsBox);
    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE void GetDependencies(ResourceSnapshotInfo ^info) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnPostEngineInstanceCreation() override {ConversionUtils::CallOnDeserializationFunction(m_pMeshInst);}
    VOVERRIDE void OnBeforeExport(SceneExportInfo ^info) override {if (m_pMeshInst!=NULL) m_pMeshInst->SetMarkInsideSerializationSession(true);}
    VOVERRIDE void OnAfterExport(SceneExportInfo ^info) override {if (m_pMeshInst!=NULL) m_pMeshInst->SetMarkInsideSerializationSession(false);}

    VOVERRIDE void BeginExport(ShapeCollection ^meshShapes, String ^relFilename);
    VOVERRIDE void EndExport();
    
  public: 
    // set directly before exporting
    float fCombinedFarClipDist;
    unsigned int iCombinedVisibleBitmask, iCombinedCollisionBitmask;
    bool bCombinedStaticShadows;
    bool bCombinedDynamicShadows;

  private:
    Zone ^pParentZone;
    ShapeCollection ^relevantShapes;
    VisStaticMeshInstance_cl *m_pMeshInst;
    float _x,_y,_z;
    unsigned __int64 _uniqueID;
    String ^_objectKey;
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
