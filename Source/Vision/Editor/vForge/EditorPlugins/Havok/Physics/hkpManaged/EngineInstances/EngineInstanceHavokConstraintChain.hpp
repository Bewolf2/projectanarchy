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

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintChainStateBase.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/EngineInstances/EngineInstanceHavokConstraintBase.hpp>

namespace HavokManaged
{
  /// Managed C++ representation of Havok constraint chains.
  /// Used for managed/native communication.
  public ref class EngineInstanceHavokConstraintChain : public EngineInstanceHavokConstraintBase
  {
  public:
    /// @name Construction/Destruction
    //@{
    EngineInstanceHavokConstraintChain(ShapeBase ^ownerShape);
    virtual void DisposeObject() override;
    //@}

    /// @name Base Class Overrides
    //@{
    property bool SupportsComponents
    {
      virtual bool get() override { return true; }
    }

    virtual bool CanAttachComponent(ShapeComponent ^component, String ^%sErrorMsgOut) override;
    virtual void OnAttachComponent(ShapeComponent ^component) override;
    virtual void OnRemoveComponent(ShapeComponent ^component) override;

    virtual IntPtr GetNativeObject() override { return IntPtr(m_pConstraintChain); }
    virtual bool OnExport(SceneExportInfo ^info) override;
    virtual void OnBeforeExport(SceneExportInfo ^info) override {if (m_pConstraintChain!=NULL) m_pConstraintChain->SetMarkInsideSerializationSession(true);}
    virtual void OnAfterExport(SceneExportInfo ^info) override {if (m_pConstraintChain!=NULL) m_pConstraintChain->SetMarkInsideSerializationSession(false);}
    //@}

    /// Sets the constraint chain type and its properties
    void SetConstraintChainType(HavokConstraintChainType_e type, 
      ConstraintChainStateBase ^constraintChainState);

    /// Updates the properties of the constraint chain.
    /// Please note that an update of the constraint chain properties won't recognize whether the new
    /// new configuration is valid - IsConfigurationValid() might thus return the wrong result.
    void SetConstraintChainProperties(ConstraintChainStateBase ^constraintChainState);


    /// @name Chain Quality
    //@{

    /// Returns the number of links the chain currently has.
    unsigned int GetNumLinks();

    /// Returns the average error of the approximation of the path with the chain 
    /// links. If the path runs straight or there is no path defined, this error is
    /// always 0.
    float GetApproximationError();

    //@}

  protected:
    /// Creates the appropriate instance of the native Havok constraint chain
    vHavokConstraintChain *CreateConstraintChainInstance();

    /// Removes the current native constraint chain instance and recreates it
    virtual void RecreateNativeObject() override;

    /// Updates the key of the native object from the cached data
    virtual void UpdateObjectKey() override;

  protected:
    HavokConstraintChainType_e m_eType;  ///< Type of the constraint. The properties class depends on the type.
    ConstraintChainStateBase ^m_pState; ///< Constraint type-specific state object. Holds the properties of the constraint.
    vHavokConstraintChain *m_pConstraintChain;        ///< native constraint instance

    WeakReference ^m_pOwnerShapeRef;  ///< Weak reference to the owner shape of this engine instance. Used for updating components after recreation of the native object.
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
