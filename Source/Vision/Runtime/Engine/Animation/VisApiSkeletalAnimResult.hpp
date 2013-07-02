/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSkeletalAnimResult.hpp

#ifndef VIS_SKELETAL_ANIM_RESULT_HPP_INCLUDED
#define VIS_SKELETAL_ANIM_RESULT_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimResult.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeleton.hpp>

// predefines
class hkvQuat;
class VisSkeletalAnimResult_cl;
typedef VSmartPtr<VisSkeletalAnimResult_cl>  VisSkeletalAnimResultPtr;


#ifdef _VISION_PS3
#pragma diag_push
#pragma diag_suppress=1011
#endif

#if defined(__ghs__)
#pragma ghs nowarning 997
#endif



/// \brief
///   Skeletal animation result class derived from base animation result
/// 
/// Every type of animation also has a different animation result. The anim result class is a base
/// class which includes result that are needed for all animation result types.
/// 
/// This subclass has additional translation and rotation for each bone.
class VisSkeletalAnimResult_cl: public VisAnimResult_cl
{
public:

  /// \brief
  ///   Constructor of the skeletal anim result class.
  /// 
  /// \param pSkeleton
  ///   The corresponding skeleton.
  VISION_APIFUNC VisSkeletalAnimResult_cl(const VisSkeleton_cl* pSkeleton);

    
  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisSkeletalAnimResult_cl();


  /// \brief
  ///   Clones skeletal animation result.
  /// 
  /// Creates new skeletal animation result instance, duplicates data and returns pointer.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pSkeletalAnimResult: pointer to cloned result
  VISION_APIFUNC VisSkeletalAnimResult_cl *Clone() const;

  
  /// \brief
  ///   Copies data from source skeletal animation result.
  /// 
  /// \param pSrc
  ///   pointer to source result
  VISION_APIFUNC void CopyFrom(const VisSkeletalAnimResult_cl *pSrc);


  /// \brief
  ///   Returns the skeleton instance used for this result.
  /// 
  /// \return
  ///   VisSkeleton_cl* pSkeleton: pointer to skeleton
  inline VisSkeleton_cl* GetSkeleton() const 
  {
    return m_spSkeleton;
  }


  /// \brief
  ///   Returns the number of bone transformations.
  /// 
  /// Since we have one transformation per bone the number matches the bone count of the skeleton
  /// the animation it has been created on.
  /// 
  /// \return
  ///   int m_iTransformationCount: transformation result count
  inline int GetBoneTransformationCount() const
  {
    return m_iTransformationCount;
  }


  /// \brief
  ///   Clears all skeletal result data to start new additions and calls parent ClearResult.
  /// 
  /// Also the weighting sum that is used for normalization is set to zero.
  VISION_APIFUNC void ClearResult();


  /// \brief
  ///   Compares two skeletal results.
  /// 
  /// \param pResult
  ///   result to compare this result with
  /// 
  /// \return
  ///   bool bResult: true if both are identical
  VISION_APIFUNC bool EqualsResult(const VisSkeletalAnimResult_cl* pResult) const;


  /// \brief
  ///   Re-normalizes this result.
  /// 
  /// It is for instance used after several results with different
  /// weightings have been added.
  /// 
  /// \return
  ///   bool bResult : true if normalization was succesful
  VISION_APIFUNC bool NormalizeResult();

  /// \brief
  ///   Adds two result sets.
  /// 
  /// AddAnimResult adds the specified result to this result, which is useful for mixing animations
  /// normalized.
  /// 
  /// Use an optional animation weight where each bone transformation gets the same weighting.
  /// 
  /// Use an optional per bone weighting table so that each bone transformation gets a different
  /// weighting.
  /// 
  /// Also a combination of both is possible, but leaving away per bone weighting is faster.
  /// 
  /// \param pResult
  ///   result that is multiplied to this one
  /// 
  /// \param fAnimWeight
  ///   applied before multiplication
  /// 
  /// \param pPerBoneWeightingMask
  ///   applied per bone before multiplication
  /// 
  /// \return
  ///   bool bResult: true if function worked correctly
  virtual VISION_APIFUNC bool AddAnimResult(const VisSkeletalAnimResult_cl* pResult, float fAnimWeight=1.f, const float* pPerBoneWeightingMask=NULL);

