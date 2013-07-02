/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_CONVEX_SILLOUETTE_H
#define HK_CONVEX_SILLOUETTE_H


/// Represents a convex 'silhouette' made by a hkaiSilhouetteGenerator - a 2D polygon wound anticlockwise.
typedef hkArrayBase<hkVector4> hkaiConvexSilhouetteVertices;


	/// A set of convex silhouettes referring to a single pool of vertices
class hkaiConvexSilhouetteSet : public hkReferencedObject
{
public:
	//+version(1)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

	inline hkaiConvexSilhouetteSet();
	hkaiConvexSilhouetteSet( hkFinishLoadedObjectFlag f );

		/// Get the number of silhouettes contained in this set
	HK_FORCE_INLINE int getNumSilhouettes() const;
		
		/// Get the pointer to ths start of the ith silhouette.
	HK_FORCE_INLINE const hkVector4* getSilhouetteVertexPointer( int i ) const;

		/// Get the size of the ith silhouette.
	HK_FORCE_INLINE int getSilhouetteSize( int i ) const;

		/// Get the pool of vertices for all the silhouettes
	HK_FORCE_INLINE const hkArrayBase< hkVector4 >& getVertexPool() const;

		/// Clear the set of silhouettes
	inline void clear();

		/// Set up a silhouette set from given vertex pool and silhouette sizes
	inline void set( const hkArrayBase< hkVector4 >& vertexPool, const hkArrayBase<int>& silhouetteSizes, const hkQTransform& lastLocalTransform, hkVector4Parameter up);

	const hkQTransform& getTransform() const { return m_cachedTransform; }
	const hkVector4& getUp() const { return m_cachedUp; }

private:
		/// Vertex positions for all silhouettes in the set
	hkArray< hkVector4 > m_vertexPool;

		/// Offsets into the vertex pool, and used to compute the sizes.
	hkArray< int > m_silhouetteOffsets;
		
		/// Last transform that was passed to generateSilhouettesCached()
		/// Subsequent calls to generateSilhouettesCached() will reuse the cached silhouettes if the transforms are equal.
	hkQTransform m_cachedTransform;

		/// Last up vector that was passed to generateSilhouettesCached()
	hkVector4 m_cachedUp;

};


class hkaiConvexSilhouetteUtil
{
public:
	enum 
	{
		
		// Review
		MAX_SILHOUETTE_VERTICES = 128
	};
		
		/// For debugging only - Check is a valid convex hull
	static bool HK_CALL validateConvex(const hkArrayBase<hkVector4>& m_points, const hkVector4 &upAxis);	
	
};

#include <Ai/Pathfinding/Dynamic/Silhouette/hkaiConvexSilhouette.inl>

#endif // HK_CONVEX_SILLOUETTE_H

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
