/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Physics2012/Dynamics/World/hkpWorld.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintListener.hpp>

void vHavokConstraintBreakListenerProxy::constraintBreakingCallback(const hkpConstraintBrokenEvent &event)
{
	m_owner->constraintBreakingCallback(event);
}


// --------------------------------------------------------------------------
vHavokConstraintListener::vHavokConstraintListener(hkpWorld* physicsWorld)
{
  physicsWorld->markForWrite();
  m_listener.setOwner(this);
  physicsWorld->addConstraintListener(&m_listener);
  physicsWorld->unmarkForWrite();
}

// --------------------------------------------------------------------------
vHavokConstraintListener::~vHavokConstraintListener()
{
}

// --------------------------------------------------------------------------
void vHavokConstraintListener::constraintBreakingCallback(const hkpConstraintBrokenEvent &event)
{
  vHavokConstraint *pConstraint = 
    reinterpret_cast<vHavokConstraint*>(event.m_constraintInstance->getUserData());
  if (!pConstraint)
    return;

  vHavokConstraintBreakInfo_t breakInfo;
  breakInfo.m_pConstraint = pConstraint;
  breakInfo.m_fImpulse = event.m_actualImpulse;

  pConstraint->OnBreak(&breakInfo);
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
