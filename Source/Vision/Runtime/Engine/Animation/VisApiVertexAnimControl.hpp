/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexAnimControl.hpp

#ifndef VIS_VERTEX_ANIM_CONTROL_HPP_INCLUDED
#define VIS_VERTEX_ANIM_CONTROL_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimControl.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimSequence.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimSequence.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimResult.hpp>

// define smartpointer
class VisVertexAnimControl_cl;
typedef VSmartPtr<VisVertexAnimControl_cl> VisVertexAnimControlPtr;


#define VVERTANIMCTRL_DEFAULTS 0


/// \brief
///   Vertex animation control class derived from base animation control (VisAnimControl_cl class)
/// 
/// Overwrites OnUpdateControlState, SampleAnimation to do special vertex delta interpolation.
class VisVertexAnimControl_cl : public VisAnimControl_cl, public VRefCounter
{
public:

  /// \brief
  ///   Constructor of the vertex anim control class
  /// 
  /// \param iFlags
  ///   Flags of the controller. The following flags are supported:
  ///   \li VANIMCTRL_LOOP : the animation is looped
  /// 
  ///   \li VANIMCTRL_NO_SEQUENCE_EVENTS : no sequence events are triggered
  /// 
  ///   \li VANIMCTRL_NO_INTERPOLATION : turns off frame interpolation
  VISION_APIFUNC VisVertexAnimControl_cl(unsigned int iFlags = VVERTANIMCTRL_DEFAULTS);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisVertexAnimControl_cl();


  /// \brief
  ///   Overridden function
  VISION_APIFUNC virtual void OnSetAnimSequence(const VisAnimSequence_cl* pAnimSequence) const {VASSERT(pAnimSequence!=NULL);}


  /// \brief
  ///   Overridden OnUpdateControlState function to update the state of the vertex animation
  ///   control.
  /// 
  /// \param fTimeDelta
  ///   time past since last state update
  VISION_APIFUNC virtual bool OnUpdateControlState(float fTimeDelta);


  /// \brief
  ///   Calls SampleAnimation and sets some parameters.
  /// 
  /// We need it because every control could have different parameters. In this case we don't save
  /// a local vertex result because of memory reasons, instead we provide the method with a list of
  /// vertices as parameter to add the result directly.
  /// 
  /// \param fWeight
  ///   weight to apply to vertex deltas before adding
  /// 
  /// \param pVertexAnimResult
  ///   result that includes the vertex list
  /// 
  /// \return
  ///   bool bResult: true if update was successful
  VISION_APIFUNC bool UpdateVertexAnimResult(float fWeight, VisVertexAnimResult_cl* pVertexAnimResult);


  /// \brief
  ///   Returns the current ceiling frame.
  /// 
  /// \return
  ///   VisVertexDeltaKeyFrame_cl *pFrame: pointer to current ceiling frame.
  inline const VisVertexDeltaKeyFrame_cl* GetCeilingFrame() const
  {
    return m_pCeilingFrame;
  }

  /// \brief
  ///   Returns the current floor frame.
  /// 
  /// \return
  ///   VisVertexDeltaKeyFrame_cl *pFrame: pointer to current ceiling frame.
  inline const VisVertexDeltaKeyFrame_cl* GetFloorFrame() const
  {
    return m_pFloorFrame;
  }


  /// \brief
  ///   Returns the current fraction, i.e. the weighting factor [0..1] between floor and ceiling
  ///   frame
  /// 
  /// \return
  ///   float fWeighting : weighting factor [0..1] between floor and ceiling frame
  inline float GetFraction(void)
  {
    return m_fFraction;
  }


  ///
  /// @name Static Helper Functions
  /// @{
  ///

  /// \brief
  ///   Static helper function to create and setup a new vertex animation control instance.
  /// 
  /// \param pAnimSequence
  ///   the animation sequence to playback by the control
  /// 
  /// \param iControlFlags
  ///   defines some animation control behaviour: 
  ///   \li VANIMCTRL_LOOP is used to play animations looped
  /// 
  /// \param fControlSpeed
  ///   the playback speed; 1.0f for normal speed
  /// 
  /// \param bPlay
  ///   specifies whether the animation should immediately be started
  /// 
  /// \return
  ///   VisVertexAnimControl_cl *pAnimControl: new animation control instance
  VISION_APIFUNC static VisVertexAnimControl_cl *Create(VisAnimSequence_cl* pAnimSequence, int iControlFlags = VVERTANIMCTRL_DEFAULTS, float fControlSpeed = 1.f, bool bPlay = true);

  V_DECLARE_SERIALX( VisVertexAnimControl_cl, VISION_APIFUNC )
  VISION_APIFUNC void SerializeX( VArchive &ar );

  ///
  /// @}
  ///

private:

  /// \brief
  ///   Performs the current frame interpolation.
  void SampleAnimation(VisAnimResult_cl* pDestResult, float fSequenceTime);

  float m_fWeight;

  // keep pointer to the keyframes and fraction used for the current interpolation
  VisVertexDeltaKeyFrame_cl* m_pCeilingFrame;
  VisVertexDeltaKeyFrame_cl* m_pFloorFrame;
  float m_fFraction;

  VisAnimState_cl m_LocalAnimState;
};

#endif //VIS_VERTEX_ANIM_CONTROL_HPP_INCLUDED

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
