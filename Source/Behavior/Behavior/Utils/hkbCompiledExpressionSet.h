/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_COMPILED_EXPRESSION_SET_H
#define HKB_COMPILED_EXPRESSION_SET_H

#include <Behavior/Behavior/Variables/hkbVariableInfo.h>

extern const class hkClass hkbCompiledExpressionSetTokenClass;

extern const class hkClass hkbCompiledExpressionSetClass;

class hkbVariableValueSet;
class hkbSymbolIdMap;
class hkbContext;

template <typename T, typename A> class hkStringMap;


	/// Represents a set of expressions that have been compiled into a compact form.
class hkbCompiledExpressionSet : public hkReferencedObject
{
	//+version(1)
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCompiledExpressionSet();

			/// Add an expression to the exiting expressions.
		bool addExpression( const char* expression, 
							hkStringMap<int,hkContainerHeapAllocator>* varNameToIdMap,
							hkStringMap<int,hkContainerHeapAllocator>* characterPropertyNameToIdMap,
							hkStringBuf& errorStr );

			/// Clear all expressions
		void clear();

			/// Get the number of expressions
		int getNumExpressions() const;

			/// Evaluate a single expression 
		bool evaluateExpression( const hkbContext& context, int expressionIndex, hkReal& result ) const;

			/// Returns the variables used in the expression set.
		void getVariables( hkPointerMap<int, bool>& variables ) const;

			/// A Token is either an operator, number or variable index. A list of Tokens is used as our Reverse Polish Notation (RPN).
		struct Token
		{			
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbCompiledExpressionSet::Token );
			HK_DECLARE_REFLECTION();

			Token()
				:	m_data(0.0f),
					m_type(TOKEN_TYPE_NONE),
					m_operator(OP_NOP)
			{}

				/// Designates the token type: operator, number, or variable index.
			enum TokenType
			{
				TOKEN_TYPE_NONE,
				TOKEN_TYPE_OPERATOR,
				TOKEN_TYPE_NUMBER,
				TOKEN_TYPE_VARIABLE_INDEX,
				TOKEN_TYPE_OPENING_PAREN,
				TOKEN_TYPE_CLOSING_PAREN,
				TOKEN_TYPE_COMMA,
				TOKEN_TYPE_CHARACTER_PROPERTY_INDEX,
			};

			/// The operators allowed in expressions.
			/// New operators should be appended at the end for proper versioning
			enum Operator
			{
				OP_NOP, // nothing

				OP_RAND01,

				OP_LOGICAL_NOT,
				OP_UNARY_MINUS,
				OP_UNARY_PLUS,
				OP_SIN,
				OP_COS,
				OP_ASIN,
				OP_ACOS,
				OP_SQRT,
				OP_FABS,
				OP_CEIL,
				OP_FLOOR,
				OP_SQRTINV,

				OP_MUL,	// "*"
				OP_DIV,	// "/"
				OP_ADD,	// "+"
				OP_SUB,	// "-"
				OP_LOGICAL_OR,	// "||"
				OP_LOGICAL_AND,	// "&&"
				OP_EQ,	// "=="
				OP_NEQ,	// "!="
				OP_LT,	// "<"
				OP_GT,	// ">"
				OP_LEQ,	// "<="
				OP_GEQ,	// ">="
				OP_POW,
				OP_MAX2,
				OP_MIN2,
				OP_RANDRANGE,
				OP_ATAN2APPROX,

				OP_CLAMP,

				OP_MOD, // "%"

				OP_DEG2RAD,
				OP_RAD2DEG,
				OP_COSD,
				OP_SIND,
				OP_ACOSD,
				OP_ASIND,
				OP_ATAN2APPROXD,

				OP_SIGN,
				OP_LERP,
				OP_CLERP,
				OP_COND
			};
	
				
				/// Contains either a number or variable index.
			hkReal m_data; 

				/// Contains the type of the token.
			hkEnum<TokenType,hkInt8> m_type;

				/// Contains an operator 
			hkEnum<Operator, hkInt8> m_operator;

			Token( hkFinishLoadedObjectFlag ) {}
		};

		struct FunctionOpStruct
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR,hkbCompiledExpressionSet::FunctionOpStruct);
			const char* functionName;
			hkEnum<Token::Operator, hkInt8> m_operator;
		};

		static const int m_functionOpCount = 27;
		static FunctionOpStruct m_functionOps[m_functionOpCount];

	private:

			// reverse polish notation structure (list of token)
		hkArray<struct Token> m_rpn; 

			// the indices of the start of each expression in the m_rpn. if the startIndex is -1 or 
			// the start index of an expression is the same as the end index of an expression then the
			// expression is invalid.
		hkArray<int> m_expressionToRpnIndex;

			// number of expressions
		hkInt8 m_numExpressions;

	private:

			// gets the next token from a string
		bool getNextToken(	const hkStringBuf& expression, 
							Token& token, 
							int& currIndex, 
							hkStringMap<int,hkContainerHeapAllocator>* varNameToIdMap,
							hkStringMap<int,hkContainerHeapAllocator>* characterPropertyNameToIdMap,
							const Token& previousToken );

			// gets the next mathematical operator token from a string
		bool getNextOperatorToken(	const hkStringBuf& expression, 
									Token& token, 
									int& currIndex, 
									const Token& previousToken );

			// gets the next functional operator token from a string
		bool getNextFunctionOperatorToken(	const hkStringBuf& expression, 
											Token& token,
											int& currIndex );

			// gets the next number token from a string
		bool getNextNumberToken(	const hkStringBuf& expression, 
									Token& token,
									int& currIndex );

			// returns true if the next character is an operand
		bool isNextCharacterOperand( const hkStringBuf& expression, int currIndex );

			// returns true when the operator is left associative and false when right associative
		bool isOperatorLeftAssociative( Token::Operator op );

			// is the token a function operator
		bool isTokenFunction( Token::Operator op );

			// returns the precedence of a given operator - 5 (highest precedence) to 1 (lowest precedence)
		int getPrecedence( Token::Operator op );

			// get the number of arguments the function takes
		int getFunctionArgumentCount( Token::Operator op );

			// move the currIndex past leading white space
		void skipLeadingWhiteSpace( const hkStringBuf& expression, int& currIndex );

	public:

		hkbCompiledExpressionSet( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_rpn(flag), m_expressionToRpnIndex(flag) {}
};

#include <Behavior/Behavior/Utils/hkbCompiledExpressionSet.inl>

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
