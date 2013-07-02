/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_CONVEX_2D_UTILS_H
#define HK_CONVEX_2D_UTILS_H

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteReferenceFrame/hkaiSilhouetteReferenceFrame.h>
struct hkaiSilhouetteGenerationParameters;

/// Helpers for making a "2D" convex hull.
class hkaiConvex2dUtils
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkaiConvex2dUtils );

		enum 
		{
				/// Smallest allowed silhouette is a triangle (with a duplicated point)
			MIN_SILHOUETTE_SIZE = 4
		};
	
			/// Input for createConvexHullSilhouette
		struct Convex2DHullInput
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, Convex2DHullInput );
			Convex2DHullInput( const hkQTransform& transform )
				: m_transform(transform)
			{
			}
				/// Vertex array
			const hkVector4* m_verts;
				/// Num verts
			int m_numVerts;
				/// Up (projection of hull)
			hkVector4 m_up;
				/// Reference axis (defines 'left-right')
			hkVector4 m_referenceAxis;
				/// Transform to be applied to the vertices
			const hkQTransform& m_transform;
		};

		static hkBool32 HK_CALL getSilhouetteFromPointCloud( const hkVector4* points, int numPoints, const hkaiSilhouetteGenerationParameters& params, const hkQTransform& transform, hkSimdRealParameter extraRadius, hkSimdRealParameter weldTolerance, hkArrayBase<hkVector4>& verticesOut);

			/// Make a 2D anti-clockwise wound convex hull with the 0th output vertex be duplicated as the last vertex, and
			/// the 0th vertex will be the 'lowest, left-most' vertex with respect to the reference frame for the 2D-projection.
			/// Returns true if the silhouetteOut has enough space for the convex hull
		static hkBool32 HK_CALL createConvexHullSilhouette( Convex2DHullInput& input, hkArrayBase<hkVector4>& silhouetteOut );

			/// Same as createConvexHullSilhouette but using the OBB
		static void HK_CALL createObbSilhouette( const hkaiSilhouetteGenerationParameters& generationParams, const hkAabb& localAabb, const hkQTransform& transform, hkSimdRealParameter extraRadius, hkArrayBase<hkVector4>& silhouetteOut );

			/// Same as createConvexHullSilhouette but using the AABB
		static void HK_CALL createAabbSilhouette( const hkaiSilhouetteGenerationParameters& generationParams, const hkAabb& genAabb, hkSimdRealParameter extraRadius, hkArrayBase<hkVector4>& silhouetteOut );

			/// Expand the vertices in a hull to move the edges of the hull out by 'radius'. The corners may be 'beveled' if the angle exceeds some threshold given by bevelThreshold
		static void HK_CALL expandByRadius( hkArrayBase<hkVector4>& silhouettePoints, hkVector4Parameter up, hkSimdRealParameter radius, hkSimdRealParameter bevelThreshold );

				/// Helper to weld any points on a hull closer to each other than tolerance
		static void HK_CALL weldCoincidentVerts(hkArrayBase<hkVector4> & points, hkSimdRealParameter tolerance);

			/// Helper to remove any points on a hull which are collinear to within an area threshold.
		static void HK_CALL removeColinearVerts(const hkReal areaUnscaledThreshold, hkVector4Parameter referenceAxis, hkArrayBase<hkVector4>& points);

			/// Helper to confirm hull is bounding in DEBUG only.
		static bool HK_CALL validateHullIsBounding(const hkArray<hkVector4>& inputPoints, hkVector4Parameter upAxis, const hkArray<hkVector4>& hullPoints, hkBool allowDuplicates );

			/// Helper to get the number of bytes allocated on stack for hull generation
		static int  HK_CALL getStackSizeInBytesRequiredForHullGeneration(int inputPointsSize);

};

#endif // HK_CONVEX_2D_UTILS_H

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
