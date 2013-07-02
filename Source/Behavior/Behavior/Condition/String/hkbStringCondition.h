/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_STRING_CONDITION_H
#define HKB_STRING_CONDITION_H

#include <Behavior/Behavior/Condition/hkbCondition.h>

extern const class hkClass hkbStringConditionClass;

/// A condition that is described by a string.  A user-settable function is used to evaluate
/// the condition based on the string that describes it.
class hkbStringCondition : public hkbCondition
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbStringCondition() {}

		///////////////////////////
		// hkbCondition interface 
		///////////////////////////

			// hkbCondition interface implementation
		virtual bool isTrue( const hkbContext& context ) const HK_OVERRIDE;

	public:

			// The string that will be passed to the evaluation function.  You must manage the string memory yourself.
		hkStringPtr m_conditionString;	//+hk.Description("The string that will be passed to the evaluation function.")

	public:

			/// A function type for evaluating conditions described by strings.
		typedef bool HK_CALL EvaluationFunction( const char* conditionString );

			/// Set this to point to your function that can evaluate conditions described by strings.
		static EvaluationFunction* m_evaluationFunction;

			/// This is returned if m_evaluationFunction is null.
		static bool m_defaultReturnValue;

	public:

		hkbStringCondition( hkFinishLoadedObjectFlag flag ) : hkbCondition(flag), m_conditionString(flag) {}

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
