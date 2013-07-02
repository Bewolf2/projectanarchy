/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_DEMOS_VARIABLE_TWEAKING_HELPER_H
#define HK_DEMOS_VARIABLE_TWEAKING_HELPER_H


extern const class hkClass hkVariableTweakingHelperVector4VariableInfoClass;

extern const class hkClass hkVariableTweakingHelperRealVariableInfoClass;

extern const class hkClass hkVariableTweakingHelperIntVariableInfoClass;

extern const class hkClass hkVariableTweakingHelperBoolVariableInfoClass;

extern const class hkClass hkVariableTweakingHelperClass;

class hkbBehaviorGraph;

/// This class provides a more tweaker friendly interface to behavior variables.
/// The names of the variables and their values are stored explicitly so
/// that they show up in the tweaker menu.  Once per frame, updateVariables()
/// should be called to propagate the values stored in this class to the
/// actual variables.
class hkVariableTweakingHelper
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_UTILITIES, hkVariableTweakingHelper);
		HK_DECLARE_REFLECTION();

		hkVariableTweakingHelper();
		hkVariableTweakingHelper( hkbBehaviorGraph* behavior );
		~hkVariableTweakingHelper();
		hkVariableTweakingHelper( hkFinishLoadedObjectFlag f );

		/// set the values of the variables from the ones being tweaked
		void updateVariables();

		/// find the index of a variable in the list of bools
		int getBoolVariableIndex( const char* name );

		/// find the index of a variable in the list of ints
		int getIntVariableIndex( const char* name );

		/// find the index of a variable in the list of reals
		int getRealVariableIndex( const char* name );

		/// find the index of a variable in the list of vector4s
		int getVector4VariableIndex( const char* name );

		/// turn on and off tweaking for a variable
		void setTweakOn( const char* variableName, bool tweakOn );
		
		struct BoolVariableInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkVariableTweakingHelper::BoolVariableInfo );
			HK_DECLARE_REFLECTION();

			BoolVariableInfo() {}

			hkStringPtr m_name;
			hkBool m_value;
			hkBool m_tweakOn;

			BoolVariableInfo( hkFinishLoadedObjectFlag flag ) : m_name(flag) {} 
		};

		struct IntVariableInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkVariableTweakingHelper::IntVariableInfo );
			HK_DECLARE_REFLECTION();

			IntVariableInfo() {}

			hkStringPtr m_name;
			hkInt32 m_value;
			hkBool m_tweakOn;

			IntVariableInfo( hkFinishLoadedObjectFlag flag ) : m_name(flag) {}
		};

		struct RealVariableInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkVariableTweakingHelper::RealVariableInfo );
			HK_DECLARE_REFLECTION();

			RealVariableInfo() {}

			hkStringPtr m_name;
			hkReal m_value;
			hkBool m_tweakOn;

			RealVariableInfo( hkFinishLoadedObjectFlag flag ) : m_name(flag) {}
		};

		struct Vector4VariableInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkVariableTweakingHelper::Vector4VariableInfo );
			HK_DECLARE_REFLECTION();

			Vector4VariableInfo() {}

			hkStringPtr m_name;
			hkReal m_x;
			hkReal m_y;
			hkReal m_z;
			hkReal m_w;
			hkBool m_tweakOn;

			Vector4VariableInfo( hkFinishLoadedObjectFlag flag ) : m_name(flag) {}
		};

		hkArray<struct hkVariableTweakingHelper::BoolVariableInfo> m_boolVariableInfo;
		hkArray<struct hkVariableTweakingHelper::IntVariableInfo> m_intVariableInfo;
		hkArray<struct hkVariableTweakingHelper::RealVariableInfo> m_realVariableInfo;
		hkArray<struct hkVariableTweakingHelper::Vector4VariableInfo> m_vector4VariableInfo;

	private:

		hkbBehaviorGraph* m_behavior; //+nosave

		// In these lists we store the variable indices (as defined by the behavior)
		// for each of the structs stored in the above lists.  We store this information
		// separately because it is not meant to be tweaked.
		hkArray<int> m_boolIndices; //+nosave
		hkArray<int> m_intIndices; //+nosave
		hkArray<int> m_realIndices; //+nosave
		hkArray<int> m_vector4Indices; //+nosave
};

#endif // HK_DEMOS_VARIABLE_TWEAKING_HELPER_H

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
