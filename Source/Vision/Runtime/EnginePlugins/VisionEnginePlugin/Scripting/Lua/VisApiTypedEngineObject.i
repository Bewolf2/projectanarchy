/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VisTypedEngineObject_cl;
%nodefaultdtor VisTypedEngineObject_cl;

class VisTypedEngineObject_cl : public VTypedObject
{
public:
  //keep compatiblity with old binding
  %extend {
    int GetComponentCount()
    {
      return self->Components().Count();
    }
    
    bool AddComponent(IVObjectComponent *component)
    {
      if (component == NULL) return false;
      return self->AddComponent(component)==TRUE;
    }

    bool RemoveComponent(IVObjectComponent *component)
    {
      if (component == NULL) return false;
      return self->RemoveComponent(component)==TRUE;
    }
    
    bool RemoveComponentOfType(const char *szTypeName, const char * szOptionalComponentName = NULL)
    {
      if(szTypeName==NULL)
        return false;
      
      IVObjectComponent *pComponent = NULL;
      
      if(szOptionalComponentName)
        pComponent = self->Components().GetComponentOfTypeAndName(szTypeName, szOptionalComponentName);
      else
        pComponent = self->Components().GetComponentOfType(szTypeName);
      
      if(pComponent==NULL)
        return false;
        
      return self->RemoveComponent(pComponent)==TRUE;
    }    
  }

  void RemoveAllComponents();
};

//Implement this method native in order to return the concrete type instance
//instead of the interface IVObjectComponent
%native(VisTypedEngineObject_cl_AddComponentOfType) int VisTypedEngineObject_cl_AddComponentOfType(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_AddComponentOfType(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 1, VisTypedEngineObject_cl, pSelf)

    //param #2: type name of the component
    if(!SWIG_lua_isnilstring(L,2)) luaL_error(L, "Expected a string value as parameter 2 for VisTypedEngineObject_AddComponentOfType");
    const char * szComponentType = lua_tostring(L, 2);       
    
    //param #3: optional name of the component
    const char * szComponentName = NULL;
    if(lua_isstring(L, 3))  szComponentName = lua_tostring(L, 3);
    
    bool bCreateDynamicProperty = true;
    if(lua_isboolean(L, 4)) bCreateDynamicProperty = lua_toboolean(L,4)==TRUE;

    // Search for a component type with the specified name
    VType *pType = Vision::GetTypeManager()->GetType(szComponentType);
    if (pType==NULL)
    {
      Vision::Error.Warning("[Lua] AddComponentOfType: did not find component of the specified type: %s", szComponentType);
      lua_settop(L, 0); //remove all items from the stack
      lua_pushnil(L);
      return 1;
    }

    // Create an instance of that component type
    VSmartPtr<IVObjectComponent> spComponent = (IVObjectComponent *)pType->CreateInstance();
    if (spComponent==NULL)
    {
      Vision::Error.Warning("[Lua] AddComponentOfType: Failed construction an instance of the specified component type: %s", szComponentType);
      lua_settop(L, 0); //remove all items from the stack
      lua_pushnil(L);
      return 1;
    }

    if(szComponentName && bCreateDynamicProperty)
    {
      spComponent->SetComponentName(szComponentName);
        
      //prepare stack in order to set the result of the casted object component as property
      //remove everything, expect self
      lua_settop(L, 1);                                           //stack: userdata, TOP
      lua_pushstring(L, szComponentName);                         //stack: userdata, key, TOP
      LUA_PushObjectProxy(L, spComponent, pSelf);                  //stack: userdata, key, result, TOP

      //set the dynamic property
      VisionLuaClassSet(L);                                       //stack: userdata, key, result, TOP
      lua_replace(L, 1);                                          //stack: result, key, TOP
      lua_pop(L, 1);                                              //stack: result, TOP

    }
    else
    {   
      //if the component has a name, but the user likes to avoid a dynamic property,
      //we only set the name of the component and try to cast
      
      if(szComponentName) spComponent->SetComponentName(szComponentName);
      
      //clean stack (remove all call params including self)
      lua_settop(L, 0);                                          //stack: TOP
      LUA_PushObjectProxy(L, spComponent, pSelf);                 //stack: result, TOP
    }

    // Check if the component can actually be added. Don't do it if it can't.
    VString sCanAddError;
    if (!pSelf->CanAddComponent(spComponent, sCanAddError))
    {
      Vision::Error.Warning("[Lua] AddComponentOfType: Can't add component of type '%s'. Error: %s", szComponentType, sCanAddError.AsChar());
      lua_settop(L, 0); //remove all items from the stack
      lua_pushnil(L);
      return 1;
    }

    // Add the component to the current object
    pSelf->AddComponent(spComponent);
    
    return 1;
  }
