/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_EDGE_GEOMETRY_RAYCASTER_H
#define HKAI_EDGE_GEOMETRY_RAYCASTER_H

#include <Ai/Internal/Boolean/hkaiFaceEdges.h>

// Uncomment the line below to enable debug display code
//#define HKAI_DEBUG_DISPLAY_EDGE_GEOMETRY_RAYCASTER

class hkaiEdgeGeometry;
struct hkcdRay;
class hkaiEdgeGeometryHitFilter;
class hkaiVolume;
class hkBitField;

	/// Raycasting against an edge geometry. Used during edge connection testing and triangle removal.
class hkaiEdgeGeometryRaycaster : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

		hkaiEdgeGeometryRaycaster(  );

		~hkaiEdgeGeometryRaycaster();

		hkResult init( const hkaiEdgeGeometry* geom );
		hkResult init( const hkGeometry* geom, const hkArrayBase< hkRefPtr<const hkaiVolume> >& carvers, const hkBitField& cuttingTriangles );

			/// Initialize the raycaster without building the tree. Only raycasting against specific faces is supported in this case.
		hkResult initNoTree( const hkGeometry* geom );

		// Tests if the specified ray hits the edge geometry
		hkBool32 castRay( hkVector4Parameter start, hkVector4Parameter end ) const;
		hkBool32 castRayExtruded( hkVector4Parameter start, hkVector4Parameter end, hkVector4Parameter extrusion ) const;

			/// When to early out during raycasting
		enum BundleEarlyOut
		{
				/// Stop the whole bundle when any ray hits
			STOP_BUNDLE_ON_HIT,

				/// Stop each ray when it hits. Only exits if entire bundle is "stopped"
			STOP_INDIVIDUAL_ON_HIT,

				/// Don't early out
			STOP_NEVER
		};

		// Tests if any of four rays hit the edge geometry
		// The closer together the rays are, the faster the query will perform
		hkBool32 castRayBundle( const hkVector4* starts, const hkVector4* ends, BundleEarlyOut earlyOut ) const;
		hkBool32 castRayBundleExtruded( const hkVector4* starts, const hkVector4* ends, hkVector4Parameter extrusion, BundleEarlyOut earlyOut ) const;

			/// Specialized query for use in edge connection code.
			/// Ray starts and ends are in three groups of three: 012, 345, 678
			/// We do an AABB query on the rays + (-extrusion) and check each active ray against the extruded geometry.
			/// If a ray hits the geometry, we mask out its group.
			/// Stop when all the ray groups are dead.
			/// If any ray groups are still alive at the end, the query "misses" and we return false.
		hkBool32 castSmallRayBundleExtruded( const hkVector4* starts, const hkVector4* ends, hkVector4Parameter extrusion ) const;

		void setIgnoreFaceIndex(int faceIndex) { m_ignoreFaceIndex = faceIndex; }
		void clearIgnoreFaceIndex() { m_ignoreFaceIndex = -1; }
		int getIgnoreFaceIndex() const { return m_ignoreFaceIndex; }

			/// Ignore carvers and painters
		void setupForRemoveUnreachableTriangles();

			/// Ignore painters
		void setupForEdgeConnections();

		void setFaceFlagMask( hkaiEdgeGeometry::FaceFlags flags) { m_faceFlagMask = flags; }
		void resetFaceFlagMask() { m_faceFlagMask = hkaiEdgeGeometry::FLAGS_ALL; }
		hkaiEdgeGeometry::FaceFlags getFaceFlagMask() const { return m_faceFlagMask; }

		const hkaiEdgeGeometryHitFilter* getHitFilter() const { return m_hitFilter; }
		void setHitFilter( const hkaiEdgeGeometryHitFilter* filter );

		const hkVector4& getFacePlane (int faceIndex) const {return m_facePlanes[faceIndex]; }


		//
		// "Narrowphase" queries against a particular face
		//
		hkBool32 raycastAgainstFace( int faceId, hkVector4Parameter rayStart, hkVector4Parameter rayEnd, hkSimdReal& hitFractionInOut ) const;
 		hkBool32 raycastAgainstFace( int faceId, hkVector4Parameter rayStart, hkVector4Parameter rayEnd, hkVector4Parameter facePlane, hkVector4Parameter extrusion, hkSimdReal& hitFractionInOut ) const;
		hkBool32 raycastAgainstExtrudedFace( int faceId, hkVector4Parameter rayStart, hkVector4Parameter rayEnd, const hkcdRay& ray, hkVector4Parameter extrusion, bool stopOnFirstHit, hkSimdReal& hitFractionInOut ) const;
 
		void raycastBundleAgainstFace( int faceId, const hkVector4* bundleStarts, const hkVector4* bundleEnds, const hkcdRay* rays, hkVector4& hitFractions, hkVector4ComparisonParameter mask, BundleEarlyOut earlyOut, hkBool32 extruded, hkVector4Parameter extrusion ) const;
		hkaiEdgeGeometry::FaceFlags getFlagsForFace( int faceId ) const ;

	protected:

		int getNumVertsForFace( int faceId ) const;
		void getVertsForFace( int faceId, hkArrayBase<hkVector4>& vertsOut ) const ;
		bool shouldSkipFace( int faceId ) const ;

			/// Collisions are ignored with face of this index
		int							m_ignoreFaceIndex;		

			/// Collisions are ignored if (face.m_flags & m_faceFlagMask) == 0
		hkaiEdgeGeometry::FaceFlags	m_faceFlagMask;			


		hkRefPtr<const hkaiEdgeGeometry>	m_edgeGeometry;
		hkRefPtr<const hkGeometry>			m_geometry;
		hkRefPtr<hkGeometry>				m_carverGeometry;

		hkaiFaceEdges						m_faceEdges;
		hkArray<hkVector4>					m_facePlanes;
		void*								m_tree;

		hkRefPtr<const hkaiEdgeGeometryHitFilter>	m_hitFilter;

	public:

#ifdef HKAI_DEBUG_DISPLAY_EDGE_GEOMETRY_RAYCASTER
		static hkBool			s_debugDisplayOn;
#endif
};

#endif	// HKAI_EDGE_GEOMETRY_RAYCASTER_H

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
