/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/PlayerUIDialog.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUI.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/GUIManager.h>
#if (RPG_UI_SCALEFORM)
#include <Vision/Samples/Engine/RPGPlugin/GUIManager_Scaleform.h>
#endif
#include <Vision/Samples/Engine/RPGPlugin/PlayerControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

// DEMO
#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponentManager.h>
#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionHandler.h>
#include <Vision/Samples/Engine/RPGPlugin/Action.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionMove.h>


// TODO: correct AI header for ray query
#include <Ai/Pathfinding/Character/Behavior/hkaiPathFollowingBehavior.h>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

namespace
{
  class RPG_ClosestAttackableRaycastResult : public VisPhysicsRaycastBase_cl
  {
  public:
    RPG_ClosestAttackableRaycastResult(RPG_BaseEntity const *const entityToIgnore);
    virtual void Reset();
    virtual bool onHit(VisPhysicsHit_t &hit);
    virtual bool allHits();

  public:
    RPG_BaseEntity const *const m_entityToIgnore;
    VisPhysicsHit_t m_closestHit;
    VisPhysicsHit_t m_closestAttackableHit;
    bool m_hit;
    bool m_foundAttackable;
  };

  const int RPG_INPUT_MAP_NUM_INPUTS = 64;
  const int RPG_INPUT_MAP_NUM_ALTERNATIVES = 20;
}

RPG_ClosestAttackableRaycastResult::RPG_ClosestAttackableRaycastResult(RPG_BaseEntity const *const entityToIgnore)
: VisPhysicsRaycastBase_cl()
, m_entityToIgnore(entityToIgnore)
, m_closestHit()
, m_closestAttackableHit()
, m_hit(false)
, m_foundAttackable(false)
{
  iCollisionBitmask = hkpGroupFilter::calcFilterInfo(0, 0, 0, 0);
}

void RPG_ClosestAttackableRaycastResult::Reset()
{
  VisPhysicsRaycastBase_cl::Reset();
  m_closestHit.Reset();
  m_closestAttackableHit.Reset();
  m_hit = false;
  m_foundAttackable = false;
  iCollisionBitmask = hkpGroupFilter::calcFilterInfo(0, 0, 0, 0);
}

bool RPG_ClosestAttackableRaycastResult::onHit( VisPhysicsHit_t &hit )
{
  if(!hit.pHitObject)
  {
    return true;
  }
  
  if(hit.pHitObject == m_entityToIgnore)
  {
    return true; // ignore self
  }

  if(hit.pHitObject->IsFrom(RPG_DamageableEntity))
  {
    RPG_DamageableEntity* hitEntity = static_cast<RPG_DamageableEntity*>(hit.pHitObject);
    RPG_AttackableComponent* attackableComponent = static_cast<RPG_AttackableComponent*>(hitEntity->Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_AttackableComponent)));
    if (attackableComponent)
    {
      // hit an attackable entity
      if(hit.fHitFraction < m_closestAttackableHit.fHitFraction)
      {
        m_closestAttackableHit = hit;
        m_foundAttackable = true;
      }
    }
  }

  if(hit.fHitFraction < m_closestHit.fHitFraction)
  {
    m_closestHit = hit;
  }
  
  m_hit = true;

  return true; // keep looking
}

bool RPG_ClosestAttackableRaycastResult::allHits()
{
  return true;
}

// PlayerUIDialog
PlayerUIDialog::PlayerUIDialog(RPG_PlayerControllerComponent *playerController)
  : VDialog(),
  m_playerController(playerController)
  , m_cursor()
  , m_selectDestinationVFX("Particles/UI/RPG_UI_SelectDestination.xml")
  , m_selectDestinationVFXInterval(0.1f)
  , m_lastSelectDestinationVFXTime(0.0f)
  , m_cursorTextureFilename("GUI/Textures/Mouse.dds")

  , m_wasDown_PlayerMoveOrMelee(false)
  , m_wasDown_PlayerRangedAttack(false)
  , m_wasPressedTime(0.0f)

  , m_touchInput(NULL)
  , m_lastTargetEntity(NULL)
  , m_lastTargetPoint(0.0f, 0.0f, 0.0f)
  , m_debugUI(false)
{
}

