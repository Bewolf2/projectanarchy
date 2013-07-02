/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_VARIABLE_VALUE_H
#define HKB_VARIABLE_VALUE_H


extern const class hkClass hkbVariableValueClass;

	// Stores the value of a variable that can be retrieved as various simple types.
	// 
	// This class does not allow conversion of types.  You should only get the
	// value of the variable as the appropriate type.  For example, if you call setReal()
	// and then getInt(), the results will not be meaningful.
class hkbVariableValue
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbVariableValue );
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

			/// Get a variable value for a type that fits in 4 bytes.
		template <typename T>
		T get() const;

			/// Set a variable value for a type that fits in 4 bytes.
		template <typename T>
		void set( T value );

	private:

			// all values are 4 bytes so we use an int
		int m_value;
};

	/// The minimum and maximum value for a word-sized behavior variable.
struct hkbVariableBounds
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbVariableBounds );
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

	hkbVariableValue m_min;
	hkbVariableValue m_max;
};

#include <Behavior/Behavior/Variables/hkbVariableValue.inl>

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
