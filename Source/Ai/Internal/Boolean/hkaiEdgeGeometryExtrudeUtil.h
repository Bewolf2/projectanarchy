/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_GEOMETRY_EXTRUDE_UTIL_H
#define HKAI_GEOMETRY_EXTRUDE_UTIL_H

#include <Ai/Internal/Boolean/hkaiEdgeGeometry.h>
#include <Ai/Internal/Boolean/hkaiFaceEdges.h>
#include <Ai/Internal/Boolean/hkaiUniqueEdges.h>
#include <Common/Base/Container/BitField/hkBitField.h>

/// The hkaiGeometryExtrudeUtil provides functionality to extrude hkaiEdgeGeometry. There are two styles
/// of extrusion handled via the 'extrude' and 'silhouetteExtrude'. The 'extrude' method does extrusion, by
/// looking for edges which are not shared in the original model. These edges are extruded using a quad that
/// runs from the original geometry, to a copy of the geometry placed at the extrusion distance. The technique
/// works fine for a single sided surface to extrude, but not for a closed shape, as all edges will be shared,
/// and thus nothing will be extruded.
///
/// The 'silhouetteExtrude' looks at shared edges whose faces which face in the opposite directions to the extrusion
/// direction. These edges along with edges that are not shared are then used as the boundary edges for the
/// extrusion. The technique works correctly with closed geometry. There is an issue when there are faces that contain the
/// extrusion direction vector. In this situation the extrusion edge can be determined to be either the top or the bottom of
/// the face - and may flip between the two due to numeric precision issues.
class hkaiGeometryExtrudeUtil
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiGeometryExtrudeUtil );
		typedef hkaiEdgeGeometry::Edge Edge;

		struct IndexMap
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, IndexMap );
			IndexMap();

			hkResult init(int size);
			void clear();

			int add(int index);

			HK_FORCE_INLINE int get(int index) const
			{
				return m_map[index];
			}

			HK_FORCE_INLINE int getNumIndices() const { return m_freeIndex; }

			int m_freeIndex;
			hkArray<int> m_map;
		};


			/// Extrudes using the 'silhouette' of the input geometry
        static hkResult HK_CALL silhouetteExtrude(const hkaiEdgeGeometry& geom, const hkVector4& extrudeDirection, hkaiEdgeGeometry& geomOut);
		
			/// Extrudes using the 'silhouette' of the input geometry. Reuses storage between calls.
		hkResult HK_CALL silhouetteExtrudeImpl(const hkaiEdgeGeometry& geom, const hkVector4& extrudeDirection, hkaiEdgeGeometry& geomOut);

		hkaiFaceEdges m_faceEdges;
		hkaiUniqueEdges m_uniqueEdges;
		hkBitField m_forwardFaces;
		hkArray<int> m_backFaceMap;
		IndexMap m_frontVertexMap;
		IndexMap m_backVertexMap;
		hkArray<int> m_edgeToSideFaceMap;
		hkArray<int> m_edgeFaces; 

};



#endif // HKAI_GEOMETRY_EXTRUDE_UTIL_H

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