// VDialog
void PlayerUIDialog::OnActivate()
{
  VDialog::OnActivate();

  // get the dimensions of the active render context and scale the UI to those dimensions
  int renderContextWidth, renderContextHeight;
  VisRenderContext_cl::GetCurrentContext()->GetSize(renderContextWidth, renderContextHeight);
  SetSize(static_cast<float>(renderContextWidth), static_cast<float>(renderContextHeight));

  m_inputMap = new VInputMap(RPG_INPUT_MAP_NUM_INPUTS, RPG_INPUT_MAP_NUM_ALTERNATIVES);
  InitInputMap();

  RPG_GuiManager::GetInstance()->OnActivate(this);
}

void PlayerUIDialog::OnDeactivate()
{
  VDialog::OnDeactivate();
  RPG_GuiManager::GetInstance()->OnDeactivate();

  m_inputMap = NULL;
}

void PlayerUIDialog::OnSetFocus(bool focus)
{
  VDialog::OnSetFocus(focus);
#if defined(_VISION_MOBILE)
  m_cursor = GetMenuManager()->LoadCursorResource("GUI/Textures/MouseMobile.tga");
#else
  VASSERT_MSG(!m_cursorTextureFilename.IsEmpty(), "Cursor texture filename hasn't been specified.");
  m_cursor = GetMenuManager()->LoadCursorResource(m_cursorTextureFilename.AsChar());
  VASSERT_MSG(m_cursor, "Cursor failed to load.");
#endif

#if defined(SUPPORTS_MULTITOUCH) && (HAVOK_VISION_RESTRUCTURING) && !defined(_VISION_ANDROID)

  m_touchInput = NULL;

  IVInputDevice* inputDevice = &VInputManagerPC::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN);
  if(inputDevice != &VInputManagerPC::s_NoInputDevice)
    m_touchInput = static_cast<IVMultiTouchInput*>(inputDevice);

#endif
}

void PlayerUIDialog::OnTick(float deltaTime)
{
  VDialog::OnTick(deltaTime);

#if defined (SUPPORTS_MULTITOUCH) && (HAVOK_VISION_RESTRUCTURING) && !defined(_VISION_ANDROID)

  if (m_touchInput != NULL)
  {     
    int count = m_touchInput->GetNumberOfTouchPoints();
    if (count > 1)
      return;
  }

#endif

  RPG_Character *characterEntity = static_cast<RPG_Character *>(m_playerController->GetOwner());
  if (!characterEntity)
  {
    // @todo: handle player death properly
    Vision::Error.Warning("Player character has probably died, which isn't yet handled. Set the character's Unlimited Health entity property to true to prevent this.");
    return;
  }

  HandleInput(characterEntity);
  RPG_GuiManager::GetInstance()->OnTick(deltaTime);
}

VCursor *PlayerUIDialog::GetCurrentCursor(VGUIUserInfo_t& user)
{
  return m_cursor;
}