%}

//Implement this method native in order to return the concrete type instance
//instead of the interface IVObjectComponent
%native(VisTypedEngineObject_cl_GetComponentAt) int VisTypedEngineObject_cl_GetComponentAt(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_GetComponentAt(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 1, VisTypedEngineObject_cl, pSelf)

    //param #2: type name of the component
    if(!lua_isnumber(L,2)) luaL_error(L, "Expected a numeric index as parameter 2 for VisTypedEngineObject_cl_GetComponentAt");
    int iIndex = (int)lua_tonumber(L, 2);
   
    lua_settop(L, 0);                                              //stack: TOP
    if(iIndex>0 && iIndex<=pSelf->Components().Count())
      LUA_PushObjectProxy(L, pSelf->Components().GetAt(iIndex-1)); //stack: result, TOP
    else
      lua_pushnil(L);                                              //stack: nil, TOP

    return 1;
  }
%} 


//Shortcut
%native(VisTypedEngineObject_cl_AddTriggerTarget) int VisTypedEngineObject_cl_AddTriggerTarget(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_AddTriggerTarget(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl)
    
    //insert the class name of the component as parameter #2
    lua_pushstring(L, "VisTriggerTargetComponent_cl");
    lua_insert(L, 2);
    
    return VisTypedEngineObject_cl_AddComponentOfType(L);
  }
%}

//Shortcut
%native(VisTypedEngineObject_cl_AddTriggerSource) int VisTypedEngineObject_cl_AddTriggerSource(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_AddTriggerSource(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl)
    
    //insert the class name of the component as parameter #2
    lua_pushstring(L, "VisTriggerSourceComponent_cl");
    lua_insert(L, 2);
    
    return VisTypedEngineObject_cl_AddComponentOfType(L);
  }
%}

//Shortcut
%native(VisTypedEngineObject_cl_AddAnimation) int VisTypedEngineObject_cl_AddAnimation(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_AddAnimation(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl)
    
    //insert the class name of the component as parameter #2
    lua_pushstring(L, "VAnimationComponent");
    lua_insert(L, 2);
    
    return VisTypedEngineObject_cl_AddComponentOfType(L);
  }
%}

//Shortcut
//%native(VisTypedEngineObject_cl_AddTimedValue) int VisTypedEngineObject_cl_AddTimedValue(lua_State *L);
//%{
//  SWIGINTERN int VisTypedEngineObject_cl_AddTimedValue(lua_State *L)
//  {
//    IS_MEMBER_OF(VisTypedEngineObject_cl)
//    
//    //insert the class name of the component as parameter #2
//    lua_pushstring(L, "VTimedValueComponent");
//    lua_insert(L, 2);
//    
//    return VisTypedEngineObject_cl_AddComponentOfType(L);
//  }
//%}

//Shortcut
%native(VisTypedEngineObject_cl_AddTransitionStateMachine) int VisTypedEngineObject_cl_AddTransitionStateMachine(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_AddTransitionStateMachine(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl)
    
    //insert the class name of the component as parameter #2
    lua_pushstring(L, "VTransitionStateMachine");
    lua_insert(L, 2);
    
    return VisTypedEngineObject_cl_AddComponentOfType(L);
  }
