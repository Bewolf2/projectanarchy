/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file AnimatedWarrior.hpp

#ifndef ANIMATED_CHARACTER_HPP_INCLUDED
#define ANIMATED_CHARACTER_HPP_INCLUDED

#ifndef _VISION_PSP2
#define ANIMATEDCHARACTER_ROTATIONSPEED 180.0f
#else
#define ANIMATEDCHARACTER_ROTATIONSPEED 70.0f
#endif

#ifndef _VISION_DOC

// animation sequence IDs
#define ANIMID_IDLE                  0
#define ANIMID_WALK                  1
#define ANIMID_RUN                   2
#define ANIMID_TURN                  3
#define ANIMID_WALKBACKWARDS         4
#define ANIMID_WALKBACKWARDS_ARMED   5
#define ANIMID_IDLE_TORCH            6
#define ANIMID_WALK_TORCH            7
#define ANIMID_RUN_TORCH             8
#define ANIMID_TURN_TORCH            9
#define ANIMID_WALKBACKWARDS_TORCH  10
#define MAX_FULLBODY_ANIMID         ANIMID_WALKBACKWARDS_TORCH

#define ANIMID_UPPERBODY_ARM        11
#define ANIMID_UPPERBODY_DISARM     12
#define ANIMID_UPPERBODY_ATTACK     13
#define MAX_UPPERBODY_ANIMID        ANIMID_UPPERBODY_ATTACK

#define MAX_ANIMID MAX_UPPERBODY_ANIMID + 1

#define FULLBODY_CONTROLCOUNT       4
#define UPPERBODY_CONTROLCOUNT      2


class AttachedDagger_cl; class AttachedTorch_cl; class VisAnimLayerMixerNode_cl; class VisAnimNormalizeMixerNode_cl;

// define smartpointer
class FullBodyControl_cl;
typedef VSmartPtr<FullBodyControl_cl> FullBodyControlPtr;

class FullBodyControl_cl: public VisSkeletalAnimControl_cl
{
public:
  int m_iMixerInputIndex;
  
  FullBodyControl_cl(VisSkeleton_cl* pSkeleton)
  :VisSkeletalAnimControl_cl(pSkeleton, VSKELANIMCTRL_DEFAULTS|VANIMCTRL_LOOP)
  {m_iMixerInputIndex=-1;}

};


class AnimatedWarrior_cl : public VisBaseEntity_cl
{
public:
  // type for full body states
  enum FBSTATETYPE
  {
    FBSTATETYPE_NORMAL = 0,
    FBSTATETYPE_ARMED = 1
  };

  // State base class
  class State
  {
  public:
    State( const char *szName, AnimatedWarrior_cl *pEntity ) 
    {
      m_pEnt = pEntity; m_szName = szName; 
    }
    virtual ~State()
    {
      //char sdf = 0;
    }
    const char *GetName() { return m_szName; }

  protected:
    AnimatedWarrior_cl *m_pEnt;
    const char *m_szName;
  };

  // full body state base class
  class FullBodyState : public State
  {
  public:
    FullBodyState( const char *szName, AnimatedWarrior_cl *pEntity ) : State( szName, pEntity ) 
    {
      VASSERT(szName&&pEntity);
    }
    virtual ~FullBodyState()
    {
      //char sdf = 0;
    }

    virtual void OnEnter() {}
    virtual void OnEvent( int iEvent ) {}
    virtual void OnThink() {}

    // actions
    virtual bool Stop() { return false; }
    virtual bool Walk() { return false; }
    virtual bool RotateLeft(float fRotationSpeed = 1.0f) { DoRotateLeft(fRotationSpeed * ANIMATEDCHARACTER_ROTATIONSPEED); return true; }
    virtual bool RotateRight(float fRotationSpeed = 1.0f) { DoRotateRight(fRotationSpeed * ANIMATEDCHARACTER_ROTATIONSPEED); return true; }
    virtual bool Run() { return false; }
    virtual bool WalkBackwards() { return false; }
    virtual bool Arm() { return false; }
    virtual bool DisArm() { return false; }

