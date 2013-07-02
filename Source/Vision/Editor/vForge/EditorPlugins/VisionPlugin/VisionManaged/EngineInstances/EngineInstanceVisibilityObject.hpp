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
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;


namespace VisionManaged
{
  public ref class EngineInstanceVisibilityObject : public IEngineInstanceObject3D
  {
  public:     

    EngineInstanceVisibilityObject();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override  {}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override  {return true;} // don't export; it will be serialized when linked

    //  IEngineInstanceObject3D
    VOVERRIDE void *GetObject3D() override  {return m_pVisObj;}

    // special functions

    void SetTestFlags(bool bFrustum, bool bPortal, bool bHOCQ, float fNearClip, float fFarClip, bool bActive);
    void SetContextBitmask(unsigned int iMask) {m_pVisObj->SetVisibleBitmask(iMask);}
    VisVisibilityObject_cl *GetVisibilityObject() {return m_pVisObj;}
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    void SetBoxDimensions(float dx, float dy, float dz) 
    {
      m_fBoxDimX = dx*0.5f;
      m_fBoxDimY = dy*0.5f;
      m_fBoxDimZ = dz*0.5f;
      UpdateBox();
    }

    void UpdateBox()
    {
      hkvAlignedBBox bbox (hkvVec3 (m_fCenterX-m_fBoxDimX, m_fCenterY-m_fBoxDimY, m_fCenterZ-m_fBoxDimZ),
                           hkvVec3 (m_fCenterX+m_fBoxDimX, m_fCenterY+m_fBoxDimY, m_fCenterZ+m_fBoxDimZ));

      m_pVisObj->SetWorldSpaceBoundingBox(bbox);
    }

  private:

    VisVisibilityObject_cl *m_pVisObj;
    float m_fCenterX,m_fCenterY,m_fCenterZ;
    float m_fBoxDimX,m_fBoxDimY,m_fBoxDimZ;
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
