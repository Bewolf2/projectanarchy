/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Physics2012/Dynamics/Entity/Util/hkpEntityCallbackUtil.h>
#include <Physics2012/Dynamics/World/Util/hkpWorldCallbackUtil.h>

#if defined (HK_PLATFORM_SPU)
#include <Physics2012/Dynamics/Collide/Deprecated/Dispatch/hkpSpuCollisionCallbackUtil.h>
#endif

void HK_CALL hkFireContactPointAddedCallback(hkpWorld* world, hkpEntity* entityA, hkpEntity* entityB, hkpContactPointAddedEvent& event)
{
#if !defined (HK_PLATFORM_SPU)
	{
		hkpWorldCallbackUtil ::fireContactPointAdded( world,   event );
		hkpEntityCallbackUtil::fireContactPointAdded( entityA, event );
		hkpEntityCallbackUtil::fireContactPointAdded( entityB, event );
	}
#else
	if( g_FireContactPointAddedCallback )
	{
		g_FireContactPointAddedCallback( entityA, entityB, event ); 
	}
#endif
}

void HK_CALL hkFireContactProcessCallback(hkpWorld* world, hkpEntity* entityA, hkpEntity* entityB, hkpContactProcessEvent& event)
{
#if !defined (HK_PLATFORM_SPU)
	{
		hkpWorldCallbackUtil ::fireContactProcess( world,   event );
		hkpEntityCallbackUtil::fireContactProcess( entityA, event );
		hkpEntityCallbackUtil::fireContactProcess( entityB, event );
	}
#else
	if( g_FireContactProcessCallback )
	{
		g_FireContactProcessCallback( entityA, entityB, event ); 
	}
#endif
}

void HK_CALL hkFireContactPointRemovedCallback(hkpWorld* world, hkpEntity* entityA, hkpEntity* entityB, hkpContactPointRemovedEvent& event)
{
#if !defined (HK_PLATFORM_SPU)
	{
		hkpWorldCallbackUtil ::fireContactPointRemoved( world,   event );
		hkpEntityCallbackUtil::fireContactPointRemoved( entityA, event );
		hkpEntityCallbackUtil::fireContactPointRemoved( entityB, event );
	}
#else
	if( g_FireContactPointRemovedCallback )
	{
		g_FireContactPointRemovedCallback( entityA, entityB, event ); 
	}
#endif
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
