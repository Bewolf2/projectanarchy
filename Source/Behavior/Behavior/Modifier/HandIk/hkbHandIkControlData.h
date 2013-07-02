/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_HAND_IK_CONTROL_DATA_H
#define HKB_HAND_IK_CONTROL_DATA_H


extern const class hkClass hkbHandIkControlDataClass;

class hkbHandle;

/// The position and normal data that influence the behavior of an hkbHandIkModifier.
struct hkbHandIkControlData
{
	// +version(2)

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbHandIkControlData );
	HK_DECLARE_REFLECTION();

	hkbHandIkControlData()
		:	m_targetPosition(hkVector4::getZero()),
			m_targetRotation(hkQuaternion::getIdentity()),
			m_targetNormal(hkVector4::getZero()),
			m_targetHandle(HK_NULL),
			m_transformOnFraction(1.0f),
			m_normalOnFraction(0.0f),
			m_fadeInDuration(0.0f),
			m_fadeOutDuration(0.0f),
			m_extrapolationTimeStep(0.0f),
			m_handleChangeSpeed(1.0f),
			m_handleChangeMode(hkbHandIkControlData::HANDLE_CHANGE_MODE_ABRUPT),
			m_fixUp(false)
	{}

		/// The target hand position in world space.  This must be set as a homogeneous position with a valid W element (usually 1).  
		/// To indicate that there is no target position, you must set m_targetPosition to (0,0,0,0) and m_positionOnFraction to 0.
		/// This facilitates blending between valid and invalid target data.
	hkVector4 m_targetPosition;	//+default(0,0,0,0)
								//+hk.Description("The target hand position in world space.")

		/// The target hand rotation in world space.
	hkQuaternion m_targetRotation;	//+default(0,0,0,1)
									//+hk.Description("The target hand rotation in world space.")

		/// The target normal of the back of the hand in world space.  To indicate that there is no target normal, you must set
		/// m_targetNormal to (0,0,0) and m_normalOnFraction to 0.  This facilitates
		/// blending between valid and invalid target data.
	hkVector4 m_targetNormal;	//+default(0,0,0)
								//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
								//+hk.Description("The target normal of the back of the hand in world space.")

		/// The target handle. 
	hkbHandle* m_targetHandle;	//+default(HK_NULL)
								//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_CHARACTER_PROPERTY")
								//+hk.Description("The target handle.")

		/// By what fraction to transform the hand toward the target.
	hkReal m_transformOnFraction;	//+default(1)
									//+hk.Description("By what fraction to transform the hand toward the target.")
									//+hk.RangeReal(absmin=0.0,absmax=1.0)

		/// By what fraction to rotate the hand normal toward the target.
	hkReal m_normalOnFraction;	//+default(0)
								//+hk.Description("By what fraction to rotate the hand normal toward the target.")
								//+hk.RangeReal(absmin=0.0,absmax=1.0)

		/// Duration in seconds to fade in the IK when it is turned on (see hkbHandIkModifier).
	hkReal m_fadeInDuration;	//+default(0)
								//+hk.Description("Duration in seconds to fade in the IK when it is turned off.")
								//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10.0)

		/// Duration in seconds to fade out the IK when it is turned off (see hkbHandIkModifier).
	hkReal m_fadeOutDuration;	//+default(0)
								//+hk.Description("Duration in seconds to fade out the IK when it is turned off.")
								//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10.0)

		/// If the handle is on a rigid body then while evaluating the handle we can extrapolate it
		/// to get its future position.
	hkReal m_extrapolationTimeStep; //+default(0.0f)
									//+hk.Description("If the handle is on a rigid body then while evaluating the handle we can extrapolate it to get its future position.")

		/// If the handle has changed then how fast would you like to go from the old handle's position
		/// and rotation towards the new handle.
	hkReal m_handleChangeSpeed; //+default(1.0f)
								//+hk.Description("If the handles have changed then how fast would you like to go from the old handles position and rotation towards the new handle.")

	enum HandleChangeMode
	{
			/// Switch to the new handle immediately 
		HANDLE_CHANGE_MODE_ABRUPT = 0,

			/// Move to the new handle with constant velocity
		HANDLE_CHANGE_MODE_CONSTANT_VELOCITY,
	};

		/// What do you want to do when the handle changes.
	hkEnum<HandleChangeMode, hkInt8> m_handleChangeMode;	//+default(hkbHandIkControlData::HANDLE_CHANGE_MODE_ABRUPT)
															//+hk.Description("What do you want to do when the handle changes.")

		/// Whether we want to do handIk fixup.
	hkBool m_fixUp; //+default(false)
					//+hk.Description("Whether we want to do handIk fixup.")

	hkbHandIkControlData( hkFinishLoadedObjectFlag flag ) {}
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
