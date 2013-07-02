/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Common/Base/Math/Vector/hkFourTransposedPoints.h>
#include <Common/Base/Math/Vector/hkIntVector.h>

template<bool isFirstBlock>
HK_FORCE_INLINE void hkaiVelocityCandidateGenerator::generateCandidateBlock(int blockIdx, hkFourTransposedPoints & candidateBlockOut) const
{
	hkVector4 forwardFactors; 
	hkVector4 rightFactors; 
	getBlockFactors<isFirstBlock>(blockIdx, forwardFactors, rightFactors);
	forwardFactors.mul((hkSimdRealParameter)m_maxSpeed);
	rightFactors.mul((hkSimdRealParameter)m_maxSpeed);

	candidateBlockOut.setOuterProduct((hkVector4Parameter)m_forward, forwardFactors);
	candidateBlockOut.addOuterProduct((hkVector4Parameter)m_right, rightFactors);

}

template<bool isFirstBlock>
HK_FORCE_INLINE void hkaiVelocityCandidateGenerator::generateProgressScores(
	int blockIdx, 
	hkSimdRealParameter mainForward, 
	hkSimdRealParameter mainRight, 
	hkSimdRealParameter wallForward, 
	hkSimdRealParameter wallRight,
	hkSimdRealParameter bonusForward,
	hkSimdRealParameter bonusRight,
	hkSimdRealParameter desiredSpeedRecip,
	hkSimdRealParameter wallPenalty,
	hkVector4 & scoresOut) const
{
	

	hkVector4 forwardFactors; 
	hkVector4 rightFactors; 
	getBlockFactors<isFirstBlock>(blockIdx, forwardFactors, rightFactors);

	// main versus wall-follow
	hkVector4 mainScores, wallScores, scores;
	mainScores.setMul(forwardFactors, mainForward);
	mainScores.addMul(rightFactors, mainRight);
	mainScores.mul(hkSimdReal_2);
	wallScores.setMul(forwardFactors, wallForward);
	wallScores.addMul(rightFactors, wallRight);
	wallScores.mul(hkSimdReal_2);

	hkVector4 wallPenaltyV; wallPenaltyV.setAll(wallPenalty);
	wallScores.sub(wallPenaltyV);
	scores.setMax(mainScores, wallScores);
	
	// velocity laziness
	hkVector4 lazinessPenalty;
	lazinessPenalty.setMul(forwardFactors, forwardFactors);
	lazinessPenalty.addMul(rightFactors, rightFactors);
	lazinessPenalty.mul(desiredSpeedRecip);
	lazinessPenalty.mul((hkSimdRealParameter)m_maxSpeed);
	scores.sub(lazinessPenalty);

	// bonus
	scores.addMul(forwardFactors, bonusForward);
	scores.addMul(rightFactors, bonusRight);

	// scaled to actual speed
	scores.mul((hkSimdRealParameter)m_maxSpeed);

	scoresOut = scores;
}

template<>
HK_FORCE_INLINE void hkaiVelocityCandidateGenerator::getBlockFactors<true>( int blockIdx, hkVector4 & forwardFactors, hkVector4 & rightFactors ) const
{
	HK_ASSERT(0x3d7be513, blockIdx == 0);

	forwardFactors = m_firstBlockForward;
	rightFactors = m_firstBlockRight;
}

template<>
HK_FORCE_INLINE void hkaiVelocityCandidateGenerator::getBlockFactors<false>( int blockIdx, hkVector4 & forwardFactors, hkVector4 & rightFactors ) const
{
	HK_ASSERT(0x3d7be514, blockIdx != 0);

	getRawBlockFactors(blockIdx, forwardFactors, rightFactors);
}

HK_FORCE_INLINE void hkaiVelocityCandidateGenerator::getRawBlockFactors(int blockIdx, hkVector4 & forwardFactors, hkVector4 & rightFactors) const
{
	hkIntVector zero; zero.setZero();

	hkVector4 bias = hkVector4::getConstant<HK_QUADREAL_MINUS1>();
	hkSimdReal scale = hkSimdReal::getConstant<HK_QUADREAL_INV_127>();

	hkIntVector bytes;
	bytes.loadNotAligned<2>((hkUint32*)(m_template + (blockIdx << 3)));

	hkIntVector shorts;
	shorts.setCombineHead8To16(zero, bytes);

	hkIntVector forwardLongs;
	forwardLongs.setCombineHead16To32(zero, shorts);

	hkIntVector rightLongs;
	rightLongs.setCombineTail16To32(zero, shorts);

	hkVector4 forwardFactorsInt;
	forwardLongs.convertU32ToF32(forwardFactorsInt);

	hkVector4 rightFactorsInt;
	rightLongs.convertU32ToF32(rightFactorsInt);


	forwardFactors.setAddMul(bias, forwardFactorsInt, scale);
	rightFactors.setAddMul(bias, rightFactorsInt, scale);
}

HK_FORCE_INLINE void hkaiVelocityCandidateGenerator::generateWeightings(hkVector4Parameter ideal, hkSimdReal & forwardWeighting, hkSimdReal & rightWeighting) const
{
	forwardWeighting = m_forward.dot<3>(ideal);
	rightWeighting = m_right.dot<3>(ideal);
}

HK_FORCE_INLINE void hkaiVelocityCandidateGenerator::getPrev(hkVector4 & prevOut) const
{
	prevOut.setMul(m_forward, m_firstBlockForward.getComponent<0>());
	prevOut.addMul(m_right, m_firstBlockRight.getComponent<0>());
	prevOut.mul(m_maxSpeed);
}

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
