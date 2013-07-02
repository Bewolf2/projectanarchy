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
using namespace CSharpFramework::DynamicProperties;
using namespace CSharpFramework::Scene;



//#include <Postprocessing/VPostProcessingManager.hpp>

namespace VisionManaged
{
  public ref class EngineInstancePostProcessingEffect : public IEngineShapeInstance
  {
  public:
    EngineInstancePostProcessingEffect();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override  {return System::IntPtr((void*)NULL);}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override  {}
    VOVERRIDE void SetPosition(float x,float y,float z) override  {}
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override  {}
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override  {}
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override  {return false;}
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    // special:
    bool SetClassName(String ^className);
    void SetKey(String ^sKey);

    //DynamicPropertyCollectionType ^ GetClassPropertyType(DynamicPropertyManager ^manager);
    static DynamicPropertyCollectionType ^ GetClassPropertyType(DynamicPropertyManager ^manager, String ^className);
    void SetVariable(String ^varName, String ^varValue);
    void SetAllVariables(DynamicPropertyCollection ^pProperties);

    void Activate();
    void Deactivate();
    bool IsActivated() {return m_bActivated;}

    static void DeactivateAll();
  private:

    //PostProcessingEffect *m_pEffect;
    bool m_bActivated;
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
