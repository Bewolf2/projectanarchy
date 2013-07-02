/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MODIFIER_UTILS_H
#define HKB_MODIFIER_UTILS_H

#include <Behavior/Behavior/Event/hkbEvent.h>

class hkbContext;
class hkbModifier;
class hkbGeneratorOutput;
struct hkbNodeInfo;
struct hkbSyncVariableSetup;

	/// Helper functions generators and modifiers.
class hkbModifierUtils
{
	public:
			/// Utility function that gets called from the generate/modify function of the generators
			/// and modifiers that wrap other modifiers. This syncs variables, calls modify and calls
			/// post node output callback.
		static void HK_CALL modify( const hkbContext& context,
									hkbModifier& modifier, 
									hkbGeneratorOutput& inOut,
									hkReal timeOffset );

			/// Utility function that gets called on modifiers for handling events. This calls handleEvent 
			/// and updates the active state of the modifier.
		static void HK_CALL handleEvent(	const hkbContext& context,
											hkbModifier& modifier,
											const hkbEvent e );

			/// Utility function that gets called on modifiers for updating. This syncs variables, calls update 
			/// and updates the active state of the modifier.
		static void HK_CALL update( const hkbContext& context,
									hkbModifier& modifier,
									hkReal timestep,
									hkbSyncVariableSetup& syncVariableSetup );

	private:

			/// Helper function called by modify, handleEvent and update.
		static void HK_CALL enableModifier( const hkbContext& context, hkbModifier& modifier );

			/// Helper function called by modify, handleEvent and update. If the modifier gets deactivated the this returns true, otherwise false.
			/// On PlayStation(R)3 the modifier is activated again after deactivation.
		static bool HK_CALL disableModifier( const hkbContext& context, hkbModifier& modifier );

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
