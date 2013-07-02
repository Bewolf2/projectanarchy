/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_RIGID_BODY_RAGDOLL_CONTROL_DATA_H
#define HKB_RIGID_BODY_RAGDOLL_CONTROL_DATA_H

extern const class hkClass hkbKeyFrameControlDataClass;

extern const class hkClass hkbRigidBodyRagdollControlDataClass;

	/// This struct stores data related to keyframing ragdoll rigid bodies.
	/// See controller implementations for more information on how this data is used (Eg. hkbpRagdollRigidBodyController, hkbnpRagdollRigidBodyController).
struct hkbKeyFrameControlData
{
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbKeyFrameControlData );

		/// This parameter blends the desired target for a bone between model space (0.0) or local space (1.0).
		/// Usually the controller will be much stiffer and more stable when driving to model space.
		/// However local space targeting can look more natural.
		/// It is similar to the deprecated bone controller hierarchyGain parameter
	hkReal m_hierarchyGain;	//+default(0.17f)
							//+hk.RangeReal(absmin=0.0,absmax=1.0)
							//+hk.Description("This parameter blends the desired target for a bone between model space: zero, or local space: one. Usually the controller will be much stiffer and more stable when driving to model space. However local space targeting can look more natural.")

		/// This gain dampens the velocities of the bodies. The current velocity of the body is
		/// scaled by this parameter on every frame before the controller is applied. It is
		/// applied every step and is generally more aggressive than standard linear or angular damping.
		/// A value of 0 means no damping.
	hkReal m_velocityDamping;	//+default(0.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("This gain dampens the velocities of the bodies. The current velocity of the body is scaled by this parameter on every frame before the controller is applied. It is applied every step and is generally more aggressive than standard linear or angular damping. A value of 0 means no damping.")

		/// This gain controls the proportion of the difference in acceleration that is
		/// applied to the bodies. It dampens the effects of the velocity control.
	hkReal m_accelerationGain;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("This gain controls the proportion of the difference in acceleration that is applied to the bodies. It dampens the effects of the velocity control.")

		/// This gain controls the proportion of the difference in velocity that is
		/// applied to the bodies. It dampens the effects of the position control.
	hkReal m_velocityGain;	//+default(0.6f)
							//+hk.RangeReal(absmin=0.0,absmax=1.0)
							//+hk.Description("This gain controls the proportion of the difference in velocity that is applied to the bodies. It dampens the effects of the position control.")

		/// This gain controls the proportion of the difference in position that is
		/// applied to the bodies. It has the most immediate effect. High gain
		/// values make the controller very stiff. Once the controller is too stiff
		/// it will tend to overshoot. The velocity gain can help control this.
	hkReal m_positionGain;	//+default(0.05f)
							//+hk.RangeReal(absmin=0.0,absmax=1.0)	
							//+hk.Description("This gain controls the proportion of the difference in position that is applied to the bodies. It has the most immediate effect. High gain values make the controller very stiff. Once the controller is too stiff it will tend to overshoot. The velocity gain can help control this.")

		/// The position difference is scaled by the inverse delta time to compute a
		/// velocity to be applied to the rigid body. The velocity is first clamped to this
		/// limit before it is applied.
	hkReal m_positionMaxLinearVelocity;	//+default(1.4f)
										//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)
										//+hk.Description("The position difference is scaled by the inverse delta time to compute a velocity to be applied to the rigid body. The velocity is first clamped to this limit before it is applied.")

		/// The orientation difference is scaled by the inverse delta time to compute an angular
		/// velocity to be applied to the rigid body. The velocity is first clamped to this
		/// limit before it is applied.
	hkReal m_positionMaxAngularVelocity;	//+default(1.8f)
											//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)
											//+hk.Description("The orientation difference is scaled by the inverse delta time to compute an angular velocity to be applied to the rigid body. The velocity is first clamped to this limit before it is applied.")

		/// This gain allows for precise matching between keyframes and the current position.
		/// It works like the m_positionGain: it calculates an optimal
		/// \code deltaVelocity = (keyFramePosition - currentPosition) / deltaTime \endcode
		/// scales it by m_snapGain, clips it against m_snapMaxXXXVelocity and scales it down
		/// \code if (keyFramePosition - currentPosition) > m_snapMaxXXXXDistance \endcode
	hkReal m_snapGain;	//+default(0.1f)
						//+hk.RangeReal(absmin=0.0,absmax=1.0) 
						//+hk.Description("This gain allows for precise matching between keyframes and the current position. It works like the m_positionGain: it calculates an optimal deltaVelocity, scales it by m_snapGain, clips it against m_snapMaxXXXVelocity and scales it down if the implied positional difference is larger than m_snapMaxXXXXDistance.")

		/// See m_snapGain. The linear velocity calculated from the snapGain is clamped to this
		/// limit before being applied.
	hkReal m_snapMaxLinearVelocity;	//+default(0.3f)
									//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)
									//+hk.Description("See m_snapGain. The linear velocity calculated from the snapGain is clamped to this limit before being applied.")

		/// See m_snapGain. The angular velocity calculated from the snapGain is clamped to this
		/// limit before being applied.
	hkReal m_snapMaxAngularVelocity;	//+default(0.3f)
										//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)
										//+hk.Description("See m_snapGain. The angular velocity calculated from the snapGain is clamped to this limit before being applied.")

		/// This sets the max linear distance for the snap gain to work at full strength.
		/// The strength of the controller peaks at this distance.
		/// If the current distance is bigger than m_snapMaxLinearDistance, the snap velocity will be scaled
		/// by sqrt( maxDistane/currentDistance ).
	hkReal m_snapMaxLinearDistance;	//+default(0.03f)
									//+hk.RangeReal(absmin=0.0,absmax=10.0,softmax=1.0)
									//+hk.Description("This sets the max linear distance for the snap gain to work at full strength. The strength of the controller peaks at this distance.")

		/// This sets the max angular distance for the snap gain to work at full strength.
		/// The strength of the controller peaks at this distance.
		/// If the current distance is bigger than m_snapMaxAngularDistance, the snap velocity will be scaled
		/// by sqrt( maxDistane/currentDistance ).
	hkReal m_snapMaxAngularDistance;	//+default(0.1f)
										//+hk.RangeReal(absmin=0.0,absmax=10.0,softmax=1.0)
										//+hk.Description("This sets the max angular distance for the snap gain to work at full strength. The strength of the controller peaks at this distance.")

		// These useful defaults are appropriate in most circumstances
	hkbKeyFrameControlData() :
		m_hierarchyGain(0.17f),
		m_velocityDamping(0.0f),
		m_accelerationGain(1.0f),
		m_velocityGain(0.6f),
		m_positionGain(0.05f),
		m_positionMaxLinearVelocity(1.4f),
		m_positionMaxAngularVelocity(1.8f),
		m_snapGain(0.1f),
		m_snapMaxLinearVelocity(0.3f),
		m_snapMaxAngularVelocity(0.3f),
		m_snapMaxLinearDistance(0.03f),
		m_snapMaxAngularDistance(0.1f) {}

	hkbKeyFrameControlData( hkFinishLoadedObjectFlag flag ) {}

};

	/// The controls that influence the behavior of an hkbRigidBodyRagdollModifier.
