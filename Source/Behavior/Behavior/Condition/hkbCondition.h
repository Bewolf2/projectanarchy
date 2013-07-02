/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CONDITION_H
#define HKB_CONDITION_H

#include <Common/Base/Object/hkReferencedObject.h>

extern const class hkClass hkbConditionClass;

class hkbContext;

	/// An object that evaluates to true or false.  Conditions do not get cloned when a behavior graph
	/// is shallow cloned, so you need to ensure that isTrue() is re-entrant and that your class
	/// does not include any character-specific state.
class hkbCondition : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCondition() {}

			/// Evaluates the condition.  Conditions do not get cloned when a behavior graph
			/// is shallow cloned, so you need to ensure that isTrue() is re-entrant and that your class
			/// does not include any character-specific state.
		virtual bool isTrue( const hkbContext& context ) const = 0;

			/// Returns whether or not this condition requires a lua state
		virtual bool requiresLua() const { return false; }

	public:

		hkbCondition( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

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
