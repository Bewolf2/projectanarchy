/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_MODULE_HPP
#define __VHAVOK_AI_MODULE_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>

// included here for LoadNavMeshDeprecated
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshInstance.hpp>
class hkaiWorld;
class hkaiNavMesh;
class hkaiViewerContext;

class hkpWorld;

class vHavokAiModule : public IVisCallbackHandler_cl, public IHavokStepper
{
public:
	///
	/// @name Constructor / Destructor / Global Accessor
	/// @{
	///

	vHavokAiModule();
	VOVERRIDE ~vHavokAiModule();
	VHAVOKAI_IMPEXP static vHavokAiModule* GetInstance() {return &g_GlobalManager;}

	///
	/// @}
	///

	///
	/// @name One time Initialization/Deinitialization to register callbacks
	/// @{
	///

	/// \brief
	/// should be called at plugin initialization time
	void OneTimeInit();

	/// \brief
	/// should be called at plugin de-initialization time
	void OneTimeDeInit();

	/// \brief
	/// should be called before/after Havok base system initialization, respectively
	void Init();
	void DeInit();

	///
	/// @}
	///

	///
	/// @name IVisCallbackHandler_cl overridden functions
	/// @{
	///

	/// \brief
	/// implements IVisCallbackHandler_cl
	VOVERRIDE void OnHandleCallback( IVisCallbackDataObject_cl* pData ) HKV_OVERRIDE;

	VOVERRIDE int GetCallbackSortingKey(VCallback *pCallback) HKV_OVERRIDE;

	///
	/// @}
	///

	///
	/// @name Functions to create/modify ai world
	/// @{
	///

	/// \brief
	/// manual access to world construction/destruction.  Note that world gets automatically created when plugin gets loaded
	VHAVOKAI_IMPEXP bool CreateAiWorld();
	VHAVOKAI_IMPEXP void RemoveAiWorld();

	///
	/// @}
	///

	///
	/// @name Functions to create/modify ai world
	/// @{
	///

	/// \brief
	/// For manually connecting to Havok physics world.  Note that this connection should already happen if specified in exported scene.
	VHAVOKAI_IMPEXP void SetPhysicsWorld(hkpWorld* physics);
	VHAVOKAI_IMPEXP void SetConnectToPhysicsWorld(bool connect);

	///
	/// @}
	///

	///
	/// @name IHavokStepper overrides
	/// @{
	///

	VHAVOKAI_IMPEXP VOVERRIDE void Step( float dt ) HKV_OVERRIDE;

	/// Called by the Havok physics module on de-initialization. It is called before anything
	/// was actually deleted, with the hkpWorld marked for write.
	VHAVOKAI_IMPEXP VOVERRIDE void OnDeInitPhysicsModule() HKV_OVERRIDE;

	///
	/// @}
	///

	///
	/// @name Miscellaneous accessor methods
	/// @{
	///

	VHAVOKAI_IMPEXP hkaiWorld* GetAiWorld() { return m_aiWorld; }
	VHAVOKAI_IMPEXP const hkaiWorld* GetAiWorld() const { return m_aiWorld; }

	VHAVOKAI_IMPEXP hkArray<class hkaiBehavior*>& getCharacterBehaviors()				{ return m_behaviors; }
	VHAVOKAI_IMPEXP const hkArray<class hkaiBehavior*>& getCharacterBehaviors() const	{ return m_behaviors; }

		/// deprecated interface
	VHAVOKAI_IMPEXP bool LoadNavMeshDeprecated(const char* filename, VArray<vHavokAiNavMeshInstance*>* navMeshInstancesOut = HK_NULL);

	///
	/// @}
	///

	///
	/// @name Demo code
	/// @{
	///

	/// \brief
	/// for computing a path (note that a lot more options are available by instead issuing pathfinding requests through the hkaiWorld object)
	VHAVOKAI_IMPEXP bool ComputePath(hkvVec3* startPoint, hkvVec3* endPoint, float radius, VArray<hkvVec3>& pathPoints) const;

	/// \brief
	/// for computing and drawing a path
	VHAVOKAI_IMPEXP bool ComputeAndDrawPath(IVRenderInterface *pRenderer, hkvVec3* startPoint, hkvVec3* endPoint, float radius, float height, float displayOffset, unsigned int color) const;

	/// \brief
	/// for debug rendering all nav mesh instances in ai world
	VHAVOKAI_IMPEXP void DebugRender(float displayOffsetHavokScale, bool colorRegions = true);

	///
	/// @}
	///

protected:
	/// \brief
	///   Saves or loads ai world global setting to/from the passed chunk file.  
	void GlobalsChunkFileExchange(VChunkFile &file, CHUNKIDTYPE iID);

	/// \brief
	/// connects/disconnects to hkpWorld
	void ConnectToPhysicsWorld();
	void DisconnectFromPhysicsWorld();

	hkaiWorld* m_aiWorld;

	hkpWorld* m_physicsWorld;
	bool m_connectToPhysicsWorld;
	class hkaiPhysics2012WorldListener* m_physicsWorldListener;
	hkArray<class hkaiBehavior*> m_behaviors;

	hkaiViewerContext* m_aiViewerContext;

	/// one global instance of our manager
	static vHavokAiModule g_GlobalManager;
};

#endif	// __VHAVOK_AI_MODULE_HPP

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
