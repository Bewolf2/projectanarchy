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
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::PropertyEditors;
using namespace CSharpFramework::Shapes;
using namespace ManagedFramework;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>

namespace VisionManaged
{

  [Flags]
  public enum class TriggerBoxObserverFlags_e
  {
    [NonEditableFlagAttribute]
    None        = TRIGGERBOXOBSERVERFLAGS_NONE,

    MainCamera  = TRIGGERBOXOBSERVERFLAGS_MAINCAMERA,
    ObservedEntities  = TRIGGERBOXOBSERVERFLAGS_ENTITYLIST,
    AllEntities       = TRIGGERBOXOBSERVERFLAGS_ALL_ENTITIES,
  };

  public enum class TriggerShape_e
  {
    Box = 0,
    Sphere = 1
  };

  public ref class EngineInstanceTriggerBox : public IEngineInstanceObject3D
  {
  public:     

    EngineInstanceTriggerBox();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override  {}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    //  IEngineInstanceObject3D
    VOVERRIDE void *GetObject3D() override {return GetTriggerEntity();}

    // special functions
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode, unsigned int iColor);
    TriggerBoxEntity_cl *GetTriggerEntity() {return m_pEntityWP ? ((TriggerBoxEntity_cl *)m_pEntityWP->GetPtr()) : NULL;}
    void SetBoxDimensions(TriggerShape_e eType, float x, float y, float z) {m_eShapeType=eType;_bx=x;_by=y;_bz=z;UpdateAbsBox();}
    void UpdateAbsBox();

    void SetObserverFlags(TriggerBoxObserverFlags_e flags) {if (GetTriggerEntity()) GetTriggerEntity()->SetObserverFlags((int)flags);}
    void SetDestroyOnTrigger(bool bState) {if (GetTriggerEntity()) GetTriggerEntity()->m_bDestroyOnTrigger=bState;}

    void SetEnabled(bool bEnabled) {
      if(GetTriggerEntity())
        GetTriggerEntity()->SetEnabled(bEnabled);
    }

  protected:
    float _x,_y,_z;
    float _bx,_by,_bz;
    TriggerBoxEntity_cl *m_pEntity;
    VWeakPtr<VisBaseEntity_cl> *m_pEntityWP;
    TriggerShape_e m_eShapeType;
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