%}
  

//Implement this method native in order to return the concrete type instance
//instead of the interface IVObjectComponent
%native(VisTypedEngineObject_cl_GetComponentOfType) int VisTypedEngineObject_cl_GetComponentOfType(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_GetComponentOfType(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 1, VisTypedEngineObject_cl, pSelf)

    //param #2: type name of the component
    if(!SWIG_lua_isnilstring(L,2)) luaL_error(L, "Expected a string value as parameter 2 for VisTypedEngineObject_AddComponentOfType");
    const char * szComponentType = lua_tostring(L, 2);       
    
    //param #3: optional name of the component
    const char * szComponentName = NULL;
    if(lua_isstring(L, 3))  szComponentName = lua_tostring(L, 3);
    
    IVObjectComponent *pComponent = NULL;
    
    if(szComponentName)
      pComponent = pSelf->Components().GetComponentOfTypeAndName(szComponentType, szComponentName);
    else
      pComponent = pSelf->Components().GetComponentOfType(szComponentType);    

    //clean stack (remove all call params including self)
    lua_settop(L, 0);                                             //stack: TOP

    if(pComponent) LUA_PushObjectProxy(L, pComponent);            //stack: result, TOP
    else           lua_pushnil(L);                                //stack: nil, TOP

    return 1;
  }
%} 

//Implement this method native in order to return the concrete type instance
//instead of the interface IVObjectComponent
%native(VisTypedEngineObject_cl_GetComponentOfBaseType) int VisTypedEngineObject_cl_GetComponentOfBaseType(lua_State *L);
%{
  SWIGINTERN int VisTypedEngineObject_cl_GetComponentOfBaseType(lua_State *L)
  {
    IS_MEMBER_OF(VisTypedEngineObject_cl) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 1, VisTypedEngineObject_cl, pSelf)

    //param #2: type name of the component
    if(!SWIG_lua_isnilstring(L,2)) luaL_error(L, "Expected a string value as parameter 2 for VisTypedEngineObject_AddComponentOfType");
    const char * szBaseTypeName = lua_tostring(L, 2);       
    
    //param #3: optional name of the component
    const char * szComponentName = NULL;
    if(lua_isstring(L, 3))  szComponentName = lua_tostring(L, 3);
        
    IVObjectComponent *pComponent = NULL;
    
    if(szComponentName)
      pComponent = pSelf->Components().GetComponentOfBaseTypeAndName(szBaseTypeName, szComponentName);
    else
      pComponent = pSelf->Components().GetComponentOfBaseType(szBaseTypeName);

    //clean stack (remove all call params including self)
    lua_settop(L, 0);                                             //stack: TOP

    if(pComponent) LUA_PushObjectProxy(L, pComponent);            //stack: result, TOP
    else           lua_pushnil(L);                                //stack: nil, TOP
    
    return 1;
  }
%}

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisTypedEngineObject_cl, 128, "%s", self->GetClassTypeId()->m_lpszClassName)
VSWIG_CREATE_TOSTRING(VisTypedEngineObject_cl, "%s: 0x%p", self->GetClassTypeId()->m_lpszClassName, self)

#else

/// \brief Base class for any engine object.
/// \note
///   Lua engine objects, such as VisBaseEntity_cl, have no constructor and most cannot be instantiated from Lua.
///   Certain objects, however, can be created with special create functions, e.g. the \e Game:Create* functions.
///   Note that types such as hkvVec3 and VColorRef \e are simple engine objects, which are not ref counted and not
///   derived from VisTypedEngineObject_cl, accordingly there is a constructor for these types.
class VisTypedEngineObject_cl : public VTypedObject {
public:

  /// @name Object Components
  /// @{
  
  /// \brief Returns the number of attached components.
  /// \return The number of currently attached components
  /// \par Example
  ///   \code
  ///     local numComponents = self:GetComponentCount();
  ///   \endcode
  number GetComponentCount();

