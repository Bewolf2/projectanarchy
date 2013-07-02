/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbBehaviorGraph::GenerateWorkingData::GenerateWorkingData(	hkbBehaviorGraph& behaviorGraph, 
																			const hkbContext& context, 
																			hkbGeneratorOutput& output, 
																			hkReal timeOffset )
:	m_behaviorGraph(behaviorGraph),
	m_context(context),
	m_output(output),
	m_outputTracksBuffer(HK_NULL),
	m_outputTracksBufferOffset(HK_NULL),
	m_timeOffsets(HK_NULL),
	m_nextData(HK_NULL),
	m_timeOffset(timeOffset),
	m_nodeIndex(0),
	m_currentSpuPartitionIndex(-1),
	m_setCharacterPose(true),
	m_initAtStartUp(true)
{
}

template <typename T>
HK_FORCE_INLINE T hkbBehaviorGraph::getVariableValueWord( int externalVariableIndex )
{
	HK_ASSERT(0x29207d08, m_variableIdMap != HK_NULL);
	int internalVariableIndex = m_variableIdMap->getInternal( externalVariableIndex );
	return m_variableValueSet->getWord<T>( internalVariableIndex );
}

template <typename T>
HK_FORCE_INLINE const T& hkbBehaviorGraph::getVariableValueQuad( int externalVariableIndex )
{
	HK_ASSERT(0x29207d08, m_variableIdMap != HK_NULL);
	int internalVariableIndex = m_variableIdMap->getInternal( externalVariableIndex );
	return m_variableValueSet->getQuad<T>( internalVariableIndex );
}

HK_FORCE_INLINE hkReferencedObject* hkbBehaviorGraph::getVariableValueObject( int externalVariableIndex )
{
	HK_ASSERT(0x29207d08, m_variableIdMap != HK_NULL);
	int internalVariableIndex = m_variableIdMap->getInternal( externalVariableIndex );
	return m_variableValueSet->getObject( internalVariableIndex );
}

template <typename T>
HK_FORCE_INLINE void hkbBehaviorGraph::setVariableValueWord( int externalVariableIndex, T value, bool clamp )
{
	HK_ASSERT(0x29207d08, m_variableIdMap != HK_NULL);
	HK_ASSERT2(0xd8f93eb1, m_variableValueSet != HK_NULL, "You must activate the behavior graph before setting variables.");

	int internalVariableIndex = m_variableIdMap->getInternal( externalVariableIndex );

	if( clamp )
	{
		const hkbVariableBounds& bounds = m_data->m_variableBounds[ internalVariableIndex ];
		value = hkMath::clamp( value, bounds.m_min.get<T>(), bounds.m_max.get<T>() );
	}

	m_variableValueSet->setWord<T>( internalVariableIndex, value );
}

template <typename T>
HK_FORCE_INLINE void hkbBehaviorGraph::setVariableValueQuad( int externalVariableIndex, const T& value )
{
	HK_ASSERT(0x29207d08, m_variableIdMap != HK_NULL);
	int internalVariableIndex = m_variableIdMap->getInternal( externalVariableIndex );
	m_variableValueSet->setQuad<T>( internalVariableIndex, value );
}

HK_FORCE_INLINE void hkbBehaviorGraph::setVariableValueObject( int externalVariableIndex, hkReferencedObject* obj )
{
	HK_ASSERT(0x29207d08, m_variableIdMap != HK_NULL);
	int internalVariableIndex = m_variableIdMap->getInternal( externalVariableIndex );
	m_variableValueSet->setObject( internalVariableIndex, obj );
}

HK_FORCE_INLINE bool hkbBehaviorGraph::hasVariable( int externalVariableIndex ) const
{
	if(m_variableIdMap == HK_NULL)
	{
		return false;
	}

	return m_variableIdMap->getInternal( externalVariableIndex ) != -1;
}

HK_FORCE_INLINE hkbGenerator* hkbBehaviorGraph::getRootGenerator() const
{ 
	return m_rootGenerator; 
}

HK_FORCE_INLINE bool hkbBehaviorGraph::isActive() const
{ 
	return m_isActive; 
}

HK_FORCE_INLINE bool hkbBehaviorGraph::isActive( const hkbNode* nodeTemplate ) const
{
	HK_ASSERT2( 0xd78361c7, isActive(), "you should not call this on an inactive graph" );
	HK_ASSERT2( 0x4671b83c, nodeTemplate != this, "you should not pass this the root graph" );

	const hkbNode* nodeClone = getNodeClone( const_cast<hkbNode*>(nodeTemplate) );

	return (nodeClone != HK_NULL) && nodeClone->hasNodeInfo() && nodeClone->getNodeInfo().hasActivateBeenCalled();
}

HK_FORCE_INLINE hkbBehaviorGraphData* hkbBehaviorGraph::getData() const
{ 
	return m_data; 
}

