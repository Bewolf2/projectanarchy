/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template <typename T>
HK_FORCE_INLINE T hkbVariableValue::get() const
{
	HK_COMPILE_TIME_ASSERT( sizeof(T) <= 4 );

	// We use numeric conversion instead of just stuffing the bytes so that
	// we can still retrieve the value after endianness conversion.
	// This function is used for hkInt8, hkInt16 and hkInt32.
	// Template overrides below are used for bool, hkBool and hkReal. 
	return (T)m_value;
}

template <>
HK_FORCE_INLINE bool hkbVariableValue::get() const
{
	// This template override avoids a warning on some platforms
	// but is functionally the same as the general template.
	return ( m_value != 0 );
}

template <>
HK_FORCE_INLINE hkBool hkbVariableValue::get() const
{
	// This template override avoids a warning on some platforms
	// but is functionally the same as the general template.
	return ( m_value != 0 );
}

template <>
HK_FORCE_INLINE hkReal hkbVariableValue::get() const
{
	// In the case of hkReals we cannot perform numeric conversion so
	// we stuff the bytes directly.  This works fine with endianness
	// conversion as long as the platform does its floating point
	// endianness the same as its integer endianness.

	union
	{
		hkFloat32 r;
		int i;
	} u;
	
	u.i = m_value;
	return hkReal(u.r);
}

template <typename T>
HK_FORCE_INLINE void hkbVariableValue::set( T value )
{
	HK_COMPILE_TIME_ASSERT( sizeof(T) <= 4 );

	m_value = (int)value;
}

template <>
HK_FORCE_INLINE void hkbVariableValue::set( hkReal value )
{
	union
	{
		hkFloat32 r;
		int i;
	} u;
	
	u.r = hkFloat32(value);
	m_value = u.i;
}

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
