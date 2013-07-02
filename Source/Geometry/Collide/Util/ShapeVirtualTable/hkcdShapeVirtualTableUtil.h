/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKCD_SHAPE_VTABLE_UTIL_H
#define HKCD_SHAPE_VTABLE_UTIL_H

#include <Geometry/Collide/Shapes/hkcdShape.h>


	/// Utility to patch hkcdShape virtual tables.
struct hkcdShapeVirtualTableUtil
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_GEOMETRY, hkcdShapeVirtualTableUtil );

	public:

			/// Replace the virtual table for the given shape. The shape type must be valid before this is called.
		static void HK_CALL patchVirtualTable( hkcdShape* shape );

	public:

			/// Virtual table patch addresses for each hkcdShapeType that supports patching
		static hkUlong s_virtualTablePatches[];

#if	defined(HK_DEBUG) && !defined(HK_PLATFORM_SPU)

		// Debug only, for self tests
		static hkUint32 s_unregisteredFunctions;

#endif
};

// Macro for patching a shape's vtable. Applied only on SPU.
#if defined(HK_PLATFORM_SPU)
#	define HKCD_PATCH_SHAPE_VTABLE( shapePtr )	hkcdShapeVirtualTableUtil::patchVirtualTable( shapePtr )
#else
#	define HKCD_PATCH_SHAPE_VTABLE( shapePtr )
#endif

#endif	//	HKCD_SHAPE_VTABLE_UTIL_H

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
