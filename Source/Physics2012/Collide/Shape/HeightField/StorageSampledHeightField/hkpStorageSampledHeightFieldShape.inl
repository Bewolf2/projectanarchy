/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_PLATFORM_SPU)
#include <Physics2012/Dynamics/World/Simulation/Multithreaded/Spu/hkpSpuConfig.h>
extern hkSpu4WayCache* g_SpuCollideUntypedCache;
#endif

HK_FORCE_INLINE hkReal hkpStorageSampledHeightFieldShape::getHeightAtImpl( HKP_SHAPE_VIRTUAL_THIS int x, int z ) HKP_SHAPE_VIRTUAL_CONST
{

	const hkpStorageSampledHeightFieldShape* thisObj = static_cast<const hkpStorageSampledHeightFieldShape*>(HK_GET_THIS_PTR);
	const int index = z*(thisObj->m_xRes) + x;

#ifdef HK_PLATFORM_SPU
	const hkReal height = *hkGetArrayElemWithByteStridingUsingCache(thisObj->m_storage.begin(), index, sizeof(hkReal), g_SpuCollideUntypedCache, HK_SPU_AGENT_SECTOR_JOB_MAX_UNTYPED_CACHE_LINE_SIZE);
	return height;
#else
	return thisObj->m_storage[index];
#endif
}

HK_FORCE_INLINE hkBool hkpStorageSampledHeightFieldShape::getTriangleFlipImpl(HKP_SHAPE_VIRTUAL_THIS2) HKP_SHAPE_VIRTUAL_CONST
{	
	const hkpStorageSampledHeightFieldShape* thisObj = static_cast<const hkpStorageSampledHeightFieldShape*>(HK_GET_THIS_PTR);
	return thisObj->m_triangleFlip;
}

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
