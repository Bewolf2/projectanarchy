/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline void HK_CALL hkaiNavVolumeUtils::getPortalBetweenCellAabbs( const hkAabb& aabb1, const hkAabb& aabb2, hkAabb& portalOut )
{
	// Find the intersection between the two cells
	hkAabb intersect;
	intersect.m_max.setMin( aabb1.m_max, aabb2.m_max );
	intersect.m_min.setMax( aabb1.m_min, aabb2.m_min );

	// For cells that may be just slightly separated (resulting in a degenerate intersection) normalize the AABB to 
	// form the portal
	portalOut.m_min.setMin( intersect.m_min, intersect.m_max );
	portalOut.m_max.setMax( intersect.m_min, intersect.m_max );
}

inline void HK_CALL hkaiNavVolumeUtils::getMinimizingPortalPoint( hkVector4Parameter p1, hkVector4Parameter p2, const hkAabb& portal, hkVector4& pointOut )
{
	
	// Raycast against the plane
	hkVector4 dir; dir.setSub(p2, p1);
	hkVector4 hitPointOnPlane;

	hkSimdReal t;
	bool hits = getPortalPlaneIntersection(p1, p2, portal, t);
	if( !hits )
	{
		// It's possible for the ray to be parallel to the plane, e.g. if the previous and next point were clipped against the same edge:
		//
		//  *-----A
		//        |
		//        P
		//        |
		//        B-----*
		// where A and B are the previous and next points, and P is the portal we're examining.
		// If this happens, just take the midpoint of the two points
		hitPointOnPlane.setInterpolate( p1, p2, hkSimdReal_Half );
	}
	else
	{
		hitPointOnPlane.setInterpolate( p1, p2, t );
	}

	// Clamp the raycast hit to lie in the portal
	hkVector4 closestPointOnPortal;
	closestPointOnPortal.setMin(portal.m_max, hitPointOnPlane);
	closestPointOnPortal.setMax(portal.m_min, closestPointOnPortal);

	pointOut = closestPointOnPortal;
}

inline bool HK_CALL hkaiNavVolumeUtils::getPortalPlaneIntersection( hkVector4Parameter p1, hkVector4Parameter p2, const hkAabb& portal, hkSimdReal& tOut )
{
	hkVector4 _extents;
	_extents.setSub(portal.m_max, portal.m_min);
	_extents.setAbs(_extents);

	hkVector4 center; portal.getCenter(center);

	// Find the plane of the portal. The direction of the plane is given by the shortest component of the extents.
	hkVector4 extentsX; extentsX.setBroadcast<0>(_extents);
	hkVector4 extentsY; extentsY.setBroadcast<1>(_extents);
	hkVector4 extentsZ; extentsZ.setBroadcast<2>(_extents);
	hkVector4 planeX = hkVector4::getConstant<HK_QUADREAL_1000>();
	hkVector4 planeY = hkVector4::getConstant<HK_QUADREAL_0100>();
	hkVector4 planeZ = hkVector4::getConstant<HK_QUADREAL_0010>();

	hkVector4 plane = planeX;
	hkVector4 extents = extentsX; // smallest extent value

	{
		hkVector4Comparison comp = extentsY.less(extents);
		plane.setSelect(comp, planeY, plane);			// plane   = (extentsY < extentsX) ? planeY   : planeX;
		extents.setSelect(comp, extentsY, extents);	// extents = (extentsY < extentsX) ? extentsY : extentsX;
	}

	{
		hkVector4Comparison comp = extentsZ.less(extents);
		plane.setSelect(comp, planeZ, plane);
	}

	plane.setPlaneConstant(center);

	
	// Raycast against the plane

	hkVector4 dir; dir.setSub(p2, p1);
	hkSimdReal absPlaneDotDir; absPlaneDotDir.setAbs( plane.dot<3>(dir) );

	if( absPlaneDotDir < hkSimdReal_Eps)
	{
		return false;
	}
	else
	{
		tOut = - plane.dot4xyz1(p1) / plane.dot<3>(dir);
		return true;
	}

}

