/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimMixerNode.hpp

#ifndef VIS_ANIM_MIXER_NODE_HPP_INCLUDED
#define VIS_ANIM_MIXER_NODE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimResultGenerator.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimMixerNode.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimEventListener.hpp>

// different ease modes
enum EaseMode_e {EASE_NONE,EASE_LINEAR};

class IVisAnimMixerNode_cl;
class IVisAnimResultGenerator_cl;

/// \brief
///   mixer input class, used to store additional parameters for input
/// 
/// Every class that is derived from IVisAnimResultGenerator_cl can be used as input for other
/// mixers or Controls.
/// 
/// The input class includes data like a weight, per bone weighting and EaseIn/EaseOut members.
class VisAnimMixerInput_cl
{
public:
  friend class IVisAnimMixerNode_cl;        ///< mixer node has direct access to everything

  /// \brief
  ///   Constructor of the mixer input class
  /// 
  ///  It gets the pointer to the mixer node and the result generator to link them together.
  /// 
  /// \param pParentMixerNode
  ///   mixer the input is included
  /// 
  /// \param pAnimResultGenerator
  ///   pointer to result generator
  /// 
  /// \param fAnimWeight
  ///   initial animation weight
  VISION_APIFUNC VisAnimMixerInput_cl(IVisAnimMixerNode_cl* pParentMixerNode,IVisAnimResultGenerator_cl* pAnimResultGenerator, float fAnimWeight=0.f);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisAnimMixerInput_cl();


  /// \brief
  ///   Returns the current anim weight that is also influenced by the ease in and ease out
  ///   components.
  /// 
  /// \return
  ///   float fWeight: the current anim weight
  inline float GetCurrentEaseValue() const
  {
    return m_fAnimWeight;
  }


  /// \brief
  ///   Calculates the current animation weight according to the easein and easeout components.
  /// 
  /// \param fTimeDelta
  ///   time since last update
  VISION_APIFUNC void UpdateEaseInOutFactor(float fTimeDelta);

    
  /// \brief
  ///   Calculates the animation weight at a past/future time.
  /// 
  /// The timedelta is relative to the last state time +/- the specified time delta.
  /// 
  /// \param fTimeDelta
  ///   relative +/- time to current state time
  /// 
  /// \return
  ///   float fWeight: resulting weight
  VISION_APIFUNC float GetFuturePastEaseInOutFactor(float fTimeDelta);


  /// \brief
  ///   Removes the easein component.
  VISION_APIFUNC void RemoveEaseIn();


  /// \brief
  ///   Removes the easeout component.
  VISION_APIFUNC void RemoveEaseOut();

    
  /// \brief
  ///   Initializes a new EaseIn component with new values.
  /// 
  /// To start EaseIn from the current value you have to set it to true. Otherwise the EaseIn will
  /// start with an animation weight of 0.
  /// 
  /// This function removes the EaseOut component.
  /// 
  /// \param fEndTimeDelta
  ///   time delta to finish
  /// 
  /// \param bConsiderCurrent
  ///   consider current value
  VISION_APIFUNC void EaseIn(float fEndTimeDelta, bool bConsiderCurrent);


  /// \brief
  ///   Initializes the EaseOut component with new values.
  /// 
  /// To start EaseOut from the current value you have to set it to true. Otherwise the EaseOut
  /// will start with an animation weight of 1.0.
  /// 
  /// This function removes the EaseIn component.
  /// 
  /// \param fEndTimeDelta
  ///   time delta to finish
  /// 
  /// \param bConsiderCurrent
  ///   consider current value
  VISION_APIFUNC void EaseOut(float fEndTimeDelta, bool bConsiderCurrent);


  /// \brief
  ///   Initializes a new EaseIn with some parameters.
  /// 
  /// You can start and end the Ease somewhere in the future.
  /// 
  /// Since it doesn't remove the EaseOut automatically you have to take care for overlappings and
  /// resulting artefacts on your own.
  /// 
  /// \param eEaseMode
  ///   describes blending mode
  /// 
  /// \param fStartTimeDelta
  ///   time delta to start (blending starts delayed after the EaseIn has been set)
  /// 
  /// \param fEndTimeDelta
  ///   time delta to finish (duration of the blending)
  /// 
  /// \param fStartValue
  ///   weight to start from [0..1]
  /// 
  /// \param EndValue
  ///   weight to finish with [0..1]
  VISION_APIFUNC void SetEaseIn(EaseMode_e eEaseMode, float fStartTimeDelta, float fEndTimeDelta, float fStartValue, float EndValue);


