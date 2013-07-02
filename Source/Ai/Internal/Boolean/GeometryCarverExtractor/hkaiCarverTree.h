/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_CARVER_TREE_H
#define HKAI_CARVER_TREE_H

#include <Common/Base/Object/hkReferencedObject.h>
#include <Ai/Pathfinding/Common/hkaiMaterialPainter.h>

class hkaiVolume;
class hkcdDynamicAabbTree;
class hkAabb;

	/// A wrapper to build a bounding-volume tree around carvers and painters for faster queries.
class hkaiCarverTree : public hkReferencedObject
{
	public:

        HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );

		hkaiCarverTree();
		~hkaiCarverTree();

		typedef const hkArrayBase< hkRefPtr<const hkaiVolume> > CarverArray;
		typedef const hkArrayBase< hkRefPtr<const hkaiMaterialPainter> > PainterArray;

		hkResult init( CarverArray& carvers, PainterArray& painters );

			/// Returns a list of all carver volumes that might intersect the specified AABB.
		hkResult queryAabbCarvers( const hkAabb& aabb, hkArray< hkRefPtr<const hkaiVolume> >& carverHits ) const;

			/// Returns a list of all material painter volumes that might intersect the specified AABB.
		hkResult queryAabbPainters( const hkAabb& aabb, hkArray< hkRefPtr<const hkaiMaterialPainter> >& painterHits ) const;

			/// Returns whether any carver contains the specified triangle.
		hkBool32 carversContainTriangle( hkVector4Parameter A, hkVector4Parameter B, hkVector4Parameter C) const;

			/// Returns whether any carver contains the specified point
		hkBool32 carversContainPoint( hkVector4Parameter p ) const;

			/// Returns whether any carver fully contains the specified AABB
		hkBool32 carversContainAABB( const hkAabb& aabb ) const;

			/// Checks the given point against all the material painters, and returns the painted material, or the original material if no painter contains the point.
		int getPaintedMaterialForPoint( hkVector4Parameter p, int originalMaterial, hkaiMaterialPainter::ResolveOverlapCallback* overlapCallback, hkResult& resultOut ) const;

	private:
		hkRefPtr<hkcdDynamicAabbTree> m_carverTree;
		hkRefPtr<hkcdDynamicAabbTree> m_painterTree;

		const CarverArray* m_carvers;
		const PainterArray* m_painters;


};

#endif // HKAI_CARVER_TREE_H

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
