/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_DOCKING_CONTROL_DATA_H
#define HKB_DOCKING_CONTROL_DATA_H

struct hkbDockingControlData
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbDockingControlData );

	hkbDockingControlData()
	:	m_dockingBlend(0.0f),
		m_previousDockingBlend(0.0f),
		m_timeOffset(0.0f),
		m_dockingBone(-1),
		m_modelSpaceTarget()
	{
		m_modelSpaceTarget.setIdentity();
	}


	hkReal m_dockingBlend;
	hkReal m_previousDockingBlend;
	hkReal m_timeOffset;
	hkReal m_dockingBone;
	hkQsTransform m_modelSpaceTarget;
};

#endif

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