struct hkbRigidBodyRagdollControlData
{
	// +version(2)

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbRigidBodyRagdollControlData );
	HK_DECLARE_REFLECTION();

	hkbRigidBodyRagdollControlData()
	:	m_durationToBlend(0.0f)	{}

		/// This data governs how the rigid bodies should be keyframed
	HK_ALIGN16( struct hkbKeyFrameControlData m_keyFrameControlData ); //+hk.Description("The control parameters that influence the Rigid Body Ragdoll Modifier.") 

		/// DEPRECATED (HKF-1433). Use hkbBlendingTransitionEffect instead.
		/// The length of time to blend in the current ragdoll pose
		/// when transitioning between powered ragdoll and rigid body 
		/// ragdoll.
		///
		/// When going from powered ragdoll to rigid body ragdoll,
		/// there is often a visual discontinuity.  We smooth it
		/// out by blending the current ragdoll pose with the
		/// pose being driven toward.  See hkbRagdollDriverModifier.
	hkReal m_durationToBlend;	//+default(0.0f)
								//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
								//+hk.RangeReal(absmin=0.0,softmax=10.0,absmax=100.0)
								//+hk.Description("Deprecated. Use hkbBlendingTransitionEffect instead. The length of time to blend in the current ragdoll pose when transitioning between powered ragdoll and rigid body ragdoll. When going from powered ragdoll to rigid body ragdoll, there is often a visual discontinuity.  We smooth it out by blending the current ragdoll pose with the pose being driven toward.")

public:

	hkbRigidBodyRagdollControlData( hkFinishLoadedObjectFlag flag ) : m_keyFrameControlData(flag) {}
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
