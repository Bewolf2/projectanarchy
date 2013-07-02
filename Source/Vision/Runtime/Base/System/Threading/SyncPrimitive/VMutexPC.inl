/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// C28125: "The function 'InitializeCriticalSection' must be called from within a try/except block".
// InitializeCriticalSection may raise an exception only on XP or below when the system is out of memory, but as this is extremely rare and we can't do anything useful in that case,
// suppress the warning rather than attempt to handle the exception.
#pragma warning(push)
#pragma warning(disable : 28125)
VMutex::VMutex()
{
#ifdef _VISION_WINRT
	InitializeCriticalSectionEx(&m_CriticalSection, 10000,
		#ifndef HK_DEBUG
			CRITICAL_SECTION_NO_DEBUG_INFO
		#else
			0
		#endif
			);
#else
  InitializeCriticalSection(&m_CriticalSection);
#endif
}
#pragma warning(pop)

VMutex::~VMutex()
{
  DeleteCriticalSection(&m_CriticalSection);
}


void VMutex::Lock()
{
  EnterCriticalSection(&m_CriticalSection);
}

void VMutex::Unlock()
{
  LeaveCriticalSection(&m_CriticalSection);
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
