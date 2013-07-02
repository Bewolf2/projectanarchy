/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file

#ifndef VANIMATIONEVENTEFFECTTRIGGER_HPP_INCLUDED
#define VANIMATIONEVENTEFFECTTRIGGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/IVTransitionEventTrigger.hpp>

// Versions
#define ANIMATIONEVENTEFFECTTRIGGER_VERSION_0          0     // Initial version
#define ANIMATIONEVENTEFFECTTRIGGER_VERSION_CURRENT    0     // Current version


///
/// \brief
///   Struct that represents information about one particle effect trigger.
///
struct VEventEffectTriggerInfo_t : VEventTriggerInfo_t
{
public:

  VEventEffectTriggerInfo_t() : VEventTriggerInfo_t()
  {
    m_spEffectFile = NULL;
    m_iAttachToBone = 0;
    m_vPositionOffset.setZero();
    m_vOrientationOffset.setZero();
  }

  VisParticleEffectFilePtr m_spEffectFile;      ///< Pointer to the particle effect resource
  int m_iAttachToBone;                          ///< Index of the bone the effect should be attached too (optional) 
  hkvVec3 m_vPositionOffset;                    ///< Relative position either to the object root or the attached bone
  hkvVec3 m_vOrientationOffset;                 ///< Relative orientation either to the object root or the attached bone
};


/// 
/// \brief
///   Component that listens to animation events of the owner entity and triggers particle effects.
///
///   This component registers itself as an event listener to the root animation control of
///   the parent owner entity. In addition to the animation sequence and the animation event
///   it is possible to specify the filename of the particle effect, the bone of the owner 
///   entity the effect should be triggered at and the position and orientation relative to
///   the owner entity or the specified bone.    
/// 
class VAnimationEventEffectTrigger : public IVTransitionEventTrigger
{
public: 

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component Constructor
  EFFECTS_IMPEXP VAnimationEventEffectTrigger(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE);

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

  V_DECLARE_SERIAL_DLLEXP(VAnimationEventEffectTrigger, EFFECTS_IMPEXP)

  V_DECLARE_VARTABLE(VAnimationEventEffectTrigger, EFFECTS_IMPEXP)

  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:

  /// \brief
  ///   Initializes the component.
  EFFECTS_IMPEXP virtual bool CommonInit() HKV_OVERRIDE;

  /// \brief
  ///   Overridable that can be implemented to respond to animation events. The base implementation is empty.
  EFFECTS_IMPEXP virtual void OnAnimationEvent() HKV_OVERRIDE;

private:  

  // Exposed to vForge:
  VString EffectFilename;                 ///< Filename of the particle effect
  VString AttachToBone;                   ///< Name of the bone the effect should be attached too (optional) 
  hkvVec3 PositionOffset;            ///< Relative position either to the owner object root or the attached bone
  hkvVec3 OrientationOffset;         ///< Relative orientation either to the owner object root or the attached bone
};

#endif // VANIMATIONEVENTEFFECTTRIGGER_HPP_INCLUDED

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