  /// \brief
  ///   Initializes a new EaseOut with some parameters.
  /// 
  /// You can start and end the Ease somewhere in the future.
  /// 
  /// Since it doesn't remove the EaseIn automatically you have to take care for overlappings and
  /// resulting artefacts on your own.
  /// 
  /// \param eEaseMode
  ///   describes blending mode
  /// 
  /// \param fStartTimeDelta
  ///   time delta to start (blending starts delayed after the EaseIn has been set)
  /// 
  /// \param fEndTimeDelta
  ///   time delta to finish (duration of the blending)
  /// 
  /// \param fStartValue
  ///   weight to start from [0..1]
  /// 
  /// \param fEndValue
  ///   weight to finish with [0..1]
  VISION_APIFUNC void SetEaseOut(EaseMode_e eEaseMode, float fStartTimeDelta, float fEndTimeDelta, float fStartValue, float fEndValue);


  /// \brief
  ///   Indicates whether easing-in is currently active
  /// 
  /// \return
  ///   bool bStatus: true if easing-in is currently active
  inline bool IsEasingIn() const 
  {
    return ((m_fEaseInStartTime!=-1.f)&&(m_fEaseInEndTime!=-1.f)&&(m_fEaseInStartValue!=-1.f)&&(m_fEaseInEndValue!=-1.f));
  }


  /// \brief
  ///   Indicates whether easing-out is currently active
  /// 
  /// \return
  ///   bool bStatus: true if easing-out is currently active
  inline bool IsEasingOut() const 
  {
    return ((m_fEaseOutStartTime!=-1.f)&&(m_fEaseOutEndTime!=-1.f)&&(m_fEaseOutStartValue!=-1.f)&&(m_fEaseOutEndValue!=-1.f));
  }

public:

  float m_fAnimWeight;                                ///< factor for blending whole AnimResult
  float m_fLocalTime;
  float* m_pPerBoneWeightingMask;                     ///< used for optional per bone weighting
  //IVisAnimResultGenerator_cl* m_pAnimResultGenerator; // pointer to real result generator
  IVisAnimResultGeneratorPtr m_spAnimResultGenerator;   ///< smart pointer to the real result generator

  // easeIn/easeOut stuff
  float m_fEaseInStartTime, m_fEaseInEndTime;
  float m_fEaseInStartValue, m_fEaseInEndValue;
  float m_fEaseOutStartTime, m_fEaseOutEndTime;
  float m_fEaseOutStartValue, m_fEaseOutEndValue;

  EaseMode_e m_eEaseMode;
  
protected:
  
  //Helper for UpdateEaseInOutFactor, GetFuturePastEaseInOutFactor
  VISION_APIFUNC float CalcEaseInOutValue(float fTime, bool bUpdate);
  
  // serialization
  VISION_APIFUNC VisAnimMixerInput_cl();
  V_DECLARE_SERIALX( VisAnimMixerInput_cl, VISION_APIFUNC );
  VISION_APIFUNC void SerializeX( VArchive &ar );
  void CommonInit();
};


/// \brief
///   Base animation mixer node with standard handling for mixing skeletal results
/// 
/// Derive a subclass from it to use its input handling and EaseIn/EaseOut functionality and
/// overwrite OnUpdateState, OnGetCurrentLocalSpaceResult, OnGetLocalSpaceResult to get your own
/// mixing behavior.
/// 
/// This class derives from VisAnimEventListener_cl. Here the animation mixer node does not 
/// invoke animation events itself but forwards them from its child nodes, either other mixer
/// nodes or an animation controls.
/// 
/// \note
///   Please note that currently only the pre-defined mixer nodes are supported:
///   VisAnimLayerMixerNode_cl and VisAnimNormalizeMixerNode_cl. In order to
///   register your own mixer node as event listener you will need to overwrite 
///   RegisterEventListener() which then should also cover your custom mixer class.
class IVisAnimMixerNode_cl : public IVisAnimResultGenerator_cl, public VisAnimEventListener_cl
{
public:

