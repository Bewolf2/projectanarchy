/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>

#include <Ai/Pathfinding/World/hkaiWorld.h>

// needed to interface with Havok Physics
#include <Ai/Physics2012Bridge/World/hkaiPhysics2012WorldListener.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

// for loading in NavMeshes
#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingManager.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>

// for debug rendering navmeshes in world
#include <Ai/Pathfinding/Collide/NavMesh/hkaiNavMeshQueryMediator.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshDebugDisplayHandler.hpp>
#include <Ai/Visualize/Debug/hkaiNavMeshDebugUtils.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

// for visual debugger
#include <Ai/Visualize/VisualDebugger/hkaiViewerContext.h>

// Static global manager
vHavokAiModule vHavokAiModule::g_GlobalManager;


vHavokAiModule::vHavokAiModule()
{
}

vHavokAiModule::~vHavokAiModule()
{
}


void vHavokAiModule::OneTimeInit()
{
	// Register callbacks
	VSceneLoader::OnCustomChunkSerialization += this;
	vHavokPhysicsModule::OnBeforeWorldCreated += this;
	vHavokPhysicsModule::OnAfterWorldCreated += this;
	vHavokVisualDebugger::OnCreatingContexts += this;
}

void vHavokAiModule::OneTimeDeInit()
{	
	// Unregister callbacks
	VSceneLoader::OnCustomChunkSerialization -= this;
	vHavokPhysicsModule::OnBeforeWorldCreated -= this;
	vHavokPhysicsModule::OnAfterWorldCreated -= this;
	vHavokVisualDebugger::OnCreatingContexts -= this;
}

void vHavokAiModule::Init()
{
	m_aiWorld = HK_NULL;
	m_physicsWorld = HK_NULL;
	m_connectToPhysicsWorld = false;
	m_physicsWorldListener = HK_NULL;
	m_aiViewerContext = HK_NULL;

	// create ai world, don't delay so that vForge etc always has one (even if will be recreated soon enough anyway..
	CreateAiWorld(); 
}

void vHavokAiModule::DeInit()
{
	RemoveAiWorld();

	m_aiWorld = HK_NULL;
	m_physicsWorld = HK_NULL;
	m_connectToPhysicsWorld = false;
	m_physicsWorldListener = HK_NULL;
	m_aiViewerContext = HK_NULL;
}

void vHavokAiModule::Step( float dt )
{
	if (m_aiWorld)
	{
		// todo: what should time step be?
		vHavokPhysicsModule* physicsModule = vHavokPhysicsModule::GetInstance();

		if (physicsModule && physicsModule->GetJobQueue())
		{
			m_aiWorld->stepMultithreaded( dt, m_behaviors, physicsModule->GetJobQueue(), physicsModule->GetThreadPool() );
		}
		else
		{
			m_aiWorld->step(dt, m_behaviors);
		}
	}
}

/// Called by the Havok physics module on de-initialization. It is called before anything
/// was actually deleted, with the hkpWorld marked for write.
void vHavokAiModule::OnDeInitPhysicsModule()
{
	SetPhysicsWorld(HK_NULL);
}

void vHavokAiModule::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	if (pData->m_pSender == &vHavokPhysicsModule::OnBeforeWorldCreated)
	{
		SetPhysicsWorld(HK_NULL);
	}
	else if (pData->m_pSender == &vHavokPhysicsModule::OnAfterWorldCreated)
	{
		vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;
		SetPhysicsWorld( pHavokData->GetHavokModule()->GetPhysicsWorld() );
	}
	else if (pData->m_pSender == &VSceneLoader::OnCustomChunkSerialization)
	{
		// Note: loading/saving of nav mesh in .vscene file is deprecated but loading is left here for backwards compatibility
		VCustomSceneChunkDataObject &chunkData(*((VCustomSceneChunkDataObject *)pData));
		if (chunkData.m_Loader.IsSaving() || chunkData.m_iChunkID == 'AINM')
		{
			chunkData.m_iChunkID = 'AINM';
			chunkData.m_bSuccess = chunkData.m_bProcessed = true;

			GlobalsChunkFileExchange(chunkData.m_Loader, chunkData.m_iChunkID);
		}
	}
	else if (pData->m_pSender == &vHavokVisualDebugger::OnCreatingContexts)
	{
		if( m_aiWorld != HK_NULL )
		{
			vHavokVisualDebuggerCallbackData_cl* pVdbData = (vHavokVisualDebuggerCallbackData_cl*)pData;
			pVdbData->m_contexts->pushBack(m_aiViewerContext);
		}
	}
}

