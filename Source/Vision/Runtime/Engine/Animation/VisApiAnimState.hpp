/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimState.hpp

#ifndef VIS_ANIM_STATE_HPP_INCLUDED
#define VIS_ANIM_STATE_HPP_INCLUDED

/// \brief
///   Animation state class with common attributes and methods
/// 
/// All types of animations share the same animation state structure, there is no difference
/// between skeletal and vertex animation state.
/// 
/// It keeps motion delta and visibility bounding boxes.
/// 
/// The state is updated for all entities, also for the ones not visible on screen.
class VisAnimState_cl
{
public:

  /// \brief
  ///   Constructor of the anim state class.
  VISION_APIFUNC VisAnimState_cl();


  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisAnimState_cl();

  /// \brief
  ///   Sets the offset delta vector of this state.
  /// 
  /// \param vOffsetDelta
  ///   offset vector
  inline void SetOffsetDelta(const hkvVec3& vOffsetDelta)
  {
    m_vOffsetDelta = vOffsetDelta;
    m_bOffsetDeltaValid = true;
    m_bHasStates = true;
  }

  /// \brief
  ///   Adds offset delta
  /// 
  /// \param vOffsetDelta
  ///   offset vector
  inline void AddOffsetDelta(const hkvVec3& vOffsetDelta)
  {
    if (m_bOffsetDeltaValid)
    {
      m_vOffsetDelta += vOffsetDelta;
    } else
    {
      m_bOffsetDeltaValid = true;
      m_vOffsetDelta = vOffsetDelta;
    }
    
    m_bHasStates = true;
  }

  
  /// \brief
  ///   Sets the rotation deltas for yaw/pitch/roll.
  /// 
  /// \param angles
  ///   yaw, pitch and roll angles.
  inline void SetRotationDelta(const hkvVec3& angles)
  {
    m_RotationDelta = angles;
    m_bRotationDeltaXValid = m_bRotationDeltaYValid = m_bRotationDeltaZValid = true;
    m_bHasStates = true;
  }


  /// \brief
  ///   Sets the rotation yaw delta of the state.
  /// 
  /// \param fAngle
  ///   yaw rotation angle.
  inline void SetRotationDeltaYaw(float fAngle)
  {
    m_RotationDelta.x = fAngle;
    m_bRotationDeltaXValid = true;
    m_bHasStates = true;
  }


  /// \brief
  ///   Sets the rotation pitch delta of the state.
  /// 
  /// \param fAngle
  ///   pitch rotation angle.
  inline void SetRotationDeltaPitch(float fAngle)
  {
    m_RotationDelta.y = fAngle;
    m_bRotationDeltaYValid = true;
    m_bHasStates = true;
  }
  

  /// \brief
  ///   Sets the rotation roll delta of the state.
  /// 
  /// \param fAngle
  ///   roll rotation angle.
  inline void SetRotationDeltaRoll(float fAngle)
  {
    m_RotationDelta.z = fAngle;

    m_bRotationDeltaZValid = true;
    m_bHasStates = true;
  }



  /// \brief
  ///   Sets the visibility bounding box of the state.
  /// 
  /// \param pBoundingBox
  ///   visibility bounding box.
  /// 
  /// \return
  ///   Nothing.
  inline void SetVisibilityBoundingBox(const hkvAlignedBBox* pBoundingBox)
  {
    VASSERT(pBoundingBox);
    m_VisibilityBoundingBox.m_vMax = pBoundingBox->m_vMax;
    m_VisibilityBoundingBox.m_vMin = pBoundingBox->m_vMin;
    m_bVisibilityBoundingBoxValid = true;
    m_bHasStates = true;
  }

  /// \brief
  ///   Returns the offset delta vector.
  /// 
  /// \return
  ///   hkvVec3& vDelta: offset delta.
  inline const hkvVec3& GetOffsetDelta(void) const 
  {
    return m_vOffsetDelta;
  }


  /// \brief
  ///   Returns the rotation offset vector (yaw/pitch/roll).
  /// 
  /// \return
  ///   hkvVec3 RotationDelta_t&: RotationDelta.
  inline const hkvVec3& GetRotationDelta(void) const 
  {
    return m_RotationDelta;
  }


  /// \brief
  ///   Returns the visibility bounding box.
  /// 
  /// \return
  ///   const hkvAlignedBBox &bbox: visibility bounding box.
  inline const hkvAlignedBBox& GetVisibilityBoundingBox(void) const
  {
    return m_VisibilityBoundingBox;
  }