  /// \brief
  ///   Adds two result sets. As opposed to the "AddAnimResult" method, which is used by the normalize mixer, this method is used for additive animations and behaves differently
  /// 
  /// AddAnimResultAdditive adds an offset-animation to the current animation by passing the animation to add plus a reference animation/result to
  /// calculate the additive offset from.
  /// 
  /// Starting from a clean(ed) animation result, this method has two behavior patterns:
  /// - The first call is treated as the base animation. This only requires the pResult parameter, but not the pResultZero and will just assimilate the pResult values
  /// - All consequent calls will expect the pResultZero to be available for calculating the offset that will be added to the current result
  ///
  /// \param pResult
  ///   result that is added to this one
  /// 
  /// \param pResultZero
  ///   reference result to calculate the additive offset from
  ///
  /// \param fAnimWeight
  ///   applied before adding. Caveat: This will be ignored for the first call after cleaning the result (base input)
  /// 
  /// \param pPerBoneWeightingMask
  ///   applied per bone before adding. Caveat: This will be ignored for the first call after cleaning the result (base input)
  /// 
  /// \return
  ///   bool bResult: true if function worked correctly
  virtual VISION_APIFUNC bool AddAnimResultAdditive(const VisSkeletalAnimResult_cl* pResult, const VisSkeletalAnimResult_cl* pResultZero, float fAnimWeight=1.f, const float* pPerBoneWeightingMask=NULL);
  
  /// \brief
  ///   Layer one animation over another.
  /// 
  /// AddSubLayerAnimResult overlays this result over the other result, useful for overlaying or
  /// replacing whole or only parts of animations. Please refer to the Animation chapter
  /// in the Programmer's Documentation for detailed information about the layering concept.
  /// 
  /// Use an optional animation weight where each bone transformation gets the same weighting.
  /// 
  /// Use an optional per bone weighting table so that each bone transformation gets a different
  /// weighting.
  /// 
  /// Also a combination of both is possible, but leaving away per bone weighting is faster.
  /// 
  /// \param pResult
  ///   result thats layered under this one
  /// 
  /// \param fAnimWeight
  ///   applied before multiplication
  /// 
  /// \param pPerBoneWeightingMask
  ///   applied per bone before multiplication
  /// 
  /// \return
  ///   bool bResult: true if function worked correctly
  virtual VISION_APIFUNC bool AddSubLayerAnimResult(const VisSkeletalAnimResult_cl* pResult, float fAnimWeight=1.f, const float* pPerBoneWeightingMask=NULL);       ///< adds two results together

  /// \brief
  ///   Indicates whether there is a translation calculated for the specified bone.
  /// 
  /// Since a character can have a skeleton with more bones than the skeleton the animation has
  /// been built on there can always be bones that do not have animation data on a lower hierarchy
  /// of the mixing tree. As long as that part of transformation data is missing for that bone,
  /// it is not valid.
  /// Also, if we use layering mixers in code it is possible to only calculate animation data for
  /// some bones (e.g. upper-body of a character) and some will still be not calculated, and
  /// therefore not valid.
  /// 
  /// \param iBoneIndex
  ///   bone index we want to check
  /// 
  /// \return
  ///   bool bResult: true if translation data exists
  VISION_APIFUNC bool IsBoneTranslationValid(int iBoneIndex) const;


  /// \brief
  ///   Indicates whether there is a translation list.
  /// 
  /// \return
  ///   bool bResult: true if there is a translation list
  inline bool HasBoneTranslations(void) const
  {
    return m_pTranslationList!=NULL;
  }