int vHavokAiModule::GetCallbackSortingKey(VCallback *pCallback)
{
	return 1; // should be greater than vHavokAiModuleCallbackHandler_cl::GetCallbackSortingKey
}

void vHavokAiModule::GlobalsChunkFileExchange(VChunkFile &file, CHUNKIDTYPE iID)
{
	const int sanity_number = 0xbaddda1a;
	char iLocalVersion = 2;
	if (file.IsLoading())
	{
		// in loading mode, we receive this callback already when the chunk has been identified
		file.ReadChar(iLocalVersion);
		VASSERT(iLocalVersion<=2);
		int sanity_check;
		file.ReadInt(sanity_check);
		VASSERT(sanity_check == sanity_number);

		if (iLocalVersion >= 2)
		{
      bool bTmp;
			file.Readbool(bTmp); // determine if we want dynamic cutting
      SetConnectToPhysicsWorld(bTmp);
		}
	}
	else
	{
		file.StartChunk(iID); // in save mode, make a sub-chunk
		file.WriteChar(iLocalVersion);
		file.WriteInt(sanity_number);
		file.Writebool(m_connectToPhysicsWorld); // save out if we want dynamic cutting
		file.EndChunk();
	}
}

static bool _ComputePath(const hkaiWorld* aiWorld, hkvVec3* startPoint, hkvVec3* endPoint, float radius, hkaiPathfindingUtil::FindPathInput& input, hkaiPathfindingUtil::FindPathOutput& output)
{
	VASSERT(input.m_goalPoints.getSize() == 1);

	vHavokConversionUtils::VisVecToPhysVecWorld((*startPoint), input.m_startPoint);
	vHavokConversionUtils::VisVecToPhysVecWorld((*endPoint), input.m_goalPoints[0]);
	input.m_startFaceKey = HKAI_INVALID_PACKED_KEY;
	input.m_goalFaceKeys[0] = HKAI_INVALID_PACKED_KEY;

	bool foundPath = false;
	const hkaiNavMeshQueryMediator* mediator = aiWorld ? aiWorld->getDynamicQueryMediator() : HK_NULL;
	if (mediator)
	{
		hkVector4 closestPoint;
		input.m_startFaceKey = mediator->getClosestPoint(input.m_startPoint, 3.f, closestPoint);
		if (input.m_startFaceKey != HKAI_INVALID_PACKED_KEY)
			input.m_startPoint = closestPoint;

		input.m_goalFaceKeys[0] = mediator->getClosestPoint(input.m_goalPoints[0], 3.f, closestPoint);
		if (input.m_goalFaceKeys[0] != HKAI_INVALID_PACKED_KEY)
			input.m_goalPoints[0] = closestPoint;

		if (input.m_startFaceKey != HKAI_INVALID_PACKED_KEY && input.m_goalFaceKeys[0] != HKAI_INVALID_PACKED_KEY)
		{
			input.m_agentInfo.m_diameter = VIS2HK_FLOAT_SCALED(radius*2.f);
			input.m_searchParameters.m_up.set(0,0,1);

			hkaiPathfindingUtil::findPath(*aiWorld->getStreamingCollection(), input, output);

			foundPath = (output.m_outputParameters.m_status == hkaiAstarOutputParameters::SEARCH_SUCCEEDED);
		}
	}

	return foundPath;
}

bool vHavokAiModule::ComputePath(hkvVec3* startPoint, hkvVec3* endPoint, float radius, VArray<hkvVec3>& pathPoints) const
{
	hkaiPathfindingUtil::FindPathInput input(1);
	hkaiPathfindingUtil::FindPathOutput output;
	if(!_ComputePath(m_aiWorld, startPoint, endPoint, radius, input, output))
		return false;

	for (int i = 0; i < output.m_pathOut.getSize(); i++)
	{
		const hkaiPath::PathPoint& pt = output.m_pathOut[i];
		hkvVec3 vPos; vHavokConversionUtils::PhysVecToVisVecWorld(pt.m_position, vPos); 
		pathPoints.Add(vPos);
	}

	return true;
}