  /// \brief Get the component of a specified index (starting with 1, not 0 like in C++).
  /// \param index The 1-based index of the required component.
  /// \return The component, or nil if the index is out of valid range.
  /// \par Example
  ///   \code
  ///     local component = self:GetComponentAt(1);
  ///   \endcode
  mixed GetComponentAt(number index);

  /// \brief Loops through the collection of components and returns the first instance (or nil) with the exact specified type (and name - if specified).
  /// \param typeName Name of the component's type (i.e. class name)/
  /// \param componentName (\b optional) Name of the desired component.
  /// \return The component, or nil if not found.
  /// \par Example
  ///   \code
  ///     local blobShadow = self:GetComponentOfType("VBlobShadow");
  ///   \endcode
  mixed GetComponentOfType(string typeName, string componentName = nil);

  /// \brief Loops through the collection of object components and returns the first instance (or nil) of specified type or derived class (and name - if specified).
  /// \param typeName Name of the component's type (i.e. class name)
  /// \param componentName (\b optional) Name of the desired component.
  /// \return The component, or nil if not found
  /// \see GetComponentOfType
  IVObjectComponent GetComponentOfBaseType(string typeName, string componentName = nil );

  /// \brief Adds a single component to the collection of components
  /// \param component The object component to be added
  /// \return true = operation was successful, false = the component could not be added
  /// \note Most of the time AddComponentOfType is more convenient!
  /// \par Example
  ///   \code
  ///     local blobShadow = Game:CreateComponent("VBlobShadow")
  ///     entity:AddComponent(blobShadow)
  ///   \endcode
  /// \see VisGame_cl:CreateComponent
  /// \see AddComponentOfType
  boolean AddComponent(IVObjectComponent component);
  
  /// \brief Creates and adds a single component to the collection of components.
  /// In addition it will create an dynamic property for the owner entity with the specified name.
  /// \param typeName Name of the component's type (i.e. class name)
  /// \param componentName (\b optional) Name of the desired component.
  /// \param createDynamicProperty (\b optional) Set to false if you would like to avaoid the creation of a dynamic property.
  /// \return The component or nil if it there is no class of the specified type or the component is not attachable to this entity.
  /// \par Example
  ///   \code
  ///     self:AddComponentOfType("MyCustomComponentClass", "MyComponent") --creates a dynamic property MyComponent
  ///     self.MyComponent:DoSomething() --call member method on the instance of MyCustomComponentClass
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see AddTriggerSource
  /// \see AddTriggerTarget
  /// \see AddAnimation
  /// \see AddTransitionStateMachine
  mixed AddComponentOfType(string typeName, string componentName = nil, boolean createDynamicProperty = false);

  /// \brief Shortcut for AddComponentOfType("VisTriggerSourceComponent_cl", ...): Creates and adds a trigger source component to the collection of components.
  /// In addition it will create an dynamic property for the owner entity with the specified name.
  /// \param componentName (\b optional) Name of the desired component.
  /// \param createDynamicProperty (\b optional) Set to false if you would like to avaoid the creation of a dynamic property.
  /// \return A VisTriggerSourceComponent_cl component
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       self:AddTriggerSource("MySource") --creates a dynamic property MySource
  ///     end
  ///  
  ///     function OnAfterSceneLoaded(self)
  ///       -- there needs to be a trigger target called MyTarget (see TriggerTarget Sample)
  ///       self.MySource:LinkToTarget("MyTarget") 
  ///       self.once = false
  ///     end
  ///
  ///     function OnThink(self)
  ///       -- trigger all targets when this entity reaches the zero line of the x-axis
  ///       if self:GetPosition().x == 0 and not self.once then
  ///         self.MySource:TriggerAllTargets()
  ///         self.once = true
  ///       end
  ///     end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see AddTriggerTarget
  /// \see AddComponentOfType
  /// \see VisTriggerSourceComponent_cl
  VisTriggerSourceComponent_cl AddTriggerSource(string componentName = nil, boolean createDynamicProperty = false);
  
