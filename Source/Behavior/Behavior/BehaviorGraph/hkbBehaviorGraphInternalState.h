/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_GRAPH_INTERNAL_STATE_H
#define HKB_BEHAVIOR_GRAPH_INTERNAL_STATE_H

#include <Behavior/Behavior/Variables/hkbVariableValueSet.h>
#include <Behavior/Behavior/Generator/hkbGeneratorSyncInfo.h>

extern const class hkClass hkbNodeInternalStateInfoClass;

extern const class hkClass hkbBehaviorGraphInternalStateClass;

	/// The internal state of a node along with its name and sync info.
class hkbNodeInternalStateInfo : public hkReferencedObject
{
	public:
		//+vtable(1)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbNodeInternalStateInfo() {}
		hkbNodeInternalStateInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_syncInfo(flag), m_name(flag), m_internalState(flag) {}

			/// The sync info for the node.
		hkRefPtr<hkbReferencedGeneratorSyncInfo> m_syncInfo;

			/// The name of the node.
		hkStringPtr m_name;

			/// The internal state of the node.
		hkRefPtr<hkReferencedObject> m_internalState;

			/// The ID of the node (hkbNode::m_id).
		hkUint16 m_nodeId;

			/// See hkbNodeInfo::FLAG_HAS_ACTIVATE_BEEN_CALLED
		hkBool m_hasActivateBeenCalled;

			/// See hkbNodeInfo::FLAG_IS_MODIFIER_ENABLED
		hkBool m_isModifierEnabled;
};

	/// The internal state of a behavior graph.
class hkbBehaviorGraphInternalState : public hkReferencedObject
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBehaviorGraphInternalState() {}
		hkbBehaviorGraphInternalState( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_nodeInternalStateInfos(flag), m_variableValueSet(flag) {}

			/// The state of each node.
		hkArray< hkRefPtr<class hkbNodeInternalStateInfo> > m_nodeInternalStateInfos;

			/// The value of variables.
		hkRefPtr<hkbVariableValueSet> m_variableValueSet;
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