  /// \brief
  ///   Indicates whether the offset delta vector is valid (even if it is 0/0/0)
  /// 
  /// \return
  ///   bool bValid: true for valid offset delta.
  inline bool IsOffsetDeltaValid() const
  {
    return m_bOffsetDeltaValid;
  }

    
  /// \brief
  ///   Indicates whether the rotation delta vector is valid (even if it is 0/0/0)
  /// 
  /// \return
  ///   bool bValid: True for valid rotation delta
  inline bool IsRotationDeltaValid() const
  {
    return (m_bRotationDeltaXValid || m_bRotationDeltaYValid || m_bRotationDeltaZValid);
  }

  VISION_APIFUNC void ResetOffsetDelta();

  /// \brief
  ///   Rotates the offset delta
  VISION_APIFUNC void RotateOffsetDelta(float fAngle, signed char cAxis);
 

  /// \brief
  ///   Indicates whether the visibility bounding box is valid.
  /// 
  /// \return
  ///   bool bValid: True for valid visibility bounding box
  inline bool IsVisibilityBoundingBoxValid() const 
  {
    return m_bVisibilityBoundingBoxValid;
  }
  
  

  /// \brief
  ///   Fills the state with data from another state.
  /// 
  /// \param pSrc
  ///   Source state to copy from
  VISION_APIFUNC void CopyFrom(const VisAnimState_cl *pSrc);


  /// \brief
  ///   Weights another state and adds it to this state.
  /// 
  /// \param pState
  ///   Other state to add
  /// 
  /// \param fAnimWeight
  ///   weight to apply to added state
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC virtual bool AddAnimState(const VisAnimState_cl* pState, float fAnimWeight);


  /// \brief
  ///   Layers states together.
  /// 
  /// \param pState
  ///   Other state to be layered
  /// 
  /// \param fAnimWeight
  ///   weight for overlay
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC virtual bool AddLayerAnimState(const VisAnimState_cl* pState, float fAnimWeight);


  /// \brief
  ///   Layers states together.
  /// 
  /// The other state is layered under the current state.
  /// 
  /// \param pResult
  ///   Other state to be layered
  /// 
  /// \param fAnimWeight
  ///   weight for overlay
  ///
  /// \param bSkipMotionDelta
  ///   If enabled, this option will check if there is an input without a bone weighting mask and a weight of 1.0.
  ///   If so, inputs that were added before this input will not influence the animation with their motion delta
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC virtual bool AddSubLayerAnimState(const VisAnimState_cl* pResult, float fAnimWeight=1.f, bool bSkipMotionDelta=false);


  /// \brief
  ///   Clears all data structures for starting new additions.
  /// 
  /// \param bResetOffsetDelta
  ///   If Offset Delta info should be reset
  /// 
  /// The state isn't recreated every frame to get better performance.
  VISION_APIFUNC virtual void ClearState(bool bResetOffsetDelta=true);


  /// \brief
  ///   Compares two states.
  /// 
  /// \param pState
  ///   result to compare this with
  /// 
  /// \return
  ///   bool bResult : true, if both states are identical
  VISION_APIFUNC bool EqualsState(const VisAnimState_cl* pState) const;


  /// \brief
  ///   Normalizes the states after several results have been added.
  /// 
  /// \return
  ///   bool bResult: True if normalization was successful
  VISION_APIFUNC bool NormalizeState();


  /// \brief
  ///   Indicates whether the states have valid results
  /// 
  /// \return
  ///   bool bResult: true if results are valid
  inline bool HasStates(void) const 
  {
    return m_bHasStates;
  }

protected:
  bool m_bHasStates;                            ///< has any result been added or multiplied
  bool m_bOffsetDeltaValid;                     ///< offset delta
  //bool m_bRotationDeltaValid;             // rotation delta
  bool m_bRotationDeltaXValid;
  bool m_bRotationDeltaYValid;
  bool m_bRotationDeltaZValid;
  bool m_bVisibilityBoundingBoxValid;           ///< visibility bounding box

  hkvVec3 m_vOffsetDelta;
  hkvVec3 m_RotationDelta;
  hkvAlignedBBox m_VisibilityBoundingBox;

  float m_fOverallOffsetDeltaWeight;
  float m_fOverallRotationDeltaWeightX;
  float m_fOverallRotationDeltaWeightY;
  float m_fOverallRotationDeltaWeightZ;
};

#endif //VIS_ANIM_STATE_HPP_INCLUDED

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