  /// \brief
  ///   Indicates whether there is a rotation calculated for the specified bone
  /// 
  /// Since a character can have a skeleton with more bones than the skeleton the animation has
  /// been built on there can always be bones that do not have animation data on a lower hierarchy
  /// of the mixing tree. As long as that part of transformation data is missing for that bone,
  /// it is not valid.
  /// Also, if we use layering mixers in code it is possible to only calculate animation data for
  /// some bones (e.g. upper-body of a character) and some will still be not calculated, and
  /// therefore not valid.
  /// 
  /// \param iBoneIndex
  ///   bone index we want to check
  /// 
  /// \return
  ///   bool bResult: true if rotation data exists
  VISION_APIFUNC bool IsBoneRotationValid(int iBoneIndex) const;


  /// \brief
  ///   Indicates whether there is a bone rotation list.
  /// 
  /// \return
  ///   bool bResult: true if it has a rotation list
  inline bool HasBoneRotations(void) const
  {
    return m_pRotationList!=NULL;
  }


  /// \brief
  ///   Indicates whether there is a valid rotation for the specified bone
  /// 
  /// Since a character can have a skeleton with more bones than the skeleton the animation has
  /// been built on there can always be bones that don't have animation data on a lower hierarchy
  /// of the mixing tree. As long as that part of transformation data is missing for that bone it
  /// is invalid.
  /// 
  /// \param iBoneIndex
  ///   bone index for looking up validation
  /// 
  /// \return
  ///   bool bResult: true if rotation data is valid
  VISION_APIFUNC bool IsBoneScalingValid(int iBoneIndex) const;


  /// \brief
  ///   Indicates whether there is a bone scaling list.
  /// 
  /// \return
  ///   bool bResult: true if it has a scaling list
  inline bool HasBoneScaling() const
  {
    return m_pScalingList!=NULL;
  }

    
 
  /// \brief
  ///   Sets the translation and rotation of a single bone
  /// 
  /// \param iIndex
  ///   bone index
  /// 
  /// \param Translation
  ///   translation vector
  /// 
  /// \param Rotation
  ///   rotation
  /// 
  /// \return
  ///   bool bResult: true if transformation could be set
  VISION_APIFUNC bool SetBoneTransformation(int iIndex, const hkvVec3& Translation, const hkvQuat &Rotation);


  /// \brief
  ///   Sets the translation of a single bone
  /// 
  /// \param iIndex
  ///   bone index to set translation on
  /// 
  /// \param Translation
  ///   bone translation
  /// 
  /// \return
  ///   bool bResult: true if transformation could be set
  VISION_APIFUNC bool SetBoneTranslation(int iIndex, const hkvVec3& Translation);


  /// \brief
  ///   Sets the rotation of a single bone
  /// 
  /// \param iIndex
  ///   bone index to set rotation on
  /// 
  /// \param Rotation
  ///   rotation
  /// 
  /// \return
  ///   bool bResult: true if transformation could be set
  VISION_APIFUNC bool SetBoneRotation(int iIndex, const hkvQuat &Rotation);

  /// \brief
  ///   Sets the scaling of a single bone. Not supported yet.
  /// 
  /// \param iIndex
  ///   bone index to set scaling on
  /// 
  /// \param Scaling
  ///   scaling
  /// 
  /// \return
  ///   bool bResult: true if transformation could be set
  VISION_APIFUNC bool SetBoneScaling(int iIndex, const hkvVec3& Scaling);


  /// \brief
  ///   Gets the 4x4 transformation matrix of a single bone.
  /// 
  /// The matrix is generated by querying the respective transformation lists.
  /// 
  /// \param iBoneIndex
  ///   bone index to get transformation from
  /// 
  /// \param destMatrix
  ///   matrix filled with transformation
  VISION_APIFUNC void GetBoneTransformationMatrix(int iBoneIndex, hkvMat4& destMatrix) const;

  /// \brief
  ///   Sets the cached 4x4 transformation matrix of a single bone.
  ///   Called by the engine when we have a skinning space result with scaling  and we need to store
  ///   the results as matrices instead of quaterions.
  VISION_APIFUNC void SetCachedTransformationMatrix(int iBoneIndex, hkvMat4& destMatrix);

    
  /// \brief
  ///   Returns the translation of a single bone.
  /// 
  /// \param iBoneIndex
  ///   bone index to get translation from
  /// 
  /// \return
  ///   hkvVec3& Translation: translation vector
  VISION_APIFUNC const hkvVec3& GetBoneTranslation(int iBoneIndex) const;


