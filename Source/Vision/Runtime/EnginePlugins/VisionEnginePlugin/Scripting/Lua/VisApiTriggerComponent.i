
#ifndef VLUA_APIDOC

%nodefaultctor;
%nodefaultdtor;

class IVisTriggerBaseComponent_cl : public IVObjectComponent
{
public:
  %extend{
    void SetDisplayName(const char * displayName)
    {
      self->m_sVForgeDisplayName = displayName;
    }

    void SetIconFilename(const char * iconFilename)
    {
      self->m_sVForgeIconFilename = iconFilename;
    }
  }
};

//add lua tostring operator
VSWIG_CREATE_TOSTRING(IVisTriggerBaseComponent_cl, "%s: %s [%s]", self->GetClassTypeId()->m_lpszClassName, self->GetComponentName(), self->m_sVForgeDisplayName.IsEmpty()?"no display name":self->m_sVForgeDisplayName.AsChar())

class VisTriggerSourceComponent_cl : public IVisTriggerBaseComponent_cl
{
public:

  void UnlinkAllTargets();
  void TriggerAllTargets();
  
  %extend{
    int LinkToTarget(VisTriggerTargetComponent_cl *pTarget)
    {
      if(!pTarget) return -1;
      
      IVisTriggerBaseComponent_cl::OnLink(self, pTarget);
      return 1;
    }
    
    int LinkToTarget(const char * szTargetName, VisTypedEngineObject_cl * pOptional = NULL)
    {
      return self->LinkTo(szTargetName, pOptional);
    }

    int UnlinkFromTarget(VisTriggerTargetComponent_cl *pTarget)
    {
      if(!pTarget) return -1;
      
      IVisTriggerBaseComponent_cl::OnUnlink(self, pTarget);
      return 1;
    }
    
    int UnlinkFromTarget(const char * szTargetName, VisTypedEngineObject_cl * pOptional = NULL)
    {
      return self->UnlinkFrom(szTargetName, pOptional);
    }
    
    VSWIG_CREATE_CAST(VisTriggerSourceComponent_cl)
  }
};

class VisTriggerTargetComponent_cl : public IVisTriggerBaseComponent_cl
{
public:

  void UnlinkAllSources();
  
  %extend{
    int LinkToSource(VisTriggerSourceComponent_cl *pSource)
    {
      if(!pSource) return -1;
      
      IVisTriggerBaseComponent_cl::OnLink(pSource, self);
      return 1;
    }

    int LinkToSource(const char * szSourceName, VisTypedEngineObject_cl * pOptional = NULL)
    {
      return self->LinkTo(szSourceName, pOptional);
    }

    int UnlinkFromSource(VisTriggerSourceComponent_cl *pSource)
    {
      if(!pSource) return -1;
      
      IVisTriggerBaseComponent_cl::OnUnlink(pSource, self);
      return 1;
    }
    
    int UnlinkFromSource(const char * szSourceName, VisTypedEngineObject_cl * pOptional = NULL)
    {
      return self->UnlinkFrom(szSourceName, pOptional);
    }
    
    VSWIG_CREATE_CAST(VisTriggerTargetComponent_cl)
  }
};

%nodefaultdtor;
%clearnodefaultctor;


#else

/// \brief Base class for trigger components.
class IVisTriggerBaseComponent_cl : public IVObjectComponent
{
public:
    
  /// @name Trigger Base Functions
  /// @{

  /// \brief
  ///   String that defines the text to show for the link type in vForge. Not relevant inside the
  ///   engine (accordingly does not get serialized)
  /// \param displayName The new display name.
  void SetDisplayName(string displayName);
  
  /// \brief
  ///   Optional string that defines the icon filename for the link icon in vForge. Not relevant
  ///   inside the engine (accordingly does not get serialized)
  /// \param iconFilename The icon file name.
  void SetIconFilename(string iconFilename);
  
  /// @}
  
};

/// \brief Wrapper for an object component that defines a trigger source.
/// 
/// An object (e.g. entity) can have one or more trigger sources which are components that can
/// trigger an event on an arbitrary number of targets. For instance a light switch (source) can
/// toggle dynamic lights (targets).
class VisTriggerSourceComponent_cl : public IVisTriggerBaseComponent_cl
{
public:

  /// @name Trigger Source Functions
  /// @{
  
  /// \brief Triggers (invokes callback of) all connected targets.
  /// \par Example
  ///   \code
  /// -- entity with trigger target
  ///
  /// function OnCreate(self)
  ///   self:AddTriggerTarget("Switch")
  /// end
  ///
  /// -- the callback will be triggered when calling 'TriggerAllTargets'
  /// function OnTrigger(self, sourceName, targetName)	
  ///   Debug:PrintLine("Triggered!")
  /// end
  ///
  /// -- entity with trigger source
  ///
  /// function OnCreate(self)
  ///   self:AddTriggerSource("MySource") -- creates also a dynamic property "MySource"
  /// end
  /// 
  /// function OnAfterSceneLoaded(self)
  ///   self.MySource:LinkToTarget("Switch") -- link to all targets having the name "Switch"
	/// end
	///
  /// function OnThink(self)
  ///   if conditionsReached then
  ///     self.MySource:TriggerAllTargets() -- trigger all connected targets
  ///   end  
  /// end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see VisTypedEngineObject_cl::OnTrigger
  /// \see VisTypedEngineObject_cl::AddTriggerSource
  /// \see VisTypedEngineObject_cl::AddTriggerTarget
  /// \see VisTypedEngineObject_cl::LinkToSource
  /// \see LinkToTarget
  void TriggerAllTargets();
  