template<typename Volume>
inline void HK_CALL hkaiNavVolumeUtils_getCellWorldAabb(const Volume& volume, const hkaiNavVolume::Cell& cell, hkAabb& aabbOut)
{
	hkAabb tempAabb;

#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	// hkaiNavVolume::Cell look like
	//   xxyyzz##XXYYZZ
	// where each character is 2 bytes. So we can load it up in an hkIntVector and do the
	// float-to-int conversion there.

	hkIntVector vectorCell;
	vectorCell.loadNotAligned<4>((const hkUint32*)&cell);

	hkIntVector minUint, maxUint;
	hkIntVector zero; zero.setZero();
	minUint.setCombineHead16To32(zero, vectorCell); // minUint = 00xx 00yy 00zz 0000
	maxUint.setCombineTail16To32(zero, vectorCell);  // maxUint = 00XX 00YY 00ZZ 0000

	minUint.convertU32ToF32(tempAabb.m_min);
	maxUint.convertU32ToF32(tempAabb.m_max);
#else
	tempAabb.m_min.set( (hkReal) cell.m_min[0], (hkReal) cell.m_min[1], (hkReal) cell.m_min[2]);
	tempAabb.m_max.set( (hkReal) cell.m_max[0], (hkReal) cell.m_max[1], (hkReal) cell.m_max[2]);
#endif
	hkVector4 quantizationOffset, quantizationScale;
	volume.getOffsetAndScale(quantizationOffset, quantizationScale);
	tempAabb.m_min.mul( quantizationScale  );
	tempAabb.m_min.add( quantizationOffset );
	tempAabb.m_max.mul( quantizationScale  );
	tempAabb.m_max.add( quantizationOffset );

	aabbOut.m_min = tempAabb.m_min;
	aabbOut.m_max = tempAabb.m_max;
}

inline void HK_CALL hkaiNavVolumeUtils::getCellWorldAabb(const hkaiNavVolumeAccessor& volumeAccessor, const hkaiNavVolume::Cell& cell, hkAabb& aabbOut)
{
	hkaiNavVolumeUtils_getCellWorldAabb(volumeAccessor, cell, aabbOut);
}


inline void HK_CALL hkaiNavVolumeUtils::getCellWorldAabb(const hkaiNavVolume& volume, const hkaiNavVolume::Cell& cell, hkAabb& aabbOut)
{
	hkaiNavVolumeUtils_getCellWorldAabb(volume, cell, aabbOut);
}

template<typename Volume>
inline void HK_CALL hkaiNavVolumeUtils_getCellLocalCenter(const Volume& volume, hkaiNavVolume::CellIndex cellIndex, hkVector4& centerOut)
{
	hkAabb aabb;
	hkaiNavVolumeUtils::convertCellToLocalAabb( volume.getCell( cellIndex ), aabb );
	aabb.getCenter( centerOut );
}

inline void HK_CALL hkaiNavVolumeUtils::getCellLocalCenter(const hkaiNavVolume& volume, hkaiNavVolume::CellIndex cellIndex, hkVector4& centerOut)
{
	hkaiNavVolumeUtils_getCellLocalCenter(volume, cellIndex, centerOut);
}

inline void HK_CALL hkaiNavVolumeUtils::getCellLocalCenter(const hkaiNavVolumeAccessor& volumeInstance, hkaiNavVolume::CellIndex cellIndex, hkVector4& centerOut)
{
	hkaiNavVolumeUtils_getCellLocalCenter(volumeInstance, cellIndex, centerOut);
}

inline void HK_CALL hkaiNavVolumeUtils::convertCellToLocalAabb(const hkaiNavVolume::Cell& cell, hkAabb& aabbOut)
{
	aabbOut.m_min.set( (hkReal) cell.m_min[0], (hkReal) cell.m_min[1], (hkReal) cell.m_min[2]);
	aabbOut.m_max.set( (hkReal) cell.m_max[0], (hkReal) cell.m_max[1], (hkReal) cell.m_max[2]);
}

inline void HK_CALL hkaiNavVolumeUtils::getCellWorldAabb(const hkaiNavVolume& volume, int cellIndex, hkAabb& aabbOut)
{
	hkaiNavVolumeUtils_getCellWorldAabb( volume, volume.getCell(cellIndex), aabbOut );
}

inline void HK_CALL hkaiNavVolumeUtils::getCellWorldAabb(const hkaiNavVolumeAccessor& volume, int cellIndex, hkAabb& aabbOut)
{
	hkaiNavVolumeUtils_getCellWorldAabb( volume, volume.getCell(cellIndex), aabbOut );
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
