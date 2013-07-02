/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_AI_OBJECT_FACTORY_H
#define HKB_AI_OBJECT_FACTORY_H

#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>
#include <Ai/Pathfinding/Character/Behavior/hkaiEdgeFollowingBehavior.h>

class hkbAiControlDriverInfo;
class hkbCharacter;
class hkaiSingleCharacterBehavior;
class hkaiCharacter;
class hkbHandle;
class hkaiWorld;

/// An interface for creating AI-side objects used by the hkbAiBridge. An 
/// instance can be passed to the hkbAiBridge constructor and will be used 
/// throughout the lifetime of the bridge. This can be used, for example, to
/// make AI-controlled characters use a custom hkaiBehavior instead of 
/// hkaiEdgeFollowingBehavior. by default, an instance of 
/// hkbDefaultAiObjectFactory will be constructed and used by the bridge.
class hkbAiObjectFactory : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

	/// Create an avoidanceProperties for the character, based on fields from 
	/// the hkbAiControlDriverInfo.
	virtual hkaiAvoidanceSolver::AvoidanceProperties* createAvoidanceProperties(
		hkbAiControlDriverInfo const* info, 
		hkbCharacter* bCharacter) = 0;


	/// Create an HKAI-side character based on fields from the 
	/// hkbAiControlDriverInfo. The hkbCharacter is stored into the 
	/// hkaiCharacter's userdata.
	virtual hkaiCharacter* createAiCharacter(hkbAiControlDriverInfo const* info, hkbCharacter* bCharacter) = 0;

	/// Create an HKAI-side behavior to control the HKAI-side character, based
	/// on fields from the hkbAiControlDriverInfo. The character is set to 
	/// manual control by default.
	virtual hkaiSingleCharacterBehavior* createAiBehavior(
		hkbAiControlDriverInfo const* info, 
		hkbCharacter* bCharacter, 
		hkaiCharacter* aiCharacter, 
		hkaiWorld* aiWorld) = 0;

	/// Create a state handle for whichever coordinate frame should be used as
	/// the character's position for AI purposes. If the state source is the
	/// character controller, creates no state handle and returns HK_NULL.
	virtual hkbHandle* createStateHandle(hkbAiControlDriverInfo const* info, hkbCharacter* bCharacter) = 0;
};

/// The default implementation of hkbAiObjectFactory
class hkbDefaultAiObjectFactory : public hkbAiObjectFactory
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

	virtual hkaiAvoidanceSolver::AvoidanceProperties* createAvoidanceProperties(
		hkbAiControlDriverInfo const* info, 
		hkbCharacter* bCharacter) HK_OVERRIDE;

	virtual hkaiCharacter* createAiCharacter(hkbAiControlDriverInfo const* info, hkbCharacter* bCharacter) HK_OVERRIDE;

	virtual hkaiEdgeFollowingBehavior* createAiBehavior(
		hkbAiControlDriverInfo const* info, 
		hkbCharacter* bCharacter, 
		hkaiCharacter* aiCharacter, 
		hkaiWorld* aiWorld) HK_OVERRIDE;

	virtual hkbHandle* createStateHandle(
		hkbAiControlDriverInfo const* info, 
		hkbCharacter* bCharacter) HK_OVERRIDE;
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
