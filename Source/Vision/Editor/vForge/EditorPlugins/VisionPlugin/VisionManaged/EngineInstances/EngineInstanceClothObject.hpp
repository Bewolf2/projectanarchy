/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/_ClothEntity.hpp>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceParticleGroup.hpp>

namespace VisionManaged
{
  public ref class EngineInstanceClothObject : public IEngineInstanceObject3D
  {
  public:     

    EngineInstanceClothObject();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override {m_bVisible=bStatus;UpdateVisibleBitmask();}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float x,float y, float z) override;

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    //  IEngineInstanceObject3D
    VOVERRIDE void *GetObject3D() override {return GetClothEntity();}

    // constraint functions:
    void AddConstraint(EngineInstanceConstraint ^pConstraint);
    void RemoveConstraint(EngineInstanceConstraint ^pConstraint);

    // special functions
    bool SetModelFile(String ^modelfile, Vector3F scaling, String ^%sError);
    void SetPhysicsProperties(int iTicks, bool bWhenVisible, float fGravity, int iInitialCount);
    //void SetEntityKey(String ^key);

    void RunInitialPhysicsTicks();

    void SetLightgridSamplePosition(float x, float y, float z) 
    {
      ClothEntity_cl *pEnt = GetClothEntity();
      if (!pEnt) return;
      hkvVec3 vLGPos(x,y,z);
      //vLGPos += pEnt->GetClothPosition(); // cloth specific

      pEnt->SetRelativeLightGridSamplePos(vLGPos);
    }
    inline void SetCastShadows(bool bStatus)
    {
      if (GetClothEntity())
        GetClothEntity()->SetCastShadows(bStatus ? TRUE:FALSE);
    }
    void SetVisibleBitmask(unsigned int iMask) {m_iVisibleBitmask=iMask;UpdateVisibleBitmask();}
    void SetLightInfluenceBitmask(unsigned int iMask) {if (GetClothEntity()) GetClothEntity()->SetLightInfluenceBitMask(iMask);}
    void UpdateVisibleBitmask() {if (GetClothEntity()) GetClothEntity()->SetVisibleBitmask(m_bVisible ? m_iVisibleBitmask:0);}

    // access entity
    inline ClothEntity_cl *GetClothEntity()
    {
      Debug::Assert( m_pEntityWP != nullptr );
      return (ClothEntity_cl *) m_pEntityWP->GetPtr();
    }

    inline VClothMesh* GetClothMesh()
    {
      ClothEntity_cl *pEnt = GetClothEntity();
      if (!pEnt) return NULL;
      return pEnt->GetClothMesh();
    }

  private:
    bool m_bVisible;
    unsigned int m_iVisibleBitmask;
    VWeakPtr<VisBaseEntity_cl> *m_pEntityWP;
    float m_fScaleX,m_fScaleY,m_fScaleZ;
    float m_fYaw,m_fPitch,m_fRoll;
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
