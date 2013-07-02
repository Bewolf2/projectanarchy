/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_SPATIAL_QUERY_HIT_FILTER_H
#define HKAI_SPATIAL_QUERY_HIT_FILTER_H

class hkaiNavMeshInstance;
class hkaiNavVolume;

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.h>


	/// Filter class used to determine whether a spatial query "hits" a nav mesh face or nav volume cell (depending on the query type).
	/// Can also be used to exclude entire hkaiNavMeshInstance or hkaiNavVolume
class hkaiSpatialQueryHitFilter : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );
	virtual ~hkaiSpatialQueryHitFilter() {}

	//
	// Nav mesh query interface
	//

		/// Called before the mediator for an instance is examined. If the method returns false, the instance is not queried.
	hkBool32 virtual isInstanceEnabled( const hkaiNavMeshInstance* instance, hkUint32 filterInfo, hkUlong userData ) const = 0;

		/// Called before "narrowphase" checks (e.g. raycast) are done against a particular face. If the method returns false,
		/// the face is ignored.
	hkBool32 virtual isFaceEnabled( const hkaiNavMeshInstance* instance, hkaiNavMesh::FaceIndex faceIndex, hkUint32 filterInfo, hkUlong userData ) const = 0;

	//
	// Nav volume query interface
	//

		/// Called before the mediator for a volume is examined. If the method returns false, the volume is not queried.
	hkBool32 virtual isVolumeEnabled( const hkaiNavVolume* volume, hkUint32 filterInfo, hkUlong userData ) const = 0;

		/// Called before "narrowphase" checks (e.g. closest point) are done against a particular cell. If the method returns false,
		/// the cell is ignored.
	hkBool32 virtual isCellEnabled( const hkaiNavVolume* volume, hkaiNavVolume::CellIndex cellIndex, hkUint32 filterInfo, hkUlong userData ) const = 0;

};

#endif // HKAI_SPATIAL_QUERY_HIT_FILTER_H

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
