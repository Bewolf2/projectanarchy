/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_NAVMESH_INSTANCE_HPP
#define __VHAVOK_AI_NAVMESH_INSTANCE_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiElementManager.hpp>

class hkaiWorld;
class hkaiNavMeshInstance;
class hkaiNavMeshQueryMediator;
class vHavokAiNavMeshResource;

class vHavokAiNavMeshInstance : public VisTypedEngineObject_cl, public VRefCounter, public vHavokAiElementManager<class vHavokAiNavMeshInstance *>
{
public:

	///
	/// @name Constructor / Destructor
	/// @{
	///

	VHAVOKAI_IMPEXP vHavokAiNavMeshInstance(); // default for when loading from scene
	VHAVOKAI_IMPEXP vHavokAiNavMeshInstance(vHavokAiNavMeshResource* resource, hkaiSectionUid uid); 
	VHAVOKAI_IMPEXP VOVERRIDE ~vHavokAiNavMeshInstance();
	VHAVOKAI_IMPEXP VOVERRIDE void DisposeObject() HKV_OVERRIDE;
	VHAVOKAI_IMPEXP void Release();

	///
	/// @}
	///

	///
	/// @name VisTypedEngineObject_cl overrides
	/// @{
	///

	V_DECLARE_SERIAL_DLLEXP( vHavokAiNavMeshInstance, VHAVOKAI_IMPEXP );
	VHAVOKAI_IMPEXP VOVERRIDE void AssertValid() HKV_OVERRIDE;
	VHAVOKAI_IMPEXP VOVERRIDE void Serialize(VArchive &ar) HKV_OVERRIDE;
	VHAVOKAI_IMPEXP VOVERRIDE void OnSerialized(VArchive &ar) HKV_OVERRIDE;
#ifdef SUPPORTS_SNAPSHOT_CREATION
	VHAVOKAI_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

	///
	/// @}
	///

	///
	/// @name NavMesh property functions
	/// @{
	///

	/// \brief
	///   Gets the navmesh resource used by this instance
	inline vHavokAiNavMeshResource* GetResource() const { return m_resource; }

	/// \brief
	/// Calls vHavokAiModule::AddNavMeshToWorld for all navMeshes.
	VHAVOKAI_IMPEXP void AddNavMeshToWorld(hkaiWorld* world = HK_NULL);

	/// \brief
	/// Calls vHavokAiModule::RemoveNavMeshFromWorld for all navMeshes.
	VHAVOKAI_IMPEXP void RemoveNavMeshFromWorld();

	///
	/// @}
	///

	///
	/// @name Misc
	/// @{
	///

	/// \brief
	/// various accessors
	VHAVOKAI_IMPEXP hkaiNavMeshInstance* GetNavMeshInstance()										{ return m_instance; }
	VHAVOKAI_IMPEXP const hkaiNavMeshInstance* GetNavMeshInstance(int index) const					{ return m_instance; }

	/// \brief
	/// Get bounding box in Vision units
	VHAVOKAI_IMPEXP hkvAlignedBBox GetBoundingBox();

	///
	/// @}
	///

	///
	/// @name Demo code
	/// @{
	///

	/// \brief
	/// for debug rendering
	VHAVOKAI_IMPEXP void DebugRender(float displayOffsetHavokScale, bool colorRegions = false);

	///
	/// @}
	///

protected:

	void Init();
	void CreateInstance();

	hkaiSectionUid								m_uid;
	hkRefPtr<hkaiNavMeshInstance>				m_instance;
	VSmartPtr<vHavokAiNavMeshResource>			m_resource;
	hkaiWorld*									m_aiWorld;
};

#if defined(_VISION_WIIU)
	V_HAVOKAI_ELEMENT_MANAGER_TEMPLATE_DECL(vHavokAiNavMeshInstance)
#endif

#endif	// __VHAVOK_AI_NAVMESH_INSTANCE_HPP

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
