/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimLayerMixerNode.hpp

#ifndef VIS_ANIM_LAYER_MIXER_NODE_HPP_INCLUDED
#define VIS_ANIM_LAYER_MIXER_NODE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimMixerNode.hpp>

// define smartpointer
class VisAnimLayerMixerNode_cl;
typedef VSmartPtr<VisAnimLayerMixerNode_cl> VisAnimLayerMixerNodePtr;

/// \brief
///   Layer Animation mixer class derived from base mixer class
/// 
/// Overwrites OnUpdateState, OnGetCurrentLocalSpaceResult and OnGetLocalSpaceResult to mix the
/// input results together.
/// 
/// "Layered" means that the inputs with later indices override the earlier ones.
/// 
/// Added animations are weightened and added to the result of the animations that have been added
/// before and multiplied by (1-weight) result[n] = Anim[n]*w + result[n-1]*(1-w)
/// 
/// You can use this for example for overlaying upper body animation on a character.
class VisAnimLayerMixerNode_cl : public IVisAnimMixerNode_cl
{
public:

  /// \brief
  ///   Constructor of the layer mixer node
  /// 
  /// \param pSkeleton
  ///   skeleton animation is mixed on
  VISION_APIFUNC VisAnimLayerMixerNode_cl(const VisSkeleton_cl* pSkeleton);
  
  
  /// \brief
  ///   Overridden OnUpdateState function that mixes the states of the different inputs.
  /// 
  /// The EaseIn/EaseOut-Factor is also updated.
  /// 
  /// Only called from the engine once per simulation frame.
  /// 
  /// \param fTimeDelta
  ///   time past since last update
  /// 
  /// \return
  ///   bool bResult: true if update was successful.
  VISION_APIFUNC virtual bool OnUpdateState(float fTimeDelta);


  /// \brief
  ///   Overridden OnGetCurrentLocalSpaceResult function that mixes the current result of the
  ///   different inputs.
  /// 
  /// It updates the inputs on using GetCurrentLocalSpaceResult and adds them with
  /// AddSubLayerAnimResult to its cached result.
  /// 
  /// \return
  ///   bool bResult: true if successful.
  VISION_APIFUNC virtual bool OnGetCurrentLocalSpaceResult();

  /// \brief
  ///   If enabled, this option will check if there is an input without a bone weighting mask and a weight of 1.0.
  ///   If so, inputs that were added before this input will not influence the animation with their motion delta
  VISION_APIFUNC void SetEnableMotionDeltaSkipCheck(bool bEnable);

  /// \brief
  ///   If enabled, this option will check if there is an input without a bone weighting mask and a weight of 1.0.
  ///   If so, inputs that were added before this input will not influence the animation with their motion delta
  VISION_APIFUNC bool IsEnabledMotionDeltaSkipCheck();

  /// \brief
  ///   Overridden OnGetLocalSpaceResult function that mixes a past/future skeletal result of the
  ///   different inputs.
  /// 
  /// It updates the inputs using GetCurrentLocalSpaceResult and adds them with
  /// AddSubLayerAnimResult to the output result.
  /// 
  /// The result is not cached but written to the provided animation result parameter.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference to current state time
  /// 
  /// \return
  ///   bool bResult: True if successful.
  VISION_APIFUNC virtual bool OnGetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);

protected:
  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisAnimLayerMixerNode_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisAnimLayerMixerNode_cl();

  bool m_bCheckSkipMotionDelta;
};

#endif //VIS_ANIM_LAYER_MIXER_NODE_HPP_INCLUDED

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
