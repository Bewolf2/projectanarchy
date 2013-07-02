/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVALUATE_EXPRESSION_MODIFIER_H
#define HKB_EVALUATE_EXPRESSION_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Common/Base/Container/String/hkString.h>
#include <Common/Base/Container/StringMap/hkStringMap.h>
#include <Behavior/Behavior/Utils/hkbCompiledExpressionSet.h>

extern const class hkClass hkbExpressionDataArrayClass;
extern const class hkClass hkbExpressionDataClass;
extern const class hkClass hkbEvaluateExpressionModifierInternalExpressionDataClass;
extern const class hkClass hkbEvaluateExpressionModifierClass;

class hkbCompiledExpressionSet;

	/// An expression and its event mode.
struct hkbExpressionData
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbExpressionData );
	HK_DECLARE_REFLECTION();

	hkbExpressionData()
		:	m_assignmentVariableIndex(-1),
			m_assignmentEventIndex(-1),
			m_eventMode(EVENT_MODE_SEND_ONCE)
	{}

		/// The modes for sending events based on the expression.
	enum ExpressionEventMode
	{
			/// Send the event once the first time that the expression is true.
		EVENT_MODE_SEND_ONCE,

			/// Send the event every frame if the expression is true.
		EVENT_MODE_SEND_ON_TRUE,

			/// Send the event every frame in which the expression becomes true after having been false on the previous frame.
		EVENT_MODE_SEND_ON_FALSE_TO_TRUE,

			/// Send the event every frame after it first becomes true.
		EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE,
	};

		/// Expression that's input by the user in the form: "variablename = expression" or "eventname = boolean expression".
	hkStringPtr m_expression;	//+hk.Description("Expression of the form 'variablename = expression' or 'eventname = boolean expression'.")

		/// This is the variable that we will assign result to (-1 if not found). For internal use.
	hkInt32 m_assignmentVariableIndex; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

		/// This is the event we will raise if result > 0 (-1 if not found). For internal use.
	hkInt32 m_assignmentEventIndex;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

		/// Under what circumstances to send the event.
	hkEnum< ExpressionEventMode, hkInt8> m_eventMode;	//+default(hkbExpressionData::EVENT_MODE_SEND_ONCE)
														//+hk.Description("Under what circumstances to send the event.")

		/// Whether the event has been sent yet.
	hkBool m_raisedEvent;	//+nosave

		/// Whether the expression was true in the previous frame.
	hkBool m_wasTrueInPreviousFrame;	//+nosave

	hkbExpressionData( hkFinishLoadedObjectFlag flag ): m_expression(flag) {}
};

	/// An array of expressions wrapped for shared access.
class hkbExpressionDataArray : public hkReferencedObject
{
		//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbExpressionDataArray() {}

		hkbExpressionDataArray( const hkbExpressionDataArray& that );

			/// One expression data for each expression.
		hkArray<struct hkbExpressionData> m_expressionsData; //+hk.Description("The list of expressions to be evaluated.")

	public:

		hkbExpressionDataArray( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_expressionsData(flag) {}
};


	/// Deprecated.  Please use the hkbScriptGenerator instead.
	/// 
	/// A modifier for evaluating expressions.
class hkbEvaluateExpressionModifier : public hkbModifier
{
	//+version(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbEvaluateExpressionModifier();

	private:

		hkbEvaluateExpressionModifier( const hkbEvaluateExpressionModifier& evaluateExpressionMod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void precompute( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;
		
			// hkbNode interface implementation.
		virtual void getActiveVariablesSpecial( hkPointerMap<int, bool>& activeVariables ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getInternalStateUser( const hkbBehaviorGraph& rootBehavior, hkReferencedObject& internalState ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void setInternalStateUser(	const hkbContext& context,
											const hkReferencedObject& internalState,
											hkPointerMap<hkInt16, const hkbNodeInternalStateInfo*>& nodeIdToInternalStateMap ) HK_OVERRIDE;

			/// For internal use.  Returns the compiled expression.
		hkbCompiledExpressionSet* getCompiledExpressionSet() { return m_compiledExpressionSet; }

		///////////////
		// properties
		///////////////

			/// A set of expressions to be evaluated.
		hkRefPtr<hkbExpressionDataArray> m_expressions;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
														//+hk.Description("A set of expressions to be evaluated.")

	private:

		// parses the LHS of the expression and updates the expression to contain the RHS of the expression
		static bool HK_CALL parseLHS(	hkStringBuf& expression, 
										hkStringBuf& errorStr, 
										hkInt32& assignmentEventIndex,
										hkInt32& assignmentVariableIndex,
										const hkbContext* context, 
										const hkStringMap<int>* varNameToIdMap );

			// returns the event id of a passed in event name
		static hkInt32 HK_CALL getEventId( hkbBehaviorGraph* behavior, const char* name );

			/// Set the value of a variable.
		static void HK_CALL setVariableValue( int variableIndex, const hkbContext& context, hkReal variableValue );
		
			/// Remove white spaces from the start and the end of the input string
		static void HK_CALL removeWhiteSpacesFromBeginAndEnd( hkStringBuf& str );
		
			/// Copy the result of the expression into the variable
		template<typename VariableType>
		HK_FORCE_INLINE static void HK_CALL copyResultToVariable( hkReal variableValue, int rootVariableIndex, hkbBehaviorGraph& rootBehavior );

	public:

			/// For internal use.
		struct InternalExpressionData
		{
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbEvaluateExpressionModifier::InternalExpressionData );

			InternalExpressionData() {}
			InternalExpressionData( hkFinishLoadedObjectFlag flag ) {}

				// has the event been raised 
			hkBool m_raisedEvent;

				// was the expression true in the previous frame 
			hkBool m_wasTrueInPreviousFrame;
		};

	private:

		hkRefPtr<hkbCompiledExpressionSet> m_compiledExpressionSet;	//+nosave

		HKB_BEGIN_INTERNAL_STATE(0);

			// One internal expression data for each expression
		hkArray<struct hkbEvaluateExpressionModifier::InternalExpressionData> m_internalExpressionsData; //+nosave
 
		HKB_END_INTERNAL_STATE();

	public:

		hkbEvaluateExpressionModifier( hkFinishLoadedObjectFlag flag );
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
