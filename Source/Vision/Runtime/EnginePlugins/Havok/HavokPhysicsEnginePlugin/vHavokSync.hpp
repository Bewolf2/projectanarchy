/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokStaticMesh.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKSYNC_HPP_INCLUDED
#define VHAVOKSYNC_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/DebugUtil/MultiThreadCheck/hkMultiThreadCheck.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>

#include <Common/Base/Reflection/Registry/hkDefaultClassNameRegistry.h>
#include <Common/Base/Reflection/Registry/hkTypeInfoRegistry.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>

#include <Common/Base/Config/hkConfigVersion.h>
#if (HAVOK_SDK_VERSION_MAJOR >= 2010)
	#include <Common/Base/System/StackTracer/hkStackTracer.h>
#endif

#ifndef _VISION_DOC

#if (defined(_WIN32) || defined(_VISION_XENON)) && ( !defined(VBASE_LIB) && !defined(LINK_WITH_VISION) )
#undef HAVOK_NEED_SYNC_MACROS
#define HAVOK_NEED_SYNC_MACROS
#endif


#ifdef HAVOK_NEED_SYNC_MACROS

/// \brief
///   Helper structure for synchronization in multi-threaded environments.
///   Internal use only!
struct HavokSyncInfo
{
	hkMemoryInitUtil::SyncInfo baseSystemInfo; // mem, singletons etc
	hkCriticalSection* mtCheckSection; // the multithreading debug check 
	hkMonitorStream* monitors; // montor ptrs as monitors not a normal singleton
#if (HAVOK_SDK_VERSION_MAJOR >= 2010)
	hkStackTracer* mtCheckStackTracer; // the multithreading debug check 
	hkStackTracer::CallTree* mtCheckStackTree; // the multithreading debug check 
	hkUint32* mtRefLockedAllPtr;
#endif
};

void VHAVOK_IMPEXP vHavokPhysicsModuleGetSyncInfo(HavokSyncInfo& s);

#ifndef __vHavokPhysicsModuleIncludes

#if (HAVOK_SDK_VERSION_MAJOR >= 2010)

	#define VISION_HAVOK_SYNC_STATICS_IMPL( s ) \
		do { \
		extern hkBool hkBaseSystemIsInitialized; \
		extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance; \
		if (!hkBaseSystemIsInitialized && s.baseSystemInfo.m_memoryRouter) \
		{   \
			hkMemoryRouter::replaceInstance(s.baseSystemInfo.m_memoryRouter);\
			hkMemorySystem::replaceInstance(s.baseSystemInfo.m_memorySystem);\
			if (s.baseSystemInfo.m_singletonList) { hkSingletonInitNode::populate( hkSingletonInitList, s.baseSystemInfo.m_singletonList ); }  \
	   	hkBaseSystem::initSingletons();\
			HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, s.monitors);\
			HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::m_criticalSection =  s.mtCheckSection; ) \
			HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::s_stackTracer =  s.mtCheckStackTracer; ) \
			HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::s_stackTree =  s.mtCheckStackTree; ) \
			hkBaseSystemIsInitialized = true; \
		} } while(false) 

#else // 7.1

	#define VISION_HAVOK_SYNC_STATICS_IMPL( s ) \
		do { \
			extern hkBool hkBaseSystemIsInitialized; \
			extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance; \
			if (!hkBaseSystemIsInitialized && s.baseSystemInfo.m_memoryRouter) \
			{   \
				hkMemoryRouter::replaceInstance(s.baseSystemInfo.m_memoryRouter);\
				hkMemorySystem::replaceInstance(s.baseSystemInfo.m_memorySystem);\
				if (s.baseSystemInfo.m_singletonList) { hkSingletonInitNode::populate( hkSingletonInitList, s.baseSystemInfo.m_singletonList ); }  \
	   		hkBaseSystem::initSingletons();\
				HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, s.monitors);\
				HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::m_criticalSection =  s.mtCheckSection; ) \
				hkBaseSystemIsInitialized = true; \
			} } while(false) 

