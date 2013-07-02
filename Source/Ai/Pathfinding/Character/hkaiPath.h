/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_PATH_H
#define HK_AI_PATH_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>

class hkaiStreamingCollection;

/// The hkaiPath class stores a path representation as a set of positions and distances.
class hkaiPath : public hkReferencedObject
{
	// +vtable(true)
	// +version(10)

	friend class hkaiPathUtil;

	public:
		/// Flags to determine which part (if any) on a user edge a point came from, or if the point is a 'segment' start/end (where simple smoothing is not being used)
		enum PathPointBits
		{
			EDGE_TYPE_USER_START = 1,
			/// 
			EDGE_TYPE_USER_END = 2,
			///
			EDGE_TYPE_SEGMENT_START = 4,
			/// 
			EDGE_TYPE_SEGMENT_END = 8
		};
		typedef hkFlags<PathPointBits, hkUint8> PathPointFlags;

		/// Indicates the frame(s) of reference that these path positions are defined in.
		enum ReferenceFrame
		{
				/// Path points are all in world space
			REFERENCE_FRAME_WORLD,
				/// Each path point is relative to the transform of the nav section given in the m_sectionId field.
				/// To follow or draw this path, it must be converted into a common frame
			REFERENCE_FRAME_SECTION_LOCAL,
				/// Path points are all in a single section's frame (i.e. the section of a given character)
			REFERENCE_FRAME_SECTION_FIXED,
		};

			/// A single point in the path.
		struct PathPoint
		{
			// +version(3)
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_ASTAR, PathPoint);

				/// Position on the path.
			hkVector4 m_position;

				/// Path normal between the corresponding position and the next position.
				/// The normal of the final point is redudant, and equal to the second-to-last normal.
				/// For paths on nav meshes, the normal is the normal of the face that the segment is on.
				/// For paths on nav volumes, the normal is always the zero vector.
			hkVector4 m_normal;

				/// Edge data if the edge was a user edge, 0 otherwise.
			hkUint32 m_userEdgeData;

				/// Runtime index of the section this path point is attached to
			hkaiRuntimeIndex m_sectionId;

				/// Whether or not the point came from a user edge, and if so, which part.
			PathPointFlags m_flags;

			HK_FORCE_INLINE void init();

			HK_FORCE_INLINE hkBool32 isFromUserEdge() const;

			HK_FORCE_INLINE hkBool32 isTerminator() const;

			HK_FORCE_INLINE void setAsTerminator();

			inline void transformBy( const hkTransform& transform );

			void checkDeterminism() const;

		private:

			static const hkUint32 ARRAY_TERMINATOR_USER_EDGE_DATA = 0x80000000;
		};

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		HK_DECLARE_REFLECTION();

		inline hkaiPath( ReferenceFrame pathReferenceFrame );
		inline hkaiPath( hkFinishLoadedObjectFlag f );

		/// This computes the distances between points and stores them in the .w components of the positions
		void setDistances();
		
		/// Returns true if the index given is the index of the segment (point pair) ending in the last point
		HK_FORCE_INLINE bool isLastSegment( int segmentIndex ) const
		{
			return segmentIndex == m_points.getSize() - 2;
		}

		inline ReferenceFrame getReferenceFrame() const;

		int getNumPositions() const;
		const hkVector4& getPosition(int i) const;
		const hkVector4& getNormal(int i) const;
		const hkVector4& getLastPosition() const;
		const hkVector4& getLastNormal() const;
		
		const PathPoint& getLastPoint() const;
		const PathPoint& getPoint(int i) const;

		PathPointFlags getPointFlags(int i) const;

			/// Add a point with the specified normal to the path.
			/// For walking characters, the normal should be the world up direction.
			/// For wall-climbing characters, the normal should be the surface normal at that point.
			/// Fot flying characters, the normal should be zero.
		void addPoint( hkVector4Parameter position, hkVector4Parameter normal, hkaiRuntimeIndex sectionId, 
			hkUint32 userEdgeData = 0, PathPointFlags pointFlags = 0);
		
		void addPoint( const PathPoint& point );

		// Array manipulation
		inline void reserve( int n );
		inline void clear();
		inline void popBack();

	protected:
		hkArray<PathPoint> m_points;
		hkEnum<ReferenceFrame, hkUint8> m_referenceFrame; //+default(0)
};

	/// Utility methods for calculating path size for multithreaded output
class hkaiPathUtil
{
public:
		/// Gets the number of edges in the output
	static int HK_CALL getNumEdgesInPath( const struct hkaiNavMeshAStarCommand& command );

		/// Gets the number of cell in the output
	static int HK_CALL getNumCellsInPath( const struct hkaiNavVolumeAStarCommand& command );

		/// Gets the number of hkaiPath::PathPoint's in the output
	static int HK_CALL getNumPointsInPath( const struct hkaiNavMeshAStarCommand& command );

		/// Gets the number of hkaiPath::PathPoint's in the output
	static int HK_CALL getNumPointsInPath( const struct hkaiNavVolumeAStarCommand& command );

		/// Used internally by getNumEdgesInPath() and getNumCellsInPath()
	static int HK_CALL _getPathLength( const hkaiPackedKey* output, int maxOutputLength );
	
		/// Used internally by getNumPointsInPath()
	static int HK_CALL _getNumPathPoints( const hkaiPath::PathPoint* path, int maxPathLength );


		/// Transform path positions from section-local space into world space
	static void HK_CALL transformPathPositionsSectionLocalToWorld( const hkaiStreamingCollection* collection, const hkaiPath& pathIn, hkArray< hkaiPath::PathPoint >::Temp& pointsOut, int startIndex = 0 );

		/// Process all the path points and convert them from section-local space into world space
	static void HK_CALL transformPathSectionLocalToWorld( const hkaiStreamingCollection* collection, const hkaiPath& pathIn, hkaiPath& pathOut );

		/// Process all the path points and convert them from world space into section-local space
	static void HK_CALL transformPathWorldToSectionLocal( const hkaiStreamingCollection* collection, const hkaiPath& pathIn, hkaiPath& pathOut );

		/// Process all the path points and convert them from section-local space into a single section's space
	static void HK_CALL transformPathSectionLocalToSectionFixed( const hkaiStreamingCollection* collection, hkaiRuntimeIndex fixedSectionId, const hkaiPath& pathIn, hkaiPath& pathOut );

		/// Process all the path points and convert them from a single section's space into world space
	static void HK_CALL transformPathSectionFixedToWorld( const hkaiStreamingCollection* collection, hkaiRuntimeIndex fixedSectionId, const hkaiPath& pathIn, hkaiPath& pathOut );

		/// Returns the mesh instance's transform if the instance is non-NULL, or identity transform otherwise.
	static inline const hkTransform& HK_CALL getSectionTransform( const hkaiStreamingCollection* collection, hkaiRuntimeIndex fixedSectionId );

};

#include <Ai/Pathfinding/Character/hkaiPath.inl>

#endif // HK_AI_PATH_H

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
