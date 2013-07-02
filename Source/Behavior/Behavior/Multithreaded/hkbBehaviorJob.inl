/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbBehaviorJob::hkbBehaviorJob( hkbBehaviorJob::JobSubType subType, hkUint16 size, hkJobSpuType jobSpuType )
:	hkJob(HK_JOB_TYPE_BEHAVIOR, subType, size, jobSpuType),
	m_behaviorGenerateData(HK_NULL)
{
}

HK_FORCE_INLINE hkbGenerateBehaviorGraphJob::hkbGenerateBehaviorGraphJob() 
:	hkbBehaviorJob(BEHAVIOR_JOB_GENERATE_BEHAVIOR_GRAPH, sizeof(hkbGenerateBehaviorGraphJob), HK_JOB_SPU_TYPE_DISABLED )
{
}

HK_FORCE_INLINE hkbGenerateNodesJob::hkbGenerateNodesJob()
:	hkbBehaviorJob(BEHAVIOR_JOB_GENERATE_NODES, sizeof(hkbGenerateNodesJob), HK_JOB_SPU_TYPE_ENABLED )
{
}

HK_FORCE_INLINE hkbUpdateCharacterJob::hkbUpdateCharacterJob( hkbCharacter& character, hkbContext& context, hkReal deltaTime )
:	hkbBehaviorJob(BEHAVIOR_JOB_UPDATE_CHARACTER, sizeof(hkbUpdateCharacterJob), HK_JOB_SPU_TYPE_DISABLED ),
	m_character(character),
	m_context(context),
	m_deltaTime(deltaTime)
{
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
