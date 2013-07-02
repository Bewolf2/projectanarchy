/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_HAND_IK_DRIVER_H
#define HKB_HAND_IK_DRIVER_H

#include <Behavior/Behavior/Utils/hkbBlendCurveUtils.h>
#include <Behavior/Behavior/Utils/hkbHandle.h>

class hkaPose;
class hkbHandIkDriverInfo;
class hkbHandle;
class hkbGeneratorOutput;
class hkbContext;
struct hkbHandIkControlData;

	/// A driver that performs hand IK.
	///
	/// The hkbHandIkDriver performs hand IK using data that it gets from the tracks. Currently there is a track data
	/// for hand positions. There is one for each hand. The same is for hand normals. The hand IK has the ability to
	/// fade out when the modifier stops getting the track data. The duration that it should take to fade
	/// out is provided by the track data itself.
class hkbHandIkDriver : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

	public:

		hkbHandIkDriver();

			/// Do the hand ik.
		void doHandIk( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timestep, bool fixup = false );

			/// Reset the handIk driver to its initial state.
		void reset();

	private:

		struct InternalHandData
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbHandIkDriver::InternalHandData );

			// The local frame corresponding to the given name (if found)
			hkbHandle m_localFrameHandle;

			// the target handle
			hkbHandle m_targetHandle;

			// The world space transform of handle 
			hkTransform m_handleTransformWS;

			// target position in model space
			hkVector4 m_targetPositionMS;

			// target rotation in model space
			hkQuaternion m_targetRotationMS;

			// target hand normal in model space
			hkVector4 m_targetHandNormalMS;

			// how much to IK the hand towards the target
			hkReal m_transformOnFraction;

			// how much to Ik the hand normal towards the target normal
			hkReal m_normalOnFraction;

			// how long to fade in IK
			hkReal m_fadeInDuration;

			// how long to fade out IK
			hkReal m_fadeOutDuration;

			// the value of the track on fraction in the previous frame.
			hkReal m_previousTrackOnFraction;

			/// The status of the handle. Are we still smoothly moving between two handles.
			hkBool m_smoothlyChangingHandles;
		};

		hkArray< struct InternalHandData > m_internalHandData;

			// whether or not hand IK has been initialized
		hkBool m_isSetUp;

	private:

			// setup the hand ik driver
		void resetInternal( const hkbContext& context, const hkbHandIkDriverInfo& info );

		void doArmIKModelSpace( int handIndex, 
								hkReal firstJointGain, 
								hkReal secondJointGain, 
								hkReal endJointGain, 
								const hkVector4& targetInModelSpace, 
								const hkQuaternion& targetRotationInModelSpace,
								const hkbHandIkDriverInfo& info,
								hkaPose& poseInOut );
		void doArmIKWorldSpace( int handIndex, 
								hkReal firstJointGain, 
								hkReal secondJointGain, 
								hkReal endJointGain, 
								const hkVector4& targetInWorldSpace, 
								const hkQuaternion& targetRotationInModelSpace, 
								const hkQsTransform& worldFromModel,
								const hkbHandIkDriverInfo& info,
								hkaPose& poseInOut );
		void orientHand(	int handIndex, 
							hkReal fractionToRotate, 
							const hkVector4& desiredUpMS, 
							const hkQsTransform& worldFromModel,
							const hkbHandIkDriverInfo& info,
							hkaPose& poseInOut );

		void computeTargetFromHandle(	const hkbContext& context,
										const hkbGeneratorOutput& inOut,
										const hkbHandIkDriverInfo& info,
										const hkbHandIkControlData& controlData, 
										InternalHandData& handData,
										hkReal timestep );
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
