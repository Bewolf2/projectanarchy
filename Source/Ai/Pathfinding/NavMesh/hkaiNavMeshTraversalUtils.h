/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVIGATION_MESH_TRAVERSAL_UTILS_H
#define HKAI_NAVIGATION_MESH_TRAVERSAL_UTILS_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.h>
#include <Ai/Pathfinding/NavMesh/hkaiGeneralAccessor.h>

#ifdef HK_PLATFORM_SPU
#	define HK_FORCE_INLINE_ON_CPU
#else
#	define HK_FORCE_INLINE_ON_CPU HK_FORCE_INLINE
#endif

/// This set of utility functions are used to compute and evaluate traversability information for a nav mesh.
/// These utilities allow to ascertain if a character of arbitrary radius can pass through a particular face.
class hkaiNavMeshTraversalUtils
{
	private:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkaiNavMeshTraversalUtils );
	
		static hkFloat32 INVALID_CLEARANCE;

	public:

		/// Used for temporary storage in getVertexClearanceForEdge.
		struct FaceStackNode
		{	
			/// Distance squared from the vertex.
			hkReal m_distanceSquared;

			/// Index of the face.
			hkaiPackedKey m_faceKey;

			/// Edge that we came from.
			hkaiPackedKey m_edgeKey;
		};

		/// A triplet of indices representing an angle within a nav mesh face.
		/// 
		/// The angle is defined by three distinct vertices of the face. Any 
		/// such angle can be represented, but some methods require particular 
		/// kinds of angles.
		/// 
		/// In particular, some methods require the angle to be "indexable".
		/// Only indexable angles are used for clearance storage.
		/// 
		/// An angle is indexable if all the following are true:
		/// 
		/// 1. All vertex indices are distinct.
		/// 2. Exactly one of the vertices has index zero.
		/// 3. The angle is oriented CCW (see isOriented() for details).
		/// 4. If the apex of the angle is zero, the angle must subtend exactly
		///    one face edge.
		/// 5. If the apex of the angle is not zero, the two non-zero vertices
		///    must be consecutive.
		///
		/// If a face is decomposed into a triangle fan, with the fan apex at
		/// vertex zero, the indexable angles are those which are angles of the
		/// fan triangles. A face with n vertices has exactly (n-2)*3 indexable
		/// angles. 
		class Angle
		{
		public:
			/// The index of the vertices (relative to the a-vertex of the
			/// first edge of the face) making up the angle, with b the angle's
			/// apex.
			int a, b, c;

			/// Returns whether the angle is oriented counterclockwise -- that
			/// is, whether the cross product of [b,a] and [b,c] points in the
			/// same direction as the face normal.
			/// 
			/// The angle does not need to be indexable.
			hkBool32 isOriented() const;

			/// Returns whether the angle subtends the given edge of the face.
			/// 
			/// The angle must be indexable.
			/// 
			/// @param e the offset of the edge of the face to be checked.
			/// @param n the number of edges in the face.
			hkBool32 containsEdge(int e, int n) const;

			/// Returns whether the angle subtends the given edge of the face,
			/// either exactly or in addition to other edges.
			/// 
			/// The angle does not need to be indexable.
			/// 
			/// @param e the offset of the edge of the face to be checked.
			/// @param n the number of edges in the face.
			hkBool32 sectorContainsEdge(int e, int n) const;

			/// Swap a and c as necessary to make the angle ccw oriented.
			void orient();

			/// Fill the angle from the provided side-edges, which must share
			/// exactly one vertex. The resultant angle will be oriented, but
			/// may or may not be indexable.
			/// 
			/// @param x one vertex of the first side-edge.
			/// @param y the other vertex of the first side-edge.
			/// @param z one vertex of the second side-edge.
			/// @param w the other vertex of the first side-edge.
			void fromEdgePair(int x, int y, int z, int w);

			/// Return a unique index for this angle within the face.
			///
			/// The angle must be indexable.
			int index() const;

			/// Return whether the vertex indices are within range.
			/// The uniqueness of the indices is not checked, nor is the
			/// indexability of the angle.
			HK_FORCE_INLINE hkBool32 isValid(int n) const
			{
				return a >= 0 && a < n &&
					b >= 0 && b < n &&
					c >= 0 && c < n;
			}

			/// Rotate the angle vertices so that c becomes the new apex.
			///
			/// The orientation of the resultant angle will be the same as that
			/// of the original angle.
			/// 
			/// The angle will be indexable if and only if the original angle
			/// was indexable.
			void rotate();
		};

			/// Internal representation for clearance values that stores both the truncated
			/// and non-truncated values.
		struct ClearanceValue
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, ClearanceValue );
			inline ClearanceValue();
			inline void setFromHalf( const hkHalf& h );
			inline void setFromSimdReal( hkSimdRealParameter s );

			HK_FORCE_INLINE hkSimdReal	getTruncatedValue() const;
			HK_FORCE_INLINE hkSimdReal	getNextTruncatedValue() const;
			HK_FORCE_INLINE hkSimdReal	getExactValue() const;
			HK_FORCE_INLINE hkBool32	isExactValueComputed() const;

		private:
			hkSimdReal	m_truncatedValue;
			hkSimdReal	m_exactValue;
			hkBool32	m_exactValueComputed;
			hkHalf		m_valueHalf;
		};

		static void HK_CALL resetClearanceForFace( hkaiNavMeshInstance& meshInstance, hkaiNavMesh::FaceIndex faceIndex );

		static bool isVertexClearanceForEdgeLessThan(
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey faceKey,
			const hkaiNavMesh::Face& currentFace, 
			const class Angle& angle,
			hkVector4Parameter up, 
			hkSimdRealParameter maxRadius,
			hkSimdRealParameter threshold);

			/// Returns the clearance corresponding to an edge's starting vertex (m_a).
			/// If the clearance isn't known, it is computed and stored
		static void HK_CALL getVertexClearanceForEdge(
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey faceKey,
			const hkaiNavMesh::Face& currentFace, 
			const class Angle& angle,
			hkVector4Parameter up, 
			hkSimdRealParameter maxRadius,
			ClearanceValue& clearanceOut );

			/// Returns true if a circle of given diameter can traverse through a face by entering from the edgeIn edge and leaving by the edgeOut edge.
		static hkBool32 HK_CALL isEdgePairTraversable(hkSimdRealParameter diameter, const hkaiNavMeshInstance& meshInstance, hkaiNavMesh::FaceIndex faceIndex, hkaiNavMesh::EdgeIndex edgeInIndex, hkaiNavMesh::EdgeIndex edgeOutIndex, hkVector4Parameter up);
		
			/// Returns true if a circle of given diameter can traverse through a face by entering from the edgeIn edge and leaving by the edgeOut edge.
			/// This version is useful from SPU when the faces and edges are cached. Still need the outgoing edge index for some logic.
		static hkBool32 HK_CALL isEdgePairTraversable(hkSimdRealParameter diameter, 
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey currentFaceIndex, 
			const hkaiNavMesh::Face& face, hkVector4Parameter up,
			const hkaiNavMesh::Edge& edgeIn, const hkaiNavMesh::Edge& edgeOut, 
			hkaiPackedKey insideEdgeKey, 
			hkaiPackedKey edgeOutKey,
			hkVector4Parameter inA, hkVector4Parameter inB, 
			hkVector4Parameter outA, hkVector4Parameter outB);

		static hkBool32 HK_CALL isEdgeTraversable(
			hkSimdRealParameter diameter,
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey faceKey,
			const hkaiNavMesh::Face& face,
			hkVector4Parameter up, 
			const hkaiNavMesh::Edge& edge,
			hkaiPackedKey edgeKey,
			hkVector4Parameter point);

		static HK_FORCE_INLINE_ON_CPU hkSimdReal HK_CALL getGlobalClearance(
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey startFaceKey,
			const hkaiNavMesh::Edge& startEdge,
			hkVector4Parameter up,
			hkSimdRealParameter maxRadius);

		static HK_FORCE_INLINE_ON_CPU hkSimdReal HK_CALL getGlobalClearanceB(
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey faceKey,
			const hkaiNavMesh::Face& face,
			hkaiPackedKey edgeKey,
			hkVector4Parameter up,
			hkSimdRealParameter maxRadius);

		static hkSimdReal HK_CALL computeGlobalClearance(
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey startFaceKey,
			const hkaiNavMesh::Edge& startEdge,
			hkVector4Parameter up,
			hkSimdRealParameter maxClearance);

		static hkSimdReal HK_CALL computeVertexClearance(
			const hkaiGeneralAccessor& accessor,
			hkaiPackedKey faceKey,
			const hkaiNavMesh::Face& currentFace, 
			const Angle& angle,
			hkVector4Parameter up, 
			hkSimdRealParameter maxRadius);

		static hkSimdReal updateClearance(const hkaiGeneralAccessor& accessor, 
			hkaiPackedKey faceKey,
			const hkaiNavMesh::Edge& edge,
			hkVector4Parameter up,
			int clearanceIndex,
			hkSimdRealParameter clearance);

		static void updateGlobalClearance(const hkaiGeneralAccessor& mesh, int vertexIndex, 
			hkSimdRealParameter clearance);

};

#ifndef HK_PLATFORM_SPU
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshTraversalUtils.inl>
#endif

#endif // HKAI_NAVIGATION_MESH_UTILS_H

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
