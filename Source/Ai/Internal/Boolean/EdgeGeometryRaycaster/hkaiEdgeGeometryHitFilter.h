/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_EDGE_GEOMETRY_HITFILTER_H
#define HKAI_EDGE_GEOMETRY_HITFILTER_H

#include <Ai/Internal/Boolean/hkaiFaceEdges.h>

class hkaiEdgeGeometry;
class hkBitField;
struct hkcdRay;

	/// Filter that determines whether or not to ignore a hit face.
class hkaiEdgeGeometryHitFilter : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

	virtual ~hkaiEdgeGeometryHitFilter() {}

	virtual bool ignoreHit( int faceId, hkVector4Parameter start, hkVector4Parameter end, hkSimdRealParameter hitFraction, hkVector4Parameter facePlane ) const = 0;
};

	/// Base filter rejects hits based on position.
class hkaiPositionHitFilter : public hkaiEdgeGeometryHitFilter
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

	hkaiPositionHitFilter( hkReal epsilon );
	virtual ~hkaiPositionHitFilter() {}

	virtual bool ignoreHit( int faceId, hkVector4Parameter start, hkVector4Parameter end, hkSimdRealParameter hitFraction, hkVector4Parameter facePlane ) const HK_OVERRIDE;

	hkSimdReal					m_lowerEpsilon;
	hkSimdReal					m_upperEpsilon;
};


	/// More advanced filter for removeUnreachableTriangles; 
class hkaiRemoveUnreachableHitFilter : public hkaiPositionHitFilter
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

	hkaiRemoveUnreachableHitFilter( 
		const hkArrayBase<hkVector4>& vertices, 
		const hkaiEdgeGeometry* walkableGeom, 
		const hkArrayBase<int>* originalFaceIndices, 
		const hkArrayBase<int>& trianglesPerFace, 
		const hkArrayBase<int>& tris, 
		const hkBitField& trisToRemove, 
		hkReal epsilon );
	virtual ~hkaiRemoveUnreachableHitFilter() {}

	hkResult init();
	virtual bool ignoreHit( int faceId, hkVector4Parameter start, hkVector4Parameter end, hkSimdRealParameter hitFraction, hkVector4Parameter facePlane ) const HK_OVERRIDE;

	const hkArrayBase<hkVector4>&	m_trianglesVertices;
	hkRefPtr<const hkaiEdgeGeometry> m_walkableGeom;
	const hkArrayBase<int>*		m_originalFaceIndices;
	const hkArrayBase<int>&			m_trianglesPerFace;
	const hkArrayBase<int>&			m_tris;
	const hkBitField&			m_trisToRemove;
	hkArray<hkUint8>			m_mapStorage;
	hkPointerMap<int, int>		m_origToWalkableFaceMap;
	hkArray<int>				m_triOffsets;
};



#endif	// HKAI_EDGE_GEOMETRY_HITFILTER_H

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