  protected:
    // helper functions
    void DoRotateLeft(float fRotationSpeed);
    void DoRotateRight(float fRotationSpeed);
    bool IsBlending();
  };

  // upper body base state
  class UpperBodyState : public State
  {
  public:
    UpperBodyState( const char *szName, AnimatedWarrior_cl *pEntity ) : State( szName, pEntity ) 
    {
    }

    virtual void OnEnter() {}
    virtual void OnEvent( int iEvent ) {}
    virtual void OnThink() {}

    // actions
    virtual bool Arm() { return false; }
    virtual bool DisArm() { return false; }
    virtual bool Attack() { return false; }
    bool IsBlending();
  };


public:
  AnimatedWarrior_cl();
  virtual ~AnimatedWarrior_cl();

  // overridden entity functions
  virtual void InitFunction() HKV_OVERRIDE;
  virtual void DeInitFunction() HKV_OVERRIDE;
  
  void SetEnabled(bool bEnabled);

  inline bool IsEnabled()
  {
    return m_bEnabled;
  }
  
  void BlendOverFullBodyAnimation(float fBlendTime, int iNewSeqId, float fNewSpeed, float fNewPosition);
  
  void BlendOverUpperBodyAnimation(float fBlendTime, int iNewSeqId, float fNewSpeed, float fNewPosition);
  void BlendOutUpperBodyAnimation(float fBlendTime);

  virtual void ThinkFunction() HKV_OVERRIDE;
  virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  // full body state manipulation functions
  bool Walk() { return m_pCurrentFullBodyState->Walk(); }
  bool Stand() { return m_pCurrentFullBodyState->Stop(); }
  bool RotateLeft(float fRotationSpeed) { return m_pCurrentFullBodyState->RotateLeft(fRotationSpeed); }
  bool RotateRight(float fRotationSpeed) { return m_pCurrentFullBodyState->RotateRight(fRotationSpeed); }
  bool Run() { return m_pCurrentFullBodyState->Run(); }
  bool WalkBackwards() { return m_pCurrentFullBodyState->WalkBackwards(); }

  // full body state information functions
  bool IsInIdleState() { return m_pCurrentFullBodyState == m_pFullBodyIdleState[0]; }
  bool IsInWalkState() { return m_pCurrentFullBodyState == m_pFullBodyWalkState[0]; }
  bool IsInRotateState() { return m_pCurrentFullBodyState == m_pFullBodyRotateState[0]; }
  bool IsInRotateArmedState() { return m_pCurrentFullBodyState == m_pFullBodyRotateState[1]; }

  // upper body state manipulation
  bool Arm() { return m_pCurrentUpperBodyState->Arm(); }
  bool DisArm() { return m_pCurrentUpperBodyState->DisArm(); }

  // upper body state information
  bool IsInUpperBodyIdleState() { return m_pCurrentUpperBodyState == m_pUpperBodyIdleState; }
  bool IsInUpperBodyIdleArmedState() { return m_pCurrentUpperBodyState == m_pUpperBodyIdleArmedState; }
  bool IsInUpperBodyDrawTorchState() { return m_pCurrentUpperBodyState == m_pUpperBodyDrawTorchState; }
  bool IsInUpperBodyHolsterTorchState() { return m_pCurrentUpperBodyState == m_pUpperBodyHolsterTorchState; }

  FullBodyState *GetFullBodyState() { return m_pCurrentFullBodyState; }
  UpperBodyState *GetUpperBodyState() { return m_pCurrentUpperBodyState; }

#if defined USE_HAVOK
  class vHavokCharacterController* m_pCharacterController;
#endif

protected:
  void SetupAnimations();
  

