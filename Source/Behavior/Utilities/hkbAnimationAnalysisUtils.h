/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ANIMATION_ANALYSIS_UTILS_H
#define HKB_ANIMATION_ANALYSIS_UTILS_H

class hkaAnimation;
class hkaSkeleton;
class hkaDefaultAnimationControl;

	/// Utility function having to do with analysing animation.
class hkbAnimationAnalysisUtils
{
	public:

			/// Determines whether the animation is cyclic or not. Returns true if cyclic, false otherwise.
			/// The cyclicity is computed by taking the angular difference between the first and the last frames
			/// and if for each track this difference is less than tolerance then the animation is considered cyclic.
			/// The tolerance is in degrees.
		static bool HK_CALL isCyclic( const hkaAnimation& animation, hkReal tolerance=0.5f );

			/// Find the pose in animB that matches closely to the last pose in animA and return the time of that pose.

			/// The poses are computed based on animation frames and once the animation frame has been computed the time
			/// corresponding to the closest frame is returned.
		static hkReal HK_CALL findClosestPoseInBToEndPoseInA(	hkaDefaultAnimationControl& animControlA,
																hkaDefaultAnimationControl& animControlB,
																const hkaSkeleton& skelA,
																const hkaSkeleton& skelB );

	private:

			// Helper function called by both findClosestPoseInBToEndPoseInA
		static hkReal HK_CALL findClosestPoseInBToEndPoseInAInternal(	hkaDefaultAnimationControl& animControlA,
																		hkaDefaultAnimationControl& animControlB,
																		const hkPointerMap<int,int>& tracksInAnimAToTracksInAnimBMap );
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
