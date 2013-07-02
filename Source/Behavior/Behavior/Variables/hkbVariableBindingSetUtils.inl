/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template<typename VariableType, typename MemberType>
HK_FORCE_INLINE void hkbVariableBindingSetUtils::VariableToMemberCopier<VariableType, MemberType>::copy(	hkbSyncVariableSetup& syncVariableSetup,
																											hkbVariableValue& value,
																											int variableIndex,
																											MemberType* memberDataPtr,
																											const bool reverse )
{
	if ( reverse )
	{
		VariableType memberValue = hkbVariableBindingSetUtils::MemberTypeToVariableTypeConverter<VariableType, MemberType>::convert( memberDataPtr );

		if ( checkIsFinite( memberValue ) )
		{
			const hkbVariableBounds& bounds = syncVariableSetup.m_variableBounds[variableIndex];
			memberValue = hkMath::clamp(	memberValue,
														bounds.m_min.get<VariableType>(),
														bounds.m_max.get<VariableType>() );

			value.set<VariableType>( memberValue );
		}
	}
	else
	{
		*memberDataPtr = static_cast<MemberType>( value.get<VariableType>() );
	}
}

template<typename VariableType>
HK_FORCE_INLINE bool hkbVariableBindingSetUtils::VariableToMemberCopier<VariableType, hkBool>::copy(	hkbVariableValue& value,
																										int variableIndex,
																										hkBool* memberDataPtr,
																										const bool reverse )
{
	if ( reverse )
	{
		VariableType memberValue = ( static_cast<VariableType>( *memberDataPtr ) != 0 );
		value.set<VariableType>( memberValue );

		return false;
	}
	else
	{
		hkBool oldValue = *memberDataPtr;
		*memberDataPtr = (value.get<VariableType>() != 0);

		// If this is a binding to m_enable and the value changes then
		// we need to force an update of the active nodes list.
		return ( oldValue != *memberDataPtr );
	}
}

template<typename VariableType>
HK_FORCE_INLINE bool hkbVariableBindingSetUtils::checkIsFinite( VariableType value )
{
	return true;
}

template<>
HK_FORCE_INLINE bool hkbVariableBindingSetUtils::checkIsFinite<hkReal>( hkReal value )
{
	return hkMath::isFinite( value );
}

template<typename VariableType, typename MemberType>
HK_FORCE_INLINE VariableType hkbVariableBindingSetUtils::MemberTypeToVariableTypeConverter<VariableType, MemberType>::convert( MemberType* member )
{
	return static_cast<VariableType>( *member );
}

template<typename MemberType>
HK_FORCE_INLINE bool hkbVariableBindingSetUtils::MemberTypeToVariableTypeConverter<bool, MemberType>::convert( MemberType* member )
{
	return ( *member != 0 );
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