HK_FORCE_INLINE hkbVariableValueSet* hkbBehaviorGraph::accessVariableValues()
{
	return m_variableValueSet;
}

HK_FORCE_INLINE hkbSymbolIdMap* hkbBehaviorGraph::getEventIdMap() const
{
	return m_eventIdMap;
}

HK_FORCE_INLINE hkbSymbolIdMap* hkbBehaviorGraph::getAttributeIdMap() const
{
	return m_attributeIdMap;
}

HK_FORCE_INLINE hkbSymbolIdMap* hkbBehaviorGraph::getVariableIdMap() const
{
	return m_variableIdMap;
}

HK_FORCE_INLINE hkbSymbolIdMap* hkbBehaviorGraph::getCharacterPropertyIdMap() const
{
	return m_characterPropertyIdMap;
}

HK_FORCE_INLINE int hkbBehaviorGraph::getInternalEventId( int externalEventId ) const
{
	if ( m_eventIdMap == HK_NULL )
	{
		return externalEventId;
	}
	else
	{
		return m_eventIdMap->getInternal( externalEventId );
	}
}

HK_FORCE_INLINE int hkbBehaviorGraph::getExternalEventId( int internalEventId ) const
{
	if ( m_eventIdMap == HK_NULL )
	{
		return internalEventId;
	}
	else
	{
		return m_eventIdMap->getExternal( internalEventId );
	}
}

bool HK_FORCE_INLINE hkbBehaviorGraph::isLinked() const
{
	return m_isLinked;
}

void HK_FORCE_INLINE hkbBehaviorGraph::setIsLinked( bool linked )
{
	m_isLinked = linked;
	if ( !linked )
	{
		m_id = 0;
		m_nextUniqueId = 0;
	}
}

HK_FORCE_INLINE const hkbBehaviorGraph::NodeList& hkbBehaviorGraph::getActiveNodes() const 
{	
	return *m_activeNodes; 
}

HK_FORCE_INLINE hkbBehaviorGraph::NodeList& hkbBehaviorGraph::getActiveNodes()
{	
	return *m_activeNodes; 
}

template <hkbNode::TemplateOrClone templateOrClone>
HK_FORCE_INLINE hkbNodeInfo& hkbBehaviorGraph::accessNodeInfo( const hkbNode* node )
{
	hkbNode* nodeClone = const_cast<hkbNode*>( node );

	if ( templateOrClone == NODE_IS_CLONE )
	{
		HK_ASSERT2( 0xdf98a8d7, node->canBeClone(), "clone expected" );
	}
	else
	{
		HK_ASSERT2( 0x981b46c2, node->canBeTemplate(), "template expected" );
		nodeClone = getNodeClone( nodeClone );
	}

	return nodeClone->accessNodeInfo();
}

template <hkbNode::TemplateOrClone templateOrClone>
HK_FORCE_INLINE const hkbNodeInfo& hkbBehaviorGraph::getNodeInfo( const hkbNode* node ) const
{
	hkbNode* nodeClone = const_cast<hkbNode*>( node );

	if ( templateOrClone == NODE_IS_CLONE )
	{
		HK_ASSERT2( 0xdf98a8d7, node->canBeClone(), "clone expected" );
	}
	else
	{
		HK_ASSERT2( 0x981b46c2, node->canBeTemplate(), "template expected" );
		nodeClone = getNodeClone( nodeClone );
	}

	return nodeClone->getNodeInfo();
}

HK_FORCE_INLINE const hkbNodeInfo& hkbBehaviorGraph::getNodeInfoIsCloneIffIsTransitionEffect( const hkbNode* node ) const
{
	if ( node->isTransitionEffect() )
	{
		return getNodeInfo<NODE_IS_CLONE>( node );
	}
	else
	{
		return getNodeInfo<NODE_IS_TEMPLATE>( node );
	}
}

template <hkbNode::TemplateOrClone templateOrClone>
HK_FORCE_INLINE hkbGeneratorSyncInfo& hkbBehaviorGraph::accessSyncInfo( const hkbGenerator* gen )
{
	if ( templateOrClone == NODE_IS_TEMPLATE )
	{
		HK_ASSERT2( 0x981b46c2, gen->canBeTemplate(), "template expected" );
		gen = static_cast<hkbGenerator*>( getNodeClone( const_cast<hkbGenerator*>(gen) ) );
	}
	else
	{
		HK_ASSERT2( 0xdf98a8d7, gen->canBeClone(), "clone expected" );
	}

	return const_cast<hkbGenerator*>(gen)->accessSyncInfo();
}

