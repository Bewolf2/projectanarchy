/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TransitionCharacter.hpp

#ifndef TRANSITION_CHARACTER_HPP_INCLUDED
#define TRANSITION_CHARACTER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>

// Animation sequence IDs
#define ANIMID_IDLE               0
#define ANIMID_WALK               1
#define ANIMID_RUN                2
#define ANIMID_TURN               3
#define ANIMID_WALKBACKWARDS      4
#define MAX_ANIM_ID               ANIMID_WALKBACKWARDS + 1 
#define MAX_ANIM_CONTROL          2

// Predefinitions
class VisAnimNormalizeMixerNode_cl;

#ifndef _VISION_DOC

class TransitionCharacter_cl : public VisBaseEntity_cl
{
public:

  TransitionCharacter_cl();
  virtual ~TransitionCharacter_cl();

  // Overridden entity functions
  VOVERRIDE void InitFunction();
  VOVERRIDE void DeInitFunction();
  VOVERRIDE void ThinkFunction();
  VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  // Animation state manipulation functions
  void Stand();
  void Run();
  void Walk();
  void WalkBackwards(); 
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
  VisSkeletalAnimSequencePtr m_spSkeletalSequenceList[MAX_ANIM_ID];
 // VisSkeletalAnimControlPtr m_spSkeletalAnimControl[MAX_ANIM_CONTROL];

private:

  // Transition state machine
  VTransitionStateMachine *m_pStateMachine;

  // State helper for switching between the rotate state and other states
  bool m_bIsMoving;

public:
  V_DECLARE_SERIAL( TransitionCharacter_cl, )
  IMPLEMENT_OBJ_CLASS(TransitionCharacter_cl);
};

#endif // _VISION_DOC

#endif //TRANSITION_CHARACTER_HPP_INCLUDED

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