  /// \brief Shortcut for AddComponentOfType("VisTriggertTargetComponent_cl", ...): : Creates and adds a trigger target component to the collection of components.
  /// In addition it will create an dynamic property for the owner entity with the specified name.
  /// \param componentName (\b optional) Name of the desired component.
  /// \param createDynamicProperty (\b optional) Set to false if you would like to avaoid the creation of a dynamic property.
  /// \return A VisTriggertTargetComponent_cl component
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       self:AddTriggerTarget("MyTarget") --creates a dynamic property MyTarget (see TriggerSource Sample)
  ///     end
  ///  
  ///     function OnTrigger(self, source, target)
  ///       -- use tostring function to avoid problems with nil values (which occur in case a trigger source or target component has no name)
  ///       Debug:PrintLine(tostring(target) .. " has been triggered by " .. tostring(target))
  ///     end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see AddTriggerSource
  /// \see AddComponentOfType
  /// \see VisTriggertTargetComponent_cl
  VisTriggertTargetComponent_cl AddTriggerTarget(string componentName = nil, boolean createDynamicProperty = false);
  
  /// \brief Shortcut for AddComponentOfType("VAnimationComponent", ...): : Creates and adds an animation component to the collection of components.
  /// In addition it will create an dynamic property for the owner entity with the specified name.
  /// \b Every \b Lua \b created \b entity \b will \b already \b contain \b an \b animation \b component \b with \b a \b dynamic \b property \b "Animation".
  /// \param componentName (\b optional) Name of the desired component.
  /// \param createDynamicProperty (\b optional) Set to false if you would like to avaoid the creation of a dynamic property.
  /// \return A VAnimationComponent component
  /// \note This animation component is designed for Lua scripting purpose only.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       self:AddAnmiation("Animation")
  ///       self:Animation:Play("Run")
  ///
  ///       -- but if the entity was not created inside a Lua script, you need to add this component
  ///       local anotherEntity = Game:GetEntity("StreetWorker")
  ///       anotherEntity:AddAnmiation("Ani") -- we add the component with the name "Ani" (used as dynamic property)
  ///       anotherEntity.Ani:Play("Drill") -- access the component via the dynamic property
  ///     end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaAnimation.scene", Warrior #1.
  /// \see AddComponentOfType
  /// \see VAnimationComponent
  VAnimationComponent AddAnimation(string componentName = nil, boolean createDynamicProperty = false);
  
  /// \brief Shortcut for AddComponentOfType("VTransitionStateMachine", ...): : Creates and adds an TransitionStateMachine component to the collection of components.
  /// In addition it will create an dynamic property for the owner entity with the specified name.
  /// \param componentName (\b optional) Name of the desired component.
  /// \param createDynamicProperty (\b optional) Set to false if you would like to avaoid the creation of a dynamic property.
  /// \return A VTransitionStateMachine component
  /// \par Example
  ///   \code
  ///   function OnAfterSceneLoaded(self)
  ///     self:AddTransitionStateMachine("Transition") -- create with dynamic property "Transition"
  ///     self.Transition:LoadFromFile("Models\\Warrior\\WarriorEvents.vTransition", "Idle")
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
  /// \see AddComponentOfType
  /// \see VTransitionStateMachine
  VTransitionStateMachine AddTransitionStateMachine(string componentName = nil, boolean createDynamicProperty = false);
  
  /// \brief Removes a single component from the component list of this object
  /// \param component The object component to be removed
  /// \return true = successful removed the component, false = component could not be removed
  /// \see VisGame_cl::CreateComponent
  /// \see AddComponent
  /// \see AddComponentOfType
  /// \see RemoveComponentOfType
  boolean RemoveComponent(IVObjectComponent component);
  
