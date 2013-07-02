
#ifndef VLUA_APIDOC

//global constants

%immutable;
const int EVENT_TRANSITION_STARTED = 1;
const int EVENT_TRANSITION_FINISHED = 2;
const int EVENT_SEQUENCE_STARTED = 3;
const int EVENT_SEQUENCE_FINISHED = 4;
const int EVENT_TRANSITION_START_REACHED = 5;
%mutable;


%nodefaultctor VTransitionStateMachine;
%nodefaultdtor VTransitionStateMachine;
class VTransitionStateMachine : public IVObjectComponent
{
public:
 
  bool IsEnabled() const;
  void SetState(const char *szName);
  bool IsBlending();
  bool IsWaitingForSyncBlending();
  void SetForwardingAnimEvents(bool bEnable);
  int AddEventListener(VisTypedEngineObject_cl* pObj);
  bool RemoveEventListener(VisTypedEngineObject_cl* pObj);
  
  %extend{

	  VSWIG_CONVERT_BOOL_SETTER(SetEnabled);
  
    bool LoadFromFile(const char * szTransitionFile, const char * szInitialAnimation)
    {
      VisBaseEntity_cl *pOwner = (VisBaseEntity_cl *)self->GetOwner(); //we know that the owner has to be an entity
      VDynamicMesh * pMesh = (pOwner != NULL) ? pOwner->GetMesh() : NULL;
      if(!pMesh) return false;
      
      VTransitionTable *pTable = new VTransitionTable(&VTransitionManager::GlobalManager(), pMesh);
      if(!pTable->LoadFromFile(szTransitionFile)) return false;
      
      //prevent double initialization
      self->DeInit();
      self->SetInitialAnimation(szInitialAnimation);
      self->Init(pOwner, pTable, true);
      
      //get the scripting component...
      VScriptComponent *pComponent = (VScriptComponent *)pOwner->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(VScriptComponent));
      
      if(!pComponent) return false;

      //always receive events...      
      self->AddEventListener(pComponent);
      
      return true;
    }
    
    const char * GetActiveState()
    {
      if(self->GetActiveState())
        return self->GetActiveState()->GetName();
        
      return NULL;
    }

    const char * GetInactiveState()
    {
      if(self->GetInactiveState())
        return self->GetInactiveState()->GetName();
        
      return NULL;
    }

    const char* GetInitialAnimation()
    {
      return self->GetInitialAnimation().AsChar();
    }

    VSWIG_CREATE_CAST(VTransitionStateMachine)
  }
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VTransitionStateMachine, 256, "%s [%s]", self->GetComponentName()==NULL?self->GetClassTypeId()->m_lpszClassName:self->GetComponentName(), self->GetActiveState()?self->GetActiveState()->GetName():"no active state")
VSWIG_CREATE_TOSTRING(VTransitionStateMachine, "VTransitionStateMachine: %s [%s]", self->GetComponentName(), self->GetActiveState()?self->GetActiveState()->GetName():"no active state")

#else

/// \brief Lua Wrapper class for VTransitionStateMachine.
class VTransitionStateMachine : public IVObjectComponent {
public:

  /// @name Common Transition State Machine Functions
  /// @{

  /// \brief
  ///   Sets the enabled state of the transition machine.
  void SetEnabled(boolean enabled);

  /// \brief
  ///   Returns the enabled state of the transition machine.
  boolean IsEnabled();

  /// \brief Load a transition file for the transition state machine.
  /// \param transitionFile The transition file to use.
  /// \param initialAnimation The name of the initial animation.
  /// \return true on success loading the transition file and the initial animation, otherwise false.
  /// \par Example
  ///   \code
  ///   function OnCreate(self)
  ///     self:AddTransitionStateMachine("Transition")
  ///     self.Transition:LoadFromFile("Models\\Warrior\\WarriorEvents.vTransition", "Idle")
  ///   end
  ///   \endcode
  boolean LoadFromFile(string transitionFile, string initialAnimation);

