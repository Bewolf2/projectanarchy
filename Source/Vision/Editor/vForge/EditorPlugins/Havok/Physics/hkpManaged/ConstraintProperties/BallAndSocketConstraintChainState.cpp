/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/BallAndSocketConstraintChainState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokBallAndSocketConstraintChain.hpp>

#using <mscorlib.dll>

using namespace CSharpFramework::Serialization;

using namespace ManagedFramework;


namespace HavokManaged
{
  BallAndSocketConstraintChainState::BallAndSocketConstraintChainState()
  {
    SetToDefault();
  }

  BallAndSocketConstraintChainState::BallAndSocketConstraintChainState(SerializationInfo ^si, StreamingContext sc) 
    : ConstraintChainStateBase(si, sc)
  {
    if (SerializationHelper::HasElement(si, "m_fMaximumLastLinkGap"))
      m_fMaximumLastLinkGap = si->GetSingle("m_fMaximumLastLinkGap");
    else
      m_fMaximumLastLinkGap = s_fMaximumLastLinkGapDefault;
  }

  vHavokConstraintChainDesc *BallAndSocketConstraintChainState::CreateNativeDesc()
  {
    return new vHavokBallAndSocketConstraintChainDesc();
  }

  void BallAndSocketConstraintChainState::GetObjectData(SerializationInfo ^si, StreamingContext sc)
  {
    ConstraintChainStateBase::GetObjectData(si, sc);
    
    si->AddValue("m_fMaximumLastLinkGap", m_fMaximumLastLinkGap);
  }

  void BallAndSocketConstraintChainState::ManagedToNative(vHavokConstraintChainDesc &nativeDesc)
  {
    ConstraintChainStateBase::ManagedToNative(nativeDesc);
    vHavokBallAndSocketConstraintChainDesc& basDesc =
      static_cast<vHavokBallAndSocketConstraintChainDesc&>(nativeDesc);
    basDesc.m_fMaximumLastLinkGap = m_fMaximumLastLinkGap;
  }

  void BallAndSocketConstraintChainState::NativeToManaged(const vHavokConstraintChainDesc &nativeDesc)
  {
    ConstraintChainStateBase::NativeToManaged(nativeDesc);
    vHavokBallAndSocketConstraintChainDesc const& basDesc =
      static_cast<vHavokBallAndSocketConstraintChainDesc const&>(nativeDesc);
    m_fMaximumLastLinkGap = basDesc.m_fMaximumLastLinkGap;
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
