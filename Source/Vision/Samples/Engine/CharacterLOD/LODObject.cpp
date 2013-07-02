/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/CharacterLOD/CharacterLODPCH.h>
#include <Vision/Samples/Engine/CharacterLOD/LODObject.h>

#define ANIMATION_NAME "Walk"

LODObject::LODObject(void)
{
  m_LOD.Init(this);
}


void LODObject::InitFunction()
{
  SetCastShadows(TRUE);

/*
  // set-up (hardcoded) LOD values
  m_LOD.AllocateLevels(4);
  VLODLevelInfo &l0 = m_LOD.GetLevelInfo(LOD_HIGH);
    l0.SetModelFile("soldier_high.model");
    l0.SetSwitchDistance(0.f);
  VLODLevelInfo &l1 = m_LOD.GetLevelInfo(LOD_MEDIUM);
    l1.SetModelFile("soldier_medium.model");
    l1.SetSwitchDistance(600.f);
  VLODLevelInfo &l2 = m_LOD.GetLevelInfo(LOD_LOW);
    l2.SetModelFile("soldier_low.model");
    l2.SetSwitchDistance(1200.f);
  VLODLevelInfo &l3 = m_LOD.GetLevelInfo(LOD_ULTRALOW);
    l3.SetModelFile("soldier_ultralow.model");
    l3.SetSwitchDistance(2400.f);
*/
  bool bResult = m_LOD.LoadFromFile("CharacterLOD\\soldier_lod.xml");
  VVERIFY(bResult);

  m_spRootAnimCtrl = VisAnimConfig_cl::StartSkeletalAnimation(this, ANIMATION_NAME, VSKELANIMCTRL_DEFAULTS|VANIMCTRL_LOOP, 1.f);
  GetAnimConfig()->SetFlags(GetAnimConfig()->GetFlags() & (~APPLY_MOTION_DELTA));

  m_LOD.SetSkeletalAnimRootNode(m_spRootAnimCtrl);
}

void LODObject::PreThinkFunction()
{
  // It is important to perform the LOD update in the PreThink function, so that it is
  // done before the animations are updated. Performing the LOD update in the ThinkFunction
  // may cause incorrect behaviour when combining LOD with other animation features, such
  // as the transition state machine (ticket #1274).
  m_LOD.Update(); // update LOD once a frame
}


V_IMPLEMENT_SERIAL( LODObject, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(LODObject, VisBaseEntity_cl, "LODObject", 0, "Models/Soldier/soldier_high.MODEL")
END_VAR_TABLE

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