#endif

#define VISION_HAVOK_SYNC_STATICS() \
	do { \
		HavokSyncInfo s; \
		vHavokPhysicsModuleGetSyncInfo(s); \
		VISION_HAVOK_SYNC_STATICS_IMPL(s); \
	} while(false)


#if (HAVOK_SDK_VERSION_MAJOR >= 2010)

#define VISION_HAVOK_UNSYNC_STATICS_IMPL( s ) \
  do { \
    extern hkBool hkBaseSystemIsInitialized; \
    extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance; \
    if (hkBaseSystemIsInitialized) \
    {   \
      hkBaseSystemIsInitialized = false; \
      HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::s_stackTree =  HK_NULL; ) \
      HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::s_stackTracer =  HK_NULL; ) \
      HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::m_criticalSection =  HK_NULL; ) \
      HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, HK_NULL);\
      hkBaseSystem::quitSingletons();\
      if (s.baseSystemInfo.m_singletonList) { hkSingletonInitNode::depopulate( hkSingletonInitList, s.baseSystemInfo.m_singletonList ); }  \
      hkMemorySystem::replaceInstance(HK_NULL);\
      hkMemoryRouter::replaceInstance(HK_NULL);\
    } \
  } while(false) 

#else // 7.1

#define VISION_HAVOK_UNSYNC_STATICS_IMPL( s ) \
  do { \
    extern hkBool hkBaseSystemIsInitialized; \
    extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance; \
    if (hkBaseSystemIsInitialized) \
    { \
      hkBaseSystemIsInitialized = false; \
      HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::m_criticalSection =  HK_NULL; ) \
      HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, HK_NULL);\
      hkBaseSystem::quitSingletons();\
      if (s.baseSystemInfo.m_singletonList) { hkSingletonInitNode::depopulate( hkSingletonInitList, s.baseSystemInfo.m_singletonList ); }  \
      hkMemorySystem::replaceInstance(HK_NULL);\
      hkMemoryRouter::replaceInstance(HK_NULL);\
    } \
  } while(false) 

#endif

#define VISION_HAVOK_UNSYNC_STATICS() \
  do { \
  HavokSyncInfo s; \
  vHavokPhysicsModuleGetSyncInfo(s); \
  VISION_HAVOK_UNSYNC_STATICS_IMPL(s); \
  } while(false)


#define VISION_HAVOK_SYNC_THREAD_STATICS_IMPL( s ) \
	do { \
		extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance; \
		if (s.baseSystemInfo.m_memoryRouter) \
		{   \
			hkMemoryRouter::replaceInstance(s.baseSystemInfo.m_memoryRouter); \
			HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, s.monitors); \
		} } while(false) 

//New macro (VISION_HAVOK_SYNC_PER_THREAD_STATICS) to allow callbacks sync the thread specific stuff (noticeably the per thread stack ptr in the mem router)
#define VISION_HAVOK_SYNC_THREAD_STATICS() \
	do { \
		HavokSyncInfo s; \
		vHavokPhysicsModuleGetSyncInfo(s); \
		VISION_HAVOK_SYNC_THREAD_STATICS_IMPL(s); \
	} while(false)

#define VISION_HAVOK_ZERO_THREAD_STATICS() \
	do { \
		extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance; \
		hkMemoryRouter::replaceInstance(HK_NULL); \
		HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, HK_NULL); \
	} while(false)

/// \brief
///   Callback listener to adjust global statics on thread creation
///
class HavokModulThreadCreateCallbackListener_cl : public IVisCallbackHandler_cl 
{
public: 
	HavokModulThreadCreateCallbackListener_cl () {} 
	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData) 
	{ 
		VISION_HAVOK_SYNC_THREAD_STATICS(); 
	} 
};
	
