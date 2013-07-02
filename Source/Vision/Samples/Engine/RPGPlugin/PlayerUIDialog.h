/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_PLAYER_UI_DIALOG_H__
#define RPG_PLUGIN_PLAYER_UI_DIALOG_H__

#include <Vision/Runtime/Engine/Physics/IVisApiPhysicsModule.hpp>

class RPG_BaseEntity;
class RPG_Character;
class RPG_DamageableEntity;

class RPG_PlayerControllerComponent;
typedef VSmartPtr<RPG_PlayerControllerComponent> RPG_PlayerControllerComponentPtr;

enum RPG_PlayerInputs_e
{
  PI_Invalid = -1,
  PI_PrimaryAction =0,
  PI_PrimaryActionX,
  PI_PrimaryActionY,
  PI_SecondaryAction,
  PI_SpecialAction01,
  PI_ShiftModifier,
  PI_SecondaryShiftModifier,
  PI_ToggleTestPattern,
  PI_ToggleMinimap,
  PI_ToggleInventoryDialog,
  PI_ToggleCharacterDialog,
  PI_ToggleHud,
  PI_TestEffect,
  PI_TestCameraShake,
  PI_CheatUnlimitedHealth,
  PI_CheatUnlimitedMana,
  PI_Count
};

// Player UI dialog
class PlayerUIDialog : public VDialog
{
public:
  PlayerUIDialog(RPG_PlayerControllerComponent *playerController);

  VInputMap *GetInputMap() { return m_inputMap; }

private:
  //@{
  // VDialog
  void OnActivate() HKV_OVERRIDE;
  void OnDeactivate() HKV_OVERRIDE;
  void OnSetFocus(bool focus) HKV_OVERRIDE;
  void OnTick(float deltaTime) HKV_OVERRIDE;

  VCursor *GetCurrentCursor(VGUIUserInfo_t& user) HKV_OVERRIDE;
  //@}

  //@{
  // Input handling
  void MapInputTriggers();
  void HandleInput(RPG_Character* characterEntity);
  void HandleInputRelease(RPG_Character* characterEntity);
  bool GetClosestPointOnNavMeshUnderCursor(hkVector4& point, hkVector4 const& searchPoint);
  bool GetFirstAttackableEntityUnderCursor(RPG_DamageableEntity*& attackableEntity, hkvVec3& fallbackTargetPoint, RPG_BaseEntity const *const characterEntity);
  //@}

  //@{
  // Player Actions
  bool TrySpendMana(RPG_Character* characterEntity, int const manaCost);

  void ExecuteMoveOrMeleeAttack(RPG_Character* characterEntity);
  void RequestMove(RPG_Character* characterEntity, bool spawnDestinationEffect);
  void TrySpawnSelectDestinationEffect( hkvVec3 vHitPoint );
  void TryMeleeAttack(RPG_Character* characterEntity, RPG_DamageableEntity * const targetEntity);
  void ContinueMeleeAttack(RPG_Character* characterEntity, RPG_DamageableEntity * const targetEntity);
  void ReleaseMeleeAttack(RPG_Character* characterEntity);

  void ExecuteOrContinueRangedAttack(RPG_Character* characterEntity);
  void ExecuteRangedAttack(RPG_Character* characterEntity, RPG_DamageableEntity* attackableEntity, hkvVec3 const& targetPoint);
  void ContinueRangedAttack(RPG_Character* characterEntity);
  void ReleaseRangedAttack(RPG_Character* characterEntity);

  void ExecutePowerAttack(RPG_Character* characterEntity);
  void ExecuteAoeAttack(RPG_Character* characterEntity);
  //void ExecuteSpecialAction(RPG_Character* characterEntity, int const actionSlot);
  //@}

  //@{
  // Cheats
  void CheatToggleUnlimitedHealth();
  void CheatToggleUnlimitedMana();
  //@}

  //@{
  // Testing
  void PlayTestEffect();
  //@}

  void InitInputMap();
  hkvVec2 GetCursorPosition(IVGUIContext const *context) const;

private:
  RPG_PlayerControllerComponentPtr m_playerController;

  VSmartPtr<VInputMap> m_inputMap;

  VCursorPtr m_cursor;
  VString m_selectDestinationVFX;                 ///< Effect to play when player clicks on a navigation destination
  VString m_cursorTextureFilename;          
  float m_selectDestinationVFXInterval;           ///< Minimum time between select destination effect spawns
  float m_lastSelectDestinationVFXTime;           ///< Last time a select destination effect was played

  // @todo: inputs should be mapped to an array to allow their status to be updated and recorded consistently.
  bool m_wasDown_PlayerMoveOrMelee;  // @todo: @hack: until we set up a proper input array, we're just describing the state of LMouse.
  bool m_wasDown_PlayerRangedAttack;
  float m_wasPressedTime; // @todo: @temp: timer for delay in case player uses multi touch where the character should not move immediately

#if defined(SUPPORTS_MULTITOUCH)

  IVMultiTouchInput* m_touchInput;

#endif
  VisBaseEntity_cl* m_lastTargetEntity;  ///< Last entity on whom this user tried to perform an action
  hkvVec3 m_lastTargetPoint;  ///< Last position at which this user tried to perform an action

  bool m_debugUI;
};

#endif  // RPG_PLUGIN_PLAYER_UI_DIALOG_H__

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
