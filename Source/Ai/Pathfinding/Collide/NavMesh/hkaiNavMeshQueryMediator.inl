/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>

inline hkaiNavMeshQueryMediator::hkaiNavMeshQueryMediator()
{

}

inline hkaiNavMeshQueryMediator::hkaiNavMeshQueryMediator( hkFinishLoadedObjectFlag& f )
:	hkReferencedObject(f) 
{

}

inline hkaiRuntimeIndex hkaiNavMeshQueryMediator::getRuntimeIndexForMeshInstance( const hkaiNavMeshInstance* meshInstance )
{ 
	if (meshInstance)
	{
		return hkMath::max2( meshInstance->getRuntimeId(), 0 ); 
	}
	else
		return 0;
}

inline hkaiNavMeshQueryMediator::QueryInputBase::QueryInputBase()
:	m_filterInfo(0),
	m_userData(0),
	m_hitFilter(HK_NULL),
	m_instance(HK_NULL),
	m_localToWorldTransform(HK_NULL)
{
}

inline void hkaiNavMeshQueryMediator::QueryInputBase::setInstanceAndTransform( const hkaiNavMeshInstance* instance )
{
	m_instance = instance;
	m_localToWorldTransform = instance ? &instance->getTransform() : HK_NULL;
}

inline hkaiNavMeshQueryMediator::GetClosestPointInput::GetClosestPointInput()
:	m_queryRadius(hkSimdReal_5),
	m_navMeshCutter(HK_NULL)
{
	m_position = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

inline hkaiNavMeshQueryMediator::GetClosestPointInput::GetClosestPointInput(hkVector4Parameter position, hkSimdRealParameter radius)
:	m_position(position),
	m_queryRadius(radius),
	m_navMeshCutter(HK_NULL)
{
}

inline hkaiNavMeshQueryMediator::GetClosestPointInput::GetClosestPointInput(const QueryInputBase& base)
:	QueryInputBase(base),
	m_queryRadius(hkSimdReal_5),
	m_navMeshCutter(HK_NULL)
{
	m_position = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

inline hkaiNavMeshQueryMediator::RaycastInput::RaycastInput(const QueryInputBase& base)
:	QueryInputBase(base)
{
	m_from = hkVector4::getConstant<HK_QUADREAL_MAX>();
	m_to = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

inline hkaiNavMeshQueryMediator::RaycastInput::RaycastInput()
{
	m_from = hkVector4::getConstant<HK_QUADREAL_MAX>();
	m_to = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

inline hkaiNavMeshQueryMediator::BidirectionalRaycastInput::BidirectionalRaycastInput(const QueryInputBase& base)
:	QueryInputBase(base)
{
	m_center = hkVector4::getConstant<HK_QUADREAL_MAX>();
	m_forwardTo = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

inline hkaiNavMeshQueryMediator::BidirectionalRaycastInput::BidirectionalRaycastInput()
{
	m_center = hkVector4::getConstant<HK_QUADREAL_MAX>();
	m_forwardTo = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

inline hkaiNavMeshQueryMediator::AabbQueryInput::AabbQueryInput()	
{
	m_aabb.setEmpty();
}

inline hkaiNavMeshQueryMediator::CoherentInput::CoherentInput()
:	m_previousPoint( hkVector4::getConstant<HK_QUADREAL_MAX>() ),
	m_up( hkVector4::getConstant<HK_QUADREAL_MAX>() ),
	m_previousFaceKey( HKAI_INVALID_PACKED_KEY )
{
	m_isOnFaceTolerance.setFromFloat(1e-3f);
	m_coherencyTolerance.setFromFloat(0.1f);
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