bool vHavokAiModule::ComputeAndDrawPath(IVRenderInterface *pRenderer, hkvVec3* startPoint, hkvVec3* endPoint, float radius, float height, float displayOffset, unsigned int color) const
{
	hkaiPathfindingUtil::FindPathInput input(1);
	hkaiPathfindingUtil::FindPathOutput output;
	bool foundPath = _ComputePath(m_aiWorld, startPoint, endPoint, radius, input, output);

	if (input.m_startFaceKey != HKAI_INVALID_PACKED_KEY && input.m_goalFaceKeys[0] != HKAI_INVALID_PACKED_KEY)
	{
		vHavokAiNavMeshDebugDisplayHandler displayHandler;
		hkReal displayOffsetHavokScale = VIS2HK_FLOAT_SCALED(displayOffset);
		hkaiNavMeshDebugUtils::_drawPathWithRadius(input, output, color, hkColor::ORANGE, &displayHandler, 0, displayOffsetHavokScale);
	}

	hkvVec3 vStartPos; vHavokConversionUtils::PhysVecToVisVecWorld(input.m_startPoint,vStartPos); 
	hkvVec3 vEndPos; vHavokConversionUtils::PhysVecToVisVecWorld(input.m_goalPoints[0],vEndPos); 
	hkvVec3 vScaledDir ( 0.f, 0.f, height );
	VSimpleRenderState_t state(VIS_TRANSP_NONE, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_WRITETOZBUFFER);	
	int sd = (input.m_startFaceKey != HKAI_INVALID_PACKED_KEY) ? 1 : 2;
	int ed = (input.m_goalFaceKeys[0] != HKAI_INVALID_PACKED_KEY) ? 1 : 2;
	pRenderer->RenderCylinder(vStartPos, vScaledDir, radius, VColorRef(255/sd, 165/sd, 0/sd), state, RENDERSHAPEFLAGS_SOLID|RENDERSHAPEFLAGS_CAP1);
	pRenderer->RenderCylinder(vEndPos, vScaledDir, radius, VColorRef(0/ed, 128/ed, 0/ed), state, RENDERSHAPEFLAGS_SOLID|RENDERSHAPEFLAGS_CAP1);

	return foundPath;
}

bool vHavokAiModule::CreateAiWorld()
{
	if (m_aiWorld == HK_NULL)
	{
		hkaiWorld::Cinfo cinfo;
		m_aiWorld = new hkaiWorld(cinfo);

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR < 2012)
		hkaiStreamingManager* manager = new hkaiStreamingManager;
		m_aiWorld->setStreamingManager(manager);
		manager->removeReference();
#endif
	}

	if (m_connectToPhysicsWorld)
	{
		ConnectToPhysicsWorld();
	}

	if (m_aiViewerContext == HK_NULL)
	{
		m_aiViewerContext = new hkaiViewerContext;
		m_aiViewerContext->addWorld(m_aiWorld);
		hkaiViewerContext::registerAllAiProcesses();
	}

	return true;
}

void vHavokAiModule::SetPhysicsWorld(hkpWorld* physics)
{
	if (m_connectToPhysicsWorld)
	{
		DisconnectFromPhysicsWorld();
		m_physicsWorld = physics;
		ConnectToPhysicsWorld();
	}
	m_physicsWorld = physics;
}

void vHavokAiModule::SetConnectToPhysicsWorld(bool connect)
{
	m_connectToPhysicsWorld = connect;

	if (m_connectToPhysicsWorld)
	{
		ConnectToPhysicsWorld();
	}
	else
	{
		DisconnectFromPhysicsWorld();
	}
}

void vHavokAiModule::RemoveAiWorld()
{
	DisconnectFromPhysicsWorld();

  // Make sure memory is freed
  m_behaviors.clearAndDeallocate();

	if (m_aiViewerContext != HK_NULL)
	{
		VASSERT(m_aiViewerContext->getReferenceCount() == 1);
		m_aiViewerContext->removeReference();
		m_aiViewerContext = HK_NULL;
	}

	if (m_aiWorld != HK_NULL)
	{
		VASSERT(m_aiWorld->getReferenceCount() == 1);
		m_aiWorld->removeReference();
		m_aiWorld = HK_NULL;
	}
}

void vHavokAiModule::ConnectToPhysicsWorld()
{
	if(m_physicsWorldListener != HK_NULL)
		return;	// already connected

	if (m_physicsWorld != HK_NULL && m_aiWorld != HK_NULL)
	{
		m_physicsWorldListener = new hkaiPhysics2012WorldListener(m_physicsWorld, m_aiWorld);
		m_physicsWorldListener->connectToPhysicsWorld();

		m_physicsWorld->addReference();
	}
}

void vHavokAiModule::DisconnectFromPhysicsWorld()
{
	if (m_physicsWorldListener != HK_NULL)
	{
		m_physicsWorldListener->removeReference();
		m_physicsWorldListener = HK_NULL;

		// Really, m_physicsWorldListener should hold a reference to the physics world but it doesn't so we have to do it ourselves
		VASSERT(m_physicsWorld != HK_NULL);
		m_physicsWorld->removeReference();
	}
}

