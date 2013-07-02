/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshInstance.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResourceManager.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Ai/Pathfinding/World/hkaiWorld.h>

// for debug draw
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshDebugDisplayHandler.hpp>
#include <Ai/Visualize/Debug/hkaiNavMeshDebugUtils.h>

// for raycasting
#include <Geometry/Internal/Algorithms/RayCast/hkcdRayCastBox.h>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiStaticTreeNavMeshQueryMediator.h>

V_IMPLEMENT_SERIAL( vHavokAiNavMeshInstance, VisTypedEngineObject_cl, 0, &g_vHavokAiModule);

template<> DynArray_cl<vHavokAiNavMeshInstance *> vHavokAiElementManager<vHavokAiNavMeshInstance *>::elementTable(0, NULL);
template<> unsigned int vHavokAiElementManager<vHavokAiNavMeshInstance *>::g_iElementCount = 0;
template<> unsigned int vHavokAiElementManager<vHavokAiNavMeshInstance *>::g_iFreeElementCache[VIS_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE] = { 0 };
template<> unsigned int vHavokAiElementManager<vHavokAiNavMeshInstance *>::g_iNumElementsInCache = 0;
template<> unsigned int vHavokAiElementManager<vHavokAiNavMeshInstance *>::g_iCurrentElementCacheIndex = 0;
template<> unsigned int vHavokAiElementManager<vHavokAiNavMeshInstance *>::g_iResizeGranularity = 64;

vHavokAiNavMeshInstance::vHavokAiNavMeshInstance() : VisTypedEngineObject_cl()
{
	Init();
}

vHavokAiNavMeshInstance::vHavokAiNavMeshInstance(vHavokAiNavMeshResource* resource, hkaiSectionUid uid) : VisTypedEngineObject_cl()
{
#if defined(HK_DEBUG_SLOW)
	VASSERT(uid != HKAI_INVALID_SECTION_UID);
	// iterate over all previous navMeshInstances and verify uniqueness
	int numExistingInstances = ElementManagerGetSize(); 
	for (int idx = 0; idx < numExistingInstances; ++idx)
	{
		vHavokAiNavMeshInstance* existingInstance = ElementManagerGet(idx);

		if (existingInstance)
		{
			// check if uid is indeed unique.
			VASSERT(uid != existingInstance->GetNavMeshInstance()->getSectionUid());
		}
	}
#endif

	Init();

	VASSERT(resource);
	m_resource = resource;
	m_uid = uid;
	CreateInstance();
}

void vHavokAiNavMeshInstance::Init()
{
	ADDTOELEMENTMANAGER_ADDREF();
	SetObjectFlag(VObjectFlag_AutoDispose);
	m_resource = NULL;
	m_aiWorld = NULL;
	m_uid = HKAI_INVALID_SECTION_UID;
}

void vHavokAiNavMeshInstance::CreateInstance()
{
	if (m_resource && m_instance == HK_NULL)
	{
		m_resource->EnsureLoaded();

		hkaiNavMesh* navMesh = m_resource->GetNavMesh();
		VASSERT(navMesh);
		VASSERT(navMesh->getNumFaces() != 0);

		hkaiNavMeshInstance* navMeshInstance = new hkaiNavMeshInstance();

		VASSERT(m_uid != HKAI_INVALID_SECTION_UID);
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
		navMeshInstance->init(*navMesh, m_uid); 
		navMesh->m_userData = m_uid; // storing an extra copy of uid here.
#else
		navMesh->setRuntimeId(HKAI_INVALID_RUNTIME_INDEX);
		if (navMesh->getSectionUid() == HKAI_INVALID_SECTION_UID)
		{
			hkaiStreamingManager::setSectionUid(*navMesh, m_uid);
		}
		VASSERT(navMesh->getSectionUid() == m_uid);
		navMeshInstance->init(*navMesh);
#endif
		m_instance.setAndDontIncrementRefCount(navMeshInstance);
	}
}

vHavokAiNavMeshInstance::~vHavokAiNavMeshInstance()
{
	DisposeObject();
}

void vHavokAiNavMeshInstance::Release()
{
	int iOldRef = m_iRefCount;
	VRefCounter::Release();
	if (iOldRef==2 && IsObjectFlagSet(VObjectFlag_AutoDispose) && !IsObjectFlagSet(VObjectFlag_Disposing))
		DisposeObject();
}

void vHavokAiNavMeshInstance::DisposeObject()
{
	if (IsObjectFlagSet(VObjectFlag_Disposed))
	{
		// do nothing
		return;
	}

	// remove navmeshes from world
	RemoveNavMeshFromWorld();

	VASSERT(m_instance->getReferenceCount() == 1);
	m_instance = HK_NULL;

	// remove reference to navmesh resource
	m_resource = NULL;

	VisTypedEngineObject_cl::DisposeObject();

	REMOVEFROMELEMENTMANAGER_RELEASE();
}

void vHavokAiNavMeshInstance::AssertValid()
{
	VASSERT(m_resource);
	VASSERT(m_resource->IsLoaded());
}

