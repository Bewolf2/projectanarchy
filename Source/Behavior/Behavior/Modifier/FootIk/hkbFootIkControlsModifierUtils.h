/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FOOT_IK_CONTROLS_MODIFIER_UTILS_H
#define HKB_FOOT_IK_CONTROLS_MODIFIER_UTILS_H

#include <Behavior/Behavior/Driver/FootIk/hkbFootIkDriver.h>

class hkbFootIkControlsModifier;
class hkbGeneratorOutput;

	/// Used internally by the hkbFootIkControlsModifier.
class hkbFootIkControlsModifierUtils
{
	public:

			/// The core code for performing modify() on an hkbFootIkControlsModifier.
		static void HK_CALL modifyInternal( hkbFootIkControlsModifier& mod, 
											hkbGeneratorOutput& inOut,
											const hkbFootIkDriver::InternalLegData* internalLegData,
											int numInternalLegData );
			
			/// The leg output properties of the hkbFootIkControlsModifier
		static void HK_CALL setLegOutputProperties(	hkbFootIkControlsModifier& mod, 
													const hkbFootIkDriver::InternalLegData* internalLegData, 
													int numInternalLegData );

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
