/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_BEHAVIOR_GRAPH_DATA_H
#define HK_BEHAVIOR_GRAPH_DATA_H

#include <Behavior/Behavior/Event/hkbEventInfo.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphStringData.h>
#include <Behavior/Behavior/Variables/hkbVariableInfo.h>
#include <Behavior/Behavior/Variables/hkbVariableValueSet.h>

extern const class hkClass hkbBehaviorGraphDataClass;

class hkbBehaviorGraphStringData;
class hkbVariableValueSet;

/// Constant data associated with a behavior.
/// 
/// Since this data is constant, it may be shared among several instances of hkbBehaviorGraph.
class hkbBehaviorGraphData : public hkReferencedObject
{
	public:
		// +version(3)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBehaviorGraphData();
		virtual ~hkbBehaviorGraphData();

			/// Get the string data associated with the behavior.
		hkbBehaviorGraphStringData* getStringData();

			/// Set the string data associated with the behavior.
		void setStringData( hkbBehaviorGraphStringData* stringData );

			/// Get the event info given an event ID.  This method will pass back a default
			/// hkbEventInfo if m_eventInfos is too short.
		hkbEventInfo getEventInfo( int eventId );

	public:

			/// The default values for the behavior's attributes.
		hkArray<hkReal> m_attributeDefaults;

			/// Definitions of variables used by the behavior.
		hkArray<class hkbVariableInfo> m_variableInfos;

			/// Definitions of the character properties that this behavior requires to be linked.
		hkArray<class hkbVariableInfo> m_characterPropertyInfos;

			/// The information that influences how each event behaviors.
		hkArray<class hkbEventInfo> m_eventInfos;

			/// The min and max values of variables that fit into a word. For all other variables, this array stores zero.
		hkArray<hkbVariableBounds> m_variableBounds;

			/// The initial values of the variables.
		hkbVariableValueSet* m_variableInitialValues;

	public:

			/// Add a variable that fits into 32 bits, with limits.
		template <typename T>
		int addVariableWord( const char* name, hkbVariableInfo::VariableType type, T value, T minValue, T maxValue );

			/// Add a variable of type bool.  This just calls addVariableWorld<bool> with min=false and max=true.
			/// Limits are not used for anything with bool variables.
		void addVariableBool( const char* name, hkbVariableInfo::VariableType type, bool value );

			/// Add a variable that holds an object.
		int addVariableObject( const char* name, hkReferencedObject* obj );

			/// Add a variable that fits into a quadword.
		template <typename T>
		int addVariableQuad( const char* name, hkbVariableInfo::VariableType type, const T& value );

			/// Remove the last variable. For internal use only.
		void removeLastVariable();

			/// Add a character property.
		int addCharacterProperty( const char* name, hkbVariableInfo::VariableType type );

	private:

		hkRefPtr<hkbBehaviorGraphStringData> m_stringData;

	public:

		hkbBehaviorGraphData( hkFinishLoadedObjectFlag flag )
			:	hkReferencedObject(flag),
				m_attributeDefaults(flag),
				m_variableInfos(flag),
				m_characterPropertyInfos(flag),
				m_eventInfos(flag),
				m_variableBounds(flag),
				m_stringData(flag) {}
};

#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphData.inl>

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
