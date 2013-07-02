/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVTransitionEventTrigger.hpp

#ifndef IVTRANSITIONEVENTTRIGGER_HPP_INCLUDED
#define IVTRANSITIONEVENTTRIGGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>


/// 
/// \brief
///   Component that listens to animation events of the owner entity triggered by the transition
///   state machine and the animation sequences of the owner entity.
///
///   This component registers itself as an event listener to the root animation control of
///   the parent owner entity. VAnimationEventEffectTrigger derives from this class in order 
///   to listen to animation events added in the vAnimTool.
/// 
/// \note
///   Please note that this class derives from IVAnimationEventTrigger so animation events 
///   triggered by the Vision animation system (coming from the animation file) will also be
///   processed next to the events added in the vAnimTool. Inherit from this class in case
///   you want to listen to animation events both from the animation file and the vAnimTool 
///   (vTransition file).
/// 
/// \note
///   The owner entity will need to have a transition state machine component attached with 
///   a valid vTransition file that contains animation events.
///
class IVTransitionEventTrigger : public IVAnimationEventTrigger
{
public: 

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component Constructor
  EFFECTS_IMPEXP IVTransitionEventTrigger(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE);

  ///
  /// @}
  ///


  ///
  /// @name VTypedObject Overrides
  /// @{
  ///

  /// \brief
  ///   Virtual function that gets called whenever a message is sent to this object
  EFFECTS_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(IVTransitionEventTrigger, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(IVTransitionEventTrigger, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   Binary archive
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:

  /// \brief
  ///   Initializes the component.
  EFFECTS_IMPEXP virtual bool CommonInit() HKV_OVERRIDE;

};

#endif // IVTRANSITIONEVENTTRIGGER_HPP_INCLUDED

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
