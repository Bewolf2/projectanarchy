/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_VARIABLE_VALUE_SET_H
#define HKB_VARIABLE_VALUE_SET_H

#include <Behavior/Behavior/Variables/hkbVariableValue.h>

extern const class hkClass hkbVariableValueSetClass;

	/// A set of behavior variable values.  The user must keep track of the types of the variables.
class hkbVariableValueSet : public hkReferencedObject
{
	//+vtable(1)
	//+version(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbVariableValueSet() {}

			/// Get the value of a variable that fits into a machine word.
		template <typename T>
		T getWord( int variableIndex ) const;

			/// Get the value of a variable that fits into a quadword.
		template <typename T>
		const T& getQuad( int variableIndex ) const;

			/// Get the value of a variable that is an object.
		hkReferencedObject* getObject( int variableIndex ) const;

			/// Set the value of a variable that fits into a machine word.
		template <typename T>
		void setWord( int variableIndex, T value );

			/// Set the value of a variable that fits into a quadword.
		template <typename T>
		void setQuad( int variableIndex, const T& value );

			/// Set the value of a variable that is a variant.
		void setObject( int variableIndex, hkReferencedObject* obj );

			/// Get the value of a variable.  If the variable does not fit into a word then this will contain an index
			/// into m_quadVariableValues or m_variantVariableValue.
		hkbVariableValue getVariableValue( int variableIndex ) const;

			/// Access a variable that fits into a machine word.
		hkbVariableValue& accessWord( int variableIndex );

			/// Access the value of a variable that is an object.
		hkRefVariant& accessObject( int variableIndex );

			/// Add a variable value that fits into a machine word.  It will be placed at the end of the list.
		template <typename T>
		int addWord( T value );

			/// Add a variable value that fits into a quadword.  It will be placed at the end of the list.
		template <typename T>
		int addQuad( const T& value );

			/// Add a variable value that is an object.  It will be placed at the end of the list.
		int addObject( hkReferencedObject* obj );

			/// Remove the last word variable. For internal use only.
		void removeLastWord();

			/// Remove the last quad variable. For internal use only.
		void removeLastQuad();

			/// Remove the last object variable. For internal use only.
		void removeLastObject();

			/// Get the number of variable values stored.
		int getNumValues() const;

			/// Assignment operator.
		void operator = ( const hkbVariableValueSet& valueSet );
		
			/// Copys word and quad values only.  Does not copy objects.
		void copyWordAndQuadValues( const hkbVariableValueSet& valueSet );
		
			/// Remove all values.
		void clear();

	private:

			// The values of variables that fit into a word.  For all other variables, this array stores the index
			// into one of the other arrays below.
		hkArray<class hkbVariableValue> m_wordVariableValues;

			// The values of the variables that fit into a quadword.
		hkArray<hkVector4> m_quadVariableValues;

			// The values of the variables that are variants.
		hkArray<hkRefVariant> m_variantVariableValues;

	public:

		hkbVariableValueSet( hkFinishLoadedObjectFlag flag )
			:	hkReferencedObject(flag),
				m_wordVariableValues(flag),
				m_quadVariableValues(flag),
				m_variantVariableValues(flag) {}

		friend class hkbCpuBehaviorJob;

#if defined (HK_PLATFORM_HAS_SPU)
		friend class hkbSpuBehaviorUtils;
#endif
};

#include <Behavior/Behavior/Variables/hkbVariableValueSet.inl>

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
