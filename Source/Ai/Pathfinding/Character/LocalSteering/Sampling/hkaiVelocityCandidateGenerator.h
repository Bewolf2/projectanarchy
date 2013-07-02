/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VELOCITY_CANDIDATE_GENERATOR_H
#define HKAI_VELOCITY_CANDIDATE_GENERATOR_H

class hkaiVelocityCandidateList;

class hkFourTransposedPoints;

class hkaiVelocityCandidateGenerator
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiVelocityCandidateGenerator);
	HK_DECLARE_POD_TYPE();

	hkaiVelocityCandidateGenerator() { }

	void init(hkVector4Parameter forward, hkVector4Parameter right, hkVector4Parameter prev, hkSimdRealParameter maxSpeed, hkSimdRealParameter desiredSpeed);

	hkVector4 const& getForward() const { return m_forward; }
	hkVector4 const& getRight() const { return m_right; }
	HK_FORCE_INLINE void getPrev(hkVector4 & prevOut) const;
	hkSimdReal const& getMaxSpeed() const { return m_maxSpeed; }

	HK_FORCE_INLINE hkSimdReal getPrevForwardFactor() const { return m_firstBlockForward.getComponent<0>(); }
	HK_FORCE_INLINE hkSimdReal getPrevRightFactor() const { return m_firstBlockRight.getComponent<0>(); }

	static const int NUM_CANDIDATES = 128;
	static const int NUM_BLOCKS = 32;
	static const int NUM_RING_BLOCKS = 8;
	static const int FIRST_RING_BLOCK = 24;

	template<bool isFirstBlock>
	HK_FORCE_INLINE void generateCandidateBlock(
		int blockIdx, 
		hkFourTransposedPoints & candidateBlock) const;

	/// Possibly slow method to generate a single candidate
	void generateCandidate(int candidateIdx, hkVector4 & candidate) const;

	template<bool isFirstBlock>
	HK_FORCE_INLINE void generateProgressScores(
		int blockIdx, 
		hkSimdRealParameter mainForward, 
		hkSimdRealParameter mainRight, 
		hkSimdRealParameter wallForward, 
		hkSimdRealParameter wallRight,
		hkSimdRealParameter bonusForward,
		hkSimdRealParameter bonusRight,
		hkSimdRealParameter desiredSpeedRecip,
		hkSimdRealParameter wallPenalty,
		hkVector4 & scoresOut) const;

	/// Possibly slow method to generate a single score
	hkSimdReal generateProgressScore(
		int candidateIdx, 
		hkSimdRealParameter mainForward, 
		hkSimdRealParameter mainRight, 
		hkSimdRealParameter wallForward, 
		hkSimdRealParameter wallRight,
		hkSimdRealParameter bonusForward,
		hkSimdRealParameter bonusRight,
		hkSimdRealParameter desiredSpeedRecip,
		hkSimdRealParameter wallPenalty) const;

	template<bool isFirstBlock>
	HK_FORCE_INLINE void getBlockFactors(int blockIdx, hkVector4 & forwardFactors, hkVector4 & rightFactors) const;

	HK_FORCE_INLINE void getRawBlockFactors(int blockIdx, hkVector4 & forwardFactors, hkVector4 & rightFactors) const;

	HK_FORCE_INLINE void generateWeightings(hkVector4Parameter ideal, hkSimdReal & forwardWeighting, hkSimdReal & rightWeighting) const;

private:
	const hkUint8* m_template;
	hkVector4 m_forward;
	hkVector4 m_right;
	hkVector4 m_firstBlockForward;
	hkVector4 m_firstBlockRight;
	hkSimdReal m_maxSpeed;

	HK_ALIGN16(static const hkUint8 s_main_template[(NUM_BLOCKS+1)*8]);
	HK_ALIGN16(static const hkUint8 s_halfSpeed_template[(NUM_BLOCKS+1)*8]);
};

#include <Ai/Pathfinding/Character/LocalSteering/Sampling/hkaiVelocityCandidateGenerator.inl>

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
