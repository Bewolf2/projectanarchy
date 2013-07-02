
#ifndef VLUA_APIDOC

%nodefaultctor TriggerBoxEntity_cl;
%nodefaultdtor TriggerBoxEntity_cl;

class TriggerBoxEntity_cl : public VisBaseEntity_cl
{
public:

  %extend{
    VSWIG_BITMASK_SETTER(SetObserverFlags);
    VSWIG_BITMASK_GETTER_CONST(GetObserverFlags);

	VSWIG_CONVERT_BOOL_SETTER(SetEnabled);
	VSWIG_BITMASK_GETTER_CONST(IsEnabled);
    
    bool TestInside(const hkvAlignedBBox &bbox) const
    {
      return self->TestInside(bbox)==TRUE;
    }
    
    bool TestInside(const hkvVec3& pos) const
    {
      return self->TestInside(pos)==TRUE;
    }
  }

  void AddObservedEntity(VisBaseEntity_cl *pEnt);
  void RemoveObservedEntity(VisBaseEntity_cl *pEnt);
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(TriggerBoxEntity_cl, 128, "[0x%2x : %1.2f,%1.2f,%1.2f]", self->GetObserverFlags(), self->GetPosition().x, self->GetPosition().y, self->GetPosition().z )
VSWIG_CREATE_TOSTRING(TriggerBoxEntity_cl, "TriggerBoxEntity_cl [0x%2x : %1.2f,%1.2f,%1.2f] Min[%1.2f,%1.2f,%1.2f] - Max[%1.2f,%1.2f,%1.2f]", self->GetObserverFlags(), self->GetPosition().x, self->GetPosition().y, self->GetPosition().z, self->m_LocalBoundingBox.m_vMin.x,self->m_LocalBoundingBox.m_vMin.y,self->m_LocalBoundingBox.m_vMin.z, self->m_LocalBoundingBox.m_vMax.x,self->m_LocalBoundingBox.m_vMax.y,self->m_LocalBoundingBox.m_vMax.z)

#else

/// \brief Wrapper class for the Trigger Box observer entity.
/// \par Example
///   \code
///     function OnObjectEnter(self, object)
///       Debug:PrinLine(tostring(object) .. " entered the trigger box!")
///     end
///   \endcode
class TriggerBoxEntity_cl : public VisBaseEntity_cl {
public:

  /// @name Trigger Box Functions
  /// @{
  
  /// \brief Set flags to determinate the behavor of the trigger box, see detailed description.
  /// \details
  ///    You can combine these flags for your desired observation behavior of the trigger box:
  ///    - do not observe anything:        VBitmask(0)
  ///    - observe the main camera:        VBitmask(1)
  ///    - observe the static entity list: VBitmask(2)
  ///    - observe all entities:           VBitmask(4)
  /// \param mask The new observer flags.
  /// \see VBitmask
  void SetObserverFlags(VBitmask mask);
  
  /// \brief Get the observation flags of the trigger box, see detailed description.
  /// \details
  ///    Possible flags for the trigger box:
  ///    - do not observe anything:        VBitmask(0)
  ///    - observe the main camera:        VBitmask(1)
  ///    - observe the static entity list: VBitmask(2)
  ///    - observe all entities:           VBitmask(4)
  /// \return The new observer flags.
  /// \see VBitmask
  VBitmask GetObserverFlags();

  
  /// \brief Sets the active status.
  /// When set to false, the trigger box will not trigger any callbacks.
  void SetEnabled(boolean enabled);
  
  /// \brief Check whether the trigger box is set to enabled.
  boolean IsEnabled();
  
  /// \brief Test if the specified bounding box is inside the trigger box.
  /// \param box The bounding box to test.
  /// \return true if the specified bounding box is inside the trigger box, false otherwise.
  bool TestInside(hkvAlignedBBox box);
  
  /// \brief Test if the specified vector is inside the trigger box.
  /// \param vector The position to test.
  /// \return true if the specified position is inside the trigger box, false otherwise.
  bool TestInside(hkvVec3 vector);
  
  /// \brief Add an entity to the list of observed entities. This list is global and will effect all trigger boxes observing the entity list.
  /// \param entity The entity to be observed.
  void AddObservedEntity(VisBaseEntity_cl entity);
  
  /// \brief Remove an entity from the list of observed entities. This list is global and will effect all trigger boxes observing the entity list.
  /// \param entity The entity to be removed from the list.
  void RemoveObservedEntity(VisBaseEntity_cl entity);
  
  /// @}
  /// @name Trigger Box Callbacks
  /// @{
  
  /// \brief Overridable callback function.
  /// Called when an entity enters the trigger box \b and the trigger box observes this entity (or all entities).
  /// \param self The trigger box (or derived) to which this script is attached.
  /// \param object The trigger event causing object.
  /// \par Example
  ///   \code
  ///     function OnObjectEnter(self, object)
  ///       Debug:PrintLine(tostring(object) .. " entered the trigger box!")
  ///     end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the "TriggerBoxX".
  void OnObjectEnter(mixed self, mixed object);
  
  /// \brief Overridable callback function.
  /// Called when an entity leaves the trigger box \b and the trigger box observes this entity (or all entities).
  /// \param self The trigger box (or derived) to which this script is attached.
  /// \param object The trigger event causing object.
  /// \par Example
  ///   \code
  ///     function OnObjectLeave(self, object)
  ///       Debug:PrintLine(tostring(object) .. " left the trigger box!")
  ///     end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the "TriggerBoxX".
  void OnObjectLeave(mixed self, mixed object);

  /// \brief Overridable callback function.
  /// Called when camera enters the trigger box \b and the trigger box observes the camera.
  /// \param self The trigger box (or derived) to which this script is attached.
  /// \param camera The camera object (or derived).
  void OnCameraEnter(mixed self, mixed camera);
  
  /// \brief Overridable callback function.
  /// Called when the camera leaves the trigger box \b and the trigger box observes the camera.
  /// \param self The trigger box (or derived) to which this script is attached.
  /// \param camera The camera object (or derived).
  void OnCameraLeave(mixed self, mixed camera);
  
  /// @}
};

#endif 
