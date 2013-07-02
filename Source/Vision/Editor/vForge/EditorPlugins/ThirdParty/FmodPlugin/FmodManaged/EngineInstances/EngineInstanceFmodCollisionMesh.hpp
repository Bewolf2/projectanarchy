/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/EngineInstances/EngineInstanceFmodIncludes.hpp>

namespace FmodManaged
{

  /// <summary>
  /// EngineInstanceFmodCollisionMesh : class that manages the native VFmodCollisionMeshInstance class
  /// </summary>
  public ref class EngineInstanceFmodCollisionMesh : public IEngineInstanceObject3D
  {
  public:     

    EngineInstanceFmodCollisionMesh();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override;
    VOVERRIDE void SetPosition(float x, float y, float z) override;
    VOVERRIDE void SetOrientation(float yaw, float pitch, float roll) override;
    VOVERRIDE void SetScaling(float x, float y, float z) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart, Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
    VOVERRIDE void *GetObject3D() override {return m_pMeshInstance;}

    // own functions 
    void UpdateVisibleStatus();
    void SetCollisionMesh(String ^filename, float fDirectOcclusion, float fReverbOcclusion);
    void SetSelected(bool bStatus);

    void SetOcclusion(float fDirectOcclusion, float fReverbOcclusion);

  private:
    bool m_bIsVisible, m_bSelected;
    VisBaseEntity_cl *m_pEntity;
    VisStaticMeshInstance_cl *m_pStaticMeshInst;
    VFmodCollisionMeshInstance *m_pMeshInstance;
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
