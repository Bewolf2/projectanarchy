/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLightClippingVolumeComponent.hpp
#pragma once
#ifndef VLIGHT_CLIPPING_VOLUME_COMPONENT_HPP
#define VLIGHT_CLIPPING_VOLUME_COMPONENT_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeObject.hpp>

/// \brief Light clipping component which can be attached to a light source to limit its extend
class VLightClippingVolumeComponent : public IVObjectComponent
{
  enum VVersion_e 
  {
    VV_Version1 = 1,                     // initial version
    VV_Version2 = 2,                     // added clip handedness 
    VV_VersionCurrent = VV_Version2
  };

public:

  /// \brief
  ///   Handedness of light clipping.
  ///   (Do not modify numeric enum values, since they are used for indexing into arrays.) 
  enum VClipHandedness_e
  {
    VCH_Outside=0,                       // clip everything that is outside of the clipping volume
    VCH_Inside=1                         // clip everything that is inside of the clipping volume        
  };

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   constructor
  /// 
  /// \param iComponentFlags
  ///    component flags \see VObjectComponentFlags_e
  ///
  EFFECTS_IMPEXP VLightClippingVolumeComponent(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   destructor
  ///
  EFFECTS_IMPEXP ~VLightClippingVolumeComponent();

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VLightClippingVolumeComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VLightClippingVolumeComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  ///
  /// @}
  ///

  /// \brief
  ///  checks if this component can attach to a certain object
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief getter for the volume referenced by this component
  inline VCustomVolumeObject* GetVolume() { return Volume; }

  /// \brief
  ///   Returns handedness of clipping.
  inline VClipHandedness_e GetClipHandedness() const { return ClipHandedness; }

  /// \brief 
  ///   Sets handedness of clipping.
  inline void SetClipHandedness(VClipHandedness_e eClipHandedness) { ClipHandedness = eClipHandedness; }

private:
  VObjectReference<VCustomVolumeObject> Volume;
  VClipHandedness_e ClipHandedness;

};

#endif

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
