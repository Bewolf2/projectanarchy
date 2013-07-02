/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
static const char s_libraryName[] = "HavokPhysicsEnginePlugin";
#include <Common/Base/Memory/Tracker/hkTrackerClassDefinition.h>

void HK_CALL HavokPhysicsEnginePluginRegister() {}

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterInput.h>


// vCharacterInput ::
HK_TRACKER_IMPLEMENT_SIMPLE(vCharacterInput, s_libraryName)

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateClimbing.h>


// vCharacterStateClimbing ::

HK_TRACKER_DECLARE_CLASS_BEGIN(vCharacterStateClimbing)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(vCharacterStateClimbing)
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(vCharacterStateClimbing, s_libraryName, hkpCharacterStateClimbing)

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateFlying.h>


// vCharacterStateFlying ::

HK_TRACKER_DECLARE_CLASS_BEGIN(vCharacterStateFlying)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(vCharacterStateFlying)
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(vCharacterStateFlying, s_libraryName, hkpCharacterStateFlying)

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateInAir.h>


// vCharacterStateInAir ::

HK_TRACKER_DECLARE_CLASS_BEGIN(vCharacterStateInAir)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(vCharacterStateInAir)
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(vCharacterStateInAir, s_libraryName, hkpCharacterStateInAir)

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateJumping.h>


// vCharacterStateJumping ::

HK_TRACKER_DECLARE_CLASS_BEGIN(vCharacterStateJumping)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(vCharacterStateJumping)
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(vCharacterStateJumping, s_libraryName, hkpCharacterStateJumping)

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateOnGround.h>


// vCharacterStateOnGround ::

HK_TRACKER_DECLARE_CLASS_BEGIN(vCharacterStateOnGround)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(vCharacterStateOnGround)
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(vCharacterStateOnGround, s_libraryName, hkpCharacterStateOnGround)

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokVisionShapes.h>


// hkvConvexVerticesShape ::

HK_TRACKER_DECLARE_CLASS_BEGIN(hkvConvexVerticesShape)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(hkvConvexVerticesShape)
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(hkvConvexVerticesShape, s_libraryName, hkpConvexVerticesShape)


// hkvMeshMaterial ::

HK_TRACKER_DECLARE_CLASS_BEGIN(hkvMeshMaterial)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(hkvMeshMaterial)
    HK_TRACKER_MEMBER(hkvMeshMaterial, m_userData, 0, "hkStringPtr") // hkStringPtr
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS_BASE(hkvMeshMaterial, s_libraryName)


// hkvBvCompressedMeshShape ::

HK_TRACKER_DECLARE_CLASS_BEGIN(hkvBvCompressedMeshShape)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(hkvBvCompressedMeshShape)
    HK_TRACKER_MEMBER(hkvBvCompressedMeshShape, m_materials, 0, "hkArray<hkvMeshMaterial, hkContainerHeapAllocator>") // hkArray< class hkvMeshMaterial, struct hkContainerHeapAllocator >
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(hkvBvCompressedMeshShape, s_libraryName, hkpBvCompressedMeshShape)


// hkvSampledHeightFieldShape ::

HK_TRACKER_DECLARE_CLASS_BEGIN(hkvSampledHeightFieldShape)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(hkvSampledHeightFieldShape)
    HK_TRACKER_MEMBER(hkvSampledHeightFieldShape, m_pSector, 0, "VTerrainSector*") // const class VTerrainSector*
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(hkvSampledHeightFieldShape, s_libraryName, hkpSampledHeightFieldShape)


// hkvTriSampledHeightFieldCollection ::

HK_TRACKER_DECLARE_CLASS_BEGIN(hkvTriSampledHeightFieldCollection)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(hkvTriSampledHeightFieldCollection)
    HK_TRACKER_MEMBER(hkvTriSampledHeightFieldCollection, m_tileHoleMask, 0, "hkBitField") // class hkBitField
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(hkvTriSampledHeightFieldCollection, s_libraryName, hkpTriSampledHeightFieldCollection)


// hkvTriSampledHeightFieldBvTreeShape ::

HK_TRACKER_DECLARE_CLASS_BEGIN(hkvTriSampledHeightFieldBvTreeShape)
HK_TRACKER_DECLARE_CLASS_END

HK_TRACKER_CLASS_MEMBERS_BEGIN(hkvTriSampledHeightFieldBvTreeShape)
HK_TRACKER_CLASS_MEMBERS_END()
HK_TRACKER_IMPLEMENT_CLASS(hkvTriSampledHeightFieldBvTreeShape, s_libraryName, hkpTriSampledHeightFieldBvTreeShape)

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
