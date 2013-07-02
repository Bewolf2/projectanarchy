/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_GEOMETRY_CARVER_EXTRACTOR_H
#define HKAI_GEOMETRY_CARVER_EXTRACTOR_H

#include <Ai/Internal/Boolean/hkaiEdgeGeometry.h>

class hkaiEdgeGeometry;
class hkaiVolume;
class hkaiMaterialPainter;

/// The hkaiGeometryCarverExtractor can be used to apply a number of carvers (defined by hkaiVolume objects) to geometry.
class hkaiGeometryCarverExtractor
{
	public:

        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiGeometryCarverExtractor );


		/// The extractCarvers method will modify the extrudeGeom, by adding the geometry of the carvers to the extrude geometry.
		/// When the extrudeGeom is subtracted from the walkable area, the carvers faces will be cut out.
		/// NOTE - that the remains of faces on the walkable surface inside of the carver will remain, and these generally should
		/// be removed. This is handled in when hkaiNavMeshGenerationUtils is used.
		/// This is achieved by triangulating the output geometry, and removing triangles that are inside the carvers, by checking if
		/// the centroid of the triangle is inside carvers.
		/// Returns HK_SUCCESS if all memory allocations succeeded, HK_FAILURE otherwise.
		static hkResult HK_CALL addCarvers(hkaiEdgeGeometry& extrudeGeom, 
			const hkArrayBase< hkRefPtr<const hkaiVolume> >& carvers,
			const hkArrayBase< hkRefPtr<const hkaiMaterialPainter> >& painters);


		/// Computes the carver geometry in a specified AABB.
		static hkResult HK_CALL addCarversInRegion(hkaiEdgeGeometry& extrudeGeom, 
			const class hkaiCarverTree* tree,
			const class hkAabb& aabb);

	private:

		static hkResult HK_CALL addVolume(hkaiEdgeGeometry& extrudeGeom, const hkaiVolume& carver, hkaiEdgeGeometry::FaceFlags flags);
};

#endif // HKAI_GEOMETRY_CARVER_EXTRACTOR_H

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