// Input Handling
void PlayerUIDialog::HandleInput(RPG_Character* characterEntity)
{
  VASSERT(characterEntity);

  // TODO GDC hack; don't update input during spawn
  if(characterEntity->GetActionHandler().IsPerformingAction(AT_Spawn))
  {
    m_wasDown_PlayerMoveOrMelee = false;
    m_wasPressedTime = -1.f;
    return;
  }

  // player control
  if (m_inputMap->GetTrigger(PI_SpecialAction01))
  {
    ExecuteAoeAttack(characterEntity);
  }
  else if (m_inputMap->GetTrigger(PI_SecondaryAction))
  {
    ExecutePowerAttack(characterEntity);
  }
  else if (m_inputMap->GetTrigger(PI_PrimaryAction))
  {
    if (m_inputMap->GetTrigger(PI_ShiftModifier))
    {
      ExecuteOrContinueRangedAttack(characterEntity);
    }
    else if(m_inputMap->GetTrigger(PI_SecondaryShiftModifier))
    {
      ExecutePowerAttack(characterEntity);
    }
    else
    {
      ExecuteMoveOrMeleeAttack(characterEntity);
    }
  }
  // cheats
  if (m_inputMap->GetTrigger(PI_CheatUnlimitedHealth))
  {
    CheatToggleUnlimitedHealth();
  }
  else if (m_inputMap->GetTrigger(PI_CheatUnlimitedMana))
  {
    CheatToggleUnlimitedMana();
  }

#if (RPG_UI_SCALEFORM)
  // gui control
  if (m_inputMap->GetTrigger(PI_ToggleHud))
  {
    VASSERT(static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance()));
    static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance())->ToggleHUD();
  }
  else if (m_inputMap->GetTrigger(PI_ToggleMinimap))
  {
    VASSERT(static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance()));
    static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance())->ToggleMinimap();
  }
  else if (m_inputMap->GetTrigger(PI_ToggleCharacterDialog))
  {
    VASSERT(static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance()));
    static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance())->ToggleCharacterDialog();
  }
  else if (m_inputMap->GetTrigger(PI_ToggleInventoryDialog))
  {
    VASSERT(static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance()));
    static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance())->ToggleInventoryDialog();
  }
  else if (m_inputMap->GetTrigger(PI_ToggleTestPattern))
  {
    VASSERT(static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance()));
    static_cast<RPG_GuiManager_Scaleform*>(RPG_GuiManager::GetInstance())->ToggleScaleformTestMovie("GUI\\Flash\\RPG_GUI_TestPattern.swf", true);  // test an arbitrary Scaleform movie
  }
#endif

  else if (m_inputMap->GetTrigger(PI_TestEffect))
  {
    PlayTestEffect();
  }
  else if (m_inputMap->GetTrigger(PI_TestCameraShake))
  {
    RPG_EffectDefinition effectDef;
    effectDef.m_cameraShakeAmplitude = 75.f;
    effectDef.m_cameraShakeDuration = 0.1f;
    RPG_GameManager::s_instance.CreateEffect(effectDef, hkvVec3(0.f, 0.f, 0.f));
  }

  HandleInputRelease(characterEntity); // do we have any concluding events we need to clean up?
}

void PlayerUIDialog::HandleInputRelease(RPG_Character* characterEntity)
{
  const bool primaryActionInputHeld = m_inputMap->GetTrigger(PI_PrimaryAction);
  const bool shiftModifierHeld = m_inputMap->GetTrigger(PI_ShiftModifier);

  // is player ending a primary action?
  if (!primaryActionInputHeld && m_wasDown_PlayerMoveOrMelee)
  {
#ifdef _DEBUG
    const VString msg = "Release Primary Action.";
    Vision::Error.SystemMessage(msg.AsChar());
    Vision::Message.Add(1, msg.AsChar());
#endif

    ReleaseMeleeAttack(characterEntity);
    m_wasDown_PlayerMoveOrMelee = false;
    m_wasPressedTime = -1.f;
  }

  // is player ending a ranged attack?
  if ((!primaryActionInputHeld || !shiftModifierHeld) && m_wasDown_PlayerRangedAttack)
  {
#ifdef _DEBUG
    const VString msg = "Release Primary Action.";
    Vision::Error.SystemMessage(msg.AsChar());
    Vision::Message.Add(1, msg.AsChar());
#endif

    ReleaseRangedAttack(characterEntity);
    m_wasDown_PlayerRangedAttack = false;
    m_wasPressedTime = -1.f;
  }
}

