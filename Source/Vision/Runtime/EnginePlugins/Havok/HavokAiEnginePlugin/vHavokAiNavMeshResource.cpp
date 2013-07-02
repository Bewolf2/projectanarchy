/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResourceManager.hpp>

// for saving/loading files
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>
#include <Common/Base/System/Io/Writer/Buffered/hkBufferedStreamWriter.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>

// for creating a mediator
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.h>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiStaticTreeNavMeshQueryMediator.h>

#ifdef PROFILING
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>
#endif

// for memory leak debugging
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

vHavokAiNavMeshResource::vHavokAiNavMeshResource(const char* filePath)
	: VManagedResource(vHavokAiNavMeshResourceManager::GetInstance()), 
	m_navMesh(HK_NULL), 
	m_mediator(HK_NULL)
{
	Init();
	SetFilename(filePath);
}

vHavokAiNavMeshResource::vHavokAiNavMeshResource(hkaiNavMesh* navMesh, hkaiNavMeshQueryMediator* mediator) 
	: VManagedResource(vHavokAiNavMeshResourceManager::GetInstance()), 
	m_navMesh(navMesh), 
	m_mediator(mediator)
{
	Init();
	FlagAsLoaded();
	UpdateMemoryFootprint();
}

vHavokAiNavMeshResource::~vHavokAiNavMeshResource()
{
	EnsureUnloaded();
}

void vHavokAiNavMeshResource::Init()
{
	FlagAsUnLoaded();
	SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
	SetResourceFlag(VRESOURCEFLAG_AUTOUNLOAD);
	SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
}

/// VManagedResource overrides just so we can register callbacks
BOOL vHavokAiNavMeshResource::Reload()
{
	const char* pszFilename = GetFilename();
	if (pszFilename == NULL)
	{
		return FALSE;
	}

	IVFileInStream *pIn = GetParentManager()->CreateFileInStream(pszFilename,this);
	if (!pIn)
	{
		return FALSE;
	}

	VASSERT(m_navMesh == HK_NULL && m_mediator == HK_NULL);
	if (!LoadNavMesh(pIn, m_navMesh, m_mediator))
	{
		return FALSE;
	}

	VASSERT(m_navMesh != HK_NULL && m_mediator != HK_NULL);
	VASSERT(m_navMesh->getReferenceCount() == 2 && m_mediator->getReferenceCount() == 1);
	FlagAsLoaded();
	UpdateMemoryFootprint();
	return TRUE;
}

// unload from memory
BOOL vHavokAiNavMeshResource::Unload()
{
	VASSERT(m_mediator->getReferenceCount() == 1);
	m_mediator = HK_NULL;

	VASSERT(m_navMesh->getReferenceCount() == 1);
	m_navMesh = HK_NULL;

	FlagAsUnLoaded();
	UpdateMemoryFootprint();
	return true;
}


