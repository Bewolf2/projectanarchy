/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_ACTION_HANDLER_H__
#define RPG_PLUGIN_ACTION_HANDLER_H__

#include <Vision/Samples/Engine/RPGPlugin/Action.h>

class RPG_Character;
class VisBaseEntity_cl;

class RPG_Action;
typedef VSmartPtr<RPG_Action> RPG_ActionPtr;


/// Handler for Actions. A character should contain an instance of this.
class RPG_ActionHandler
{
public:

  RPG_ActionHandler();

  void Initialize(RPG_Character* characterOwner);
  void Tick(const float deltaTime);

  void PerformAction(RPG_ActionType_e const& action, bool forceAction = false, RPG_BaseEntity* const interactionEntity = NULL, hkvVec3 const& interactionPosition = hkvVec3(0, 0, 0), int const flags = 0);
  void EndAction(RPG_ActionType_e const& action = AT_None, RPG_ActionType_e const& nextAction = AT_None);
  bool IsPerformingAction(RPG_ActionType_e const& action) const;
  void SetActiveActionFlags(int const flags=0);
  void OnHavokBehaviorEvent(const VString& eventName);

private:
  bool CanPerformAction(RPG_ActionType_e const& action, bool const forceCheck = false) const;
  void StartAction(RPG_ActionType_e const& action, RPG_ActionType_e const previousAction, bool forceAction = false, RPG_BaseEntity* const interactionEntity = NULL, hkvVec3 const& interactionPosition = hkvVec3(0, 0, 0), int const flags = 0);
  void CleanUpAction(RPG_ActionType_e const& action = AT_None, RPG_ActionType_e const& nextAction = AT_None);
  bool IsPerformingAction() const;

  void ProcessAction(RPG_ActionData const& actionData, bool const forceAction = false);

  RPG_Character* m_characterOwner;

  RPG_ActionData m_activeAction;            ///< Current Action being performed by this character
  RPG_ActionData m_previousAction;          ///< If we're chaining out of a Action, this is the outgoing action.
  RPG_ActionData m_pendingAction;           ///< If we've deferred an action, it's stored here.

  bool m_endingAction;                      ///< If true, a Action is in the process of winding up - new action will be deferred
  bool m_chainingAction;                    /// If true, we're chaining into a new action, and can bypass some validation.
  float m_previousActionTime;               ///< Time at which this character last executed a Action

  RPG_ActionPtr m_Actions[AT_Count];        ///< Instanced array of Actions this character can perform
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
