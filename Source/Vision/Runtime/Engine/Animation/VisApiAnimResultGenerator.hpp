/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimResultGenerator.hpp

#ifndef VIS_ANIM_RESULT_GENERATOR_HPP_INCLUDED
#define VIS_ANIM_RESULT_GENERATOR_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiSkeletalAnimResult.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimState.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeleton.hpp>

// predefines
class IVisAnimResultGenerator_cl;
typedef VSmartPtr<IVisAnimResultGenerator_cl> IVisAnimResultGeneratorPtr;

/// \brief
///   Result generator class, parent class for all nodes of the animation tree
/// 
/// Every class that is derived from VisAnimResultGenerator can be used to build up an animation
/// tree.
/// 
/// This class keeps a reference to the skeleton of the entity where the animation is played on and
/// also a local copy of the last updated result.
/// 
/// Every subclass has to implement OnUpdateState, OnGetCurrentLocalSpaceResult and
/// OnGetLocalSpaceResult to update the state and result.
class IVisAnimResultGenerator_cl : public VisTypedEngineObject_cl, public VRefCounter
{
public:
    
  /// \brief
  ///   Constructor of the result generator class
  /// 
  /// It gets a pointer to the skeleton of the entity the animations are played on.
  /// 
  /// \param pSkeleton
  ///   entity skeleton
  VISION_APIFUNC IVisAnimResultGenerator_cl(const VisSkeleton_cl* pSkeleton);

    
  /// \brief
  ///   Destructor
  VISION_APIFUNC ~IVisAnimResultGenerator_cl();


  /// \brief
  ///   Returns the last frame the state has been updated.
  /// 
  /// Compare this value against VisGame_cl::GetUpdateSceneCount.
  /// 
  /// \return
  ///   unsigned int iFrame: frame number
  unsigned int GetLastUpdateSceneCount() const
  {
    return m_iLastFrameUpdatedSkeletalState;
  }


  /// \brief
  ///   Static function that provides the current skinning matrices result of a skeletal result.
  /// 
  /// In contrast to the world result the skinning matrices are already multiplied by the inverse
  /// of the bones initial pose world matrices.
  /// 
  /// \param pSkinningResult
  ///   filled with skinning matrices
  /// 
  /// \param pWorldResultPose
  ///   keeps world matrices of current pose
  VISION_APIFUNC static void CalculateSkinningAnimResult(VisSkeletalAnimResult_cl* pSkinningResult, const VisSkeletalAnimResult_cl* pWorldResultPose);


  /// \brief
  ///   Static function that provides the current world matrices result of a skeletal result.
  /// 
  /// In addition, an optional matrix can be passed to re-position and re-orient all matrices
  /// relatively to.
  /// 
  /// \param pWorldMatrix
  ///   optional matrix to apply to each matrix. Can for instance be the entity 4x4 transformation
  ///   matrix
  /// 
  /// \param pWorldResultPose
  ///   instance the receives the result. If pWorldMatrix is NULL, then this will be the local
  ///   result
  /// 
  /// \param pLocalResultPose
  ///   source pose
  VISION_APIFUNC static void CalculateObjectAnimResult(const hkvMat4* pWorldMatrix, VisSkeletalAnimResult_cl* pWorldResultPose, const VisSkeletalAnimResult_cl* pLocalResultPose);// const;


  /// \brief
  ///   Pure virtual function that removes all child input components.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC virtual void RemoveAllInputs() = 0;


  /// \brief
  ///   Returns the skeleton instance the animation is played on.
  /// 
  /// \return
  ///   VisSkeleton_cl *pSkeleton: pointer to skeleton
  inline VisSkeleton_cl* GetSkeleton() const
  {
    return m_LocalAnimResult.GetSkeleton();
  }


  /// \brief
  ///   Called once per frame by the engine to update the state. Calls OnUpdateState and caches the
  ///   result.
  /// 
  /// \param fTimeDelta
  ///   time past since last state update.
  /// 
  /// \return
  ///   VisAnimState_cl* pState: pointer to updated state
  VISION_APIFUNC VisAnimState_cl* UpdateState(float fTimeDelta);


  /// \brief
  ///   Returns a pointer to the current state. Does not perform an update
  /// 
  /// \return
  ///   VisAnimState_cl* pState: pointer to current state
  inline const VisAnimState_cl* GetCurrentState() const
  {
    return &m_LocalAnimState;
  }


  /// \brief
  ///   Pure virtual overridable that performs the update on this generator.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update successful
  VISION_APIFUNC virtual bool OnUpdateState(float fTimeDelta) = 0;


  /// \brief
  ///   Updates the current result by calling OnGetCurrentLocalSpaceResult and caches the result.
  /// 
  /// \return
  ///   VisAnimState_cl* pResult: pointer to updated state
  VISION_APIFUNC VisSkeletalAnimResult_cl* GetCurrentLocalSpaceResult();


  /// \brief
  ///   Pure virtual overridable that performs everything related to getting the current local
  ///   space result.
  /// 
  /// \return
  ///   bool bResult: True if update was successful
  VISION_APIFUNC virtual bool OnGetCurrentLocalSpaceResult() = 0;


  /// \brief
  ///   GetLocalSpaceResult updates the result via OnGetLocalSpaceResult with state time +/- time
  ///   delta.
  /// 
  /// \param pResult
  ///   gets filled with updated result.
  /// 
  /// \param fTimeDelta
  ///   time past since last state update.
  /// 
  /// \return
  ///   VisAnimState_cl* pState: pointer to updated state
  VISION_APIFUNC bool GetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


  /// \brief
  ///   Pure virtual overridable that performs everything related to getting the local space result
  ///   relatively to the current time.
  /// 
  /// It uses the time of the stateupdate +/- the specified timedelta and fills the result
  /// specified in the parameters (no buffering).
  /// 
  /// \param pResult
  ///   result that gets filled
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update successful
  VISION_APIFUNC virtual bool OnGetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta) = 0;


  /// \brief
  ///   Calls the GetCurrentLocalSpaceResult function and transforms the result to object space.
  /// 
  /// The result of GetCurrentLocalSpaceResult is cached but the transformation is performed
  /// in-place.
  /// 
  /// \param pMatrix
  ///   Optional Matrix to transform all bones with. Can for instance be an entity's transformation
  ///   matrix.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC bool GetCurrentObjectSpaceResult(hkvMat4* pMatrix, VisSkeletalAnimResult_cl* pResult);


  /// \brief
  ///   Returns a past/future skeletal object space result.
  /// 
  /// It calls GetLocalSpaceResult and transforms the result to object space.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// \param pMatrix
  ///   Optional Matrix to transform all bones relative to.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference to current state time.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC bool GetObjectSpaceResult(hkvMat4* pMatrix, VisSkeletalAnimResult_cl* pResult, float fTimeDelta);

  /// \brief
  ///   Resets the motion delta
  VISION_APIFUNC virtual void ResetMotionDelta();

protected:
  // serialization
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC IVisAnimResultGenerator_cl();

  VisSkeletonPtr m_spSkeleton;
  unsigned int m_iLastFrameUpdatedSkeletalState;
  bool m_bCachedLocalResult;
  VisSkeletalAnimResult_cl m_LocalAnimResult;   ///< used to keep buffered pAnimResult
  VisAnimState_cl m_LocalAnimState;             ///< used to keep buffered pAnimState
};

#endif //VIS_ANIM_RESULT_GENERATOR_HPP_INCLUDED

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
