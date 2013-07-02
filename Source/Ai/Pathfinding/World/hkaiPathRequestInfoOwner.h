/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HKAI_PATH_REQUESTER_H
#define HKAI_PATH_REQUESTER_H

	/// Basic interface for requesting paths from hkaiWorld.
	/// The prePathSearchCallback is fired during hkaiWorld::stepPathSearches(), allowing you a chance to update
	/// any information in the request.
	/// For example, the start face index for A* might change from frame to frame because nav mesh cutting would change the face indices.
	/// If prePathSearchCallback() returns false, the request will not be fully processed by the hkaiWorld; instead it will be treated
	/// as a search failure.
class hkaiPathRequestInfoOwner
{
	//+hk.MemoryTracker(ignore=True)
public:
	virtual ~hkaiPathRequestInfoOwner () { }
	virtual bool prePathSearchCallback(class hkaiNavMeshPathRequestInfo* request) = 0;
	virtual bool prePathSearchCallback(class hkaiNavVolumePathRequestInfo* request) = 0;
};

#endif //HKAI_PATH_REQUESTER_H

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
