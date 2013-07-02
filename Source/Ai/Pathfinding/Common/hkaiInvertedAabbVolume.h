/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_INVERTED_AABB_VOLUME_H
#define HKAI_INVERTED_AABB_VOLUME_H

#include <Ai/Pathfinding/Common/hkaiVolume.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

extern const class hkClass hkaiInvertedAabbVolumeClass;

/// An implementation of the hkaiVolume interface representing the exterior of an AABB.
class hkaiInvertedAabbVolume : public hkaiVolume
{
	//+vtable(true)
	//+version(0)
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		hkaiInvertedAabbVolume(const hkAabb& aabb);
		hkaiInvertedAabbVolume(hkFinishLoadedObjectFlag f) : hkaiVolume(f), m_geometry(f) {}

		virtual void calcPointsInside(const hkVector4* points, int numPoints, hkBitField& pointsInside) const HK_OVERRIDE;

		virtual hkBool32 containsTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c) const HK_OVERRIDE;

		virtual hkBool32 containsAabb( const hkAabb& aabbIn ) const HK_OVERRIDE;

		virtual hkResult calcGeometry(hkGeometry& geom) const HK_OVERRIDE;

		virtual void getAabb( hkAabb& aabbOut ) const HK_OVERRIDE;

	private:

		hkAabb m_aabb;
		hkGeometry m_geometry;
};

#endif // HKAI_INVERTED_AABB_VOLUME_H

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
