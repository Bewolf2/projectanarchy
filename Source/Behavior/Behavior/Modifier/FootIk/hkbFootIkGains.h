/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FOOT_IK_GAINS
#define HKB_FOOT_IK_GAINS

#include <Common/Base/Reflection/Attributes/hkAttributes.h>

extern const class hkClass hkbFootIkGainsClass;

/// These gains control how much smoothing occurs in the foot IK process.
struct hkbFootIkGains
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbFootIkGains );
	HK_DECLARE_REFLECTION();

	hkbFootIkGains()
	:	m_onOffGain(0.2f),
		m_groundAscendingGain(1.0f),
		m_groundDescendingGain(1.0f),
		m_footPlantedGain(1.0f),
		m_footRaisedGain(1.0f),
		m_footUnlockGain(1.0f),
		m_worldFromModelFeedbackGain(0.0f),
		m_errorUpDownBias(1.0f),
		m_alignWorldFromModelGain(0.0f),
		m_hipOrientationGain(0.0f)
	{}

		/// Gain used when transitioning from foot placement on and off. Default value is 0.2.
	hkReal m_onOffGain; //+default(0.2f)
						//+hk.RangeReal(absmin=0.0,absmax=1.0)
						//+hk.Description("Gain used when foot IK is turned on or off.")

		/// Gain used when the ground height is increasing (going up). Default value is 1.0.
	hkReal m_groundAscendingGain;	//+default(1.0f)
									//+hk.RangeReal(absmin=0.0,absmax=1.0)
									//+hk.Description("Gain used when the ground height is increasing." )

		/// Gain used when the ground height is decreasing (going down). Default value is 1.0.
	hkReal m_groundDescendingGain;	//+default(1.0f)
									//+hk.RangeReal(absmin=0.0,absmax=1.0)
									//+hk.Description("Gain used when the ground height is decreasing." )

		/// Gain used when the foot is fully planted. Depending on the height of the ankle,
		/// a value is interpolated between m_footPlantedGain and m_footRaisedGain and then 
		/// multiplied by the ascending/descending gain to give
		/// the final gain used.  Default value is 1.0.
	hkReal m_footPlantedGain;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("Gain used when the foot is fully planted. Depending on the height of the ankle, a value is interpolated between m_footPlantedGain and m_footRaisedGain and then multiplied by the ascending/descending gain to give the final gain used." )

		/// Gain used when the foot is fully raised. Depending on the height of the ankle,
		/// a value is interpolated between m_footPlantedGain and m_footRaisedGain and then 
		/// multiplied by the ascending/descending gain to give
		/// the final gain used.  Default value is 1.0.
	hkReal m_footRaisedGain;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("Gain used when the foot is fully raised. Depending on the height of the ankle, a value is interpolated between m_footPlantedGain and m_footRaisedGain and then multiplied by the ascending/descending gain to give the final gain used." )

		/// Gain used when the foot becomes unlocked.  When the foot goes from being locked to unlocked,
		/// there can be a gap between the previously locked foot position and the desired position.
		/// This gain is used to smoothly transition to the new foot position.  This gain only affects 
		/// the horizontal component of the position, since the other gains take care of vertical 
		/// smoothing.  Default value is 1.0.
	hkReal m_footUnlockGain;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("Gain used when the foot becomes unlocked. When the foot goes from being locked to unlocked, there can be a gap between the previously locked foot position and the desired position. This gain is used to smoothly transition to the new foot position. This gain only affects the horizontal component of the position, since the other gains take care of vertical smoothing." )

		/// Gain used to move the world-from-model transform up or down in order to reduce the amount that the foot IK system needs to move the feet.
		/// Default value is 0.
	hkReal m_worldFromModelFeedbackGain;	//+default(0)
											//+hk.RangeReal(absmin=0.0,absmax=1.0)
											//+hk.Description("Gain used to move the character up and down depending on how far the ground is." )

		/// Use this to bias the error toward moving the character up to avoid compressing the legs (0), or down to avoid stretching the legs (1).
		/// A value below 0.5 biases upward, and above 0.5 biases downward.
	hkReal m_errorUpDownBias;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("Use this to bias the error toward moving the character up to avoid compressing the legs 0, or down to avoid stretching the legs 1. A value below 0.5 biases upward, and above 0.5 biases downward.")

		/// If non-zero, the up vector of the character's worldFromModel of the character is aligned with
		/// the floor.  The floor normal is approximated from the raycasts done for each foot
		/// (if the character is a biped then two additional rays are cast).  Default value is 0.
	hkReal m_alignWorldFromModelGain;	//+default(0)
										//+hk.RangeReal(absmin=0.0,absmax=1.0)
										//+hk.Description("Gain used to align the character's world-from-model up-vector with the floor normal. The floor normal is approximated from the raycasts done for each foot. If the number of legs is less than 3 then this property is ignored since 3 raycasts are required to approximate the floor normal." )

		/// If non-zero, the hips are rotated to compensate for the rotation of the worldFromModel.  This
		/// makes the legs point toward the ground.  The hip rotation is done on the input pose before
		/// applying IK.  Default value is 0.
	hkReal m_hipOrientationGain;	//+default(0)
									//+hk.RangeReal(absmin=0.0,absmax=1.0)
									//+hk.Description("Gain used to rotate the hips to compensate for the rotation of the character's world-from-model. This makes the legs point toward the ground. The hip rotation is done on the input pose before applying IK." )

	hkbFootIkGains( hkFinishLoadedObjectFlag flag ) {}
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
