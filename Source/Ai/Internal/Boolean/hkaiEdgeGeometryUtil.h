/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_GEOMETRY_UTIL_H
#define HKAI_GEOMETRY_UTIL_H

#include <Ai/Internal/Boolean/hkaiEdgeGeometry.h>

struct hkaiNavMeshGenerationSettings;

/// The hkaiEdgeGeometryUtil provides processing methods for hkaiEdgeGeometry.
class hkaiEdgeGeometryUtil
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiEdgeGeometryUtil );

			/// Remove faces smaller than minFaceSize
		static void HK_CALL removeSmallFaces(hkaiEdgeGeometry& geom, hkSimdRealParameter minFaceSize);

			/// Find all the unwalkable faces. Finds the dot product between the settings' up vector and the face normal,
			/// if the result is less than walkableFactor (which comes from settings) the face is deemed 'unwalkable'
			/// walkable factor is therefore the cos of the maximum angle between "up" and a face for it to be 'walkable'
		static hkResult HK_CALL findUnwalkableFaces(const hkaiEdgeGeometry& geom, const hkaiNavMeshGenerationSettings& settings, hkArray<hkaiEdgeGeometry::FaceIndex>& unwalkableOut);

			/// Remove unwalkable faces
		static hkResult HK_CALL removeUnwalkableFaces(hkaiEdgeGeometry& geom, const hkaiNavMeshGenerationSettings& settings);
};



#endif // HKAI_GEOMETRY_UTIL_H

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