  /// \brief
  ///   Constructor of the anim mixer class
  /// 
  /// \param pSkeleton
  ///   skeleton attached to this node
  VISION_APIFUNC IVisAnimMixerNode_cl(const VisSkeleton_cl* pSkeleton);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~IVisAnimMixerNode_cl();


  /// \brief
  ///   Virtual function that gets called whenever a message is sent to this object
  VISION_APIFUNC virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);


  /// \brief
  ///   Specifies whether this mixer node should forward all animation events from its child nodes to its registered event listeners.
  /// 
  /// The mixer does not invoke animation events itself but simply forwards them from its child nodes,
  /// either other mixer nodes or animation controls. Enabling the event forwarding on a mixer node 
  /// will process all events from its underlying child nodes. This is disabled by default. 
  /// 
  /// \param bEnable
  ///   TRUE if the mixer node should forward all the animation events to its registered event listeners
  VISION_APIFUNC virtual void SetAnimEventsForwarding(bool bEnable);


  /// \brief
  ///   Returns the number of inputs on this node.
  /// 
  /// \return
  ///   int iCount: inputs count
  inline int GetInputCount() const
  {
    return m_AnimMixerInputList.GetLength();
  }


  /// \brief
  ///   Adds a result generator as an input to this mixer.
  /// 
  /// For that it creates an extra mixer input internally.
  /// 
  /// A reason for failing is that the skeletons of the mixer and the input result generator
  /// differ.
  /// 
  /// \param pAnimResultGenerator
  ///   result generator to add to mixer
  /// 
  /// \param fAnimWeight
  ///   initial weight of input
  /// 
  /// \return
  ///   int iIndex: index of the added input, or -1 if failed
  VISION_APIFUNC virtual int AddMixerInput(IVisAnimResultGenerator_cl* pAnimResultGenerator, float fAnimWeight = 1.f);


  /// \brief
  ///   Replaces a result generator at specified index with new one.
  /// 
  /// The weighting values and EasingIn/EasingOut are maintained and have to be reset manually.
  /// 
  /// \param iInputNumber
  ///   position in list
  /// 
  /// \param pAnimResultGenerator
  ///   result generator to set as mixer input
  /// 
  /// \param fAnimWeight
  ///   initial weight of input
  /// 
  /// \return
  ///   bool bResult: True if replacement was successful
  VISION_APIFUNC virtual bool SetMixerInput(int iInputNumber, IVisAnimResultGenerator_cl* pAnimResultGenerator, float fAnimWeight = 1.f);


  /// \brief
  ///   Switches a result generator at specified index with another index. This does only make sense if you use the layer mixer.
  /// 
  /// \param iInputNumber1
  ///   position in list that you want to switch with iInputNumber2
  /// 
  /// \param iInputNumber2
  ///   position in list that you want to switch with iInputNumber1
  /// 
  /// \return
  ///   bool bResult: True if switch was successful, false if one of the passed inputs is invalid
  VISION_APIFUNC virtual bool SwitchMixerInputs(int iInputNumber1, int iInputNumber2);

  
  /// \brief
  ///   Returns the mixer input at specified index.
  /// 
  /// The index is returned by the AddMixerInput function.
  /// 
  /// \param iInputNumber
  ///   Input index. Must be in valid range
  /// 
  /// \return
  ///   IVisAnimResultGenerator_cl* pGenerator: pointer to result generator 
  ///   or NULL if the mixer input at this index has been removed allready
  inline IVisAnimResultGenerator_cl* GetMixerInput(int iInputNumber) const
  {
    if (m_AnimMixerInputList.Get(iInputNumber) == NULL)
      return NULL;

    return (IVisAnimResultGenerator_cl*)((VisAnimMixerInput_cl*)m_AnimMixerInputList.Get(iInputNumber))->m_spAnimResultGenerator;
  }


  /// \brief
  ///   Removes a mixer input at specified list index.
  /// 
  /// Since we don't really remove the internal input component but just the link to the result
  /// generator, the indices of the other inputs do not change.
  /// 
  /// \param iInputNumber
  ///   position in list
  VISION_APIFUNC virtual void RemoveMixerInput(int iInputNumber);


  /// \brief
  ///   Removes all inputs from mixer.
  VISION_APIFUNC void RemoveAllInputs();


  /// \brief
  ///   Sets the weight of an input directly
  /// 
  /// To get a smooth change use the Ease functions.
  /// 
  /// \param iInputNumber
  ///   number of the input
  /// 
  /// \param fInputWeight
  ///   input weight
  VISION_APIFUNC virtual void SetInputWeight(int iInputNumber, float fInputWeight);


  /// \brief
  ///   Sets the weight to -1 so that input has no more effect on result.
  /// 
  /// \param iInputNumber
  ///   index of the input to change
  VISION_APIFUNC virtual void RemoveInputWeight(int iInputNumber);


  /// \brief
  ///   Sets a per-bone weighting mask on an input.
  /// 
  /// The input object creates an internal copy of the passed array.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \param iBoneWeightCount
  ///   number of weights
  /// 
  /// \param pPerBoneWeightingMask
  ///   list with per bone weights. Gets copied.
  /// 
  /// \return
  ///   bool bResult: true if setting was successful
  VISION_APIFUNC virtual bool ApplyPerBoneWeightingMask(int iInputNumber, int iBoneWeightCount, const float* pPerBoneWeightingMask);


  /// \brief
  ///   Deletes the per-bone weighting mask for one input.
  /// 
  /// \param iInputNumber
  ///   index of the input
  VISION_APIFUNC void RemovePerBoneWeightingMask(int iInputNumber);


  /// \brief
  ///   Gets the per-bone weighting mask for one input.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// \return
  ///   Pointers to the bone weight array, or NULL if the passed input number is invalid
  VISION_APIFUNC float* GetPerBoneWeightingMask(int iInputNumber) const;


  /// \brief
  ///   Returns the current input weight.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::GetCurrentEaseValue for the specified input
  /// index.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \return
  ///   float fWeight: current input weight
  VISION_APIFUNC float GetCurrentEaseValue(int iInputNumber) const;


  /// \brief
  ///   Calculates a future/past easeInOut value.
  /// 
  /// The timedelta is relative to the last state time +/- the specified time delta.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \param fTimeDelta
  ///   relative +/- time to current state time
  /// 
  /// \return
  ///   float fWeight: resulting weight
  inline float GetFuturePastEaseInOutFactor(int iInputNumber, float fTimeDelta) const
  {
    VASSERT(m_AnimMixerInputList.Get(iInputNumber)); 
    return ((VisAnimMixerInput_cl*)m_AnimMixerInputList.Get(iInputNumber))->GetFuturePastEaseInOutFactor(fTimeDelta);
  }


  /// \brief
  ///   Removes the EaseIn component.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::RemoveEaseIn for the specified index.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \return
  ///   bool bResult: True if removed
  VISION_APIFUNC bool RemoveEaseIn(int iInputNumber);


  /// \brief
  ///   Removes the EaseOut component.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::RemoveEaseOut for the specified index.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \return
  ///   bool bResult: True if removed
  VISION_APIFUNC bool RemoveEaseOut(int iInputNumber);


  /// \brief
  ///   Initializes an EaseIn component.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::EaseIn for the specified index.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \param fEndTimeDelta
  ///   time delta to end Ease
  /// 
  /// \param bConsiderCurrent
  ///   start from current weight
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC virtual bool EaseIn(int iInputNumber, float fEndTimeDelta, bool bConsiderCurrent);


  /// \brief
  ///   Initializes EaseOut component.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::EaseOut for the specified index.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \param fEndTimeDelta
  ///   time delta to end Ease
  /// 
  /// \param bConsiderCurrent
  ///   start from current weight
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC virtual bool EaseOut(int iInputNumber, float fEndTimeDelta, bool bConsiderCurrent);


  /// \brief
  ///   Initializes a new EaseIn component with new values.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::SetEaseIn for the specified index.
  /// 
  /// You can start and end the Ease somewhere in the future.
  /// 
  /// Since it doesn't remove the EaseOut automatically you have to take care for overlappings and
  /// resulting artefacts on your own.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \param eEaseMode
  ///   describes blending mode
  /// 
  /// \param fStartTimeDelta
  ///   time delta to start (blending starts delayed after the EaseIn has been set)
  /// 
  /// \param fEndTimeDelta
  ///   time delta to finish (duration of the blending)
  /// 
  /// \param fStartValue
  ///   weight to start from [0..1]
  /// 
  /// \param fEndValue
  ///   weight to finish with [0..1]
  VISION_APIFUNC virtual void SetEaseIn(int iInputNumber, EaseMode_e eEaseMode, float fStartTimeDelta, float fEndTimeDelta, float fStartValue, float fEndValue);


  /// \brief
  ///   Initializes the EaseOut component with new values
  /// 
  /// This function wraps around VisAnimMixerInput_cl::SetEaseOut for the specified index.
  /// 
  /// You can start and end the Ease somewhere in the future.
  /// 
  /// Since it doesn't remove the EaseIn automatically you have to take care for overlappings and
  /// resulting artefacts on your own.
  /// 
  /// \param iInputNumber
  ///   index of the input
  /// 
  /// \param eEaseMode
  ///   describes blending mode
  /// 
  /// \param fStartTimeDelta
  ///   time delta to start (blending starts delayed after the EaseIn has been set)
  /// 
  /// \param fEndTimeDelta
  ///   time delta to finish (duration of the blending)
  /// 
  /// \param fStartValue
  ///   weight to start from [0..1]
  /// 
  /// \param fEndValue
  ///   weight to finish with [0..1]
  VISION_APIFUNC virtual void SetEaseOut(int iInputNumber, EaseMode_e eEaseMode, float fStartTimeDelta, float fEndTimeDelta, float fStartValue, float fEndValue);
  

  /// \brief
  ///   Indicates whether easing-in is currently active in the specified input.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::IsEasingIn for the specified index.
  /// 
  /// \param iInputNumber
  ///   specifies which mixer input to test.
  /// 
  /// \return
  ///   bool bEasing: indicates whether easing-in is currently active
  VISION_APIFUNC bool IsEasingIn(int iInputNumber) const {VASSERT(m_AnimMixerInputList.Get(iInputNumber)); return ((VisAnimMixerInput_cl*)m_AnimMixerInputList.Get(iInputNumber))->IsEasingIn();}


  /// \brief
  ///   Indicates whether easing-out is currently active in the specified input.
  /// 
  /// This function wraps around VisAnimMixerInput_cl::IsEasingIn for the specified index.
  /// 
  /// \param iInputNumber
  ///   specifies which mixer input to test.
  /// 
  /// \return
  ///   bool bEasing: indicates whether easing-out is currently active
  VISION_APIFUNC bool IsEasingOut(int iInputNumber) const {VASSERT(m_AnimMixerInputList.Get(iInputNumber)); return ((VisAnimMixerInput_cl*)m_AnimMixerInputList.Get(iInputNumber))->IsEasingOut();}


  /// \brief
  ///   Tests all weighting mask values for validity. For debugging purposes.
  /// 
  /// Each float element has to be within the range [0..1].
  /// 
  /// \param iBoneWeightCount
  ///   number of elements
  /// 
  /// \param pPerBoneWeightingMask
  ///   array of bone weights to test
  /// 
  /// \return
  ///   bool bResult: True if all weights are valid
  static VISION_APIFUNC bool TestPerBoneWeightingMaskValid(int iBoneWeightCount, const float* pPerBoneWeightingMask);


  /// \brief
  ///   Pure virtual function to update the state. Called by the animation system.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnUpdateState(float fTimeDelta) = 0;


  /// \brief
  ///   Pure virtual function to evaluate the current local space result.
  /// 
  /// \return
  ///   bool bResult: True if update was successful
  VISION_APIFUNC virtual bool OnGetCurrentLocalSpaceResult(void) = 0;


  /// \brief
  ///   Pure virtual function to evaluate the future/past local space result.
  /// 
  /// \param pResult
  ///   result that gets filled
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnGetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta) = 0;

  /// \brief
  ///   Resets the motion delta
  VISION_APIFUNC virtual void ResetMotionDelta();

protected:

  // serialization
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC IVisAnimMixerNode_cl();

  // event listener
  VISION_APIFUNC void RegisterEventListener(IVisAnimResultGenerator_cl* pAnimResultGenerator);

  VPList m_AnimMixerInputList;   ///< list to hold all input
};

#endif //VIS_ANIM_MIXER_NODE_HPP_INCLUDED

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
