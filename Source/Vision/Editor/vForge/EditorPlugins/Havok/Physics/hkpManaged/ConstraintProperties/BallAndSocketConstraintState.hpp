/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintStateBase.hpp>

using namespace System;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;

using namespace ManagedFramework;

using namespace CSharpFramework::UndoRedo;
using namespace CSharpFramework;

namespace HavokManaged
{
  /// \brief Managed property class for ball-and-socket constraints.
  /// 
  /// The BallAndSocketConstraintState class exposes the properties for ball-and-socket
  /// constraints and provides functions for setting/retrieving them on the 
  /// native description.
  [Serializable]
  public ref class BallAndSocketConstraintState : public ConstraintStateBase
  {
  public:
    /// Constructor. Sets the default values.
    BallAndSocketConstraintState();

    /// Visualizes the joint in the engine view
    virtual void Visualize (IVRenderInterface *pRenderer, const hkvVec3& startPos, const hkvVec3& endPos, const hkvVec3& constraintPos, const hkvVec3& constraintDir, const hkvVec3& constraintOri) override;

    // Deserialisation method
    virtual void GetObjectData(SerializationInfo ^si, StreamingContext sc) override;

    /// ICloneable implementation
    virtual Object^ Clone() override;

  protected:
    // Serialisation constructor
    BallAndSocketConstraintState(SerializationInfo ^si, StreamingContext sc);

    /// Creates a new native description instance (initialized to defaults)
    virtual vHavokConstraintDesc *CreateNativeDesc() override;

    /// Copies the properties from the passed native constraint description into this
    /// managed description.
    virtual void NativeToManaged(const vHavokConstraintDesc &nativeDesc) override;
    
    /// Copies the properties from this managed joint description to the passed native 
    /// description.
    virtual void ManagedToNative(vHavokConstraintDesc &nativeDesc) override;

    /// Sets anchor specific information and position-related information in the passed 
    /// native description.
    /// This function is called once the native description has been created and the managed
    /// configuration has been set on the native one. It can be used to perform final 
    /// initialisations on the native descriptions.
    virtual void SetAnchorDataOnNative(vHavokConstraintDesc &nativeDesc, 
      AnchorDescription ^pFirstAnchor, AnchorDescription ^pSecondAnchor, 
      Vector3F &position, Vector3F &direction, Vector3F &orientation) override;
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