static unsigned int s_iSerialVersion = 1; // need to increment this everytime Serialize function is modified.
void vHavokAiNavMeshInstance::Serialize(VArchive &ar)
{
	VisTypedEngineObject_cl::Serialize(ar);
	if (ar.IsLoading())
	{
		unsigned int iVersion = 0;
		ar >> iVersion;
		VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

		ar >> m_uid;

		char szResourceFile[FS_MAX_PATH+1];
		ar.ReadStringBinary(szResourceFile, FS_MAX_PATH);

		m_resource = (vHavokAiNavMeshResource*)(vHavokAiNavMeshResourceManager::GetInstance()->LoadResource(szResourceFile));

		CreateInstance();
	}
	else
	{
		ar << s_iSerialVersion;
		ar << m_uid;

		if (m_resource != NULL)
		{
			ar.WriteStringBinary(m_resource->GetFilename());
		}
		else
		{
			ar.WriteStringBinary(NULL);
		}
	}

}

void vHavokAiNavMeshInstance::OnSerialized(VArchive &ar)
{
	VisTypedEngineObject_cl::OnSerialized(ar);

	if (ar.IsLoading())
	{
		// add to world immediately since it's being deserialized.
		AddNavMeshToWorld();
	}
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void vHavokAiNavMeshInstance::GetDependencies(VResourceSnapshot &snapshot)
{
	VisTypedEngineObject_cl::GetDependencies(snapshot);
	if (m_resource)
	{
		m_resource->GetDependencies(snapshot);
	}
}
#endif

void vHavokAiNavMeshInstance::AddNavMeshToWorld(hkaiWorld* aiWorld)
{
	if (m_aiWorld == NULL)
	{
		if (aiWorld == NULL)
		{
			aiWorld = vHavokAiModule::GetInstance() ? vHavokAiModule::GetInstance()->GetAiWorld() : HK_NULL;
		}
		VASSERT(aiWorld);

		hkaiNavMeshQueryMediator* mediator = m_resource->GetNavMeshQueryMediator();
		VASSERT(mediator);

		hkaiNavMeshInstance* navMeshInstance = m_instance;
		VASSERT(navMeshInstance);

		VASSERT(navMeshInstance->getRuntimeId() == HKAI_INVALID_RUNTIME_INDEX);

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
		aiWorld->loadNavMeshInstance(navMeshInstance, mediator);
#else
		aiWorld->loadNavMeshInstance(const_cast<hkaiStreamingManager*>(aiWorld->getStreamingManager()), navMeshInstance, mediator);
#endif
		aiWorld->stepSilhouettes();
		VASSERT(navMeshInstance->getRuntimeId() != HKAI_INVALID_RUNTIME_INDEX);

#if defined(HK_DEBUG_SLOW)
		hkaiNavMeshUtils::validate(*navMeshInstance);
#endif
		m_aiWorld = aiWorld;
	}
}

void vHavokAiNavMeshInstance::RemoveNavMeshFromWorld()
{
	if (m_aiWorld)
	{
		hkaiNavMeshInstance* navMeshInstance = m_instance;
		VASSERT(navMeshInstance != HK_NULL);
		
		VASSERT(navMeshInstance->getRuntimeId() != HKAI_INVALID_RUNTIME_INDEX);
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
		m_aiWorld->unloadNavMeshInstance(navMeshInstance, HK_NULL);
#else
		m_aiWorld->unloadNavMeshInstance(const_cast<hkaiStreamingManager*>(m_aiWorld->getStreamingManager()), navMeshInstance, HK_NULL);
#endif
		VASSERT(navMeshInstance->getRuntimeId() == HKAI_INVALID_RUNTIME_INDEX);

		m_aiWorld = HK_NULL;
	}
	VASSERT(m_aiWorld == HK_NULL);
}

hkvAlignedBBox vHavokAiNavMeshInstance::GetBoundingBox()
{
	hkvAlignedBBox bbox;
	if (m_instance != HK_NULL)
	{
		hkAabb instanceAabb;
		m_instance->getAabb(instanceAabb);

		vHavokConversionUtils::PhysVecToVisVecWorld(instanceAabb.m_min, bbox.m_vMin);
		vHavokConversionUtils::PhysVecToVisVecWorld(instanceAabb.m_max, bbox.m_vMax);
	}
	return bbox;
}

void vHavokAiNavMeshInstance::DebugRender(float displayOffsetHavokScale, bool colorRegions)
{
	// only render if it has been added to ai world
	if (m_aiWorld && m_instance != HK_NULL)
	{
		vHavokAiNavMeshDebugDisplayHandler displayHandler;
		hkaiNavMeshDebugUtils::DebugInfo displaySettings;
		displaySettings.m_displayOffset.set(0.f, 0.f, displayOffsetHavokScale);
		displaySettings.m_colorRegions = colorRegions;

		hkaiNavMeshDebugUtils::showNavMesh(&displayHandler, 0, displaySettings, *m_instance);
	}
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
