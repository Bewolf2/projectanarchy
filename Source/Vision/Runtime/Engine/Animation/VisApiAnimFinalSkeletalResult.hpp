/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimFinalSkeletalResult.hpp

#ifndef VIS_ANIM_FINAL_RESULT_INCLUDED
#define VIS_ANIM_FINAL_RESULT_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimFinalSkeletalResult.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeletalAnimResult.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimResultGenerator.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiModel.hpp>

// custom bone flags
#define VIS_REPLACE_BONE    1
#define VIS_MODIFY_BONE     2
#define VIS_LOCAL_SPACE     4
#define VIS_OBJECT_SPACE    8
#define VIS_SKINNING_SPACE  16

// predefines
class VisAnimFinalSkeletalResult_cl;
typedef VSmartPtr<VisAnimFinalSkeletalResult_cl>  VisAnimFinalSkeletalResultPtr;

/// \brief
///   Class that keeps the already remapped final skeletal result in local, object and skinning
///   space.
/// 
/// The final result updates the local result from the result generators, it performs a remapping
/// if the animation was mixed on a skeleton different from the config and finally converts the
/// final local result to the object and skinning result space.
/// 
/// Querying any of the specific results causes it to be calculated and cached until the next
/// update.
/// 
/// If Configs share the same final result they have synchronously animated skeletons but the
/// vertex animations on top can be different.
/// 
/// The final result class also supports functionality to modify bone orientations.
class VisAnimFinalSkeletalResult_cl : public VRefCounter, public VisTypedEngineObject_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the final result class.
  /// 
  /// \param pSkeleton
  ///   destination skeleton result is remapped on.
  VISION_APIFUNC VisAnimFinalSkeletalResult_cl(const VisSkeleton_cl* pSkeleton);

  
  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisAnimFinalSkeletalResult_cl();


  ///
  /// @}
  ///

  ///
  /// @name Get/update Results
  /// @{
  ///

  /// \brief
  ///   Returns the pointer to the result instance.
  /// 
  /// This function does not perform any update. To guarantee that the result is updated, use the
  /// GetCurrentLocalSpaceResult, GetCurrentObjectSpaceResult or GetCurrentSkinningSpaceResult
  /// function.
  ///
  /// \param iFlags
  ///   Define space to get cached result
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to buffered result.
  VISION_APIFUNC const VisSkeletalAnimResult_cl* GetCachedResultBySpace(int iFlags = VIS_OBJECT_SPACE) const;


  /// \brief
  ///   Returns the pointer to the skinning result instance.
  /// 
  /// This function does not perform any update. To guarantee that the result is updated, use the
  /// GetCurrentLocalSpaceResult, GetCurrentObjectSpaceResult or GetCurrentSkinningSpaceResult
  /// function.
  /// same result with GetCachedResultBySpace(VIS_SKINNING_SPACE)
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to buffered skinning result.
  inline const VisSkeletalAnimResult_cl* GetSkinningResultPointer() const 
  {
    return &m_SkinningSkeletalAnimResult;
  }


  /// \brief
  ///   Returns the pointer to the skeleton
  /// same result with GetCachedResultBySpace(VIS_LOCAL_SPACE).GetSkeleton()
  /// \return
  ///   VisSkeleton_cl* pSkeleton : The skeleton instance
  inline VisSkeleton_cl* GetSkeleton() const
  {
    return m_LocalSkeletalAnimResult.GetSkeleton();
  }


  /// \brief
  ///   Returns the number of the last frame the final result state has been updated.
  /// 
  /// Compare this value against VisGame_cl::GetUpdateSceneCount.
  /// 
  /// \return
  ///   unsigned int iFrame : last updated frame.
  unsigned int GetLastUpdateSceneCount() const
  {
    return m_iLastFrameUpdatedSkeletalState;
  }


  /// \brief
  ///   Sets the root node of the result generator tree that is attached to this final result.
  /// 
  /// A final result always references exactly one result generator. The result generator root in
  /// turn can have an arbitrary hierarchy of child generators.
  /// 
  /// \param pInput
  ///   root of result generator tree
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void SetSkeletalAnimInput(IVisAnimResultGenerator_cl* pInput);


  /// \brief
  ///   Returns a pointer to the root node of the skeletal result generator tree.
  /// 
  /// \return
  ///   IVisAnimResultGenerator_cl*: root of result generator tree
  inline IVisAnimResultGenerator_cl* GetSkeletalAnimInput() const
  {
    return m_spSkeletalAnimRoot;
  }


  /// \brief
  ///   Updates the state of the final result.
  /// 
  /// Only called from the engine once per simulation frame.
  /// 
  /// Resets the custom bone transformations.
  /// 
  /// \param fTimeDelta
  ///   time past since last state update
  /// 
  /// \return
  ///   VisAnimState_cl* pState: pointer to updated state.
  VISION_APIFUNC const VisAnimState_cl* UpdateState(float fTimeDelta);


  /// \brief
  ///   Returns the current skeletal local space result.
  /// 
  /// It updates the local result and remaps it if the destination skeleton differs.
  /// 
  /// The result is cached until the next update.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to the current skeletal local space result.
  VISION_APIFUNC const VisSkeletalAnimResult_cl* GetCurrentLocalSpaceResult();


  /// \brief
  ///   This version evaluates a past/future version of the local space result.
  /// 
  /// It thus bypasses any caching of the result.
  /// 
  /// It updates the local result and remaps it if the destination skeleton differs.
  /// 
  /// If there are any custom local bones they replace the bones in the result.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference relative to the current state time
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void GetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


  /// \brief
  ///   Returns the current skeletal object space result.
  /// 
  /// It calls GetCurrentLocalSpaceResult and transforms the result to object space. Any custom
  /// object bones replace the bones in the result.
  /// 
  /// The result is cached until the next update.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to the current skeletal object space result.
  VISION_APIFUNC const VisSkeletalAnimResult_cl* GetCurrentObjectSpaceResult();


  /// \brief
  ///   This version evaluates a past/future version of the object space result.
  /// 
  /// It thus bypasses any caching of the result.
  /// 
  /// It calls GetLocalSpaceResult and transforms the result to object space.
  /// 
  /// If there are any custom object bones they replace the bones in the result.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference to current state time.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void GetObjectSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


  /// \brief
  ///   Returns the current skeletal skinning space result.
  /// 
  /// It calls GetCurrentObjectSpaceResult and transforms the result to skinning space.
  /// 
  /// If there are any custom skinning bones they replace the bones in the result.
  /// 
  /// The result is cached until the next update.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to the current skeletal skinning space result.
  VISION_APIFUNC const VisSkeletalAnimResult_cl* GetCurrentSkinningSpaceResult();


  /// \brief
  ///   Returns the skinning space result pointer without updating and buffering it.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to skinning space result.
  inline const VisSkeletalAnimResult_cl* GetSkinningSpaceResultPointer() const
  {
    return &m_SkinningSkeletalAnimResult;
  }


  /// \brief
  ///   This version evaluates a past/future version of the skinning space result.
  /// 
  /// It thus bypasses any caching of the result.
  /// 
  /// It calls GetObjectSpaceResult and transforms the result to object space.
  /// 
  /// If there are any custom skinning bones they replace the bones in the result.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference to current state time.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void GetSkinningSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


  /// \brief
  ///   Modifies the translation of a single bone
  /// 
  /// The flags specify whether the translation should replace or modify the bone:
  /// VIS_REPLACE_BONE, VIS_MODIFY_BONE.
  /// 
  /// Furthermore, the flags specify whether the translation is used in local, object or skinning
  /// space: VIS_LOCAL_SPACE, VIS_OBJECT_SPACE, VIS_SKINNING_SPACE
  /// 
  /// Any previous translation set via this function is discarded regardless of the mode and space.
  /// 
  /// \param iBoneIndex
  ///   Index of the bone result to replace.
  /// 
  /// \param Translation
  ///   New translation.
  /// 
  /// \param iFlags
  ///   Define space and method to apply. See Description
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void SetCustomBoneTranslation(int iBoneIndex, const hkvVec3& Translation, int iFlags = (VIS_REPLACE_BONE|VIS_OBJECT_SPACE));

  /// \brief
  ///   Returns the translation of a single bone
  /// 
  /// \param iBoneIndex
  ///   Index of the bone result to retrieve.
  /// 
  /// \param Translation
  ///   Bone's translation.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void GetCustomBoneTranslation(int iBoneIndex, hkvVec3& Translation);


  /// \brief
  ///   Resets the translation of a single bone
  /// 
  /// \param iBoneIndex
  ///   Index of the bone result to replace.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void ResetCustomBoneTranslation(int iBoneIndex);

  /// \brief
  ///   Modifies the rotation of a single bone
  /// 
  /// The flags specify whether the rotation should replace or modify the bone: VIS_REPLACE_BONE,
  /// VIS_MODIFY_BONE.
  /// 
  /// Furthermore, the flags specify whether the rotation is used in local, object or skinning
  /// space: VIS_LOCAL_SPACE, VIS_OBJECT_SPACE, VIS_SKINNING_SPACE.
  /// 
  /// Any previous rotation set via this function is discarded regardless of the mode and space.
  /// 
  /// \param iBoneIndex
  ///   Index of the bone result to replace.
  /// 
  /// \param Rotation
  ///   New rotation.
  /// 
  /// \param iFlags
  ///   Define space and method to apply. See Description
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void SetCustomBoneRotation(int iBoneIndex,  const hkvQuat &Rotation, int iFlags = (VIS_REPLACE_BONE|VIS_OBJECT_SPACE));

  /// \brief
  ///   Returns the rotation of a single bone
  /// 
  /// \param iBoneIndex
  ///   Index of the bone result to retrieve.
  /// 
  /// \param Rotation
  ///   Bone's rotation.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void GetCustomBoneRotation(int iBoneIndex, hkvQuat &Rotation);

  /// \brief
  ///   Resets the rotation of a single bone
  /// 
  /// \param iBoneIndex
  ///   Index of the bone result to replace.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void ResetCustomBoneRotation(int iBoneIndex);

  /// \brief
  ///   Modifies the scaling of a single bone
  /// 
  /// The flags specify whether the scaling should replace or modify the bone: VIS_REPLACE_BONE,
  /// VIS_MODIFY_BONE.
  /// 
  /// Furthermore, the flags specify whether the scaling is used in local, object or skinning
  /// space: VIS_LOCAL_SPACE, VIS_OBJECT_SPACE, VIS_SKINNING_SPACE.
  /// 
  /// Any previous scaling set via this function is discarded regardless of the mode and space.
  ///
  /// \note
  ///   The new scaling value influences the referenced bone only. You may thus have to modify/scale the translations
  ///   of the child bones for getting a useful result.
  /// 
  /// \param iBoneIndex
  ///   Index of the bone result to replace.
  /// 
  /// \param Scaling
  ///   New scaling.
  /// 
  /// \param iFlags
  ///   Define space and method to apply. See Description
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void SetCustomBoneScaling(int iBoneIndex,  const hkvVec3& Scaling, int iFlags = (VIS_REPLACE_BONE|VIS_OBJECT_SPACE));

  /// \brief
  ///   Modifies the scaling of a single bone
  ///
  /// \param iBoneIndex
  ///   Index of the bone result to replace.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void ResetCustomBoneScaling(int iBoneIndex);

  /// \brief
  ///   Frees the memory used by all custom bones transformations.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void FreeCustomBones();

  /// \brief
  ///   Returns whether custom bone transformations are currently applied or not.
  /// 
  /// \return
  ///   bStatus: true if custom bone transformations are used.
  inline bool HasCustomBoneTransformation() const
  {
    return bHasCustomLocalSpaceBones || bHasCustomObjectSpaceBones || bHasCustomSkinningSpaceBones;
  }

  /// \brief
  ///   Fills all bones that have no animation data with initial pose data.
  /// 
  /// \param pResult
  ///   result that gets filled.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC static void FillMissingBonesWithInitialPose(VisSkeletalAnimResult_cl* pResult);
  

  /// \brief
  ///   Copies the initial pose into the passed result.
  /// 
  /// \param pResult
  ///   result that gets filled.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC static void OverwriteResultWithInitialPose(VisSkeletalAnimResult_cl* pResult);


  /// \brief
  ///   Indicates whether the local result is currently cached.
  /// 
  /// \return
  ///   bool bStatus: true if local result is cached.
  inline bool IsLocalResultCached() const
  {
    return m_bCachedLocalResult;
  }
  
  
  /// \brief
  ///   Indicates whether the object result is currently cached.
  /// 
  /// \return
  ///   bool bStatus: true if object result is cached.
  inline bool IsObjectResultCached() const
  {
    return m_bCachedObjectResult;
  }
  
  
  /// \brief
  ///   Indicates whether the skinning result is currently cached.
  /// 
  /// \return
  ///   bool bStatus: true if skinning result is cached.
  inline bool IsSkinningResultCached() const
  {
    return m_bCachedSkinningResult;
  }

  // serialization
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);


  /// \brief
  ///   Sets whether custom bones will be reset during UpdateState
  /// 
  /// The default is false. Before 6.5 the bones were always reset
  /// 
  /// \param bReset
  ///   whether custom bones will be reset during UpdateState
  /// 
  /// \return
  ///   Nothing.
  inline void SetResetCustomBones(bool bReset)
  {
    m_bResetCustomBones = bReset;
  }

  /// \brief
  ///   Indicates whether custom bones will be reset during each update
  /// 
  /// \return
  ///   whether custom bones will be reset during each update
  inline bool GetResetCustomBones() const
  {
    return m_bResetCustomBones;
  }

  /// \brief
  ///   Resets the cached local space, object space and skinning space results
  ///
  /// The animation system automatically takes care of resetting the cached results once a frame. Calling
  /// this method additionally may cause the animation system to compute the result two times a frame, which
  /// costs additional performance. It is thus not recommended to call this method, unless you want to 
  /// track down problems in our animation code.
  VISION_APIFUNC void ResetResults();

  /// \brief
  ///   Immediately resets the currently stored custom bone transformation
  ///
  /// It is not recommend to call this method (only for debugging purposes). Use SetResetCustomBones instead.
  VISION_APIFUNC void ResetCustomBones();

  /// \brief
  ///   Resets the motion delta
  VISION_APIFUNC void ResetMotionDelta();

