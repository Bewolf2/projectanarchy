/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline const hkpShape* hkpTransformShape::getChildShape() const 
{ 
#if defined(HK_PLATFORM_SPU)
	// fetch child shape if it is not yet present in spu's local shape buffer
	// (if m_childShapeSize == 0 the child shape already follows this shape consecutively in memory, otherwise we need to dma it in right after this shape)
	const hkpShape* ret = this + 1;

	if ( m_childShapeSize > 0 )
	{
		getChildShapeFromPpu();
	}

	HKP_PATCH_CONST_SHAPE_VTABLE( ret );
	return ret;
#else
	return m_childShape.getChild();
#endif
}

inline const hkTransform& hkpTransformShape::getTransform() const 
{ 
	return m_transform; 
}

inline const hkQuaternion& hkpTransformShape::getRotation() const
{
	return m_rotation;
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
