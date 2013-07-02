/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_SHAPE_SILLOUETTE_BUILDER_H
#define HK_SHAPE_SILLOUETTE_BUILDER_H

class hkpShape;

/// Builds hkaiConvexSilhouettes from an hkpShape (or shape hierarchy)
class hkaiPhysics2012SilhouetteBuilder
{
	 public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiPhysics2012SilhouetteBuilder);

    //
    // Silhouette creation
    //

	static void HK_CALL createPointCloudFromShapeRecursive( const hkpShape* shape, const hkQsTransform& transform, hkArray<hkVector4>& verticesOut, hkArray<int>& sizesOut );


		/// For each shape type, produce a point cloud bounding the 3D hull - if doNotExceed is true then we return false if more vertices are returned than the capacity of the input array.
	static void HK_CALL getVertices( const hkpShape* s, hkArray<hkVector4>::Temp& vertsOut);

};

#endif // HK_SHAPE_SILLOUETTE_BUILDER_H

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
