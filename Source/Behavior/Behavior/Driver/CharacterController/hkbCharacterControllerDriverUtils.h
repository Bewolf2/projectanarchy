/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_CONTROLLER_DRIVER_UTILS_H
#define HKB_CHARACTER_CONTROLLER_DRIVER_UTILS_H

struct hkbCharacterControllerControlData;

	/// Helper functions for updating the world from model given the character controller position.
class hkbCharacterControllerDriverUtils
{
	public:
			/// Given the character controller position and the current world from model position update the world from model
			/// and also update the horizontal displacement between the world from model and the character controller.
			///
			/// The difference between the actual position and the expected character controller position is that character controller position 
			/// after it has been integrated is the actual character controller position and it may not be where we want as might have been bumped
			/// by something where as the expected character controller position is the character controller position is the character controller position where 
			/// we want it to be.
		static void HK_CALL updateWorldFromModelPosition(	const hkVector4& worldUp,
															const hkVector4& actualCharacterControllerPosition,
															const hkVector4& expectedCharacterControllerPosition,
															const hkbCharacterControllerControlData& controlData,
															const hkReal verticalDisplacement,
															hkVector4& horizontalDisplacementInOut,
															hkVector4& worldFromModelPositionInOut );

			/// Calculate a desired velocity for the character controller.
		static void HK_CALL calculateDesiredVelocity(	const hkbCharacterControllerControlData& controlData,
														const hkVector4& worldUp,
														const hkVector4& characterControllerVelocity,
														const hkVector4& previousPosition,
														const hkQsTransform& currentTransform,
														const hkVector4& gravity,
														bool isSupported,
														hkVector4& desiredVelocity,
														hkReal timestep);


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