bool PlayerUIDialog::GetClosestPointOnNavMeshUnderCursor(hkVector4& point, hkVector4 const& searchPoint)
{
  IVGUIContext *const context = GetContext();
  VASSERT(context);
  hkvVec2 const mousePos = GetCursorPosition(context);

  hkvVec3 traceDirOut;
  VisRenderContext_cl::GetCurrentContext()->GetTraceDirFromScreenPos(mousePos.x, mousePos.y, traceDirOut, 1.0f);
  hkvVec3 const& camPos = VisRenderContext_cl::GetCurrentContext()->GetCamera()->GetPosition();

  hkVector4 rayStartHavok, rayEndHavok;
  RPG_VisionHavokConversion::VisionToHavokPoint(camPos, rayStartHavok);
  RPG_VisionHavokConversion::VisionToHavokPoint(camPos + traceDirOut * 5000.0f, rayEndHavok);

  hkaiNavMeshQueryMediator::HitDetails hit;
  if(vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->castRay(rayStartHavok, rayEndHavok, hit))
  {
    point.setInterpolate(rayStartHavok, rayEndHavok, hit.m_hitFraction);
    return true;
  }
  else
  {
    // If the ray missed the nav mesh:
    // 1. Find the nav mesh face closest to the search point
    // 2. Intersect the ray with the plane of this face
    // 3. Find the closest point on the nav mesh to the plane point
    hkVector4 dummyPoint;
    hkaiPackedKey const searchFaceKey = 
      vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->getClosestPoint(searchPoint, 1.f, dummyPoint);
    if(searchFaceKey != HKAI_INVALID_PACKED_KEY)
    {
      hkVector4 facePlane;
      hkaiNavMeshInstance const *meshInstance = vHavokAiModule::GetInstance()->GetAiWorld()->getStreamingCollection()->getInstanceAt(hkaiGetRuntimeIdFromPacked(searchFaceKey));
      {
        hkaiNavMeshUtils::calcFacePlane(*meshInstance, hkaiGetIndexFromPacked(searchFaceKey), facePlane);
      }

      hkSimdReal const f = facePlane.dot4xyz1(rayStartHavok);
      hkSimdReal const t = facePlane.dot4xyz1(rayEndHavok);
      if(f.isGreaterEqualZero() && t.isLessZero())
      {
        hkVector4 planePoint;
        {
          hkSimdReal const hitFraction = f / (f - t);
          planePoint.setInterpolate(rayStartHavok, rayEndHavok, hitFraction);
        }

        hkaiPackedKey faceKey = vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->getClosestPoint(planePoint, 5.f, point);
        return (faceKey != HKAI_INVALID_PACKED_KEY);
      }
    }
  }

  return false;
}

bool PlayerUIDialog::GetFirstAttackableEntityUnderCursor(RPG_DamageableEntity*& attackableEntity, hkvVec3& fallbackTargetPoint, RPG_BaseEntity const *const characterEntity)
{
  IVGUIContext *const context = GetContext();
  VASSERT(context);
  hkvVec2 const mousePos = GetCursorPosition(context);

  hkvVec3 traceDirOut;
  VisRenderContext_cl::GetCurrentContext()->GetTraceDirFromScreenPos(mousePos.x, mousePos.y, traceDirOut, 1.0f);
  hkvVec3 const& camPos = VisRenderContext_cl::GetCurrentContext()->GetCamera()->GetPosition();

  RPG_ClosestAttackableRaycastResult result(characterEntity);
  result.Reset();
  result.vRayStart = camPos;
  result.vRayEnd = camPos + traceDirOut * 5000.0f;

  Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&result);

  if(result.m_hit)
  {
    if(result.m_foundAttackable)
    {
      attackableEntity = static_cast<RPG_DamageableEntity*>(result.m_closestAttackableHit.pHitObject);
      fallbackTargetPoint = attackableEntity->GetPosition();
    }
    else
    {
      attackableEntity = NULL;
      fallbackTargetPoint = result.m_closestHit.vImpactPoint;
    }

    return true;
  }
  return false;
}

// Player Actions
bool PlayerUIDialog::TrySpendMana(RPG_Character* characterEntity, int const manaCost)
{
  int const availableMana = characterEntity->GetCharacterStats().GetMana();
  if (availableMana >= manaCost)
  {
    characterEntity->GetCharacterStats().SetMana(availableMana - manaCost);
    // @todo: flash mana bar on spend
    return true;
  }
  else
  {
    if (characterEntity->GetCharacterStats().HasUnlimitedMana())
    {
      characterEntity->GetCharacterStats().SetMana(characterEntity->GetCharacterStats().GetManaMax());
      return true;
    }
    else
    {
      characterEntity->GetCharacterStats().LogCharacterEvent(CE_FailedManaSpend, Vision::GetTimer()->GetTime());
      return false;
    }
  }
}


