/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_UNIQUE_EDGES_H
#define HKAI_UNIQUE_EDGES_H

#include <Ai/Internal/Boolean/hkaiEdgeGeometry.h>

/// The hkaiUnique edges class provides a way to find all of the edges that index the same vertices. The vertex indices
/// can be in either order, for example an edge going from vertex 1 to vertex 2 is considered to match another edge which
/// goes from 2 to 1. Some algorithms can operate much more quickly if they only have to deal with edges going in one direction,
/// for example the hkaiBooleanOperation only needs to calculate the intersections of unique edges once.
///
/// The getNumEdges method returns the amount of unique edges in the hkaiEdgeGeometry. Given a unique edge index (from 0 to getNumEdges)
/// the getStartUniqueEdge and getUniqueEdge, methods can be used to find all of the edges that index the same vertex indices.
/// It is also possible given an edge index from the original hkaiEdgeGeometry to get the unique edge set it is in using
/// getUniqueEdgeIndex.
class hkaiUniqueEdges
{
    public:
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiUniqueEdges);

        typedef hkaiEdgeGeometry::Edge Edge;
		typedef hkaiEdgeGeometry::FaceIndex FaceIndex;
		typedef hkaiEdgeGeometry::EdgeIndex EdgeIndex;
		typedef hkaiEdgeGeometry::VertexIndex VertexIndex;

            /// Set the geometry
        hkResult setGeometry(const hkaiEdgeGeometry& geom);
            /// Get the face start edges
        HK_FORCE_INLINE const Edge*const* getStartUniqueEdge(int edge) const;
            /// Get the end face edge
        HK_FORCE_INLINE const Edge*const* getEndUniqueEdge(int edge) const;
            /// Get the number of unique edges
        HK_FORCE_INLINE int getNumEdges() const;

		void clear();

protected:
        hkArray<const hkaiEdgeGeometry::Edge*> m_edges;
        hkArray<const hkaiEdgeGeometry::Edge*const*> m_startUniqueEdge;
};

#include <Ai/Internal/Boolean/hkaiUniqueEdges.inl>

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
