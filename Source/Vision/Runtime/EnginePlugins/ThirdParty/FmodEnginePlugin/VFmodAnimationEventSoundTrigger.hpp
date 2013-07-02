/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodAnimationEventSoundTrigger.hpp

#ifndef VFMODANIMATIONEVENTSOUNDTRIGGER_HPP_INCLUDED
#define VFMODANIMATIONEVENTSOUNDTRIGGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>

#include <Vision/Runtime/Engine/Animation/VisApiAnimEventList.hpp>

/// Serialization versions
#define V_VFMODANIMATIONEVENTSOUNDTRIGGER_VERSION_0         0x00000000                             // Initial version
#define V_VFMODANIMATIONEVENTSOUNDTRIGGER_VERSION_CURRENT   V_VFMODANIMATIONEVENTSOUNDTRIGGER_VERSION_0

///
/// \brief
///   Struct that represents information about one sound trigger.
///
struct VFmodEventSoundTriggerInfo_t : VEventTriggerInfo_t
{
public:

  VFmodEventSoundTriggerInfo_t() : VEventTriggerInfo_t()
  {
    m_spSoundResource = NULL;
  };

  VFmodSoundResourcePtr m_spSoundResource;              ///< Pointer to the sound resource
};


/// 
/// \brief
///   Component that listens to animation events of the owner entity and triggers sound effects.
///
///   This component registers itself as an event listener to the root animation control of
///   the parent owner entity. In addition to the animation sequence and the animation event
///   it is possible to specify the filename of the sound effect that should be triggered.    
/// 
class VFmodAnimationEventSoundTrigger : public IVAnimationEventTrigger
{
public: 

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component Constructor
  FMOD_IMPEXP VFmodAnimationEventSoundTrigger(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE);

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VFmodAnimationEventSoundTrigger, FMOD_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VFmodAnimationEventSoundTrigger, FMOD_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   Binary archive
  FMOD_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  ///
  /// @}
  ///

protected:

  /// \brief
  ///   Initializes the component.
  FMOD_IMPEXP VOVERRIDE bool CommonInit();

  /// \brief
  ///   Overridable that can be implemented to respond to animation events.
  FMOD_IMPEXP VOVERRIDE void OnAnimationEvent();

private:  

  // Exposed to vForge:
  VString SoundFilename;                                ///< Filename of the sound effect

};

#endif // VFMODANIMATIONEVENTSOUNDTRIGGER_HPP_INCLUDED

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