  /// \brief Link this source component to another target component
  /// \note
  ///   It doesn't matter if you perform a link from target to source or from source to target, but don't do it 
  ///   from both sides (otherwise you create two links, resulting in two calls to VisTriggerTargetComponent_cl::OnTrigger).
  /// \param target The target to link with.
  /// \return The number of linked targets.
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see TriggerAllTargets
  number LinkToTarget(VisTriggerTargetComponent_cl target);
  
  /// \brief Link this source to all targets having a specific name.
  /// \note
  ///   It doesn't matter if you perform a link from target to source or from source to target, but don't do it 
  ///   from both sides (otherwise you create two links, resulting in two calls to VisTriggerTargetComponent_cl::OnTrigger).
  /// \param targetName The name of the targets to link with (regardless to which object the target belongs).
  /// \param optional (\b optional) Link only to targets of this optional specified VisTypedEngineObject_cl.
  /// \return The number of linked targets.
  /// \par Example
  ///   \code
  /// function OnCreate(self)
  ///   self:AddTriggerSource("TurnOffSource") -- creates also a dynamic property "TurnOffSource"
  /// end
  /// 
  /// function OnAfterSceneLoaded(self)
  ///   -- link to all targets having the name "TogleOff"
  ///   -- (light sources come with a target component named "ToggleOff" and "ToggleOn")
  ///   self.TurnOffSource:LinkToTarget("ToggleOff")
	/// end
	///
  /// function OnThink(self)
  ///   if readyForDarkness then
  ///     self.TurnOffSource:TriggerAllTargets() -- will switch off all dynamic lights
  ///   end  
  /// end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see TriggerAllTargets
  number LinkToTarget(string targetName, VisTypedEngineObject_cl optional = nil);

  /// \brief Unlink this source component from another target component
  /// \param target The target to unlink from.
  /// \return The number of unlinked targets.
  /// \see VisTriggerTargetComponent_cl::UnlinkFromSource
  number UnlinkFromTarget(VisTriggerTargetComponent_cl target);
  
  /// \brief Unlink this source from all targets having a specific name.
  /// \param targetName The name of the targets to unlink from (regardless to which object the target belongs).
  /// \param optional (\b optional) Unlink only from targets of this optional specified VisTypedEngineObject_cl.
  /// \return The number of unlinked targets.
  /// \see VisTriggerTargetComponent_cl::UnlinkFromSource
  number UnlinkFromTarget(string targetName, VisTypedEngineObject_cl optional = nil);
  
  /// \brief unlink all connected targets.
  /// \see UnlinkFromTarget
  void UnlinkAllTargets();
  
  /// @}
};

/// \brief Wrapper for an object component that defines a trigger target.
/// 
/// An object (e.g. entity) can have one or more trigger targets which are components that can
/// receive trigger events sent by an arbitrary number of sources. For instance a light switch
/// (source) can toggle dynamic lights (targets).
class VisTriggerTargetComponent_cl : public IVisTriggerBaseComponent_cl
{
public:

  /// @name Trigger Target Functions
  /// @{
  
  /// \brief Link this target component to another source component
  /// \note
  ///   It doesn't matter if you perform a link from target to source or from source to target, but don't do it 
  ///   from both sides (otherwise you create two links, resulting in two calls to VisTriggerTargetComponent_cl::OnTrigger).
  /// \param source The source to link with.
  /// \return The number of linked sources.
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see VisTypedEngineObject_cl::AddTriggerSource
  /// \see VisTypedEngineObject_cl::AddTriggerTarget
  number LinkToSource(VisTriggerSourceComponent_cl source);
  
  /// \brief Link this target to all sources having a specific name.
  /// \note
  ///   It doesn't matter if you perform a link from target to source or from source to target, but don't do it 
  ///   from both sides (otherwise you create two links, resulting in two calls to VisTriggerTargetComponent_cl::OnTrigger).
  /// \param sourceName The name of the source to link with (regardless to which object the source belongs).
  /// \param optional (\b optional) Link only to sources of this optional specified VisTypedEngineObject_cl.
  /// \return The number of linked sources.
  /// \par Example
  ///   \code
  /// function OnCreate(self)
  ///   self:AddTriggerTarget("Trap") -- creates also a dynamic property "Trap"
  /// end
  /// 
  /// function OnAfterSceneLoaded(self)
  ///   -- perform linking after the scene has been loaded in order to be sure that the source exists
  ///   self.Trap:LinkToSource("MainCharacterSource")
	/// end
	/// 
  /// function OnTrigger(self, sourceName, targetName)	
  ///   ReleaseEvilSpikes()
  /// end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see VisTypedEngineObject_cl::AddTriggerSource
  /// \see VisTypedEngineObject_cl::AddTriggerTarget
  number LinkToSource(string sourceName, VisTypedEngineObject_cl optional = nil);

  /// \brief Unlink this target component from another source component
  /// \param source The source to unlink from.
  /// \return The number of unlinked sources.
  /// \see VisTriggerSourceComponent_cl::UnlinkFromTarget
  number UnlinkFromSource(VisTriggerSourceComponent_cl source);
  
  /// \brief Unlink this target from all sources having a specific name.
  /// \param sourceName The name of the sources to unlink from (regardless to which object the target belongs).
  /// \param optional (\b optional) Unlink only from targets of this optional specified VisTypedEngineObject_cl.
  /// \return The number of unlinked sources.
  /// \see VisTriggerSourceComponent_cl::UnlinkFromTarget
  number UnlinkFromSource(string sourceName, VisTypedEngineObject_cl optional = nil);
  
  /// \brief unlink all connected sources.
  /// \see UnlinkFromSource
  void UnlinkAllSources();
  
  /// @}

};

#endif