void PlayerUIDialog::ExecuteMoveOrMeleeAttack(RPG_Character* characterEntity)
{
  VASSERT(characterEntity);

#if defined (SUPPORTS_MULTITOUCH) && (HAVOK_VISION_RESTRUCTURING) && !defined(_VISION_ANDROID)

  // Wait 0.1 seconds in case player uses multi touch 
  if (m_wasPressedTime < 0.f)
  {
    m_wasPressedTime = Vision::GetTimer()->GetTime();
    return;
  }
  else if (Vision::GetTimer()->GetTime() - m_wasPressedTime < 0.1f)
  {
    return;
  }

#endif

//#ifdef _DEBUG
//  const VString msg = "Execute Primary Action.";
//  //Vision::Error.SystemMessage(msg.AsChar());
//  Vision::Message.Add(1, msg.AsChar());
//#endif

  const bool inputEdge = !m_wasDown_PlayerMoveOrMelee; // if primary action input is activated now, and wasn't previously, this is a press event.

  // Get the location of the player input and information about any hit object
  RPG_DamageableEntity* attackableEntity = NULL;
  hkvVec3 targetPoint(0.0f, 0.0f, 0.0f);
  GetFirstAttackableEntityUnderCursor(attackableEntity, targetPoint, characterEntity);

  if(attackableEntity)
  {
    // Hit an object
    if (inputEdge || attackableEntity != m_lastTargetEntity)
    {
      // Initial press event, or a continued press that slipped from one entity to another
      TryMeleeAttack(characterEntity, attackableEntity);
    }
    else
    {
      // Continued hold
      ContinueMeleeAttack(characterEntity, attackableEntity);
    }
  }
  else
  {
    // No object hit
    if (!characterEntity->IsAttacking())  // don't terminate an attack mid-swing
    {
      RequestMove(characterEntity, inputEdge);
    }
  }
  m_lastTargetEntity = attackableEntity;
  m_lastTargetPoint = targetPoint;
  m_wasDown_PlayerMoveOrMelee = true;
}

void PlayerUIDialog::RequestMove(RPG_Character* characterEntity, bool spawnDestinationEffect)
{
  VASSERT(characterEntity);

  hkVector4 hitPoint, characterPos;
  RPG_VisionHavokConversion::VisionToHavokPoint(characterEntity->GetPosition(), characterPos);
  if(GetClosestPointOnNavMeshUnderCursor(hitPoint, characterPos))
  {
    hkvVec3 vHitPoint;
    RPG_VisionHavokConversion::HavokToVisionPoint(hitPoint, vHitPoint);

    characterEntity->GetActionHandler().PerformAction(AT_Move, false, NULL, vHitPoint);

    if(spawnDestinationEffect)
      TrySpawnSelectDestinationEffect(vHitPoint);

#ifdef _DEBUG
    if (m_debugUI)
    {
      Vision::Message.DrawMessage3D("RequestPath", vHitPoint);
    }
#endif
  }
}

void PlayerUIDialog::TrySpawnSelectDestinationEffect(hkvVec3 vHitPoint)
{
  // if a visual effect for this type has been specified, play it
  if (!m_selectDestinationVFX.IsEmpty())
  {
    // don't blithely spawn a particle every tick
    if (Vision::GetTimer()->GetTime() - m_lastSelectDestinationVFXTime >= m_selectDestinationVFXInterval)
    {
      RPG_VisionEffectHelper::CreateParticleEffect(m_selectDestinationVFX, vHitPoint);
      m_lastSelectDestinationVFXTime = Vision::GetTimer()->GetTime();
    }
  }
}

void PlayerUIDialog::TryMeleeAttack(RPG_Character* characterEntity, RPG_DamageableEntity * const targetEntity)
{ 
  VASSERT(characterEntity);

  const bool hitHighlightable = targetEntity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_HighlightableComponent));
  const bool hitAttackable = targetEntity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_AttackableComponent));
  //@debug: test log output
  //Vision::Error.Warning(hitEntity->GetMesh()->GetFilename());

  if (hitHighlightable || hitAttackable)
  {
    if (hitHighlightable)  //"(RPG) Highlightable"
    {
      //Vision::Error.Warning("(RPG) Highlightable");
    }

    if (hitAttackable)  //"(RPG) Attackable"
    {
      //Vision::Error.Warning("(RPG) Attackable");
      if(characterEntity->IsTargetWithinAttackRange(targetEntity))
      {
        characterEntity->GetActionHandler().PerformAction(AT_MeleeAttack, true, targetEntity, hkvVec3(0, 0, 0), 0);
      }
      else
      {
        characterEntity->GetActionHandler().PerformAction(AT_Move, true, targetEntity, hkvVec3(0, 0, 0), MF_AttackRange);
        characterEntity->GetActionHandler().PerformAction(AT_MeleeAttack, false, targetEntity);
      }
    }
  }
  else
  {
    // fall back to the move if the entity we hit wasn't a highlightable or attackable.
    if (!characterEntity->IsAttacking())
    {
      RequestMove(characterEntity, true);
    }
  }
}

