/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_BEHAVIOR_RESOURCE_MANAGER_HPP
#define __VHAVOK_BEHAVIOR_RESOURCE_MANAGER_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorIncludes.hpp>

class vHavokBehaviorResource;

class vHavokBehaviorResourceManager : public VisResourceManager_cl
{
public:

	/// Constructor / Destructor / Global Accessor
	vHavokBehaviorResourceManager();
	VOVERRIDE ~vHavokBehaviorResourceManager();
	VHAVOKBEHAVIOR_IMPEXP static vHavokBehaviorResourceManager* GetInstance() {return &g_GlobalManager;}

	/// should be called at plugin initialization time
	void OneTimeInit();
	/// should be called at plugin de-initialization time
	void OneTimeDeInit();

	/// Virtual function that should be implemented by each resource manager to create a resource 
	VHAVOKBEHAVIOR_IMPEXP VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);

	/// Returns the project asset manager
	hkbProjectAssetManager* GetProjectAssetManager() const;

protected:
	/// one global instance of our manager
	static vHavokBehaviorResourceManager g_GlobalManager;

	/// The project asset manager used to load assets
	hkbProjectAssetManager* m_projectAssetManager;
};

#endif	// __VHAVOK_BEHAVIOR_RESOURCE_MANAGER_HPP

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