void vHavokAiNavMeshResource::AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
{
	VManagedResource::AccumulateMemoryFootprint(iUniqueSys,iUniqueGPU,iDependentSys,iDependentGPU);
#ifdef HK_DEBUG
	// calculate approximate memory foot print (will get out of date with new versions).  
	// it'd be more accurate to just save the navmeshes/mediators to a packfile in an in-memory buffer, but that would essentially double the memory footprint temporarily. (Okay?)
	// Because we don't have direct access to member arrays for hkaiNavMeshInstance, we write to in-memory buffer.
	{
		iUniqueSys = 0; // we should approximate instead of using packfile

		hkaiNavMesh* navMesh = m_navMesh;
		if (navMesh != HK_NULL)
		{
			iUniqueSys += navMesh->m_edges.getCapacity() * sizeof(hkaiNavMesh::Edge);
			iUniqueSys += navMesh->m_faces.getCapacity() * sizeof(hkaiNavMesh::Face);
			iUniqueSys += navMesh->m_vertices.getCapacity() * sizeof(hkVector4);
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
			iUniqueSys += navMesh->m_streamingSets.getCapacity() * sizeof(hkaiStreamingSet);
#else
			iUniqueSys += navMesh->m_sectionDependencies.getSize() * sizeof(hkaiSectionUid);
#endif
			iUniqueSys += sizeof(hkaiNavMesh);
		}

		hkaiNavMeshQueryMediator* mediator = m_mediator;
		if (mediator != HK_NULL)
		{
			extern hkClass hkaiStaticTreeNavMeshQueryMediatorClass;
			if (hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(mediator) == &hkaiStaticTreeNavMeshQueryMediator::staticClass())
			{
				hkaiStaticTreeNavMeshQueryMediator* staticMediator = static_cast<hkaiStaticTreeNavMeshQueryMediator*>(mediator);
				iUniqueSys += staticMediator->getMemoryFootPrint();
			}
		}
	}
#else
	iUniqueSys += 0;
#endif
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void vHavokAiNavMeshResource::GetDependencies(VResourceSnapshot &snapshot)
{
  IVFileInStream* pIn = Vision::File.Open(GetFilename());
  if (pIn != NULL)
  {
    snapshot.AddFileDependency(this, GetFilename(), pIn->GetSize());
    pIn->Close();
  }

  VManagedResource::GetDependencies(snapshot);
}
#endif

bool vHavokAiNavMeshResource::SetFilenameAndSaveNavMesh(const char* relativeFilePath, const char* rootDirectory)
{
	// save relative filepath for later
	SetFilename(relativeFilePath);
	if (relativeFilePath == HK_NULL)
	{
		return false;
	}

	if (m_navMesh == HK_NULL)
	{
		return false;
	}

	hkStringBuf actualFilePath;
	if (rootDirectory != HK_NULL)
	{
		actualFilePath.set(rootDirectory);
		actualFilePath.appendPrintf("/");
	}
	actualFilePath.append(relativeFilePath);
	actualFilePath.pathNormalize();

	IVFileOutStream* outStream = GetParentManager()->CreateFileOutStream(actualFilePath.cString(),this);
	if (outStream)
	{
		return SaveNavMesh(outStream, m_navMesh, m_mediator);
	}
	else
	{
		return false;
	}
}

bool vHavokAiNavMeshResource::SaveNavMesh(IVFileOutStream* outStream, const hkRefPtr<hkaiNavMesh>& navMesh, const hkRefPtr<hkaiNavMeshQueryMediator>& mediator)
{
	vHavokStreamWriter havokStreamWriter(outStream);
	if (!havokStreamWriter.isOk())
	{
		return false;
	}

	hkBufferedStreamWriter bufferedStreamWriter(&havokStreamWriter);
	if (!bufferedStreamWriter.isOk())
	{
		return false;
	}

	hkRootLevelContainer container;
	{
		VASSERT(navMesh)
		// save out the navmesh
		hkRootLevelContainer::NamedVariant& navmeshData = container.m_namedVariants.expandOne();
		navmeshData.set( const_cast<char*>(hkaiNavMeshClass.getName()), (void*)(navMesh), &hkaiNavMeshClass );

		VASSERT(mediator);
		// save out the mediator
		hkRootLevelContainer::NamedVariant& mediatorData = container.m_namedVariants.expandOne();
		mediatorData.set( const_cast<char*>(hkaiNavMeshQueryMediatorClass.getName()), (void*)(mediator), &hkaiNavMeshQueryMediatorClass);
	}

	hkResult res = hkSerializeUtil::saveTagfile( &container, hkRootLevelContainerClass, &bufferedStreamWriter);
	return res == HK_SUCCESS;
}

bool vHavokAiNavMeshResource::LoadNavMesh(const char* filePath, hkRefPtr<hkaiNavMesh>& navMeshOut, hkRefPtr<hkaiNavMeshQueryMediator>& mediatorOut )
{
	IVFileInStream* inStream = Vision::File.Open(filePath);
	return LoadNavMesh(inStream, navMeshOut, mediatorOut);
}

bool vHavokAiNavMeshResource::LoadNavMesh(IVFileInStream* inStream, hkRefPtr<hkaiNavMesh>& navMeshOut, hkRefPtr<hkaiNavMeshQueryMediator>& mediatorOut)
{
	vHavokStreamReader havokStreamReader(inStream);
	if (!havokStreamReader.isOk())
	{
		return false;
	}

	hkBufferedStreamReader bufferedStreamReader(&havokStreamReader);
	if (!bufferedStreamReader.isOk())
	{
		return false;
	}

	hkObjectResource* res = hkSerializeUtil::loadOnHeap(&bufferedStreamReader);
	if (!res)
	{
		return false;
	}

	hkRootLevelContainer* container = res->getContents<hkRootLevelContainer>();
	hkRefPtr<hkaiNavMesh> navMesh;
	hkRefPtr<hkaiNavMeshQueryMediator> mediator;
	if (container)
	{
		navMesh = container->findObject<hkaiNavMesh>();
		mediator = container->findObject<hkaiStaticTreeNavMeshQueryMediator>();
	}
	else
	{
		// dkw.note: loading legacy asset that didn't use a hkRootLevelContainer
		navMesh = res->getContents<hkaiNavMesh>();
	}

	// remove reference here since everything in the stream should have an extra reference count
	res->removeReference();
	if (navMesh == HK_NULL)
	{
		return false;
	}

#if defined(HK_DEBUG_SLOW)
	hkaiNavMeshUtils::validate(*navMesh);
#endif

	// create mediators if they don't exist
	if (mediator == HK_NULL)
	{
		mediator.setAndDontIncrementRefCount(hkaiNavMeshUtils::setupQueryMediator(*navMesh));
	}

	navMeshOut = navMesh;
	mediatorOut = mediator;
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
