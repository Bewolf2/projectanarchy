/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SCRIPT_CONDITION_H
#define HKB_SCRIPT_CONDITION_H

#include <Behavior/Behavior/Condition/hkbCondition.h>

extern const class hkClass hkbScriptConditionClass;

/// A condition that is described by a Script.  The Script string can be any valid Lua.
/// It may call a function which is setup in HAT to return a boolean or resolve to a boolean value directly.
class hkbScriptCondition : public hkbCondition
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbScriptCondition() {}

		///////////////////////////
		// hkbCondition interface 
		///////////////////////////

			// hkbCondition interface implementation
		virtual bool isTrue( const hkbContext& context ) const HK_OVERRIDE;

			// hkbCondition interface implementation
		virtual bool requiresLua() const HK_OVERRIDE { return true; }

	public:

			/// The script code to call when evaluating isTrue.
		hkStringPtr m_conditionScript;	//+hk.Description("The script code to call to evaluate the condition.")
										//+hkb.RoleAttribute("ROLE_SCRIPT","FLAG_NOT_VARIABLE|FLAG_NOT_CHARACTER_PROPERTY")
		
	public:

		hkbScriptCondition( hkFinishLoadedObjectFlag flag ) : hkbCondition(flag), m_conditionScript(flag) {}

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
