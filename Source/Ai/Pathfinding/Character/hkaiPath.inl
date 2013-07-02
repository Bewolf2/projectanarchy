/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>


inline void hkaiPath::PathPoint::init()
{
	m_normal.setZero();
	m_flags = 0;
	m_userEdgeData = 0;
}

inline hkaiPath::hkaiPath( ReferenceFrame pathReferenceFrame )
:	m_referenceFrame( pathReferenceFrame )
{
}

inline hkaiPath::hkaiPath( hkFinishLoadedObjectFlag f )
:	hkReferencedObject(f), 
	m_points(f)
{
}

inline hkaiPath::ReferenceFrame hkaiPath::getReferenceFrame() const
{
	return m_referenceFrame;
}

inline hkBool32 hkaiPath::PathPoint::isFromUserEdge() const 
{
	return m_flags.get(EDGE_TYPE_USER_START | EDGE_TYPE_USER_END);
}

inline hkBool32 hkaiPath::PathPoint::isTerminator() const 
{
	return (m_flags == 0) &&
		m_userEdgeData == ARRAY_TERMINATOR_USER_EDGE_DATA;
}

inline void hkaiPath::PathPoint::setAsTerminator()
{
	m_position.setZero();
	m_normal.setZero();
	m_sectionId = HKAI_INVALID_RUNTIME_INDEX;
	m_userEdgeData = ARRAY_TERMINATOR_USER_EDGE_DATA;
	m_flags = 0;
}

inline void hkaiPath::PathPoint::transformBy( const hkTransform& t)
{
	hkVector4 pos = m_position;
	hkVector4 normal = m_normal;
	m_position._setTransformedPos(t, pos);
	m_normal._setRotatedDir( t.getRotation(), normal);
}


inline int hkaiPath::getNumPositions() const
{
	return m_points.getSize();
}

inline const hkVector4& hkaiPath::getPosition( int i ) const
{
	return m_points[i].m_position;
}

inline const hkVector4& hkaiPath::getNormal( int i ) const
{
	return m_points[i].m_normal;
}

inline const hkVector4& hkaiPath::getLastPosition() const
{
	return m_points.back().m_position;
}

inline const hkVector4& hkaiPath::getLastNormal() const
{
	return m_points.back().m_normal;
}

inline const hkaiPath::PathPoint& hkaiPath::getLastPoint() const
{
	return m_points.back();
}

inline const hkaiPath::PathPoint& hkaiPath::getPoint( int i ) const
{
	return m_points[i];
}

inline hkaiPath::PathPointFlags hkaiPath::getPointFlags( int i ) const
{
	return m_points[i].m_flags;
}

inline void hkaiPath::addPoint( hkVector4Parameter p, hkVector4Parameter n, hkaiRuntimeIndex sectionId, hkUint32 userEdgeData /*= 0*/, PathPointFlags pointFlags /*= USER_EDGE_NONE*/ )
{
	HK_ASSERT(0x288909d2, pointFlags.get() < 2*EDGE_TYPE_SEGMENT_END);

	PathPoint& seg = m_points.expandOne();
	seg.m_position = p;
	seg.m_normal = n;
	seg.m_sectionId = sectionId;
	seg.m_userEdgeData = userEdgeData;
	seg.m_flags = pointFlags;
}

inline void hkaiPath::addPoint( const PathPoint& point )
{
	m_points.pushBack(point);
}

inline void hkaiPath::reserve( int n )
{
	m_points.reserve(n);
}

inline void hkaiPath::clear()
{
	m_points.clear();
}

inline void hkaiPath::popBack()
{
	m_points.popBack();
}

inline const hkTransform& HK_CALL hkaiPathUtil::getSectionTransform( const hkaiStreamingCollection* collection, hkaiRuntimeIndex fixedSectionId )
{
	if (fixedSectionId == HKAI_INVALID_RUNTIME_INDEX)
	{
		// Individual path points might have no sectionId, as for invalid start/goal points.
		// These points are already in world space
		return hkTransform::getIdentity();
	}

	const hkaiNavMeshInstance* instance = collection->getInstanceAt(fixedSectionId);
	return instance ? instance->getTransform() : hkTransform::getIdentity();
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
