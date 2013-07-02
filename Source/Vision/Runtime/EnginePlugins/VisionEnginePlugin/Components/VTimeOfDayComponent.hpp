/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTimeOfDayComponent.hpp

#ifndef VTIMEOFDAYCOMPONENT_HPP_INCLUDED
#define VTIMEOFDAYCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

// Versions
#define TIMEOFDAY_ATTACHMENT_VERSION_0          0     // Initial version
#define TIMEOFDAY_ATTACHMENT_VERSION_1          1     // Added serialization of Intensity and new Sun-BackLight attachment-type
#define TIMEOFDAY_ATTACHMENT_VERSION_CURRENT    1     // Current version

/// \brief
///   Attachment type for time of day components
enum VTimeOfDayAttachmentType_e
{
  TIMEOFDAY_ATTACHMENT_SUNLIGHTSOURCE = 0,              ///< The light source is the sun 
  TIMEOFDAY_ATTACHMENT_MOONLIGHTSOURCE = 1,             ///< The light source is the moon
  TIMEOFDAY_ATTACHMENT_CORONALIGHTSOURCE = 2,           ///< The light source is exclusively used for displaying a corona, not for illuminating the scene 
  TIMEOFDAY_ATTACHMENT_SUNBACKLIGHTSOURCE = 3,          ///< The light source is the actual backlight of the sun  
  TIMEOFDAY_ATTACHMENT_ENABLEDATNIGHTLIGHTSOURCE = 4,   ///< The light source is an artificial light source that is toggled on or off depending on the time of day 
};


/// \brief
///   Time of Day component for light sources
///
/// The time of day component can be attached to light sources. Once attached to a light source, it
/// will take control of the light source's position, direction, and color properties, using the current
/// time of day to update the light. Direct calls to the light's SetPosition, SetDirection, and SetColor
/// methods should be avoided once a time of day component is attached to a light source.
class VTimeOfDayComponent : public IVObjectComponent, public IVisCallbackHandler_cl
{
public: 

  ///
  /// @name Construction / destruction
  /// @{
  ///

  /// \brief
  ///   Constructor of the time of day component
  ///
  /// \param iDeprecated
  ///   Not used anymore. Will be removed in next major release
  ///
  /// \param iComponentFlags
  ///   Component flags. Simply passed to the base component class.
  ///
  ///
  /// \sa VTimeOfDayAttachmentType_e
  EFFECTS_IMPEXP VTimeOfDayComponent(int iDeprecated=0, int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Destructor of the time of day component
  EFFECTS_IMPEXP virtual ~VTimeOfDayComponent();

  ///
  /// @}
  ///

  ///
  /// @name Time of Day component Properties
  /// @{
  ///


  ///
  /// \brief
  ///   Sets the attachment type.
  /// 
  /// \param eAttachmentType
  ///   Specifies the attachment type (see VTimeOfDayAttachmentType_e). A moonlight will always
  ///   be positioned at the opposite side of the sky as a primary light. A corona light will always
  ///   be positioned at the same place as a primary light. A sun-backlight will always be reflected
  ///   at the Z-Axis in relationship to the primary light.
  /// 
  inline void SetAttachementType(VTimeOfDayAttachmentType_e eAttachmentType) {AttachmentType = eAttachmentType;}

  ///
  /// \brief
  ///   Gets the attachment type.
  /// 
  /// \return
  ///   VTimeOfDayAttachmentType_e: Attachment type of time of day component. See SetAttachementType for details
  /// 
  inline VTimeOfDayAttachmentType_e GetAttachementType() const {return AttachmentType;}

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes.
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden function. Time of day components can be attached to VisLightSource_cl instances.
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VTimeOfDayComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VTimeOfDayComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  ///
  /// @}
  ///

  ///
  /// @name IVisCallbackHandler_cl Overrides
  /// @{
  ///

  /// 
  /// \brief
  ///   Overridden function that gets called by all callbacks that this handler is registered to.
  /// 
  /// \param pData
  ///   See IVisCallbackHandler_cl.
  ///
  /// \see
  ///   IVisCallbackHandler_cl
  /// 
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///
  /// @}
  ///

protected:  

  virtual void UpdateParent();

  bool m_bIsLightClass; // true if attached to VisLightSource_cl
  VColorRef m_iColor;   // original light color in case we modify it during the time of day

public:

  // Exposed to vForge
  VTimeOfDayAttachmentType_e AttachmentType;  ///< Attachment type of the time of day component
  float Intensity;

};


#endif // VTIMEOFDAYCOMPONENT_HPP_INCLUDED

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
