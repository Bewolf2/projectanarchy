/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VELOCITY_CANDIDATE_SCORES_H
#define HKAI_VELOCITY_CANDIDATE_SCORES_H

#include <Common/Base/Math/Vector/hkVectorN.h>
#include <Ai/Pathfinding/Character/LocalSteering/Sampling/hkaiVelocityCandidateGenerator.h>

/// An array of scalar scores, intended to be parallel to a velocity candidate list.
///
/// This class exists to abstract the underlying storage method as much as possible. Currently
/// storage is dynamic and by pointer, but may later be fixed-size through the hkMxVector type.
class hkaiVelocityCandidateScores
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiVelocityCandidateScores);

	void setAll(hkSimdRealParameter initScore);

	/// Single-element access methods; may be slow
	///@{
	void setOne(int scoreIndex, hkReal score);
	hkReal getOne(int scoreIndex) const;
	///@}

	hkBool32 prefixEqual(hkaiVelocityCandidateScores const& other, int numToCheck) const;
	hkBool32 allEqual(hkaiVelocityCandidateScores const& other) const;

	hkInplaceVectorN< hkaiVelocityCandidateGenerator::NUM_CANDIDATES> m_scores;
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
