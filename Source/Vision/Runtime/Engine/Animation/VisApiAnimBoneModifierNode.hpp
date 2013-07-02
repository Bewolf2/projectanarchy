/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimBoneModifierNode.hpp

#ifndef VIS_ANIM_LOCALBONE_MODIFIER_NODE_HPP_INCLUDED
#define VIS_ANIM_LOCALBONE_MODIFIER_NODE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimResultGenerator.hpp>

// predefines
class VisAnimBoneModifierNode_cl;
typedef VSmartPtr<VisAnimBoneModifierNode_cl>  VisAnimBoneModifierNodePtr;

/// \brief
///   Result generator node that allows you to replace or modify the local space bones
/// 
/// You can specify which animation component (translation, rotation) you want to replace or
/// modify.
/// 
/// \note
///   This node can only change the local space results of the bone. Use the final result for
///   changing the global or skinning space bones.
/// 
/// \note
///   The custom data is NOT reset after the state update and you have to remove the custom flag
///   from  every bone that should not get modified in the next frame. 
/// 
/// \note
///   Modify the final result directly if you want to apply a temporary change.
/// 
/// \note
///   Since the current result can only be updated once a frame and caches the result you have to
///   set the custom bones before the update to see the result in the same frame.
class VisAnimBoneModifierNode_cl : public IVisAnimResultGenerator_cl
{
public:

  /// \brief
  ///   Constructor of the local bone modifier node.
  /// 
  /// \param pSkeleton
  ///   skeleton used to create result
  VISION_APIFUNC VisAnimBoneModifierNode_cl(const VisSkeleton_cl* pSkeleton);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisAnimBoneModifierNode_cl();


  /// \brief
  ///   Replaces the result generator that is used for input.
  /// 
  /// \param pAnimResultGenerator
  ///   result generator used for input
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetModifierInput(IVisAnimResultGenerator_cl* pAnimResultGenerator);

  /// \brief
  ///   Returns the result generator that is used as input
  inline IVisAnimResultGenerator_cl *GetModifierInput() const 
  { 
    return m_spResultGeneratorInput; 
  }

  /// \brief
  ///   Removes all inputs.
  VISION_APIFUNC void RemoveAllInputs();


  /// \brief
  ///   Updates the state of the local bone modifier node.
  /// 
  /// It resets the custom bones, so the user has to set the cutom bones every frame.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \return
  ///   bool bResult: true if update successful
  VISION_APIFUNC bool OnUpdateState(float fTimeDelta);


  /// \brief
  ///   Returns the current skeletal local space result.
  /// 
  /// It updates the local result of the input and modifies them with the custom bones.
  /// 
  /// Current means that we use the time calculated in the state update.
  /// 
  /// \return
  ///   bool bResult: true if successful.
  VISION_APIFUNC bool OnGetCurrentLocalSpaceResult(void);


  /// \brief
  ///   Updates a past/future skeletal local space result.
  /// 
  /// If there are any custom local bones they replace the bones in the result.
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


  /// \brief
  ///   Frees the memory used by all custom bones transformations.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void FreeCustomBones(void);


  /// \brief
  ///   Sets a custom bone translation of a single bone.
  /// 
  /// A flag can be specified that defines whether the translation replaces resp. modifies the
  /// bone.
  /// 
  /// Since this node is inside the graph it will always affect the LOCAL SPACE bone.
  /// 
  /// A previously set translation for the specified bone is overwritten regardless which
  /// replace/modify methods it used.
  /// 
  /// \param iBoneIndex
  ///   index of the bone result to replace.
  /// 
  /// \param Translation
  ///   new translation.
  /// 
  /// \param iFlags
  ///   defines the method to apply: VIS_REPLACE_BONE or VIS_MODIFY_BONE (mutually exclusive).
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void SetCustomBoneTranslation(int iBoneIndex, const hkvVec3& Translation, int iFlags = VIS_REPLACE_BONE);

  /// \brief
  ///   Sets a custom bone rotation of a single bone.
  /// 
  /// A flag can be specified that defines whether the rotation replaces resp. modifies the bone.
  /// 
  /// Since this node is inside the graph it will always affect the LOCAL SPACE bone.
  /// 
  /// A previously set rotation for the specified bone is overwritten regardless which
  /// replace/modify methods it used.
  /// 
  /// \param iBoneIndex
  ///   index of the bone result to replace.
  /// 
  /// \param Rotation
  ///   new rotation.
  /// 
  /// \param iFlags
  ///   defines the method to apply: VIS_REPLACE_BONE or VIS_MODIFY_BONE (mutually exclusive).
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void SetCustomBoneRotation(int iBoneIndex, const hkvQuat &Rotation, int iFlags = VIS_REPLACE_BONE);

  /// \brief
  ///   Sets a custom bone scaling of a single bone.
  /// 
  /// A flag can be specified that defines whether the scaling replaces resp. modifies the bone.
  /// 
  /// Since this node is inside the graph it will always affect the LOCAL SPACE bone.
  /// 
  /// A previously set scaling for the specified bone is overwritten regardless which
  /// replace/modify methods it used.
  /// 
  /// \param iBoneIndex
  ///   index of the bone result to replace.
  /// 
  /// \param Scaling
  ///   new scaling.
  /// 
  /// \param iFlags
  ///   defines the method to apply: VIS_REPLACE_BONE or VIS_MODIFY_BONE (mutually exclusive).
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void SetCustomBoneScaling(int iBoneIndex, const hkvVec3& Scaling, int iFlags);

  /// \brief
  ///   Resets the motion delta
  VISION_APIFUNC void ResetMotionDelta();

  /// \brief
  ///   Removes the custom bone flag from the specified bone.
  /// 
  /// In the next frame this bone does not get modified or replaced anymore.
  /// 
  /// \param iIndex
  ///   Bone for which to remove the custom bone flag.
  inline void RemoveCustomBoneFlags(int iIndex) {if(m_iCustomFlagsTranslation) m_iCustomFlagsTranslation[iIndex] = 0;
                                                 if(m_iCustomFlagsRotation) m_iCustomFlagsRotation[iIndex] = 0;}

protected:
  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisAnimBoneModifierNode_cl, VISION_APIDATA );
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisAnimBoneModifierNode_cl();
  void CommonInit();

  // optional custom transformations, in difference to the final result we only have local space bones!
  bool bHasCustomBones;
  int* m_iCustomFlagsTranslation;
  int* m_iCustomFlagsRotation;
  int* m_iCustomFlagsScaling;
  hkvVec4* m_CustomBoneTranslation;
  hkvQuat* m_CustomBoneRotation;
  hkvVec4* m_CustomBoneScaling;

  IVisAnimResultGeneratorPtr m_spResultGeneratorInput;  ///< keeps a reference on the input result generator


  /// \brief
  ///   Apply all custom bone replacements or modifications.
  void ApplyCustomBones(VisSkeletalAnimResult_cl* pResult);
};

#endif //VIS_ANIM_BONE_MODIFIER_NODE_HPP_INCLUDED

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
