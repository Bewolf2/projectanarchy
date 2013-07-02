/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshLinker.hpp>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshInstance.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>

// for navmesh streaming
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingUtils.h>

vHavokAiNavMeshLinker::vHavokAiNavMeshLinker()
{

}

vHavokAiNavMeshLinker::~vHavokAiNavMeshLinker()
{
	ClearNavMeshes();
}

void vHavokAiNavMeshLinker::ClearNavMeshes()
{
	m_navMeshes.clear();
}

void vHavokAiNavMeshLinker::AddNavMesh(vHavokAiNavMeshInstance* navMesh)
{
	m_navMeshes.pushBack(navMesh);
}

bool vHavokAiNavMeshLinker::LinkNavMeshes()
{
  NavMeshLinkSetting navMeshLinkSetting;
  navMeshLinkSetting.m_edgeMatchTolerance = 0.1f;
  navMeshLinkSetting.m_maxStepHeight = 0.5f;
  navMeshLinkSetting.m_maxOverhang = 1e-2f;
  navMeshLinkSetting.m_maxSeparation = 0.1f;
  navMeshLinkSetting.m_cosPlanarAlignmentAngle = 5.0f;
  navMeshLinkSetting.m_cosVerticalAlignmentAngle = 60.0f;
  navMeshLinkSetting.m_minEdgeOverlap = 0.02f;

  return LinkNavMeshes(navMeshLinkSetting);
}

bool vHavokAiNavMeshLinker::LinkNavMeshes(const NavMeshLinkSetting& navMeshLinkSetting)
{
	const int numNavMeshes = m_navMeshes.getSize();
	hkLocalArray<hkaiNavMesh*> havokAiNavMeshes(numNavMeshes);
	hkLocalArray<hkaiNavMeshInstance*> havokAiInstances(numNavMeshes);
	hkLocalArray<hkaiNavMeshQueryMediator*> havokAiMediators(numNavMeshes);
	hkLocalArray<hkAabb> aabbs(numNavMeshes);
	hkLocalArray<hkaiSectionUid> arrayIndexToUid(numNavMeshes);
	{
		for (int idx = 0; idx < numNavMeshes; ++idx)
		{
			vHavokAiNavMeshInstance* visionInstance = m_navMeshes[idx];
			vHavokAiNavMeshResource* visionResource = m_navMeshes[idx]->GetResource();

			hkaiNavMeshInstance* havokInstance = visionInstance->GetNavMeshInstance();
			hkaiNavMeshQueryMediator* havokMediator = visionResource->GetNavMeshQueryMediator();
			hkaiNavMesh* havokNavMesh = visionResource->GetNavMesh();
#if defined(HK_DEBUG_SLOW)
			VASSERT(havokInstance->getOriginalMesh() == havokNavMesh);
			hkaiNavMeshUtils::validate(*havokInstance->getOriginalMesh());
#endif

			// this should be sufficient as we should have all the navmeshes at this point (Even the unloaded ones?)
			hkaiSectionUid uid = havokInstance->getSectionUid();
			if (arrayIndexToUid.indexOf(uid) != -1)
			{
				// duplicate key found!
				HK_WARN(0x69585a95, "Cannot stitch navmeshes together because duplicate hkaiSectionUid found for navmesh instance");
				return false;
			}
			arrayIndexToUid.pushBack(uid);

			havokAiInstances.pushBack(havokInstance);
			havokAiMediators.pushBack(havokMediator);
			havokAiNavMeshes.pushBack(havokNavMesh);

			hkAabb aabb;
			havokInstance->getAabb(aabb);
			aabbs.pushBack(aabb);
		}
	}

	// stitch together navMeshes
	if (havokAiInstances.getSize() > 1)
	{
		// Find overlaps between AABBs
		hkArray<hkKeyPair>::Temp pairs;
		hkaiStreamingUtils::findPotentialDependencies(aabbs, pairs);

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR < 2012)
		hkaiStreamingUtils::EdgeRemap edgeRemap;
#endif

		// Find potential connections between the nav mesh tiles
		for (int i = 0; i < pairs.getSize(); i++)
		{
			hkUint32 indexA = pairs[i].m_keyA;
			hkUint32 indexB = pairs[i].m_keyB;

			hkaiSectionUid uidA = arrayIndexToUid[indexA];
			hkaiSectionUid uidB = arrayIndexToUid[indexB];
			VASSERT(havokAiInstances[indexA]->getSectionUid() == uidA);
			VASSERT(havokAiInstances[indexA]->getOriginalMesh() == havokAiNavMeshes[indexA]);
			VASSERT(havokAiInstances[indexB]->getSectionUid() == uidB);
			VASSERT(havokAiInstances[indexB]->getOriginalMesh() == havokAiNavMeshes[indexB]);

			hkaiStreamingUtils::FindEdgeOverlapInput input;
			input.m_meshA = havokAiNavMeshes[indexA];
			input.m_uidA = uidA;
			input.m_mediatorA = havokAiMediators[indexA];
			input.m_meshB = havokAiNavMeshes[indexB];
			input.m_uidB = uidB;
			input.m_mediatorB = havokAiMediators[indexB];

      // set link settings
      input.m_edgeMatchTolerance = navMeshLinkSetting.m_edgeMatchTolerance;
      input.m_edgeMatchingParams.m_maxStepHeight = navMeshLinkSetting.m_maxStepHeight;
      input.m_edgeMatchingParams.m_maxOverhang = navMeshLinkSetting.m_maxOverhang; // slightly bigger than the default, since the meshes were built independently.
      input.m_edgeMatchingParams.m_maxSeparation = navMeshLinkSetting.m_maxSeparation;
      input.m_edgeMatchingParams.m_cosPlanarAlignmentAngle = navMeshLinkSetting.m_cosPlanarAlignmentAngle;
      input.m_edgeMatchingParams.m_cosVerticalAlignmentAngle = navMeshLinkSetting.m_cosVerticalAlignmentAngle;
      input.m_edgeMatchingParams.m_minEdgeOverlap = navMeshLinkSetting.m_minEdgeOverlap;

			// For each overlapping pair of AABBs, try to match up edges between the meshes

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
			hkaiStreamingUtils::generateStreamingInfo(input);
#else
			hkaiStreamingUtils::findEdgeOverlaps(input, edgeRemap);
#endif
		}

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR < 2012)
		// Process all the edge changes that we accumulated
		hkaiStreamingUtils::remapSplitEdges(havokAiNavMeshes, edgeRemap);
#endif
	}

	for (int idx = 0; idx < havokAiInstances.getSize(); ++idx)
	{
		hkaiNavMeshUtils::validate(*havokAiInstances[idx]->getOriginalMesh());
	}
	return true;
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