  // lookup table for animation IDs
  struct FullBodyAnimIDLookup
  {
    int iIdleID;
    int iWalkID;
    int iRunID;
    int iTurnID;
    int iWalkBackwardsID;
  };

  // full body state: Idle
  class FullBodyIdleState : public FullBodyState
  {
  public:
    FullBodyIdleState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType );
    virtual void OnEnter() HKV_OVERRIDE;
    virtual bool Stop() HKV_OVERRIDE;
    virtual bool Walk() HKV_OVERRIDE;
    virtual bool Run() HKV_OVERRIDE;
    virtual bool Arm() HKV_OVERRIDE;
    virtual bool WalkBackwards() HKV_OVERRIDE;
    virtual bool DisArm() HKV_OVERRIDE;
    virtual bool RotateLeft(float fRotationSpeed = 1.0f) HKV_OVERRIDE;
    virtual bool RotateRight(float fRotationSpeed = 1.0f) HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
protected:
    enum TRIGGEREDACTION { ACTION_NONE, ACTION_WALK, ACTION_RUN, ACTION_WALKBACKWARDS};
    TRIGGEREDACTION m_eTriggeredAction;
    FBSTATETYPE m_eStateType;
    FullBodyAnimIDLookup *m_pAnimSet;
  };

  // full body state: Rotate
  class FullBodyRotateState : public FullBodyState
  {
    enum RotateDir { ROTATE_NONE, ROTATE_LEFT, ROTATE_RIGHT };
  public:
    FullBodyRotateState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType );
    virtual void OnEnter() HKV_OVERRIDE;
    virtual bool Walk() HKV_OVERRIDE;
    virtual bool Run() HKV_OVERRIDE;
    virtual bool WalkBackwards() HKV_OVERRIDE;
    virtual bool Stop() HKV_OVERRIDE;
    virtual bool RotateLeft(float fRotationSpeed = 1.0f) HKV_OVERRIDE;
    virtual bool RotateRight(float fRotationSpeed = 1.0f) HKV_OVERRIDE;
    virtual bool Arm() HKV_OVERRIDE;
    virtual bool DisArm() HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
  protected:
    enum TRIGGEREDACTION { ACTION_NONE, ACTION_STOP, ACTION_WALK, ACTION_RUN, ACTION_WALKBACKWARDS };
    TRIGGEREDACTION m_eTriggeredAction;
    RotateDir m_rotateDir;
    bool m_bAnimStartPending;
    FBSTATETYPE m_eStateType;
    FullBodyAnimIDLookup *m_pAnimSet;
  };

  // full body state: Walk
  class FullBodyWalkState: public FullBodyState
  {
  public:
    FullBodyWalkState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType );
    virtual void OnEnter() HKV_OVERRIDE;
    virtual bool Stop() HKV_OVERRIDE;
    virtual bool Run() HKV_OVERRIDE;
    virtual bool WalkBackwards() HKV_OVERRIDE;
    virtual bool Arm() HKV_OVERRIDE;
    virtual bool DisArm() HKV_OVERRIDE;
    virtual void OnEvent( int iEvent ) HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
  protected:
    enum TRIGGEREDACTION { ACTION_NONE, ACTION_STOP, ACTION_RUN, ACTION_WALKBACKWARDS };
    TRIGGEREDACTION m_eTriggeredAction;
    FBSTATETYPE m_eStateType;
    FullBodyAnimIDLookup *m_pAnimSet;
  };

  // full body state: Run
  class FullBodyRunState: public FullBodyState
  {
  public:
    FullBodyRunState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType  );
    virtual void OnEnter() HKV_OVERRIDE;
    virtual bool Stop() HKV_OVERRIDE;
    virtual bool Walk() HKV_OVERRIDE;
    virtual bool WalkBackwards() HKV_OVERRIDE;
    virtual bool Arm() HKV_OVERRIDE;
    virtual bool DisArm() HKV_OVERRIDE;
    virtual void OnEvent(int iEvent) HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
  protected:
    enum TRIGGEREDACTION { ACTION_NONE, ACTION_STOP, ACTION_WALK, ACTION_WALKBACKWARDS};
    TRIGGEREDACTION m_eTriggeredAction;
    FBSTATETYPE m_eStateType;
    FullBodyAnimIDLookup *m_pAnimSet;
  };

  // full body state: Walk Backwards
  class FullBodyWalkBackwardsState: public FullBodyState
  {
  public:
    FullBodyWalkBackwardsState( AnimatedWarrior_cl *pEntity, const char *szName, FBSTATETYPE eStateType  );
    virtual void OnEnter() HKV_OVERRIDE;
    virtual bool Stop() HKV_OVERRIDE;
    virtual bool Walk() HKV_OVERRIDE;
    virtual bool Run() HKV_OVERRIDE;
    virtual bool Arm() HKV_OVERRIDE;
    virtual bool DisArm() HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
  protected:
    enum TRIGGEREDACTION { ACTION_NONE, ACTION_STOP, ACTION_WALK, ACTION_RUN};
    TRIGGEREDACTION m_eTriggeredAction;
    FBSTATETYPE m_eStateType;
    FullBodyAnimIDLookup *m_pAnimSet;
  };

  // Upper Body State: Idle
  class UpperBodyIdleState : public UpperBodyState
  {
  public:
    UpperBodyIdleState( AnimatedWarrior_cl *pEntity ) : UpperBodyState("Idle", pEntity) {}
    virtual bool Arm() HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
  private:
    enum TRIGGEREDACTION{ACTION_NONE, ACTION_DRAW_TORCH};
    TRIGGEREDACTION m_eTriggeredAction;
  };

  class UpperBodyIdleArmedState : public UpperBodyState
  {
  public:
    UpperBodyIdleArmedState( AnimatedWarrior_cl *pEntity ) : UpperBodyState("Idle", pEntity) {}
    virtual bool DisArm() HKV_OVERRIDE;
    virtual bool Attack() HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
  private:
    enum TRIGGEREDACTION{ACTION_NONE, ACTION_HOLSTER_TORCH, ACTION_ATTACK};
    TRIGGEREDACTION m_eTriggeredAction;
  };


  // Upper Body State: Draw Torch
  class UpperBodyDrawTorchState : public UpperBodyState
  {
  public:
    UpperBodyDrawTorchState(AnimatedWarrior_cl *pEntity) : UpperBodyState("DrawTorch", pEntity) {}
    virtual void OnEnter() HKV_OVERRIDE;
    virtual void OnEvent(int iEvent) HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
    bool IdleArmed();
    virtual bool DisArm() HKV_OVERRIDE;
  private:
    enum TRIGGEREDACTION{ACTION_NONE, ACTION_IDLE_ARMED, ACTION_HOLSTER_TORCH};
    TRIGGEREDACTION m_eTriggeredAction;
  };


  // Upper Body State: Holster Torch
  class UpperBodyHolsterTorchState : public UpperBodyState
  {
  public:
    UpperBodyHolsterTorchState(AnimatedWarrior_cl *pEntity) : UpperBodyState("HolsterTorch", pEntity) {}
    virtual void OnEnter() HKV_OVERRIDE;
    virtual void OnEvent(int iEvent) HKV_OVERRIDE;
    virtual void OnThink() HKV_OVERRIDE;
    bool Idle();
    virtual bool Arm() HKV_OVERRIDE;
  private:
    enum TRIGGEREDACTION{ACTION_NONE, ACTION_IDLE, ACTION_DRAW_TORCH};
    TRIGGEREDACTION m_eTriggeredAction;
  };


  // Upper Body State: Attacking
  class UpperBodyAttackingState : public UpperBodyState
  {
  public:
    UpperBodyAttackingState( AnimatedWarrior_cl *pEntity ) : UpperBodyState("Attacking", pEntity) {}
    virtual void OnEnter() HKV_OVERRIDE;
    virtual void OnEvent(int iEvent) HKV_OVERRIDE;
  private:
    enum TRIGGEREDACTION{ACTION_NONE, ACTION_IDLE_ARMED};
    TRIGGEREDACTION m_eTriggeredAction;
  };

  // possible fade states of the upper body
  enum UpperBodyFadeState
  {
    FADESTATE_NONE,
    FADESTATE_FADEIN,
    FADESTATE_FADEOUT
  };

  // head rotation handling functions
  void TurnHead( float fDegreesPerSecond, float fTimeDiff, float fMinAngle, float fMaxAngle );
  void RelaxHead( float fTimeDiff );

  // state handling related functions
  void SetFullBodyState(FullBodyState *pNewState)
  { 
    m_pCurrentFullBodyState = pNewState;
    m_pCurrentFullBodyState->OnEnter(); 
  }
  void SetUpperBodyState( UpperBodyState *pNewState ) { m_pCurrentUpperBodyState = pNewState; m_pCurrentUpperBodyState->OnEnter(); }

  // upper body fade-in handling
  void StartUpperBodyFadeIn();
  void StartUpperBodyFadeOut();
  void UpdateUpperBodyFade();

  // Torch related functions
  void ShowTorch();
  void HideTorch();

  // helper functions
  void TriggerEvent(int iEvent);
  FullBodyAnimIDLookup *CreateAnimIDLookupForStateType(FBSTATETYPE eStateType);
  FullBodyControl_cl* GetFreeFullBodyControl(void);
  void RemoveInactiveFullBodyControls(void);

  // protected variables
  bool m_bEnabled;
  bool m_bModelValid;

  FullBodyState *m_pCurrentFullBodyState;
  UpperBodyState *m_pCurrentUpperBodyState;
  UpperBodyFadeState m_eUpperBodyFadeState;

  bool m_bHeadInMovement;
  int m_iHeadBoneIndex;
  float m_fHeadRotationAngles[3];
  AttachedTorch_cl *m_pTorch;

  // animation system members
  VSmartPtr<VisAnimConfig_cl> m_spAnimConfig; // keep reference on anim config
  VisSkeletalAnimSequencePtr m_spSkeletalSequenceList[MAX_ANIMID];
  
  int m_iPrimaryUpperBodyControlIndex;
  VisSkeletalAnimControlPtr m_spUpperBodyControls[UPPERBODY_CONTROLCOUNT];
  FullBodyControlPtr m_spPrimaryFullBodyControl;
  VRefCountedCollection<FullBodyControl_cl> m_FullBodyControlList;

  // available full body states
  FullBodyState *m_pFullBodyIdleState[2];
  FullBodyState *m_pFullBodyWalkState[2];
  FullBodyState *m_pFullBodyRunState[2];
  FullBodyState *m_pFullBodyRotateState[2];
  FullBodyState *m_pFullBodyWalkBackwardsState[2];

  // available upper body states
  UpperBodyIdleState *m_pUpperBodyIdleState;
  UpperBodyIdleArmedState *m_pUpperBodyIdleArmedState;
  UpperBodyDrawTorchState *m_pUpperBodyDrawTorchState;
  UpperBodyHolsterTorchState *m_pUpperBodyHolsterTorchState;
  UpperBodyState *m_pUpperBodyAttackingState;

  // full body animation ID sets
  FullBodyAnimIDLookup *m_pFullBodyAnimIDLookup[2];
public:
  VisAnimLayerMixerNode_cl* m_pLayerMixer;
  VisAnimNormalizeMixerNode_cl* m_pNormalizeMixer;

  V_DECLARE_SERIAL( AnimatedWarrior_cl, )
  IMPLEMENT_OBJ_CLASS(AnimatedWarrior_cl);
};

#endif // _VISION_DOC


#endif //ANIMATED_CHARACTER_HPP_INCLUDED

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
