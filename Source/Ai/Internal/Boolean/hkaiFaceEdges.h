/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_FACE_EDGES_H
#define HKAI_FACE_EDGES_H

#include <Ai/Internal/Boolean/hkaiEdgeGeometry.h>

/// The hkaiFaceEdges class provides a fast and convenient way of determine all of the edges that belong to
/// a face of hkaiEdgeGeometry. The class holds pointers to the edges stored in the hkaiEdgeGeometry that is being
/// examined, so the hkaiEdgeGeometry must be in scope for at least as long as the hkaiFaceEdges. Also note that the
/// hkaiEdgeGeometry should be considered immutable whilst being examined through hkaiFaceEdges, as a change to hkaiEdgeGeometry
/// will not be tracked by hkaiFaceEdges, and a change in allocation on the hkaiEdgeGeometrys edges will cause the hkaiFaceEdge
/// to have dangling pointers.
class hkaiFaceEdges
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiFaceEdges);

		typedef hkaiEdgeGeometry::Edge Edge;
		typedef hkaiEdgeGeometry::FaceIndex FaceIndex;

			/// Set the geometry.
			/// This populates the mapping
		hkResult setGeometry(const hkaiEdgeGeometry& geom);
			/// Get the face start edges
		const Edge*const* getStartFaceEdge(int face) const { return m_faceStartEdges[face]; }
			/// Get the end face edge
		const Edge*const* getEndFaceEdge(int face) const { return m_faceStartEdges[face + 1]; }
			/// Get the number of faces
		int getNumFaces() const { return m_faceStartEdges.getSize() - 1; }

			/// Find the edge in the face with the start index. -1 if not found
		const Edge* findEdgeWithStartVertex(int face, int a) const;
			/// Find the edge in the face wit the end vertex index. -1 if not found
		const Edge* findEdgeWithEndVertex(int face, int b) const;

		void clear();

	protected:
		hkArray<const hkaiEdgeGeometry::Edge*> m_faceEdges;
		hkArray<const hkaiEdgeGeometry::Edge*const*> m_faceStartEdges;
};

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
