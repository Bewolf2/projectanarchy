/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_NAVMESH_LINKER_HPP
#define __VHAVOK_AI_NAVMESH_LINKER_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>

class vHavokAiNavMeshInstance;

class vHavokAiNavMeshLinker
{
public:
  struct NavMeshLinkSetting
  {
    float m_edgeMatchTolerance;
    float m_maxStepHeight;
    float m_maxSeparation;
    float m_maxOverhang;
    float m_cosPlanarAlignmentAngle;
    float m_cosVerticalAlignmentAngle;
    float m_minEdgeOverlap;
  };

	VHAVOKAI_IMPEXP vHavokAiNavMeshLinker();
	VHAVOKAI_IMPEXP ~vHavokAiNavMeshLinker();

	VHAVOKAI_IMPEXP void ClearNavMeshes();

		/// add navmesh to be linked
	VHAVOKAI_IMPEXP void AddNavMesh(vHavokAiNavMeshInstance* navMesh);

		/// Call this function to stitch together the navmeshes that have been added to this linker.
  VHAVOKAI_IMPEXP bool LinkNavMeshes();
  VHAVOKAI_IMPEXP bool LinkNavMeshes(const NavMeshLinkSetting& navMeshLinkSetting);

protected:
	hkArray<vHavokAiNavMeshInstance*>	m_navMeshes;
};

#endif	// __VHAVOK_AI_NAVMESH_BUILDER_HPP

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
