/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// GameState_cl Source
//***********************************************************************

#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Samples/Engine/Accelerometer/GameState.hpp>

// InitFunction: Is called upon entity initialization.
void GameState_cl::InitFunction()
{
  // Add target component
  VisTriggerTargetComponent_cl *pTargetComp = new VisTriggerTargetComponent_cl();
  AddComponent(pTargetComp);

  // Get trigger box entity
  VisBaseEntity_cl *pTriggerBoxEntity = Vision::Game.SearchEntity("TriggerBox");
  VASSERT(pTriggerBoxEntity);

  // Find source component
  VisTriggerSourceComponent_cl *pSourceComp = NULL;
  for (int i=0;i<pTriggerBoxEntity->Components().Count();i++)
  {
    IVObjectComponent *pComp = pTriggerBoxEntity->Components().GetAt(i);
    if(strcmp(pComp->GetComponentName(),"OnObjectEnter")==0)
    {
      pSourceComp = (VisTriggerSourceComponent_cl*) pComp;
      break;
    }
  }
  VASSERT(pSourceComp);

  // Link source and target component 
  IVisTriggerBaseComponent_cl::OnLink(pSourceComp, pTargetComp);
  
  m_eState = GAME_STATE_RUN;
}

// DeInitFunction:: Is called upon entity de-initialization.
void GameState_cl::DeInitFunction()
{
}

// ThinkFunction: Is called once per frame.
void GameState_cl::ThinkFunction()
{
}

void GameState_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VisObject3D_cl::MessageFunction(iID,iParamA,iParamB);

  if (iID==VIS_MSG_TRIGGER)
    m_eState = GAME_STATE_RESTART;
}

void GameState_cl::SetCurrentState(GAME_STATE eState)
{
  m_eState = eState;
}

GAME_STATE GameState_cl::GetCurrentState() const
{
  return m_eState;
}

V_IMPLEMENT_SERIAL( GameState_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(GameState_cl, VisBaseEntity_cl, "GameState_cl", 0, "")
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