HavokModulThreadCreateCallbackListener_cl __myThreadCreateListenerInstance; \

/// \brief
///   Callback listener to adjust global statics on thread end
///
class HavokModulThreadFinishCallbackListener_cl : public IVisCallbackHandler_cl \
{
public: 
	HavokModulThreadFinishCallbackListener_cl () { }\
	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData) \
	{
		VISION_HAVOK_ZERO_THREAD_STATICS();
	}
};
HavokModulThreadFinishCallbackListener_cl __myThreadFinishListenerInstance;

#define VISION_HAVOK_SYNC_PER_THREAD_STATICS(P) \
	(P)->OnWorkerThreadCreated += __myThreadCreateListenerInstance; \
	(P)->OnWorkerThreadFinished += __myThreadFinishListenerInstance;

#define VISION_HAVOK_UNSYNC_PER_THREAD_STATICS(P) \
	(P)->OnWorkerThreadCreated -= __myThreadCreateListenerInstance; \
	(P)->OnWorkerThreadFinished -= __myThreadFinishListenerInstance;


// A listener for Havok module callbacks
class vHavokModuleCallbackHandler_cl : public IVisCallbackHandler_cl
{
public:

	vHavokModuleCallbackHandler_cl() {}

	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData)
	{
		if ( pData->m_pSender == &vHavokPhysicsModule::OnBeforeInitializePhysics )
		{
			vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;
			VISION_HAVOK_SYNC_STATICS();
			VISION_HAVOK_SYNC_PER_THREAD_STATICS( pHavokData->GetHavokModule() );

			// Register LOCAL classes
			{
				hkDefaultClassNameRegistry& dcnReg	= hkDefaultClassNameRegistry::getInstance();
				hkTypeInfoRegistry&			tyReg	= hkTypeInfoRegistry::getInstance();
				hkVtableClassRegistry&		vtcReg	= hkVtableClassRegistry::getInstance();

				dcnReg.registerList(hkBuiltinTypeRegistry::StaticLinkedClasses);
				tyReg.registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos);
				vtcReg.registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos, hkBuiltinTypeRegistry::StaticLinkedClasses);
			}
		}
		else if ( pData->m_pSender == &vHavokPhysicsModule::OnAfterDeInitializePhysics )
		{
			vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;
			VISION_HAVOK_UNSYNC_STATICS();
			VISION_HAVOK_UNSYNC_PER_THREAD_STATICS( pHavokData->GetHavokModule() );
		}
	}
};

static vHavokModuleCallbackHandler_cl g_HavokModuleCallbackHandler;

#define VISION_HAVOK_SYNC_ALL_STATICS() \
	vHavokPhysicsModule::OnBeforeInitializePhysics += &g_HavokModuleCallbackHandler;\
	vHavokPhysicsModule::OnAfterDeInitializePhysics += &g_HavokModuleCallbackHandler;

#define VISION_HAVOK_UNSYNC_ALL_STATICS() \
	vHavokPhysicsModule::OnBeforeInitializePhysics -= &g_HavokModuleCallbackHandler;\
	vHavokPhysicsModule::OnAfterDeInitializePhysics -= &g_HavokModuleCallbackHandler;


#endif

#else // None needed

#define VISION_HAVOK_SYNC_STATICS() { }
#define VISION_HAVOK_UNSYNC_STATICS() { }
#define VISION_HAVOK_SYNC_THREAD_STATICS() { }
#define VISION_HAVOK_SYNC_PER_THREAD_STATICS(P) { }
#define VISION_HAVOK_UNSYNC_PER_THREAD_STATICS(P) { }
#define VISION_HAVOK_ZERO_THREAD_STATICS() { }
#define VISION_HAVOK_SYNC_ALL_STATICS() { }
#define VISION_HAVOK_UNSYNC_ALL_STATICS() {}

#endif // macros

#endif // DOCS

#endif //VHAVOKSYNC_HPP_INCLUDED

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
