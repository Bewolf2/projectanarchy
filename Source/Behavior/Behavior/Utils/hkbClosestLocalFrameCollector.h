/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CLOSEST_LOCAL_FRAME_COLLECTOR_H
#define HKB_CLOSEST_LOCAL_FRAME_COLLECTOR_H

#include <Common/Base/Types/Geometry/LocalFrame/hkLocalFrame.h>
#include <Behavior/Behavior/Utils/hkbHandle.h>

	/// A collector that collects the local frame that is closest to a given point in world coordinates.
class hkbClosestLocalFrameCollector : public hkLocalFrameCollector
{
	public:
	
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// Construct given the world position near which we want to collect a frame, and the name
			/// of the local frame that we want to collect (pass in HK_NULL to ignore the names).
		hkbClosestLocalFrameCollector( const char* localFrameName );

			// hkLocalFrameCollector implementation
		virtual void addFrame( const hkLocalFrame* frame, hkReal distance ) HK_OVERRIDE;

			/// Set the rigid body that defines the coordinate frame for the frames
			/// that are being collected.
		void setRigidBody( hkbRigidBodyHandle rb );

			/// Set the character and animation bone index that define the coordinate
			/// frame for the frames that are being collected.
		void setCharacterAndBone( hkbCharacter* character, hkInt16 animationBoneIndex );

			/// Set the minimum distance the sensed handle should be from the sensor position.
		void setMinimumDistance( hkReal minDistance );

			/// Set the group that the local frame to collect belongs to.
		void setGroup( const hkLocalFrameGroup* group );

			/// The collected frame is put into this handle.
		hkbHandle m_handle;

			/// The distance to the closest frame.
		hkReal m_closestDist;

	private:

			// This handle has the defaults that are copied into m_handle when a handle is collected.
		hkbHandle m_defaultHandle;

			// The minimum distance between the sensor position and the sensed handle.
		hkReal m_minDistance;

			// The name of the local frames to collect.
		const char* m_localFrameName;

			// The group that the local frame to collect belongs to.
		const hkLocalFrameGroup* m_group;
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