protected:

  /// \brief
  ///   Apply the custom bones. Internal function, do not use.
  /// 
  /// \param pResult
  ///   output destination
  /// 
  /// \param iSpace
  ///   Space to use
  void ApplyCustomBones(VisSkeletalAnimResult_cl* pResult, int iSpace);

  /// \brief
  ///   Initializes member variables
  void CommonInit();


  unsigned int m_iLastFrameUpdatedSkeletalState;          ///< last frame state has been updated
  VisSkeletonPtr m_spSkeleton;                            ///< destination skeleton result is remapped on

  // buffering
  bool m_bCachedLocalResult;                              ///< flag that indicates whether the local space result is up to date
  bool m_bCachedObjectResult;                             ///< flag that indicates whether the object space result is up to date
  bool m_bCachedSkinningResult;                           ///< flag that indicates whether the skinning space result is up to date
  VisSkeletalAnimResult_cl m_LocalSkeletalAnimResult;     ///< Cached results in local space
  VisSkeletalAnimResult_cl m_ObjectSkeletalAnimResult;    ///< Cached results in object space
  VisSkeletalAnimResult_cl m_SkinningSkeletalAnimResult;  ///< Cached results in skinning space
  VisSkeletalAnimResult_cl* m_pLocalSkeletalAnimResult;   ///< we have one pointer for optimization reasons. If there are no custom bones and no remapping we use the local result of the result generator directly.

  bool  m_bResetCustomBones;                              ///<whether custom bones will be reset during each update
  // optional custom transformations
  bool bHasCustomLocalSpaceBones;
  bool bHasCustomObjectSpaceBones;
  bool bHasCustomSkinningSpaceBones;
  int* m_iCustomFlagsTranslation;
  int* m_iCustomFlagsRotation;
  int* m_iCustomFlagsScaling;
  hkvVec4* m_CustomBoneTranslation;
  hkvQuat* m_CustomBoneRotation;
  hkvVec4* m_CustomBoneScaling;

  bool m_bHasValidState;
  VisAnimState_cl m_SkeletalAnimState;              ///< buffered anim state
  VisAnimState_cl* m_pSkeletalAnimState;            ///< pointer to anim state

  // pointer to skeletal anim tree
  IVisAnimResultGeneratorPtr m_spSkeletalAnimRoot;
  VisSkeletonRemappingPtr m_spSkeletonRemapping;    ///< has to be recreated when root changes!

  /// \brief
  ///   Sets the flag for the corresponding space and resets its buffered results.
  /// 
  /// \param iFlags
  ///   specifies the space.
  void SetFlagsForUsedSpace(int iFlags);

  void InvalidateState( );


  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisAnimFinalSkeletalResult_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisAnimFinalSkeletalResult_cl();
  void SetSkeleton(VisSkeleton_cl* pSkeleton);
  ///
  /// @}
  ///

};

#endif // VIS_ANIM_FINAL_RESULT_INCLUDED

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