  /// \brief Sets a new animation state. 
  /// \param newStateName Name of the animation that will be set as active state.
  /// \par Example
  ///   \code
  ///   function OnCreate(self)
  ///     self:AddTransitionStateMachine("Transition")
  ///     self.Transition:LoadFromFile("Models\\Warrior\\WarriorEvents.vTransition", "Idle")
  ///   end
  ///
  ///   function OnAfterSceneLoaded(self)
  ///     self.Transition:SetState("Walk")
  ///   end
  ///
  ///   function OnTransitionEvent(self, eventType)
  ///     -- handle all possible animation events
  ///     if eventType == Vision.EVENT_TRANSITION_STARTED then
  ///       Debug:PrintLine(self:GetKey()..": Transition started '"..self.Transition:GetActiveState().."'")
  ///     elseif eventType == Vision.EVENT_TRANSITION_FINISHED then
  ///       Debug:PrintLine(self:GetKey()..": Transition finished '"..self.Transition:GetInactiveState().."'/'"..self.Transition:GetActiveState().."'")
  ///     elseif eventType == Vision.EVENT_SEQUENCE_STARTED then
  ///       Debug:PrintLine(self:GetKey()..": Sequence started '"..self.Transition:GetActiveState().."'")
  ///     elseif eventType == Vision.EVENT_SEQUENCE_FINISHED then
  ///       Debug:PrintLine(self:GetKey()..": Sequence finished '"..self.Transition:GetActiveState().."'")
  ///     elseif eventType == Vision.EVENT_TRANSITION_START_REACHED then
  ///       Debug:PrintLine(self:GetKey()..": Transition Start reached '"..self.Transition:GetInactiveState().."'/'"..self.Transition:GetInactiveState().."'")
  ///     else
  ///       Debug:PrintLine(self:GetKey()..": Unkown transition event "..eventType.."'"..self.Transition:GetInactiveState().."'/'"..self.Transition:GetActiveState().."'")
  ///     end
  ///   end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaAnimation.scene", Warrior #2.
  void SetState(string newStateName);
  
  /// \brief Gets the name of the current state. 
  /// \return Name of the animation that is set as active state.
  /// \see SetState
  /// \see GetInactiveState
  string GetActiveState();

  /// \brief Gets inactive state of the state machine (source animation state). 
  /// \return String of the recently active animation sequence, or nil if TransitionStateMachine has not been initialized.
  /// \see SetState  
  /// \see GetActiveState
  string GetInactiveState();

  /// \brief Gets the initial animation state. 
  /// \return Name of the initial animation state.
  string GetInitialAnimation();

  /// \brief Indicates whether the state machine is currently blending to a new state. 
  /// \return true if the state machine is currently blending, otherwise false.
  boolean IsBlending();
  
  /// \brief Indicates whether the state machine is currently waiting for an event to start blending. 
  /// This situation may occur when the state machine has been instructed to perform
  /// a synchronized blend operation, and thus needs to wait for the sync-animation-event
  /// to be reached before the blending can be initiated. 
  /// \return true in case the state machine is waiting, otherwise false.
  boolean IsWaitingForSyncBlending();
  
  /// \brief Set/unset the state machine to forward the animation events to the registered event listeners.
  /// This is useful when the inner animations of the state machine trigger animation events that we
  /// want to handle. The events forwarding is disabled by default.
  /// \param enable Set to true to enable events forwarding, false to disable it.
  void SetForwardingAnimEvents(boolean enable);
  
  /// \brief Adds a listener to the state machine that will receive transition events via the MessageFunction.
  /// \param pObj Base object to be added as a listener, i.e. an entity.
  /// \return Index of the listener in the internal list.  
  int AddEventListener(VisTypedEngineObject_cl* pObj);
  
  /// \brief Removes a listener from the transition state machine.
  /// \param pObj Base object to be removed as a listener, i.e. an entity.
  /// \return TRUE if the listener could be removed, FALSE otherwise.
  bool RemoveEventListener(VisTypedEngineObject_cl* pObj);
  
  /// @}
};

#endif