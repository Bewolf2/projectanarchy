/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiCachedFrameNode.hpp

#ifndef VIS_CACHED_FRAME_NODE_HPP_INCLUDED
#define VIS_CACHED_FRAME_NODE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimControl.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimSequence.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeletalAnimSequence.hpp>

// define smartpointer
class VisCachedFrameNode_cl;
typedef VSmartPtr<VisCachedFrameNode_cl> VisCachedFrameNodePtr;

/// \brief
///   Cached Frame Nodes represent one cached result that can be set by the user
/// 
/// One can set it by sampling a specified sequence with a time value or manually setting different
/// values.
class VisCachedFrameNode_cl : public IVisAnimResultGenerator_cl
{
public:

  /// \brief
  ///   Constructor of the cached frame node class
  /// 
  /// \param pSkeleton
  ///   pointer to the skeleton animation is played on
  VISION_APIFUNC VisCachedFrameNode_cl(const VisSkeleton_cl* pSkeleton);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisCachedFrameNode_cl();


  /// \brief
  ///   Set the animation state of the cached frame node.
  /// 
  /// All values of the passed state get copied to the local state.
  /// 
  /// \param pState
  ///   state to copy the data from
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetState(const VisAnimState_cl* pState);


  /// \brief
  ///   Sets a result on the cached framenode.
  /// 
  /// All values of the specified result get copied to the local result.
  /// 
  /// \param pResult
  ///   result to copy the data from
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetResult(const VisSkeletalAnimResult_cl* pResult);


  /// \brief
  ///   Overridden OnGetCurrentLocalSpaceResult function that updates the current skeletal result.
  /// 
  /// The state is cached until the next update.
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnGetCurrentLocalSpaceResult(void);


  /// \brief
  ///   Overridden OnGetLocalSpaceResult function that updates the specified result for a
  ///   past/future time.
  /// 
  /// \param pResult
  ///   result that gets filled
  /// 
  /// \param fDeltaTime
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnGetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fDeltaTime);


  /// \brief
  ///   Overridden OnUpdateState function that updates the skeletal state.
  /// 
  /// This function is called once per simulation frame.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC virtual bool OnUpdateState(float fTimeDelta);


  /// \brief
  ///   Overridden function that removes all input child components.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC virtual void RemoveAllInputs();


  /// \brief
  ///   Samples one frame for the specified time and animation sequence.
  /// 
  /// \param fTime
  ///   time to sample
  /// 
  /// \param pAnimSequence
  ///   animation sequence that gets sampled
  VISION_APIFUNC void SampleSingleAnimFrame(float fTime, const VisAnimSequence_cl* pAnimSequence);

protected:
  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisCachedFrameNode_cl, VISION_APIDATA );
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisCachedFrameNode_cl();

};

#endif //VIS_CACHED_FRAME_NODE_HPP_INCLUDED

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
