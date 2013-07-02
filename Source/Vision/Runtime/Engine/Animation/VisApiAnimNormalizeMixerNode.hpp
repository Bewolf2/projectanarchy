/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimNormalizeMixerNode.hpp

#ifndef VIS_ANIM_NORMALIZE_MIXER_NODE_HPP_INCLUDED
#define VIS_ANIM_NORMALIZE_MIXER_NODE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimMixerNode.hpp>

// define smartpointer
class VisAnimNormalizeMixerNode_cl;
typedef VSmartPtr<VisAnimNormalizeMixerNode_cl> VisAnimNormalizeMixerNodePtr;

/// \brief
///   Derived from base mixer class that implements normalized input mixing
/// 
/// Implements OnUpdateState, OnGetCurrentLocalSpaceResult and OnGetLocalSpaceResult to mix the
/// input results and always normalizes all result weightings to 1.
class VisAnimNormalizeMixerNode_cl : public IVisAnimMixerNode_cl
{

public:
    
  /// \brief
  ///   Constructor of the normalize mixer node
  /// 
  /// \param pSkeleton
  ///   skeleton animation is mixed on
  VISION_APIFUNC VisAnimNormalizeMixerNode_cl(const VisSkeleton_cl* pSkeleton);


  /// \brief
  ///   Overridden OnUpdateState function that mixes the states of the different inputs.
  /// 
  /// It adds the inputs using AddAnimState and normalizes the final result with the inverse sum of
  /// all weights.
  /// 
  /// The EaseIn/EaseOut-Factor is also updated.
  /// 
  /// Only called by the engine once per simulation frame.
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
  /// It updates the inputs using GetCurrentLocalSpaceResult, adds them with AddSubLayerAnimResult
  /// to its buffered result and normalizes it.
  /// 
  /// \return
  ///   bool bResult: true if successful.
  VISION_APIFUNC virtual bool OnGetCurrentLocalSpaceResult();


  /// \brief
  ///   Overridden OnGetLocalSpaceResult function that mixes a past/future skeletal result of the
  ///   different inputs.
  /// 
  /// It updates the inputs using GetLocalSpaceResult, adds them with AddSubLayerAnimResult to its
  /// buffered result and normalizes it.
  /// 
  /// The result is not kept in the buffered result but written to the passed result reference.
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
  ///   bool bResult: true if successful.
  VISION_APIFUNC bool OnGetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


protected:
  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisAnimNormalizeMixerNode_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisAnimNormalizeMixerNode_cl();
};

#endif //VIS_ANIM_NORMALIZE_MIXER_NODE_HPP_INCLUDED

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
