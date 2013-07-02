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
using namespace VisionManaged;

class vHavokConstraint; // forward

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintStateBase.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/EngineInstances/EngineInstanceHavokConstraintBase.hpp>

namespace HavokManaged
{
  /// Managed C++ representation of Havok constraints. 
  /// Used for managed/native communication.
  public ref class EngineInstanceHavokConstraint : public EngineInstanceHavokConstraintBase
  {
  public:     
    EngineInstanceHavokConstraint();
    virtual void DisposeObject() override;
    virtual IntPtr GetNativeObject() override { return IntPtr(m_pConstraint); }

    /* Overridden IEngineShapeInstance functions */
    virtual bool OnExport(SceneExportInfo ^info) override;
    virtual void OnBeforeExport(SceneExportInfo ^info) override {if (m_pConstraint!=NULL) m_pConstraint->SetMarkInsideSerializationSession(true);}
    virtual void OnAfterExport(SceneExportInfo ^info) override {if (m_pConstraint!=NULL) m_pConstraint->SetMarkInsideSerializationSession(false);}

    /// Visualizes the constraint in the engine view.
    void VisualizeConstraintState(VisionViewBase ^view, ShapeRenderMode mode);

    /// Sets the constraint type and its properties
    void SetConstraintType(HavokConstraintType_e type, ConstraintStateBase ^constraintState);

    /// Updates the properties of the constraint.
    /// Please note that an update of the constraint properties won't recognize whether the new
    /// new configuration is valid - IsConfigurationValid() might thus return the wrong result.
    void SetConstraintProperties(ConstraintStateBase ^constraintState);


  protected:
    /// Ensures that the anchors are in correct order. I.e., if there is a world-space
    /// and an entity based anchor, the entity-based anchor must be the first one.
    void EnsureAnchorOrder();

    /// Removes the current native constraint instance and recreates it
    virtual void RecreateNativeObject() override;

    /// Updates the key of the native object from the cached data
    virtual void UpdateObjectKey() override;

    HavokConstraintType_e m_constraintType;  ///< Type of the constraint. The properties class depends on the type.
    ConstraintStateBase ^m_pConstraintState; ///< Constraint type-specific state object. Holds the properties of the constraint.

    vHavokConstraint *m_pConstraint;        ///< native constraint instance
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