template <hkbNode::TemplateOrClone templateOrClone>
HK_FORCE_INLINE const hkbGeneratorSyncInfo& hkbBehaviorGraph::getSyncInfo( const hkbGenerator* gen ) const
{
	if ( templateOrClone == NODE_IS_TEMPLATE )
	{
		HK_ASSERT2( 0x981b46c2, gen->canBeTemplate(), "template expected" );
		gen = static_cast<hkbGenerator*>( getNodeClone( const_cast<hkbGenerator*>(gen) ) );
	}
	else
	{
		HK_ASSERT2( 0xdf98a8d7, gen->canBeClone(), "clone expected" );
	}

	return gen->getSyncInfo();
}

HK_FORCE_INLINE const hkbGeneratorSyncInfo& hkbBehaviorGraph::getSyncInfoIsCloneIffIsTransitionEffect( const hkbGenerator* gen ) const
{
	if ( gen->isTransitionEffect() )
	{
		return getSyncInfo<NODE_IS_CLONE>( gen );
	}
	else
	{
		return getSyncInfo<NODE_IS_TEMPLATE>( gen );
	}
}

template <hkbNode::TemplateOrClone fromTemplateOrClone, hkbNode::TemplateOrClone toTemplateOrClone>
HK_FORCE_INLINE void hkbBehaviorGraph::copySyncInfo( const hkbGenerator* from, const hkbGenerator* to )
{
	const hkbGeneratorSyncInfo& fromSyncInfo = getSyncInfo<fromTemplateOrClone>( from );
	hkbGeneratorSyncInfo& toSyncInfo = accessSyncInfo<toTemplateOrClone>( to );

	toSyncInfo = fromSyncInfo;
}

HK_FORCE_INLINE bool hkbBehaviorGraph::isShallowClone() const
{ 
	return ( m_nodeTemplateToCloneMap != HK_NULL ); 
}

HK_FORCE_INLINE const hkArray<hkbGenerateNodesJob*>& hkbBehaviorGraph::getBehaviorJobs() const
{
	return m_jobs;
}

HK_FORCE_INLINE hkPseudoRandomGenerator* hkbBehaviorGraph::getPseudoRandomGenerator() const
{
	return m_pseudoRandomGenerator;
}

HK_FORCE_INLINE hkbStateMachine* hkbBehaviorGraph::getStateMachineTemplateFromId( hkUint16 id ) const
{
	HK_ASSERT2( 0x4571bd81, m_globalTransitionData != HK_NULL, "did you call computeUniqueNodeIds()?");

	hkbStateMachine* stateMachine = HK_NULL;
	stateMachine = m_globalTransitionData->m_idToStateMachineTemplateMap.getWithDefault( id, HK_NULL );

	HK_ASSERT( 0xa6e3541b, stateMachine != HK_NULL );

	return stateMachine;
}

HK_FORCE_INLINE hkbBehaviorGraph* hkbBehaviorGraph::getBehaviorTemplateFromStateMachineTemplate( hkbStateMachine* stateMachineTemplate ) const
{
	HK_ASSERT2( 0x4571bd81, m_globalTransitionData != HK_NULL, "did you call computeUniqueNodeIds()?");

	int index = m_globalTransitionData->m_stateMachineTemplateToIndexMap.getWithDefault( stateMachineTemplate, -1 );

	HK_ASSERT(0x7590ad03, index >= 0);

	return m_globalTransitionData->m_stateMachineInfos[index].m_behaviorTemplate;
}

HK_FORCE_INLINE const hkInt32* hkbBehaviorGraph::getInternalToRootVariableIdMap() const
{
	return m_internalToRootVariableIdMap.begin();
}

HK_FORCE_INLINE const hkInt32* hkbBehaviorGraph::getInternalToCharacterCharacterPropertyIdMap() const
{
	return m_internalToCharacterCharacterPropertyIdMap.begin();
}

HK_FORCE_INLINE const hkInt32* hkbBehaviorGraph::getInternalToRootAttributeIdMap() const
{
	return m_internalToRootAttributeIdMap.begin();
}

HK_FORCE_INLINE void hkbBehaviorGraph::requestUpdateActiveNodesForEnable( bool ifThisIsTrue )
{
	m_updateActiveNodesForEnable = (bool)m_updateActiveNodesForEnable || ifThisIsTrue;
	m_updateActiveNodes = m_updateActiveNodes || m_updateActiveNodesForEnable;
}

HK_FORCE_INLINE void hkbBehaviorGraph::relinquishUniqueIdForTransitionEffect( hkUint16 id )
{
	HK_ASSERT2( 0xc892b164, ( id >= FIRST_TRANSITION_EFFECT_ID ), "ID is out of range for a Transition Effect" );

	m_uniqueIdPool.pushBack( id );
}

HK_FORCE_INLINE hkbBehaviorGraph* hkbBehaviorGraph::getTemplate()
{
	if(m_template != HK_NULL)
	{
		return m_template.val();
	}
	else
	{
		return this;
	}
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
