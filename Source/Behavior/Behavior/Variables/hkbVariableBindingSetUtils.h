/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_VARIABLE_BINDING_SET_UTILS_H
#define HKB_VARIABLE_BINDING_SET_UTILS_H

#include <Behavior/Behavior/Variables/hkbVariableBindingSet.h>

class hkbCharacterSetup;
class hkbSymbolIdMap;
class hkbVariableValueSet;
class hkbVariableValue;
class hkbVariableInfo;
struct Binding;

	/// The information required for syncing variables on the SPU.
struct hkbSyncVariableSetup
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbSyncVariableSetup );

		/// The current value of the behavior variables (from the root behavior).
	HK_ALIGN16( hkbVariableValueSet* m_variableValues );

		/// The values of the character properties that the character associated with behavior has.
	hkbVariableValueSet* m_characterPropertyValues;

		/// A map from internal variable IDs to those in m_variableValues.
	const hkInt32* m_variableIdMap;

		/// A map from internal character property IDs to those in m_characterPropertyValues.
	const hkInt32* m_characterPropertyIdMap;

		/// Definitions of variables used by the local behavior (not the root behavior).
	hkbVariableInfo* m_variableInfos;

		/// Definitions of the character properties used by the local behavior (not the root behavior).
		/// Definitions of the character properties that this behavior requires to be linked.
	hkbVariableInfo* m_characterPropertyInfos;

		/// The min and max values of variables that fit into a word.
	hkbVariableBounds* m_variableBounds;

		/// The number of behavior variable infos and also the size of m_variableIdMap.
	int m_numVariableInfos;

		/// The number of character property infos and also the size of m_characterPropertyIdMap.
	int m_numCharacterPropertyInfos;

		/// The number of minimum and maximum values.
	int m_numMinMaxVariableValues;

		/// This is set by the variable syncing code and DMAed back if any modifier's m_enable property changed.
	bool m_hasEnableChanged;

	void setRootBehavior( hkbBehaviorGraph& rootBehavior );
	void setCurrentBehavior( hkbBehaviorGraph& currentBehavior );
	void setCharacterSetup( const hkbCharacterSetup& characterSetup );
};

	/// Used internally by the hkbVariableBindingSet.
class hkbVariableBindingSetUtils
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR,hkbVariableBindingSetUtils);

		/// Core code of copying variable values into the bound objects.
		static void HK_CALL copyVariablesToMembersInternal( hkbSyncVariableSetup& syncVariableSetup,
															const hkbVariableBindingSet::Binding& b,
															const bool isBindingToEnable,
															hkbBindable* bindable,
															bool reverse );

	private:
		
		// Compute the member data pointer for a binding.
		static void* HK_CALL computeMemberDataPtr( const hkbVariableBindingSet::Binding& binding, hkbBindable* bindable );

		// Returns true if a bool property changed (when reverse is false).
		template<typename VariableType>
		static bool HK_CALL copyVariableToMemberInternal(	hkbSyncVariableSetup& syncVariableSetup,
															const hkbVariableBindingSet::Binding& b,
															hkbVariableValue& value,
															int variableIndex,
															void* memberDataPtr,
															const bool reverse );

		template<typename VariableType, typename MemberType>
		class VariableToMemberCopier
		{
			public:
				HK_FORCE_INLINE static void HK_CALL copy(	hkbSyncVariableSetup& syncVariableSetup,
															hkbVariableValue& value,
															int variableIndex,
															MemberType* memberDataPtr,
															const bool reverse);
		};

		template <typename VariableType>
		class VariableToMemberCopier<VariableType, hkBool>
		{
			public:
				// Returns true if the property changed (when reverse is false).
				HK_FORCE_INLINE static bool HK_CALL copy(	hkbVariableValue& value,
															int variableIndex,
															hkBool* memberDataPtr,
															const bool reverse );
		};

		template<typename VariableType, typename MemberType>
		class MemberTypeToVariableTypeConverter
		{
			public:
				HK_FORCE_INLINE static VariableType HK_CALL convert( MemberType* member );
		};

		template<typename MemberType>
		class MemberTypeToVariableTypeConverter<bool, MemberType>
		{
			public:
				HK_FORCE_INLINE static bool HK_CALL convert( MemberType* member );
		};

		// Whether the variable value is finite or not.
		template<typename VariableType>
		HK_FORCE_INLINE static bool HK_CALL checkIsFinite( VariableType value );

};

#include <Behavior/Behavior/Variables/hkbVariableBindingSetUtils.inl>

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