void PlayerUIDialog::ContinueMeleeAttack(RPG_Character* characterEntity, RPG_DamageableEntity * const targetEntity)
{
  VASSERT(characterEntity);
  const bool hitHighlightable = targetEntity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_HighlightableComponent));
  const bool hitAttackable = targetEntity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_AttackableComponent));
  //@debug: test log output
  //Vision::Error.Warning(hitEntity->GetMesh()->GetFilename());

  if (hitHighlightable || hitAttackable)
  {
    if (hitHighlightable)  //"(RPG) Highlightable"
    {
      //Vision::Error.Warning("(RPG) Highlightable");

#ifdef _DEBUG
      if (m_debugUI)
      {
        Vision::Message.DrawMessage3D("(RPG) Highlightable", targetEntity->GetPosition());
      }
#endif
    }

    if (hitAttackable)  //"(RPG) Attackable"
    {
      // if player's holding down primary action input on an existing attack action, this sets its continue flag to true.
      if (characterEntity->GetActionHandler().IsPerformingAction(AT_MeleeAttack))
      {
        characterEntity->GetActionHandler().SetActiveActionFlags(1);
      }

#ifdef _DEBUG
      if (m_debugUI)
      {
        Vision::Message.DrawMessage3D("(RPG) Attackable", targetEntity->GetPosition() + hkvVec3(0.0f, 0.0f, 16.0f)); // (@hack: offset a little to avoid overwriting other component messages)
      }
#endif
    }
  }
  else
  {
    if(!characterEntity->IsAttacking())
    {
      // fall back to the move if the entity we hit wasn't a highlightable or attackable.
      RequestMove(characterEntity, false);
    }
  }
}

void PlayerUIDialog::ReleaseMeleeAttack(RPG_Character* characterEntity)
{
  VASSERT(characterEntity);
  // player has released primary action input. Flip off the continue flag on his melee action.
  if (characterEntity->GetActionHandler().IsPerformingAction(AT_MeleeAttack))
  {
    characterEntity->GetActionHandler().SetActiveActionFlags(0);
  }
}


void PlayerUIDialog::ExecuteOrContinueRangedAttack(RPG_Character* characterEntity)
{
#ifdef _DEBUG
  const VString msg = "Execute or Continue Ranged Attack.";
  //Vision::Error.SystemMessage(msg.AsChar());
  Vision::Message.Add(1, msg.AsChar());
#endif

  const bool inputEdge = !m_wasDown_PlayerRangedAttack; // if ranged attack input is activated now, and wasn't previously, this is a press event.

  RPG_DamageableEntity* attackableEntity = NULL;
  hkvVec3 targetPoint(0.0f, 0.0f, 0.0f);
  GetFirstAttackableEntityUnderCursor(attackableEntity, targetPoint, characterEntity);
  // TODO - intersect ray with horizontal plane at characterEntity's root node if this returns false
  // TODO - modify static mesh target height based on normal of interaction point

  if(inputEdge)
  {
    // Initial press event, or a continued press that slipped from one entity to another
    ExecuteRangedAttack(characterEntity, attackableEntity, targetPoint);
  }
  else
  {
    if(attackableEntity != m_lastTargetEntity ||
       !targetPoint.isIdentical(m_lastTargetPoint))
    {
      // updates interaction data for currently active ranged attack action
      ExecuteRangedAttack(characterEntity, attackableEntity, targetPoint);
    }

    // continued hold
    ContinueRangedAttack(characterEntity);
  }
  m_lastTargetEntity = attackableEntity;
  m_lastTargetPoint = targetPoint;
  m_wasDown_PlayerRangedAttack = true;
}

