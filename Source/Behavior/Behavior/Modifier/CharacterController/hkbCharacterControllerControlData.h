/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_CONTROLLER_CONTROL_DATA_H
#define HKB_CHARACTER_CONTROLLER_CONTROL_DATA_H

/// The controls that influence the behavior of the character controller and the relationship between the world from model and the character controller.
struct hkbCharacterControllerControlData
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbCharacterControllerControlData );

	hkbCharacterControllerControlData()
	:	
		m_initialVelocity(hkVector4::getZero()),
		m_verticalGain(1.0f),
		m_horizontalCatchUpGain(1.0f),
		m_maxVerticalSeparation(0.0f),
		m_maxHorizontalSeparation(0.0f),
		m_motionMode(0.0f),
		m_setInitialVelocity(0.0f),
		m_initialVelocityCoordinates(0.0f),
		m_forceDownwardMomentum(1.0f),
		m_applyGravity(1.0f)		
	{}

		/// This is the initial velocity that should be added, if we are going to add initial velocity.
	hkVector4 m_initialVelocity;

		/// The gain used to decide how to move the world-from-model transform towards the character controller in the vertical direction.
	hkReal m_verticalGain;	

		/// When going up stairs the character controller may lose velocity due to collisions with the steps. This gain is used to decide 
		/// how much of that loss should be taken into account while computing the character controller velocity.
	hkReal m_horizontalCatchUpGain;	

		/// The maximum distance between the world-from-model and the character controller in the vertical direction.
	hkReal m_maxVerticalSeparation; 

		/// The maximum distance between the actual character controller position due to loss in velocity and the expected character controller position in the horizontal direction.
	hkReal m_maxHorizontalSeparation;	

		/// If this is 0 it means that we want the character controller to follow the animation.  If this is 1 it means the character controller is
		/// dynamically controlled by physics.
	hkReal m_motionMode;

		/// If this is 1 it means that we need to add in some initial velocity on this frame. If this is 0, we don't need to.
	hkReal m_setInitialVelocity;

		/// If this is 0 the initial velocity is in world space.  If this is 1 it is in model space.
	hkReal m_initialVelocityCoordinates;

		/// If this is 1 then the motion in the animation has its vertical component removed before adding it to the character controller.  This allows 
		/// physics to dominate motion in the vertical direction.
	hkReal m_forceDownwardMomentum;

		/// If this is 1 then gravity is applied to the character controller.
	hkReal m_applyGravity;		
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
