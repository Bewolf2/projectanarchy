/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FOOT_IK_DRIVER_H
#define HKB_FOOT_IK_DRIVER_H

class hkaPose;
class hkaSkeleton;
class hkaFootPlacementIkSolver;
class hkbContext;
class hkbGeneratorOutput;
struct hkbFootIkGains;

	/// Performs foot IK on the incoming pose.
class hkbFootIkDriver : public hkReferencedObject
{
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	
		hkbFootIkDriver();
		~hkbFootIkDriver();

			/// Do the foot ik.
		void doFootIk( hkbContext& context, hkbGeneratorOutput& inOut );

			/// Get world from model feedback.
		hkReal getWorldFromModelFeedback() const;

			/// Reset the footIk driver to its initial state.
		void reset();

	public:

			// Internal and output data for each leg.
		struct InternalLegData
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbFootIkDriver::InternalLegData );

				// The position of the ground below the foot, as computed by the foot IK raycasts.
				// These are stored because we need to delay one frame before adjusting the world-from-model.
			hkVector4 m_groundPosition;

				// The normal of the ground where the feet hit the ground.
			hkVector4 m_groundNormal;

				// The foot IK solver used for this leg.
			hkaFootPlacementIkSolver* m_footIkSolver;

				// The distance between the input foot height and the ground.
			hkReal m_verticalError;

				// Whether or not the ground was hit by the raycast.
			hkBool m_hitSomething;

				// Whether or not the foot is planted in model space in the incoming animation. If the height 
				// of the ankle goes below m_footPlantedAnkleHeightMS the foot is considered planted. 
			hkBool m_isPlantedMS;

				// Construct with defaults.
			InternalLegData()
			:	m_groundPosition( hkVector4::getZero() ),
				m_footIkSolver(HK_NULL),
				m_verticalError(0.0f),
				m_hitSomething(false),
				m_isPlantedMS(false) {}				
		};

			/// get the internal leg data
		const hkArray<InternalLegData>& getInternalLegData() const;

	private:

		// Disable the footIk controller
		void disableFootIk();

		// Set up the footIk solver
		void setup( const hkbContext& context, const hkaSkeleton& skeleton );

		void adjustHipOrientation( const hkbContext& context, hkbGeneratorOutput& inOut, hkaPose& pose, const hkVector4& upVector, hkbFootIkGains& gains );
		void alignWorldFromModelUpWithGroundNormal( const hkbContext& context, const hkVector4& raycastUp, hkbGeneratorOutput& inOut, hkbFootIkGains& gains );
		void computeGroundNormal( const hkbContext& context, const hkQsTransform& worldFromModel, const hkVector4& raycastUp, hkVector4& groundNormal );
		void computeGroundNormalForNarrowQuadrupeds(	hkVector4* legPositions, 
														const int numLegs,
														const hkbContext& context, 
														const hkQsTransform& worldFromModel, 
														const hkVector4& raycastUp, 
														hkVector4& groundNormal );
		void adjustGroundNormalUsingAlignFractions( const hkbContext& context, 
													const hkQsTransform& worldFromModel, 
													hkVector4& groundNormalInOut );

		// Compute the plane normal from a set of points.
		bool computePlaneNormal(	const hkVector4* const points, 
									const int numPoints, 
									const hkVector4& up, 
									hkVector4& normal );

		// Compute the plane forward from a set of points which lie on the plane perpendicular to the plane forward.
		bool computePlaneForward(	const hkVector4* const points, 
									const int numPoints,
									const hkVector4& forward, 
									const hkVector4& up,
									hkVector4& planeForward );

			// This computes the weighted average of the min and the max distance between the actual feet position 
			// relative to the ground and the desired feet position relative to the ground based on m_gains.m_errorUpDownBias.
		hkReal computeError( hkReal errorUpDownBias );

			// Compute axis by interpolating between the from-axis and the to-axis based on alignFraction.
		static void HK_CALL computeAxis( const hkReal alignFraction, const hkVector4& from, hkVector4& toInOut );

	private:

			// The size of this array should be same as the m_legs array size.
		hkArray< struct InternalLegData > m_internalLegData;

			// The distance to move the world from model up and down in order to reduce the amount that the foot IK system needs to move the feet. 
		hkReal m_worldFromModelFeedback;

			// Data to store the previous footIk track data. 
		hkReal m_prevIsFootIkEnabled;

			// whether or not foot IK has been initialized
		hkBool m_isSetUp;

			// whether or not the ground position for each leg has been computed
		hkBool m_isGroundPositionValid;

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
