/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_SINGLE_CHARACTER_BEHAVIOR_H
#define HK_AI_SINGLE_CHARACTER_BEHAVIOR_H

#include <Common/Base/Types/hkRefPtr.h>
#include <Ai/Pathfinding/Character/Behavior/hkaiBehavior.h>
#include <Ai/Pathfinding/Character/Utilities/hkaiCharacterUtil.h>
#include <Ai/Pathfinding/Character/hkaiCharacter.h>

	/// Convenience abstract base class for behaviors managing a single character.
class hkaiSingleCharacterBehavior : public hkaiBehavior
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING);

	hkaiSingleCharacterBehavior(hkaiCharacter* character, hkaiWorld* world);
	~hkaiSingleCharacterBehavior();

		/// Request a path with multiple potential goals. 
		/// The path to the closest goal (in terms of total distance) will be computed.
	virtual void requestPathWithMultipleGoals(const hkVector4* goals, int numGoals, int priority = 0) HK_OVERRIDE;

	virtual void requestVolumePathWithMultipleGoals(const hkVector4* goals, int numGoals, int priority = 0) HK_OVERRIDE;

	virtual int getNumCharacters() const HK_OVERRIDE { return 1; }

		/// Return whether the character is either at the goal, or slowing towards it.
	virtual bool isNearGoal() const HK_OVERRIDE;

		/// Return the managed character. The index parameter must be 0.
	virtual hkaiCharacter* getCharacter(int index) HK_OVERRIDE;

	virtual void cancelRequestedPath() HK_OVERRIDE;

	/// For characters in manual control, this should be the state that is set once manual control is released.
	/// For characters not in manual control, this should return STATE_MANUAL_CONTROL.
	/// This function is currently only used by the Behavior/AI bridge; if it is not needed, it
	/// does not need to be overridden by custom hkaiSingleCharacterBehavior subclasses.
	virtual hkaiCharacter::State getSavedCharacterState()
	{
		HK_ERROR(0x4375f17d, "Not implemented");
		return hkaiCharacter::STATE_MANUAL_CONTROL;
	}


protected:

	hkRefPtr<hkaiCharacter> m_character;

	hkaiCharacterUtil::CallbackType m_callbackType;
};

#endif // HK_AI_SINGLE_CHARACTER_BEHAVIOR_H

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
