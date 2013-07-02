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
using namespace ManagedFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>

namespace VisionManaged
{
  public ref class EngineInstancePrefab: public IEngineInstanceObject3D
  {
  public:
	  EngineInstancePrefab(void);

    VOVERRIDE void DisposeObject() override;
    VOVERRIDE void *GetObject3D() override  {return m_instance;}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override {m_bVisible=bStatus;}
    VOVERRIDE void SetVisibleBitmask(unsigned int iMask) {m_iVisibleBitmask=iMask;}
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    /// \brief
    /// Sets the export flag for separate binary prefab storage
    ///
    /// \param exportInstance flag to indicate whether the prefab should be stored 
    ///                       separately (true) or not (false)
    ///
    void SetExportInstance(bool exportInstance);

    /// \brief
    /// Retrieves the flag indicating whether the prefab should be stored separately
    ///
    /// \return value indicating whether the prefab will be stored separately(true) 
    ///         or not (false)
    ///
    bool GetExportInstance();

    /// \brief
    /// Sets the filename of the prefab used to store the binary prefab
    ///
    /// \param ^filename string containing the filename of the prefab
    ///
    void SetFilename(String ^filename);

  private:
    /// \brief
    /// Instance of the engine core's prefab instance
    ///
    VPrefabInstance* m_instance;

    /// \brief
    /// Visibility flag
    ///
    bool m_bVisible;

    /// \brief 
    /// Visibility bitmask
    ///
    unsigned int m_iVisibleBitmask;

    /// \brief
    /// Export-flag indicating whether to export the binaty separately.
    ///
    bool m_bExport;
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