void PlayerUIDialog::ExecuteRangedAttack(RPG_Character* characterEntity, RPG_DamageableEntity* attackableEntity, hkvVec3 const& targetPoint)
{
  if (attackableEntity)
  {
      // Ranged attack against character
    characterEntity->GetActionHandler().PerformAction(AT_RangedAttack, true, attackableEntity);
    }
    else
    {
      // Ranged attack against point
    characterEntity->GetActionHandler().PerformAction(AT_RangedAttack, true, NULL, targetPoint);
  }
}

void PlayerUIDialog::ContinueRangedAttack(RPG_Character* characterEntity)
{
  VASSERT(characterEntity);

  // if player's holding down ranged attack input on an existing ranged attack action, this sets its continue flag to true.
  if (characterEntity->GetActionHandler().IsPerformingAction(AT_RangedAttack))
  {
    characterEntity->GetActionHandler().SetActiveActionFlags(1);
  }
}

void PlayerUIDialog::ReleaseRangedAttack(RPG_Character* characterEntity)
{
  VASSERT(characterEntity);
  // player has released ranged attack input. Flip off the continue flag on his ranged attack action.
  if (characterEntity->GetActionHandler().IsPerformingAction(AT_RangedAttack))
  {
    characterEntity->GetActionHandler().SetActiveActionFlags(0);
  }
}

void PlayerUIDialog::ExecutePowerAttack(RPG_Character* characterEntity)
{
  VASSERT(characterEntity);

  if (!characterEntity->IsDoingSpecialAttack())
  {
  if (TrySpendMana(characterEntity, characterEntity->GetCharacterActionData().GetPowerAttackManaCost())) // @todo: data-drive action mana costs
  {
    RPG_DamageableEntity* attackableEntity = NULL;
    hkvVec3 targetPoint(0.0f, 0.0f, 0.0f);
    if(GetFirstAttackableEntityUnderCursor(attackableEntity, targetPoint, characterEntity))
    {
      characterEntity->GetActionHandler().PerformAction(AT_PowerAttack, true, attackableEntity, targetPoint);
    }
    else
    {
      characterEntity->GetActionHandler().PerformAction(AT_PowerAttack, true, NULL, m_lastTargetPoint);
    }
    m_lastTargetEntity = attackableEntity;
    m_lastTargetPoint = targetPoint;
  }
  }
}

void PlayerUIDialog::ExecuteAoeAttack(RPG_Character* characterEntity)
{
  VASSERT(characterEntity);

  if (!characterEntity->IsDoingSpecialAttack())
  {
  if (TrySpendMana(characterEntity, characterEntity->GetCharacterActionData().GetAoeAttackManaCost())) // @todo: data-drive action mana costs
  {
    characterEntity->GetActionHandler().PerformAction(AT_AoeAttack, true);
  }
  }
}

void PlayerUIDialog::PlayTestEffect()
{
  RPG_Character *characterEntity = static_cast<RPG_Character *>(m_playerController->GetOwner());
  VASSERT(characterEntity);
  const hkvVec3 position = characterEntity->GetPosition();
  RPG_VisionEffectHelper::PlayFmodSoundEvent("Characters/Barbarian", "Melee_Basic_Swing", position);  // first argument refers to event group names in the FMOD project specified by GameManager::FMOD_EVENT_PROJECT
}

void PlayerUIDialog::CheatToggleUnlimitedHealth()
{
  RPG_Character *characterEntity = static_cast<RPG_Character *>(m_playerController->GetOwner());
  VASSERT(characterEntity);
  characterEntity->GetCharacterStats().SetUnlimitedHealth(!characterEntity->GetCharacterStats().HasUnlimitedHealth());

  VString msg;
  if (characterEntity->GetCharacterStats().HasUnlimitedHealth())
  {
    msg = "CHEAT - Unlimited Health: ON";
  }
  else
  {
    msg = "CHEAT - Unlimited Health: OFF";
  }
  Vision::Error.SystemMessage(msg.AsChar());
  Vision::Message.Add(1, msg.AsChar());
}

