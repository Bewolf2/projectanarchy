/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline const hkpConvexShape* hkpConvexTransformShape::getChildShape() const 
{
#if defined(HK_PLATFORM_SPU)
	// fetch child shape if it is not yet present in SPU's local shape buffer
	// (if m_childShapeSize == 0 the child shape already follows this shape consecutively in memory, otherwise we need to DMA it in right after this shape)
	const hkpConvexShape* ret = this + 1;

	if ( m_childShapeSizeForSpu > 0 )
	{
		getChildShapeFromPpu(sizeof(*this));
	}

	HKCD_PATCH_SHAPE_VTABLE( const_cast<hkpConvexShape*>(ret) );
	return ret;
#else
	return static_cast<const hkpConvexShape*>( m_childShape.getChild() ); 
#endif
}

inline void hkpConvexTransformShape::setTransform( const hkTransform& transform)
{
	m_transform.setFromTransformNoScale( transform );
	m_extraScale.setZero();
}

inline void hkpConvexTransformShape::getTransform( hkTransform* HK_RESTRICT transform ) const
{ 
	HK_ON_DEBUG( hkSimdReal eps; eps.setFromFloat( HK_REAL_EPSILON ); );
	HK_WARN_ON_DEBUG_IF( !m_transform.m_scale.allEqual<3>( hkVector4::getConstant<HK_QUADREAL_1>(), eps ), 0x37b36bdf, "Non-identity scale is being lost in hkQsTransform to hkTransform conversion" );
	m_transform.copyToTransformNoScale( *transform );
}

inline const hkQsTransform& hkpConvexTransformShape::getQsTransform() const 
{
	return m_transform; 
}

inline const hkVector4& hkpConvexTransformShape::getExtraScale() const 
{
	return m_extraScale;
}

#ifdef HK_PLATFORM_SPU

inline void hkpConvexTransformShape::initializeSpu( const hkpConvexShape* childShape, const hkQsTransform& transform, hkReal radius )
{
	
	setType(HKCD_SHAPE_TYPE_FROM_CLASS(hkpConvexTransformShape));
	HKCD_PATCH_SHAPE_VTABLE(this);
	m_childShape = childShape; 
	m_childShapeSizeForSpu = 0;
	m_radius = radius;
	setTransform(transform);
	m_userData = childShape->getUserData();
}

#endif

HK_FORCE_INLINE int hkpConvexTransformShape::getNumCollisionSpheres() const
{
	return getChildShape()->getNumCollisionSpheres();
}

HK_FORCE_INLINE void hkpConvexTransformShape::transformVertex(hkVector4Parameter localVertex, hkVector4* HK_RESTRICT vertexOut) const
{
	// Reconstruct aabb center from w coordinates
	hkVector4 center;
	center.set(m_transform.m_translation.getW(), m_transform.m_scale.getW(), m_extraScale.getW(), hkSimdReal_0);

	// Apply transform's scale and add then apply the extra scale relative to the aabb center
	hkVector4 vertexInAabb; vertexInAabb.setSub(localVertex, center);	
	vertexOut->setMul(localVertex, m_transform.getScale());
	vertexOut->addMul(m_extraScale, vertexInAabb);

	// Rotate and translate
	vertexOut->_setRotatedDir(m_transform.getRotation(), *vertexOut);
	vertexOut->add(m_transform.getTranslation());
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
