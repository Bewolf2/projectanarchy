/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptComponent.hpp

#ifndef VSCRIPTCOMPONENT_HPP_INCLUDED
#define VSCRIPTCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponent.hpp>
class VScriptInstance;

/// \class  VScriptMember
/// \brief The Script member class is used to remebmer member variables created in the OnExpose callback.
class VScriptMember
{
public:

  /// \brief  The default constructor is just used in collections, please use VScriptMember(name, value, type) instead.
  VScriptMember();

  /// \brief  Constructor.
  VScriptMember(const char * szName, const char * szValue = NULL, const char * szType = NULL);

  /// \brief  Destructor.
  ~VScriptMember();

  /// \brief  Sets a new value for this member.
  /// \param  szValue The new value regardless of the current type.
  void SetValue(const char * szValue);

  /// \brief  Gets the type as const char* (could be NULL)
  inline const char * GetType() { return m_szType; }
  /// \brief  Gets the name as const char*
  inline const char * GetName() { return m_szName; }
  /// \brief  Gets the value as const char* (could be NULL)
  inline const char * GetValue()  { return m_szValue; }

  /// \brief  Assignment operator.
  /// \param  copy  The script member to copy.
  void operator=(const VScriptMember &copy);

  /// \brief  Equality operator. Checks just the name!
  /// \param  other  The variable used for comparison.
  /// \return true if the variables are considered equivalent.
  bool operator==(const VScriptMember & other) const;

  /// \brief  Not Equality operator. Checks just the name!
  /// \param  other  The variable used for comparison.
  /// \return true if the variables are NOT considered equivalent.
  bool operator!=(const VScriptMember & other) const;

protected:
  const char *m_szType;
  const char *m_szName;
  const char *m_szValue;
};

/// \brief
///   Object component class that provides scripting support
/// 
/// This component keeps a pointer to the script instance and additionally a LUA reference ID of
/// the wrapper object.
class VScriptComponent : public IVObjectComponent, public IVisCallbackHandler_cl 
{
public:
  V_DECLARE_SERIAL_DLLEXP( VScriptComponent, SCRIPT_IMPEXP )
  V_DECLARE_VARTABLE(VScriptComponent, SCRIPT_IMPEXP)

  /// \brief
  ///   Constructor
  SCRIPT_IMPEXP VScriptComponent();

  /// \brief
  ///   Destructor
  SCRIPT_IMPEXP ~VScriptComponent();

  /// \brief
  ///   Sets the script file used by this component
  ///
  /// The script file needs to be set before the component is attached to its
  /// parent object (via SetOwner). Changing the script file afterwards will not
  /// have any effect.
  ///
  /// Please note that the script file property is not serialized. Instead you can
  /// access the script instance directly once the script component is serialized to
  /// file, e.g. after being exported from vForge.
  ///
  /// \param szScriptFile
  ///   Script file to load for this component
  inline void SetScriptFile(const char *szScriptFile) { ScriptFile = szScriptFile; }


  /// \brief
  ///   Overridden IVObjectComponent::SetOwner function
  SCRIPT_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Retrieve a display name that shows the filename
  SCRIPT_IMPEXP void GetCustomDisplayName(VString &sDestName);

  /// \brief
  ///   Internal use: Returns the LUA ID of the wrapper object
  inline int GetScriptRefID() const 
  { 
    return m_iScriptRefID; 
  }

  /// \brief
  ///   Internal use: Sets the wrapper object ID
  inline void SetScriptRefID(int iScriptRefID)
  { 
    m_iScriptRefID = iScriptRefID;
  }

  /// \brief
  ///   Returns the script instance.
  inline VScriptInstance* GetScriptInstance() const
  { 
    return m_spInstance;
  }

  /// \brief
  ///   Sets the script instance
  SCRIPT_IMPEXP void SetScriptInstance(VScriptInstance* pInstance);

  /// \brief
  ///   Implements IVisCallbackHandler_cl::OnHandleCallback
  SCRIPT_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  /// \brief
  ///   Implements VisTypedEngineObject_cl::MessageFunction
  SCRIPT_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  /// \brief
  ///   Respond to variable changes in the editor
  SCRIPT_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

  /// \brief
  ///   Serializes this component
  SCRIPT_IMPEXP virtual void Serialize( VArchive &ar );

  /// \brief
  ///   Internal use: Registers the component with callbacks depending on m_iFunctions flags (i.e.
  ///   which functions were present in the script instance)
  SCRIPT_IMPEXP void RegisterCallbacks();
  
  /// \brief
  ///   Internal use: Deregisters the component with callbacks
  SCRIPT_IMPEXP void DeregisterCallbacks();

  /// \brief
  ///   Internal use: Returns the flags for the functions present in the script instance. Flags are
  ///   constants of type VSCRIPT_FUNC_xyz
  SCRIPT_IMPEXP int GetSupportedFunctions();

  /// \brief
  ///   Static helper function that returns a set of flags indicating which functions were present
  ///   on the script instance. Flags are constants of type VSCRIPT_FUNC_xyz
  SCRIPT_IMPEXP static int CheckAvailableFunctions(VScriptInstance* pInstance);

  /// \brief Enable or disable script think function
  /// \param bStatus Set to true to enable, false to disable.
  inline void SetThinkFunctionStatus(bool bStatus) { m_bScriptThinkEnabled = bStatus; }

  /// \brief Check the status of the script think function
  /// \return true if enabled, false to disabled.
  inline bool GetThinkFunctionStatus() { return m_bScriptThinkEnabled; }

  /// \brief
  ///   A fast way to test whether the script has any of the pre-defined functions passed as bitflags to this function. Flags are constants of type VSCRIPT_FUNC_xyz.
  inline bool HasFunction(int iFunctionConst) const
  {
    return (m_iFunctions & iFunctionConst) != 0;
  }

  /// \brief  Sets a member variable defined in the OnExpose callback (if not present it will create a new variable!)
  /// \param  name  The name of the member.
  /// \param  value The value of the member (the type is evaluated by the value!).
  /// \return true if it succeeds, false if it fails.
  SCRIPT_IMPEXP virtual bool SetVariable(const char * name, const char * value);

#ifdef WIN32

  /// \brief Collecta all (member) variables defined in the OnExpose callback
  /// \param  customVarList The collection to store the found members.
  /// \return The number of collected variables
  SCRIPT_IMPEXP virtual int GetCustomVariableInfo(DynObjArray_cl<VisVariable_cl> &customVarList);
#endif // WIN32

private:
  void TriggerOnExpose();

protected:  
  VScriptInstancePtr m_spInstance;
  VisAnimControl_cl *m_pLastSenderAnimControl;
  int m_iScriptRefID;
  int m_iFunctions;   ///<Cached list of flags for which functions were present in the script instance (VSCRIPT_FUNC_xyz constants)
  VString ScriptFile;
  bool m_bScriptThinkEnabled;

  LinkedList_cl<VScriptMember> m_CustomExposeVars;  
  LinkedList_cl<VScriptMember> m_DefaultExposeVars; 
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
