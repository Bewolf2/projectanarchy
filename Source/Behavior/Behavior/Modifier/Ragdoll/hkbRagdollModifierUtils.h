/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_RAGDOLL_MODIFIER_UTILS_H
#define HKB_RAGDOLL_MODIFIER_UTILS_H

class hkbPoweredRagdollControlsModifier;
class hkbRigidBodyRagdollControlsModifier;
class hkbKeyframeBonesModifier;
class hkbGeneratorOutput;

#include <Behavior/Behavior/Generator/hkbGeneratorOutputUtils.h>

class hkbRagdollModifierUtils
{
	public:
		static void HK_CALL poweredRagdollControlsModifierModify(	const hkbPoweredRagdollControlsModifier& mod,
																	hkbGeneratorOutput& inOut );

		static void HK_CALL rigidBodyRagdollControlsModifierModify(	const hkbRigidBodyRagdollControlsModifier& mod,
																	hkbGeneratorOutput& inOut );

		static void HK_CALL keyframeBonesModifierModify(	const hkbKeyframeBonesModifier& mod,
															hkbGeneratorOutput& inOut );

	private:
		static void HK_CALL setRealTrackData(	hkbGeneratorOutput& inOut,
												hkbGeneratorOutput::StandardTracks track,
												hkReal trackData );
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
