/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_PLATFORM_SPU)
#	include <Common/Base/Spu/Config/hkSpuConfig.h>
#	include <Common/Base/Memory/PlatformUtils/Spu/SpuDmaCache/hkSpu4WayCache.h>
extern class hkSpu4WayCache* g_SpuCollideUntypedCache;
#endif

//
//	Sets the shape info at the given shape key

HK_FORCE_INLINE void hkpListShape::setShapeInfo(hkpShapeKey shapeKey, hkUint16 newShapeInfo)
{
	m_childInfo[shapeKey].m_shapeInfo = newShapeInfo;
}

//
//	Gets the shape info at the given shape key

HK_FORCE_INLINE hkUint16 hkpListShape::getShapeInfo(hkpShapeKey shapeKey) const
{
#ifdef HK_PLATFORM_SPU
	const ChildInfo* cInfo = hkGetArrayElemUsingCacheHalfSize<ChildInfo>(m_childInfo.begin(), shapeKey, g_SpuCollideUntypedCache, HK_SPU_UNTYPED_CACHE_LINE_SIZE);
	return cInfo->m_shapeInfo;
#else
	return m_childInfo[shapeKey].m_shapeInfo;
#endif
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
