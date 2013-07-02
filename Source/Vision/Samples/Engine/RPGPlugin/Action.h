/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_ACTION_H__
#define RPG_PLUGIN_ACTION_H__

class RPG_BaseEntity;
class RPG_Character;

/// Enum representing types of Actions that can be performend
enum RPG_ActionType_e
{
  AT_None=-1,
  AT_MeleeAttack=0,
  AT_RangedAttack,
  AT_PowerAttack,
  AT_AoeAttack,
  AT_Dodge,
  AT_Block,
  AT_Die,
  AT_Challenge,
  AT_Move,
  AT_Heal,
  AT_Spawn,
  AT_Count
};

/// Action data that can be replicated across the network
struct RPG_ActionData
{
  RPG_ActionData();
  RPG_ActionData(RPG_ActionType_e const& action, RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition, int const flags);
  void Clear();

  RPG_ActionType_e m_Action;                ///< The action being performed (enum)
  RPG_BaseEntity* m_interactionEntity;    ///< Entity upon whom the action is being performed @todo: Replace this with net entity ID once networking is integrated
  hkvVec3 m_interactionPosition;            ///< The position at which the action is being performed
  int m_flags;                              ///< Optional flags used by this action
};

class RPG_Action : public VRefCounter
{
public:
  RPG_Action(RPG_ActionType_e identifier, RPG_Character *owner);

  //@{
  // Starting, stopping and updating
  virtual void Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition);
  virtual void End(RPG_ActionType_e nextAction = AT_None);
  virtual void Tick(float const deltaTime);

  virtual void Cleanup(RPG_ActionType_e nextAction = AT_None); /// occurs after an action has been told to shut down. Perform any required character state restoration here.
  virtual void UpdateFlags(const int flags);
  virtual void UpdateInteraction(RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition);

  virtual bool CanChainAction(RPG_ActionType_e const& nextAction) const;              /// Can this action be chained after the current one finishes?

  virtual bool CanOverrideActionWith(RPG_ActionType_e const& newAction) const;        /// Can the current action be overridden by the named action?
  virtual bool CanThisActionOverride(RPG_ActionType_e const& actionToOverride) const; /// Can this action override the named action?
  virtual bool CanPerform(bool const forceCheck = false) const;                       /// Can this action be performed?

  virtual void OnHavokBehaviorEvent(const VString& eventName) {};                     /// Handle events coming from the Behavior system
  //@}

  //@{
  // Utility functions
  void FaceTargetPoint(const hkvVec3& targetPoint);
  bool OwnerIsLocalPlayer() const;
  void ValidateInteractionEntity();
  //@}

protected:
  //@{
  // core action members
  RPG_ActionType_e m_identifier;
  RPG_Character* m_characterOwner;
  int m_flags;
  //@}

  //@{
  // supplemental members
  RPG_BaseEntity* m_interactionEntity;        ///< entity with whom this action interacts (optional)
  hkvVec3 m_interactionPosition;                ///< location at which this action interacts (optional)
  float m_startTime;
  //@}

  bool m_debugDisplayInfo;                      ///< display debug information about this action
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
