/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TransitionCharacter.hpp

#ifndef TRANSITION_BARBARIAN_HPP_INCLUDED
#define TRANSITION_BARBARIAN_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>

// Animation sequence IDs
#define ANIMID_IDLE               0
#define ANIMID_WALK               1
#define ANIMID_RUN                2
#define MAX_ANIM_ID               ANIMID_RUN + 1 
#define MAX_ANIM_CONTROL          2

// Predefinitions
class VisAnimNormalizeMixerNode_cl;
class vHavokCharacterController;

class TransitionBarbarian_cl : public VisBaseEntity_cl
{
public:

  TransitionBarbarian_cl();
  virtual ~TransitionBarbarian_cl();

  // Overridden entity functions
  VOVERRIDE void InitFunction();
  VOVERRIDE void DeInitFunction();
  VOVERRIDE void ThinkFunction();
  VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  // Animation state manipulation functions
  void Stand();
  void Run();
  void Walk();
  void RotateLeft(); 
  void RotateRight();

  // Access transition state machine for GUI
  VTransitionStateMachine* GetStateMachine();

protected:

  // Helper functions
  void DoRotateLeft();
  void DoRotateRight();
  void SetupAnimations();
  bool m_bModelValid;

  // Animation system members
  VisSkeletalAnimSequence_cl* m_pSkeletalSequenceList[MAX_ANIM_ID];
  VisSkeletalAnimControl_cl* m_pSkeletalAnimControl[MAX_ANIM_CONTROL];

private:

  vHavokCharacterController *m_pPhys;

  // Transition state machine
  VTransitionStateMachine *m_pStateMachine;

public:
  V_DECLARE_SERIAL( TransitionBarbarian_cl, )
  IMPLEMENT_OBJ_CLASS( TransitionBarbarian_cl );
};

#endif // TRANSITION_BARBARIAN_HPP_INCLUDED

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