  /// \brief Removes a single component from the component list of this object via the type and name of the component.
  /// \param typeName Name of the component's type (i.e. class name).
  /// \param componentName (\b optional) Name of the desired component (if not specified the first component with a matching type will be deleted).
  /// \return true = successful removed the component, false = component could not be removed
  /// \see VisGame_cl::CreateComponent
  /// \see AddComponent
  /// \see AddComponentOfType
  /// \see RemoveAllComponents
  boolean RemoveComponentOfType(string typeName, string componentName = nil)
  
  /// \brief Removes all components from the component list of this object
  /// \see VisGame_cl::CreateComponent
  /// \see AddComponent
  /// \see AddComponentOfType
  /// \see RemoveComponentOfType
  void RemoveAllComponents();
  
  /// @}  
  /// @name Operators
  /// @{
  
  /// \brief Compare with another VisTypedEngineObject_cl, e.g. obj1 == obj2
  /// \param other The other typed engine object.
  /// \return true if the internal pointers are equal, otherwise false.
  boolean operator == (VisTypedEngineObject_cl other);
  
  /// @}  
  /// @name Callbacks
  /// @{
  
  /// \brief Overridable callback function. Called before a scene is being updated.
  /// \param self The object to which this script is attached.
  void OnUpdateSceneBegin(mixed self);

  /// \brief Overridable callback function. Called after a scene was updated.
  /// \param self The object to which this script is attached.
  void OnUpdateSceneFinished(mixed self);

  /// \brief Overridable callback function. Called after a scene has been loaded.
  /// \param self The object to which this script is attached.
  /// \note The scene script even provides this callback, without a self parameter.
  /// \details This callback is called when all scene objects have been created. It is thus
  ///          a useful callback for resolving references to other entities and objects.
  void OnAfterSceneLoaded(mixed self);

  /// \brief Overridable callback function. Called when the script is connected to an object.
  /// \details This callback is useful for one-time initialization of object-specific variables and properties.
  /// \param self The object to which this script has just been attached.
  /// \note OnCreate is not called upon de-serialization of an object and its script.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       -- the script is attached to a VisObject3D_cl object
  ///       self:SetPosition(-400, 0, 100)
  ///     end
  ///   \endcode
  /// \see OnDestroy
  /// \see Timer::GetTimeDiff
  void OnCreate(mixed self);

  /// \brief Overridable callback function. Called when the script is detached of an object.
  /// \details This callback is useful for one-time deinitialization of object-specific variables and properties.
  /// \param self The object of which this script has just been detached.
  /// \note OnDestroy is called when detaching the script from the owner, thus it can be used for cleanup purposes.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       self.warrior = Game:CreateEntity(Vision.hkvVec3(),"Warrior", "Warrior.model")
  ///     end
  ///     
  ///     function OnDestroy(self)
  ///       self.warrior:Remove()
  ///     end
  ///   \endcode
  /// \see OnCreate
  void OnDestroy(mixed self);

  /// \brief Overridable callback function. Called in regular intervals.
  /// \param self The object to which this script is attached.
  /// \note To determine the time interval you can use the VisionTimer::GetTimeDelta function.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       self.TimeDiff = 0
  ///     end
  ///
  ///     function OnThink(self)
  ///       self.TimeDiff = self.TimeDiff + Timer:GetTimeDiff()
  ///       -- toggle visibility in 3 sec cycles
  ///       if self.TimeDiff > 3 then
  ///         self:SetVisible(not self:GetVisible())
  ///         self.TimeDiff = self.TimeDiff - 3
  ///       end
  ///     end
  ///   \endcode
  /// \see Timer::GetTimeDiff
  /// \see OnCreate
  void OnThink(mixed self);

