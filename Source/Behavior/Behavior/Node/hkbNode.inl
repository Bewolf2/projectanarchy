/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbNode::ChildrenInfo::ChildrenInfo( hkArray<hkbNodeChildInfo>& childInfos ) 
:	m_childInfos(childInfos)
{
}

HK_FORCE_INLINE hkbNode::CloneState hkbNode::getCloneState() const
{
	return m_cloneState;
}

HK_FORCE_INLINE void hkbNode::setCloneState( hkbNode::CloneState cloneState )
{
	m_cloneState = cloneState;
}

HK_FORCE_INLINE bool hkbNode::canBeTemplate() const
{
	return ( getCloneState() != CLONE_STATE_CLONE );
}

HK_FORCE_INLINE bool hkbNode::canBeClone() const
{
	return ( getCloneState() != CLONE_STATE_TEMPLATE );
}

HK_FORCE_INLINE hkbNodeType hkbNode::getType() const
{
	HK_ASSERT( 0x93b48a61, m_type != HKB_NODE_TYPE_INVALID );
	return m_type;
}

HK_FORCE_INLINE bool hkbNode::isGenerator() const
{
	return (m_type >= HKB_NODE_TYPE_FIRST_GENERATOR) && (m_type < HKB_NODE_TYPE_FIRST_MODIFIER);
}

HK_FORCE_INLINE bool hkbNode::isModifier() const
{
	return (m_type >= HKB_NODE_TYPE_FIRST_MODIFIER);;
}

HK_FORCE_INLINE bool hkbNode::isTransitionEffect() const
{
	return (m_type >= HKB_NODE_TYPE_FIRST_TRANSITION_EFFECT) && (m_type < HKB_NODE_TYPE_FIRST_MODIFIER);
}

HK_FORCE_INLINE	void hkbNode::insertSymbolId( int symbolId, hkPointerMap<int, bool>& symbolIds )
{
	if ( symbolId >= 0 )
	{
		symbolIds.insert( symbolId, true );
	}
}
// can't construct hkbNodeInfo on SPU
#if !defined(HK_PLATFORM_SPU)

HK_FORCE_INLINE hkbNodeInfo& hkbNode::accessNodeInfo()
{
	HK_ASSERT2( 0xf871b26a, canBeClone(), "clone expected" );
	HK_ASSERT( 0xd89351b2, m_nodeInfo != HK_NULL );

	return *m_nodeInfo;
}

HK_FORCE_INLINE hkbNodeInfo& hkbNode::createAndAccessNodeInfo()
{
	HK_ASSERT2( 0x9ab712b7, canBeClone(), "clone expected" );

	if ( m_nodeInfo == HK_NULL )
	{
		m_nodeInfo = new hkbNodeInfo( this );
	}

	return *m_nodeInfo;
}

#endif

/// Returns the node's info.
HK_FORCE_INLINE const hkbNodeInfo& hkbNode::getNodeInfo() const
{
	HK_ASSERT2( 0xf871b26a, canBeClone(), "clone expected" );
	HK_ASSERT2( 0xd81b9461, m_nodeInfo != HK_NULL, "the node info should have been created before calling getNodeInfo" );

	return *m_nodeInfo;	
}

HK_FORCE_INLINE bool hkbNode::hasNodeInfo() const
{
	return ( m_nodeInfo != HK_NULL );
}

HK_FORCE_INLINE void hkbNode::discardNodeInfo()
{
	HK_ASSERT2( 0xd894b571, m_nodeInfo != HK_NULL, "discardNodeInfo should only be called if there is node info" );
	delete m_nodeInfo;
	m_nodeInfo = HK_NULL;
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
