/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EXPRESSION_CONDITION_H
#define HKB_EXPRESSION_CONDITION_H

#include <Behavior/Behavior/Condition/hkbCondition.h>
#include <Behavior/Behavior/Utils/hkbCompiledExpressionSet.h>

extern const class hkClass hkbExpressionConditionClass;

	/// Deprecated.  Please use the hkbScriptGenerator to send an event to start the transition instead.
	///
	/// A condition that is described by an expression string.  
class hkbExpressionCondition : public hkbCondition
{
	//+version(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbExpressionCondition();

		~hkbExpressionCondition();

		///////////////////////////
		// hkbCondition interface 
		///////////////////////////

			// hkbCondition interface implementation
		virtual bool isTrue( const hkbContext& context ) const HK_OVERRIDE;

	public:

			/// The expression that will be evaluated.
		hkStringPtr m_expression; //+hk.Description("The expression that will be evaluated.")
		
	public:

			/// For internal use.  This compiles the expression.  
		void compile( const hkbContext& context );

			/// Returns the compiled expression.
		hkbCompiledExpressionSet* getCompiledExpressionSet();
		
	private:
		
			// The expression compiled into an efficient RPN form.
		hkRefPtr<hkbCompiledExpressionSet> m_compiledExpressionSet;	//+nosave

	public:

		hkbExpressionCondition( hkFinishLoadedObjectFlag flag ) : hkbCondition(flag), m_expression(flag), m_compiledExpressionSet(flag) {}

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
