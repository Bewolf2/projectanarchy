/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_ACTION_MOVE_H__
#define RPG_PLUGIN_ACTION_MOVE_H__

#include <Vision/Samples/Engine/RPGPlugin/Action.h>

class RPG_ControllerComponent;

/// Flags that determine how to move
enum RPG_Action_Move_Flags_e
{
  MF_None = V_BIT(0),             ///< Just move to the position
  MF_AttackRange = V_BIT(1),      ///< Move within attack range
};

/// Action that can move to a position or target entity
class RPG_Action_Move : public RPG_Action
{
public:
  RPG_Action_Move(RPG_Character *owner);

  virtual void Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition) HKV_OVERRIDE;
  virtual void Cleanup(RPG_ActionType_e nextAction) HKV_OVERRIDE; /// occurs after an action has been told to shut down. Perform any required character state restoration here.
  virtual void Tick(float const deltaTime) HKV_OVERRIDE;
  virtual void UpdateInteraction(RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition) HKV_OVERRIDE;

  virtual bool CanChainAction(RPG_ActionType_e const& nextAction) const HKV_OVERRIDE;              /// Can this action be chained after the current one finishes?

protected:
  bool CheckRange() const;

  RPG_ControllerComponent* m_controllerComponent;
  bool m_positionChanged;
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
