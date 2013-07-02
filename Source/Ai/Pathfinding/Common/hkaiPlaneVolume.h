/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_PLANE_VOLUME_H
#define HKAI_PLANE_VOLUME_H

#include <Ai/Pathfinding/Common/hkaiVolume.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

extern const class hkClass hkaiPlaneVolumeClass;

class hkAabb;

/// The hkaiPlaneVolume class is a simple implementation of the hkaiVolume interface that uses a set of planes
/// to define a convex volume. For a point to be 'inside' the volume, it must be inside all of the planes.
/// This simple mechanism means that the hkaiPlaneVolume can represent a convex hull.
///
/// In this implementation the geometric representation is just held as a member of the class, and it is a requirement
/// of the client to ensure that the planes and geometry represent the same volume.
class hkaiPlaneVolume : public hkaiVolume
{
	//+vtable(true)
	//+version(2)
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		hkaiPlaneVolume();
		hkaiPlaneVolume(hkFinishLoadedObjectFlag f);

		//
		// hkaiVolume implementation
		//
		virtual void calcPointsInside(const hkVector4* points, int numPoints, hkBitField& pointsInside) const HK_OVERRIDE;
		virtual hkResult calcGeometry(hkGeometry& geom) const HK_OVERRIDE;
		virtual hkBool32 containsTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c) const HK_OVERRIDE;
		virtual hkBool32 containsAabb( const hkAabb& aabbIn ) const HK_OVERRIDE;
		virtual void getAabb( hkAabb& aabbOut ) const HK_OVERRIDE;

			/// Set as an AABB.
		void setFromAabb(const hkAabb& aabb);
			/// Transform the contents in-place.
		void transform(const hkTransform& trans);
		
			/// Creates a copy of this hkaiPlaneVolume, transformed by the specified transform.
		hkaiPlaneVolume* clone( const hkTransform& t = hkTransform::getIdentity() ) const;

			/// Read-only access to the internal geometry
		const hkGeometry& getGeometry() const { return m_geometry; }

		void setInverted( bool inv ) { m_isInverted = inv; }

		bool isInverted() const { return m_isInverted; }


		//
		// Static utility methods
		//

			/// Constructs a volume from the convex hull of a set of points.
		static void HK_CALL createConvexVolume( const hkVector4* points, int numPoints, hkaiPlaneVolume& volumeOut );

			/// Assign the hkGeometry from the hkAabb
		static void HK_CALL createAabbGeometry(const hkAabb& aabb, hkGeometry& geometryOut);

			/// Set the hkaiPlaneVolume to be the extruded 2D convex hull of the input points.
			/// The input points are projected onto the plane perpendicular to up, the 2D convex
			/// hull is computed, and then the convex hull is unprojected.
			/// The cutoff height is used to exclude points over the specified height from the hull computation.
			/// This is intended to help compute a carver for the "footprint" of a building.
		static void HK_CALL createExtruded2DConvexHullVolume( const hkVector4* inputVerts, int numVerts, hkVector4Parameter up, hkSimdRealParameter cutoffHeight, hkSimdRealParameter verticalExpansion, hkaiPlaneVolume& volumeOut );

protected:
			/// Whether or not the point is within the planes. Does not consider m_isInverted.
		hkBool32 contains(hkVector4Parameter point) const;

			/// Update internal CD info
		void updateInternalInfo();

		hkArray<hkVector4> m_planes;
		hkGeometry m_geometry;

			/// Whether the volume should be considered inverted.
			/// Inverted volumes can be used as carvers to remove all geometry "outside" of a region.
		hkBool m_isInverted; //+default(false)

		//
		// Internal collision detection info.
		//

			/// AABB of the hkGeometry, not necessarily what is returned by getAabb();
		hkAabb m_aabb; 


};

#endif // HKAI_PLANE_VOLUME_H

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
