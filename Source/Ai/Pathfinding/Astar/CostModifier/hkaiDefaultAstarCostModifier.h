/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_DEFAULT_NAV_MESH_COST_MODIFIER_H
#define HK_AI_DEFAULT_NAV_MESH_COST_MODIFIER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.h>
#include <Ai/Pathfinding/Astar/CostModifier/hkaiAstarCostModifier.h>

extern const class hkClass hkaiDefaultAstarCostModifierClass;

struct hkaiAgentTraversalInfo;

/// For nav mesh searches, this cost modifier uses the user data stored in the faces to decide how to affect the path cost.
/// The modified cost is computed by adding a fraction of m_maxCostPenalty to the original cost.
/// This fraction is determined by the face user data - 0x0 means no penalty, 0xFFFFFFFF means full penalty.
///
/// For nav volume searches, if the face data is in the valid range, it is used as an index into the
/// m_costMultiplierLookupTable array, otherwise no modification is performed.
///
/// See NavMeshCostModificationDemo and NavVolumeCostModificationDemo for an example of how to use this.
class hkaiDefaultAstarCostModifier : public hkaiAstarCostModifier
{
	//+version(3)

	public:

		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );

		enum 
		{
			NAV_VOLUME_LOOKUP_TABLE_SIZE = 32
		};
		
			/// Constructor, by default enables all collisions between all layers
		hkaiDefaultAstarCostModifier();

		hkaiDefaultAstarCostModifier(hkFinishLoadedObjectFlag flag) 
			: hkaiAstarCostModifier(flag) { }

		~hkaiDefaultAstarCostModifier() {}

			/// Sets the cost multiplier for the nav volume cell category.
		void setCostMultiplier( int cellCategory, hkReal multiplier );

		//
		// hkaiAstarCostModifier interface
		//

			/// Returns the modified cost of walking through the pair of edges (and through the face)
		virtual hkSimdReal getModifiedCost( const NavMeshGetModifiedCostCallbackContext& context ) const HK_OVERRIDE;

			/// Returns the modified cost of walking through the pair of cells (and through the edge)
		virtual hkSimdReal getModifiedCost( const NavVolumeGetModifiedCostCallbackContext& context ) const HK_OVERRIDE;


	public:

		hkReal m_maxCostPenalty; //+default(1.0f)

		hkHalf m_costMultiplierLookupTable[ NAV_VOLUME_LOOKUP_TABLE_SIZE ];

};

#endif // HK_AI_DEFAULT_NAV_MESH_COST_MODIFIER_H

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