void PlayerUIDialog::CheatToggleUnlimitedMana()
{
  RPG_Character *characterEntity = static_cast<RPG_Character *>(m_playerController->GetOwner());
  VASSERT(characterEntity);
  characterEntity->GetCharacterStats().SetUnlimitedMana(!characterEntity->GetCharacterStats().HasUnlimitedMana());

  VString msg;
  if (characterEntity->GetCharacterStats().HasUnlimitedMana())
  {
    msg = "CHEAT - Unlimited Mana: ON";
  }
  else
  {
    msg = "CHEAT - Unlimited Mana: OFF";
  }
  Vision::Error.SystemMessage(msg.AsChar());
  Vision::Message.Add(1, msg.AsChar());
}

void PlayerUIDialog::InitInputMap()
{
#ifdef SUPPORTS_MOUSE
  m_inputMap->MapTrigger(PI_PrimaryAction, V_MOUSE, CT_MOUSE_LEFT_BUTTON);
  m_inputMap->MapTrigger(PI_SecondaryAction, V_MOUSE, CT_MOUSE_RIGHT_BUTTON);
  m_inputMap->MapTrigger(PI_SpecialAction01, V_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
#endif

#ifdef SUPPORTS_KEYBOARD
  // player actions
  m_inputMap->MapTrigger(PI_ShiftModifier, V_KEYBOARD, CT_KB_LSHIFT);
  m_inputMap->MapTrigger(PI_SecondaryShiftModifier, V_KEYBOARD, CT_KB_RSHIFT);
  m_inputMap->MapTrigger(PI_SpecialAction01, V_KEYBOARD, CT_KB_1, VInputOptions::Once());

  // gui
  m_inputMap->MapTrigger(PI_ToggleTestPattern, V_KEYBOARD, CT_KB_S, VInputOptions::Once());
  m_inputMap->MapTrigger(PI_ToggleCharacterDialog, V_KEYBOARD, CT_KB_C, VInputOptions::Once());
  m_inputMap->MapTrigger(PI_ToggleInventoryDialog, V_KEYBOARD, CT_KB_I, VInputOptions::Once());
  m_inputMap->MapTrigger(PI_ToggleMinimap, V_KEYBOARD, CT_KB_M, VInputOptions::Once());
  m_inputMap->MapTrigger(PI_ToggleHud, V_KEYBOARD, CT_KB_H, VInputOptions::Once());

  // testing
  m_inputMap->MapTrigger(PI_TestEffect, V_KEYBOARD, CT_KB_Z, VInputOptions::Once());
  m_inputMap->MapTrigger(PI_TestCameraShake, V_KEYBOARD, CT_KB_X, VInputOptions::Once());
  
  // cheats
  m_inputMap->MapTrigger(PI_CheatUnlimitedHealth, V_KEYBOARD, CT_KB_Z, VInputOptions::Once());
  m_inputMap->MapTrigger(PI_CheatUnlimitedMana, V_KEYBOARD, CT_KB_X, VInputOptions::Once());
#endif

#if defined(_VISION_MOBILE)
  // vSceneViewer profiling menu uses -900 for depth
  // Explicitly place the touchscreen area 'behind' that at -950
  // Though now the rest of the GUI probably doesn't work!
  float const depth = -950.0f;
  int const width = Vision::Video.GetXRes();
  int const height = Vision::Video.GetYRes();

  VTouchArea *touchScreen = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(0.0f, 0.0f, width, height), depth);
  m_inputMap->MapTrigger(PI_PrimaryAction, touchScreen, CT_TOUCH_ANY);
  m_inputMap->MapTrigger(PI_PrimaryActionX, touchScreen, CT_TOUCH_ABS_X);
  m_inputMap->MapTrigger(PI_PrimaryActionY, touchScreen, CT_TOUCH_ABS_Y);
#endif
}

// Read the mapped absolute touch position on mobile, mouse cursor pos on desktop
hkvVec2 PlayerUIDialog::GetCursorPosition(IVGUIContext const *context) const
{
#if defined(_VISION_MOBILE)
  float const touchX = m_inputMap->GetTrigger(PI_PrimaryActionX);
  float const touchY = m_inputMap->GetTrigger(PI_PrimaryActionY);
  return hkvVec2(touchX, touchY);
#else
  return context->GetCurrentMousePos();
#endif
}

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
