/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_NAVMESH_SILHOUETTE_SELECTOR_H
#define HK_NAVMESH_SILHOUETTE_SELECTOR_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/Dynamic/Silhouette/hkaiCompoundSilhouette.h>

	/// Callback from hkaiNavMeshCutter::cutSilhouettesIncremental to query which original faces need to be cut and by what shapes.
class hkaiNavMeshSilhouetteSelector
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshSilhouetteSelector);
	virtual ~hkaiNavMeshSilhouetteSelector() {}

	/// Return a list of face keys which must be cut this frame, and a list of face keys which were cut before but aren't now.
	/// If the bitfield is non-null, it specifies which sections will be updated.
	virtual void getUpdatedFaces( const class hkaiNavMeshCutter* cutter, hkArray<hkaiPackedKey>::Temp& cutFaceKeysOut, hkArray<hkaiPackedKey>::Temp& uncutFaceKeysOut, const class hkBitField* sectionsToUpdate ) = 0;

	/// For a given face, return a list of silhouettes which are relevant to this face.
	virtual void gatherSilhouettesForFace( hkaiPackedKey faceKey, const struct hkaiSilhouetteGenerationParameters& genParams, hkArray< hkaiCompoundSilhouette >::Temp& silsOut, hkArray< int >::Temp& silMaterialIds ) = 0;

	/// Setup a multithreaded cutting job for the face
	virtual void setupJobForFace( hkaiPackedKey faceKey, struct hkaiNavMeshCutFaceJob& job )
	{
		HK_ASSERT2(0x629988e7, false, "Multithreaded setup not supported for this selector.");
	}
};

#endif // HK_NAVMESH_SILHOUETTE_SELECTOR_H

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
