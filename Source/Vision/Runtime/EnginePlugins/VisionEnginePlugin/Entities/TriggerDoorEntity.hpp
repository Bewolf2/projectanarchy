/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TriggerDoorEntity.hpp

#ifndef TRIGGERDOORENTITY_HPP_INCLUDED
#define TRIGGERDOORENTITY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>

#define TRIGGERDOOR_OPEN    "OpenDoor"
#define TRIGGERDOOR_CLOSE   "CloseDoor"


/// \brief
///   Simple trigger door class that can offset position upon trigger events. Used by a lot of the sci-fi scenes
///
/// This class adds two trigger target components to instances of this class so it can be linked visually inside vForge to trigger boxes
class TriggerDoorEntity_cl : public VisBaseEntity_cl
{
protected:
  enum DOOR_STATE 
  {
	  DOOR_CLOSED = 0,
	  DOOR_CLOSING,
	  DOOR_OPEN,
	  DOOR_OPENING
  };

  
  DOOR_STATE m_state;
	float MovingTime;
	float m_fCurrentPos;
	hkvVec3 m_vStartPos;
	hkvVec3 RelativeEndPos;

public:
	EFFECTS_IMPEXP TriggerDoorEntity_cl();
	EFFECTS_IMPEXP virtual ~TriggerDoorEntity_cl();

  EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  V_DECLARE_SERIAL_DLLEXP(TriggerDoorEntity_cl, EFFECTS_IMPEXP);
  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;
	IMPLEMENT_OBJ_CLASS(TriggerDoorEntity_cl);
};

#endif

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
