/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerDoorEntity.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

static int TRIGGERDOOR_ID_OPEN = -1;
static int TRIGGERDOOR_ID_CLOSE = -1;


TriggerDoorEntity_cl::TriggerDoorEntity_cl() : RelativeEndPos (0.0f, 0.0f, 250.0f)
{
	m_state = DOOR_CLOSED;
	MovingTime = 1.0f;
	m_fCurrentPos = 0.0f;
}

TriggerDoorEntity_cl::~TriggerDoorEntity_cl()
{
}

void TriggerDoorEntity_cl::InitFunction()
{
	VisBaseEntity_cl::InitFunction();
  
	GetPosition(m_vStartPos);
  SetUseEulerAngles(FALSE);

	m_Components.EnsureCapacity(m_Components.Count()+2);

	VisTriggerTargetComponent_cl *pOpenComp = 
		new VisTriggerTargetComponent_cl(
		TRIGGERDOOR_OPEN,  // trigger name, used in message function
		VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);  // component flag(s)
	AddComponent(pOpenComp);

	VisTriggerTargetComponent_cl *pCloseComp = 
		new VisTriggerTargetComponent_cl(
		TRIGGERDOOR_CLOSE,  // trigger name, used in message function
		VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);  // component flag(s)
	AddComponent(pCloseComp);
}

void TriggerDoorEntity_cl::ThinkFunction()
{
	if (m_state == DOOR_OPENING || m_state == DOOR_CLOSING)
	{
		float dtime = Vision::GetTimer()->GetTimeDifference();
		m_fCurrentPos += (dtime / MovingTime) * ((m_state == DOOR_OPENING) ? 1.0f : -1.0f);
		if (m_fCurrentPos >= 1.0f)
		{
			m_fCurrentPos = 1.0f;
			m_state = DOOR_OPEN;
		}
		else if (m_fCurrentPos <= 0.0f)
		{
			m_fCurrentPos = 0.0f;
			m_state = DOOR_CLOSED;
		}
    float fWeight = 0.5f - 0.5f * hkvMath::cosRad (m_fCurrentPos * hkvMath::pi ()); // smooth acceleration
		SetPosition(m_vStartPos + m_cachedRotMatrix * (RelativeEndPos * fWeight));
	}
}

void TriggerDoorEntity_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
	VisObject3D_cl::MessageFunction(iID,iParamA,iParamB);
	if (iID==VIS_MSG_TRIGGER)
	{
		// cache IDs to avoid string comparisons during runtime
		if (VISION_UNLIKELY(TRIGGERDOOR_ID_OPEN == -1))
		{
			// name to integer ID conversion:
			TRIGGERDOOR_ID_OPEN = IVObjectComponent::RegisterStringID(TRIGGERDOOR_OPEN);
			TRIGGERDOOR_ID_CLOSE = IVObjectComponent::RegisterStringID(TRIGGERDOOR_CLOSE);
		}

		VisTriggerTargetComponent_cl *pTarget = (VisTriggerTargetComponent_cl *)iParamB;
		if (pTarget->m_iComponentID == TRIGGERDOOR_ID_OPEN && 
			(m_state == DOOR_CLOSED || m_state == DOOR_CLOSING))
			m_state = DOOR_OPENING;
		else if (pTarget->m_iComponentID == TRIGGERDOOR_ID_CLOSE  && 
			(m_state == DOOR_OPEN || m_state == DOOR_OPENING))
			m_state = DOOR_CLOSING;
		return;
	}
}

V_IMPLEMENT_SERIAL(TriggerDoorEntity_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule);
void TriggerDoorEntity_cl::Serialize(VArchive &ar)
{
  VisBaseEntity_cl::Serialize(ar);
  if (ar.IsLoading())
  {
    char iVersion;
    ar >> iVersion; VVERIFY(iVersion==0 && "invalid version number");
    ar >> MovingTime;
    RelativeEndPos.SerializeAsVisVector (ar);
    m_vStartPos.SerializeAsVisVector (ar);
    char iState;
    ar >> iState; m_state = (DOOR_STATE)iState;
    ar >> m_fCurrentPos;
  } 
  else
  {
    ar << (char)0; // version
    ar << MovingTime;
    RelativeEndPos.SerializeAsVisVector (ar);
    m_vStartPos.SerializeAsVisVector (ar);
    ar << (char)m_state;
    ar << m_fCurrentPos;
  }
}



START_VAR_TABLE(TriggerDoorEntity_cl, VisBaseEntity_cl, "Entity class for doors", 0, "defaultBox.model")
	DEFINE_VAR_FLOAT(TriggerDoorEntity_cl, MovingTime, "Moving time", "1.0", 0, 0);
	DEFINE_VAR_VECTOR_FLOAT(TriggerDoorEntity_cl, RelativeEndPos, "Relative end position", "0.0/0.0/250.0", 0, 0); 
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