  /// \brief
  ///   Gets the rotation of a single bone.
  /// 
  /// \param iBoneIndex
  ///   Bone index to get rotation from.
  /// 
  /// \return
  ///   hkvQuat &Rotation: rotation.
  VISION_APIFUNC const hkvQuat& GetBoneRotation(int iBoneIndex) const;

  /// \brief
  ///   Gets the scaling of a single bone. Not supported yet.
  /// 
  /// \param iBoneIndex
  ///   bone index to get transformation from
  /// 
  /// \return
  ///   hkvVec3& Scaling: scaling
  VISION_APIFUNC const hkvVec3& GetBoneScaling(int iBoneIndex) const;


  /// \brief
  ///   Applies a weight to this result.
  /// 
  /// All components of transformations are weighted if they are present.
  /// 
  /// \param fWeight
  ///   weight to apply
  VISION_APIFUNC void ApplyWeightToAnimResult(float fWeight);


  /// \brief
  ///   Returns the number of bones with missing animation data.
  /// 
  /// \return
  ///   int: number of missing bones
  inline int GetMissingBonesCount(void) const
  {
    int iCount=0; 
    for(int i=0; i<m_iTransformationCount; i++)
      if((HasBoneTranslations() && !IsBoneTranslationValid(i))&&(HasBoneRotations() && !IsBoneRotationValid(i)))
        iCount++;
    return iCount;
  }
    
  /// \brief
  ///   Sets a new skeleton on this result.
  /// 
  /// \param pSkeleton
  ///   pointer to new skeleton.
  /// 
  /// \return
  ///   Nothing.
  void SetSkeleton(VisSkeleton_cl* pSkeleton);


  inline hkvVec4* GetTranslationListPtr() const { return m_pTranslationList; }
  inline hkvQuat *GetRotationListPtr() const { return m_pRotationList; }
 
  /// \brief
  ///   Used to allocate the translation list.
  /// 
  /// \return
  ///   bool bResult: true if allocation was successful
  VISION_APIFUNC bool AllocateTranslationLists(void);


  /// \brief
  ///   Used to allocate the rotation list.
  /// 
  /// \return
  ///   bool bResult: true if allocation was successful
  VISION_APIFUNC bool AllocateRotationLists(void);

  /// \brief
  ///   Used to allocate the scaling list.
  /// 
  /// \return
  ///   bool bResult: true if allocation was successful
  VISION_APIFUNC bool AllocateScalingLists();

protected:

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisSkeletalAnimResult_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisSkeletalAnimResult_cl();
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);

private:

  void CommonInit();
  int m_iTransformationCount;                  ///< count of different transformations, one per bone
  float m_fOverallAnimWeight;
  
  float *m_fpOverallTranslationWeightList;     ///< sums up all translation weights
  float *m_fpOverallRotationWeightList;        ///< sums up all rotation weights
  float *m_fpOverallScalingWeightList;         ///< sums up all scaling weights

  bool *m_bpTranslationValidList;             ///< tells for every bone if there is a translation
  bool *m_bpRotationValidList;                ///< tells for every bone if there is a rotation
  bool *m_bpScalingValidList;                 ///< tells for every bone if there is a scaling factor

  hkvVec4* m_pTranslationList;                ///< one translation per bone
  hkvQuat *m_pRotationList;          ///< one rotation per bone
  hkvVec4* m_pScalingList;                    ///< one scale per bone   //TODO hkvVec3 instead?

  hkvMat4*  m_pCachedTransforms;              ///<Cached transforms if we have a skinnig result with scaling
  
  // we keep a pointer to the skeleton cause the result is played or mixed together on one special skeleton
  // and we sometimes need to convert a result from one to another skeleton!
  VisSkeletonPtr m_spSkeleton;



};


#ifdef _VISION_PS3
#pragma diag_pop
#endif

#if defined(__ghs__)
#pragma ghs endnowarning
#endif

#endif //VIS_ANIM_RESULT_HPP_INCLUDED

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
