#ifndef VLUA_APIDOC

%nodefaultctor IVTimeOfDay;
%nodefaultdtor IVTimeOfDay;

class IVTimeOfDay : public VisTypedEngineObject_cl
{
public:

  void SetDayTime(float fTime);

  float GetDayTime();

  void IncDayTime(float fTimeDiff);

  VColorRef GetSunColor();

  VColorRef GetAmbientColor();

  %extend{
    hkvVec3 GetSunDirection()
    {
      hkvVec3 vDirection;
      self->GetSunDirection(vDirection);
      
      return vDirection;
    }
  }
};

#else

/// \brief Returns the type name of this object.
/// \par Example
///   \code
///     local handler = Renderer:GetTimeOfDayHandler()
///
///     if handler == nil then return end
///
///     handler:SetDayTime(0.6)
///     local sunColor = handler:GetSunColor()
///   \endcode
class IVTimeOfDay : public VisTypedEngineObject_cl
{
public:

  /// @name Time of Day
  /// @{
  
  /// \brief Virtual function to set the time of day.
  /// \param time The time of day may range from 0 (0:00) to 1 (24:00).
  void SetDayTime(number time);

  /// \brief Returns the current time of day.
  /// \return The current time of day between 0 (0:00) and 1 (24:00).
  number GetDayTime();

  /// \brief Increments the current time of day.
	/// \param timeDiff The time to add to the current tiem of day in the range from 0 (0:00) to 1 (24:00).
	/// \par Example
  ///   \code
  ///     --a handler has to be present...
  ///     Renderer:GetTimeOfDayHandler():IncDayTime(0.2)
  ///   \endcode
  void IncDayTime(number timeDiff);

  /// @}
  /// @name Sun and Ambient
  /// @{
  
  /// \brief Returns the direction of incoming sun light for the current time of day.
  /// \return The sun's current direction.
  hkvVec3 GetSunDirection();

  /// \brief Returns the sun color for the current time of day.
  /// \return The sun's current color.
  VColorRef GetSunColor();

  /// \brief Returns the ambient color for the current time of day.
  /// \return The current ambient color of the scene.
  VColorRef GetAmbientColor();
  
  /// @}
};
    
#endif