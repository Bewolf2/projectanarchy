/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_CHARACTER_REPATH_LISTENER_H
#define HK_AI_CHARACTER_REPATH_LISTENER_H

#include <Ai/Pathfinding/Character/hkaiCharacter.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>

class hkaiBehavior;

	/// A simple listener that load-balances the repathing of a list of characters
class hkaiCharacterRepathListener : public hkReferencedObject, public hkaiWorld::Listener
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

	hkaiCharacterRepathListener();
	~hkaiCharacterRepathListener();

	//
	// hkaiWorld::Listener interface
	//
	void dynamicNavMeshModifiedCallback( hkaiWorld::NavMeshModifiedCallbackContext& context );
	void postStepCallback(class hkaiWorld* world, const hkArrayBase<hkaiBehavior*>& behaviors);
	void navMeshInstanceRemoved( const class hkaiWorld* world, hkaiNavMeshInstance* navMeshInstance, hkaiDirectedGraphInstance* hierarchyGraph  );

		/// Given a list of AI behaviors, determine which behaviors' paths overlap with cut faces or unloaded sections.
	void collideFacesAgainstPaths(const hkaiStreamingCollection* collection, const hkArrayBase<hkaiBehavior*>& behaviors, hkBitField& indicesToRepathOut) const;

	inline void setPathDistanceHorizon( hkReal d ) { m_pathDistanceHorizon = d; }
	inline hkReal getPathDistanceHorizon() const { return m_pathDistanceHorizon; }

	inline void setRepathIfSectionUnloaded( bool repath ) { m_repathIfSectionUnloaded = repath; }
	inline bool getRepathIfSectionUnloaded() const { return m_repathIfSectionUnloaded; }
	
protected:
		/// Distance out along paths that is checked for collision with cut faces.
		/// This defaults to HK_REAL_MAX (the entire path is checked), but could be reduced to improve performance.
	hkReal m_pathDistanceHorizon;

		/// Faces cut in the current hkaiWorld step.
		/// These are later processed by collideFacesAgainstPaths
	hkArray<hkaiPackedKey> m_cutFaceKeys;

		/// List of sections that have been unloaded since the last frame.
		/// Characters with paths through these sections are repathed.
	hkSet<hkaiRuntimeIndex> m_unloadedSections;

		/// Whether or not paths are invalidated when a section that they go through is unloaded.
	hkBool m_repathIfSectionUnloaded; //+default(true)
};
#endif // HK_AI_CHARACTER_REPATH_LISTENER_H

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
