/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_MATERIAL_PAINTER_H
#define HKAI_MATERIAL_PAINTER_H

#include <Common/Base/Types/hkRefPtr.h>

extern const class hkClass hkaiMaterialPainterClass;

class hkaiVolume;

	/// A material painter is an hkaiVolume which can be used at navmesh generation
	/// time to mark all the geometry contained in it with a given material.
	/// Faces partially contained inside the painter volume are cut along the volume
	/// boundary, and only those resulting pieces which are completely contained in
	/// the volume are affected.
class hkaiMaterialPainter : public hkReferencedObject
{
		//+version(0)
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();


		/// Callback to resolve overlapping material volumes
		class ResolveOverlapCallback
		{
		public:
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiMaterialPainter::ResolveOverlapCallback);

			/// Callback function for resolving material painting for overlapping painters
			/// \param testPoint Point being tested against painter volumes
			/// \param originalMaterial Material value before any painter is applied
			/// \param overlappingPainters Set of material painters that all contain this test point
			/// \return Material value to apply at this point
			
			virtual int resolveOverlappingPainters( hkVector4Parameter testPoint, int originalMaterial, const hkArrayBase<const hkaiMaterialPainter*>& overlappingPainters ) = 0;
			

			virtual ~ResolveOverlapCallback() {}
		};


		hkaiMaterialPainter(hkFinishLoadedObjectFlag f);

		hkaiMaterialPainter(int material, const hkaiVolume* volume);
		virtual ~hkaiMaterialPainter();

		HK_FORCE_INLINE int getMaterial() const { return m_material; }
		HK_FORCE_INLINE const hkaiVolume& getVolume() const { return *(m_volume.val()); }
		HK_FORCE_INLINE const hkaiVolume* getVolumePtr() const { return m_volume; }

		HK_FORCE_INLINE void setMaterial(int material) { m_material = material; }
		void setVolume(const hkaiVolume* volume);

	private:

		int m_material;
		hkRefPtr<const hkaiVolume> m_volume;

};

#endif // HKAI_MATERIAL_PAINTER_H

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
