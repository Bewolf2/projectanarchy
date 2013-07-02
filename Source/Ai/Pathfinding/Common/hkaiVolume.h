/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VOLUME_H
#define HKAI_VOLUME_H

extern const class hkClass hkaiVolumeClass;

class hkBitField;
struct hkGeometry;

/// The hkaiVolume class is an interface used to describe a volume object. This can be used to describe:
/// - An area where a character shouldn't directly interact (a carver). See hkaiNavMeshGenerationSettings::m_carvers and hkaiNavVolumeGenerationSettings::m_carvers.
/// - An area with a specific material, independent of the original triangulation (a painter). See hkaiNavMeshGenerationSettings::m_carvers and hkaiNavVolumeGenerationSettings::m_carvers.
/// - An area with different generation settings (an override). See hkaiNavMeshGenerationUtils::m_overrideSettings.
///
/// Note that an hkaiVolume implementation can be 'inverted' - that is, the space considered 'outside' of the volume is 
/// bounded, and the space considered 'inside' the volume is unbounded. See \ref hkaiInvertedAabbVolume for an example.
/// Inverted volumes can be useful to remove everything except a particular subset of a walkable area.
class hkaiVolume : public hkReferencedObject
{
	//+vtable(true)
	//+version(0)
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		hkaiVolume() {}
		hkaiVolume(hkFinishLoadedObjectFlag f) : hkReferencedObject(f) {}

			/// Compute the AABB of the volume. This is used for building a bounding volume around the carvers.
			/// For inverted volumes (where the 'inside' space of the volume is unbounded), this should return
			/// an AABB of maximum size (i.e. [-HK_REAL_MAX, HK_REAL_MAX]). See \ref hkaiInvertedAabbVolume for
			/// an example of this usage.
		virtual void getAabb( class hkAabb& aabbOut ) const = 0;

			/// A bit is set in the bit field pointsInside if the corresponding point is inside the volume.
		virtual void calcPointsInside(const hkVector4* points, int numPoints, hkBitField& pointsInside) const = 0;

			/// Create geometry that represents the volume.
			/// Returns HK_FAILURE if any necessary memory allocations failed. Otherwise HK_SUCCESS.
			/// When used a carver, this geometry will be added to the extrusion geometry which when subtracted from the
			/// walkable surface will cut the carvers volume out of the walkable surface, making the inside of the
			/// carver separate, from the rest of the walkable surface.
		virtual hkResult calcGeometry(hkGeometry& geom) const = 0;

			/// Returns whether the given triangle is fully contained in the volume. False negatives are allowed.
		virtual hkBool32 containsTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c) const = 0;

			/// Returns whether the given AABB is fully contained in the volume.
			/// This is currently only used to speed up carvers in nav volume building.
			/// False negatives are allowed.
		virtual hkBool32 containsAabb( const hkAabb& aabbIn ) const = 0;
};

#endif // HKAI_VOLUME_H

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