void vHavokAiModule::DebugRender(float displayOffsetHavokScale, bool colorRegions)
{
	vHavokAiNavMeshDebugDisplayHandler displayHandler;
	hkaiNavMeshDebugUtils::DebugInfo displaySettings;
	displaySettings.m_displayOffset.set(0.f, 0.f, displayOffsetHavokScale);
	displaySettings.m_colorRegions = colorRegions;

	if (m_aiWorld != HK_NULL)
	{
		hkaiNavMeshDebugUtils::showStreamingCollection(&displayHandler, 0, displaySettings, m_aiWorld->getStreamingCollection());
	}
}

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>
#include <Common/Base/System/Io/Writer/Buffered/hkBufferedStreamWriter.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiStaticTreeNavMeshQueryMediator.h>
bool vHavokAiModule::LoadNavMeshDeprecated(const char* filename, VArray<vHavokAiNavMeshInstance*>* navMeshInstancesOut)
{
	if (!Vision::File.Exists(filename))
	{
		return false;
	}

	vHavokStreamReader havokStreamReader(filename);
	if (!havokStreamReader.isOk())
	{
		return false;
	}

	hkBufferedStreamReader bufferedStreamReader(&havokStreamReader);
	if (!bufferedStreamReader.isOk())
	{
		return false;
	}

	VArray<vHavokAiNavMeshResource*> navMeshResources;
	{
		hkArray< hkRefPtr<hkaiNavMesh> > navMeshes;
		hkArray< hkRefPtr<hkaiNavMeshQueryMediator> > mediators;
		hkObjectResource* res = hkSerializeUtil::loadOnHeap(&bufferedStreamReader);
		hkRootLevelContainer* container = res->getContents<hkRootLevelContainer>();
		if (container)
		{
			hkaiNavMesh* navMesh = container->findObject<hkaiNavMesh>();
			hkaiNavMeshQueryMediator* mediator = container->findObject<hkaiStaticTreeNavMeshQueryMediator>();

			while(navMesh)
			{
				navMeshes.pushBack(navMesh);
				mediators.pushBack(mediator);

				navMesh = container->findObject<hkaiNavMesh>(navMesh);
				mediator = container->findObject<hkaiStaticTreeNavMeshQueryMediator>(mediator);
			}
		}
		else
		{
			// dkw.note: loading legacy asset that didn't use a hkRootLevelContainer
			hkaiNavMesh* navMesh = res->getContents<hkaiNavMesh>();

			if (navMesh == HK_NULL)
			{
				return false;
			}

			navMeshes.pushBack(navMesh);
			mediators.pushBack(HK_NULL);
		}

		res->removeReference();
		if (navMeshes.getSize() == 0)
		{
			return false;
		}

		for (int i = 0; i < navMeshes.getSize(); i++)
		{
			if (mediators[i] == HK_NULL)
			{
				VASSERT(navMeshes[i]->getReferenceCount() == 1);
				mediators[i].setAndDontIncrementRefCount(hkaiNavMeshUtils::setupQueryMediator( *navMeshes[i] ));
			}
			else
			{
				VASSERT(navMeshes[i]->getReferenceCount() == 2);
			}

			VASSERT(mediators[i]->getReferenceCount() == 1);
			vHavokAiNavMeshResource* resource = new vHavokAiNavMeshResource(navMeshes[i], mediators[i]);
			VASSERT(navMeshes[i]->getReferenceCount() == 3);
			VASSERT(mediators[i]->getReferenceCount() == 2);
			navMeshResources.Add(resource);
		}
	}

	VArray<vHavokAiNavMeshInstance*> navMeshInstancesLocal;
	VArray<vHavokAiNavMeshInstance*>* navMeshInstancesPtr;
	if (navMeshInstancesOut != HK_NULL)
	{
		navMeshInstancesPtr = navMeshInstancesOut;
	}
	else
	{
		navMeshInstancesPtr = &navMeshInstancesLocal;
	}

	hkaiWorld* aiWorld = GetAiWorld();
	for (int i = 0; i < navMeshResources.GetLength(); i++)
	{
		VASSERT(navMeshResources[i]->GetNavMesh()->getReferenceCount() == 2);
		VASSERT(navMeshResources[i]->GetNavMeshQueryMediator()->getReferenceCount() == 1);
		vHavokAiNavMeshInstance* navMeshInstance = new vHavokAiNavMeshInstance(navMeshResources[i], i);
		VASSERT(navMeshResources[i]->GetNavMesh()->getReferenceCount() == 3);
		VASSERT(navMeshResources[i]->GetNavMeshQueryMediator()->getReferenceCount() == 1);

		navMeshInstance->AddNavMeshToWorld(aiWorld);

		navMeshInstancesPtr->Add(navMeshInstance);
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