  /// \brief Overridable callback function. Called when the object is saved to or loaded from disk.
  /// \param self The object to which this script is attached.
  /// \param ar The object that controls the serialization. It has functions for writing out and reading in data.
  /// \par Example
  ///   \code
  /// function OnSerialize(self, ar)
  ///   if (ar:IsLoading()) then
  ///     local s = ar:Read()
  ///   else
  ///     ar:Write("Hello, Lua")
  ///   end
  /// end
  ///   \endcode
  /// \see VArchive::IsLoading
  /// \see VArchive::IsSaving
  /// \see VArchive::Read
  /// \see VArchive::Write
  void OnSerialize(mixed self, VArchive ar);

  /// \brief Overridable callback function. Called when the object collides with another object or a world polygon.
  /// \param self The object to which this script is attached.
  /// \param info A table containing the following fields:
  ///   - hkvVec3 "HitPoint": collision type of this object
  ///   - hkvVec3 "HitNormal": collision type of the collider
  ///   - hkvVec3 "RelativeVelocity": this object
  ///   - hkvVec3 "Force": this object (0 for Havok Physics)
  ///   - string "ColliderType": the typpe object that we collided with
  ///   - mixed "ColliderObject": the distance from this object to the collision point
  /// \note Possible collider types are:
  ///   - "Entity" Collision with an entity (rigid body or character controller)
  ///   - "Mesh" Collision eit a static mesh
  ///   - "Terrain" Collision with the terrain
  ///   - "Decoration" Collisionn with a decoration group
  /// \par Example
  ///   \code
  ///     function OnCollision(self, info)
  ///       Debug:PrintLine("Collision at " .. tostring(info.HitPoint) .. " with " .. info.ColliderType .. ", force: " .. info.Force)
  ///     end
  ///   \endcode
  void OnCollision(mixed self, table info);

  /// \brief Overridable callback function. Called when an animation event occurs.
  /// \param self The object to which this script is attached.
  /// \param eventName The name of the event, in some cases this could also be a number (When there is no name for this event).
  /// \param animSeqName The sequence name where the animation event happend.
  /// \par Example
  ///   \code
  ///    function OnAnimationEvent(self, eventName, animSeqName)
  ///      Debug:PrintLine(self:GetKey()..": Animation event '" .. eventName .. "' in sequence " .. animSeqName)
  ///    end
  ///   \endcode
  /// \see VAnimationComponent
  /// \see vForge Sampe "Scripting/LuaAnimation.scene", Warrior #1.
  void OnAnimationEvent(mixed self, mixed eventName, string animSeqName);
  
  /// \brief Overridable callback function. Called when an transition event occurs.
  /// \param self The object to which this script is attached.
  /// \param eventType The event type as one of the following constants (or a customized):
  ///   - Vision.EVENT_TRANSITION_STARTED: A transition started.
  ///   - Vision.EVENT_TRANSITION_FINISHED: A transition finished.
  ///   - Vision.EVENT_SEQUENCE_STARTED: A sequence has been started.
  ///   - Vision.EVENT_SEQUENCE_FINISHED: A sequence has been finished.
  ///   - Vision.EVENT_TRANSITION_START_REACHED: Reached the start of a transition.
  /// \par Example
  ///   \code
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
  /// \see VTransitionStateMachine
  /// \see vForge Sampe "Scripting/LuaAnimation.scene", Warrior #2.
  void OnTransitionEvent(mixed self, number eventType);

  /// \brief Overridable callback function. Called when an trigger event occurs.
  /// \param self The object to which this script is attached.
  /// \param source The name of the source component (nil if the source component has no name).
  /// \param target The name of the target trigger component (nil if the target component has no name).
  /// \par Example
  ///   \code
  ///     function OnTrigger(self, source, target)
  ///       -- use tostring function to avoid problems with nil values
  ///       Debug:PrintLine(tostring(target) .. " has been triggered by " .. tostring(target))
  ///     end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaTrigger.scene", Lua scripts attached to the Entity "SlowRobot" or "FastRobot".
  /// \see AddTriggerSource
  /// \see AddTriggerTarget
  void OnTrigger(mixed self, string source, string target);
  
  /// @}
  
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
