/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/StiffSpringConstraintChainState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStiffSpringConstraintChain.hpp>

#using <mscorlib.dll>

using namespace CSharpFramework::Serialization;
using namespace ManagedFramework;


namespace HavokManaged
{
  StiffSpringConstraintChainState::StiffSpringConstraintChainState()
  {
    SetToDefault();
  }

  StiffSpringConstraintChainState::StiffSpringConstraintChainState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintChainStateBase(si, sc)
  {
    if (SerializationHelper::HasElement(si, "m_fLinkPivotOffset"))
      m_fLinkPivotOffset = si->GetSingle("m_fLinkPivotOffset");
    else
      m_fLinkPivotOffset = s_fLinkPivotOffsetDefault;
  }

  vHavokConstraintChainDesc *StiffSpringConstraintChainState::CreateNativeDesc()
  {
    return new vHavokStiffSpringConstraintChainDesc();
  }

  void StiffSpringConstraintChainState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintChainStateBase::GetObjectData(si, sc);
    si->AddValue("m_fLinkPivotOffset", m_fLinkPivotOffset);
  }

  void StiffSpringConstraintChainState::ManagedToNative(vHavokConstraintChainDesc &nativeDesc)
  {
    ConstraintChainStateBase::ManagedToNative(nativeDesc);
    vHavokStiffSpringConstraintChainDesc& ssDesc =
      static_cast<vHavokStiffSpringConstraintChainDesc&>(nativeDesc);
    ssDesc.m_fLinkPivotOffset = m_fLinkPivotOffset;
  }

  void StiffSpringConstraintChainState::NativeToManaged(const vHavokConstraintChainDesc &nativeDesc)
  {
    ConstraintChainStateBase::NativeToManaged(nativeDesc);
    vHavokStiffSpringConstraintChainDesc const& ssDesc =
      static_cast<vHavokStiffSpringConstraintChainDesc const&>(nativeDesc);
    m_fLinkPivotOffset = ssDesc.m_fLinkPivotOffset;
  }
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
