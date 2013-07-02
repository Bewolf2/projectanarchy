#ifdef VLUA_APIDOC

/// \brief Physics module class (Havok): 
///   Provides Lua scripts, that are attached to custom volume object with a vHavokTriggerVolume component,
///   with the below described trigger volume callbacks. Please note that this is not a wrapper class.
class vHavokTriggerVolume : public IVObjectComponent
{
public:
	
	/// @}
  /// @name Trigger Volume Callbacks
  /// @{

  /// \brief  
  ///   Called when a rigid body enters the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the rigid body belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnObjectEnter(self, object)
  ///       Debug:PrintLine(tostring(object) .. " entered the trigger volume!")
  ///     end
  ///   \endcode
  void OnObjectEnter(mixed self, mixed object);

  /// \brief  
  ///   Called when a rigid body leaves the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the rigid body belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnObjectLeave(self, object)
  ///       Debug:PrintLine(tostring(object) .. " left the trigger volume!")
  ///     end
  ///   \endcode
  void OnObjectLeave(mixed self, mixed object);
  
  /// \brief  
  ///   Called when a character controller enters the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the character controller belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnCharacterEnter(self, object)
  ///       Debug:PrintLine(tostring(object) .. " entered the trigger volume!")
  ///     end
  ///   \endcode
  void OnCharacterEnter(mixed self, mixed object);

  /// \brief  
  ///   Called when a character controller leaves the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the character controller belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnCharacterLeave(self, object)
  ///       Debug:PrintLine(tostring(object) .. " left the trigger volume!")
  ///     end
  ///   \endcode
  void OnCharacterLeave(mixed self, mixed object);

  ///
  /// @}
  ///
  
};

#endif
