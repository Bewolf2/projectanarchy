/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_NAV_MESH_GEN_SNAPSHOT_H
#define HK_AI_NAV_MESH_GEN_SNAPSHOT_H

#include <Ai/Internal/NavMesh/hkaiNavMeshGenerationSettings.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>

extern const hkClass hkaiNavMeshGenerationSnapshotClass;

	/// A simple container class for saving the input settings and geometry for nav mesh generation
class hkaiNavMeshGenerationSnapshot
{
	//+version(1)
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiNavMeshGenerationSnapshot);

	hkaiNavMeshGenerationSnapshot( const  hkGeometry& geometry, const hkaiNavMeshGenerationSettings& settings );
	hkaiNavMeshGenerationSnapshot(hkFinishLoadedObjectFlag f)
	:	m_geometry(f),
		m_settings(f)
	{	}

	const hkGeometry& getGeometry() const { return m_geometry; }
	const hkaiNavMeshGenerationSettings& getSettings() const { return m_settings; }

protected:
	struct hkGeometry m_geometry;
	struct hkaiNavMeshGenerationSettings m_settings;

};
#endif // HK_AI_NAV_MESH_GEN_SNAPSHOT_H

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
