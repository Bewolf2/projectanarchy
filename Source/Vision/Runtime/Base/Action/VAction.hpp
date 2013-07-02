/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VAction.hpp

#ifndef _VISION_VACTION_HPP
#define _VISION_VACTION_HPP

#include <Vision/Runtime/Base/System/ModuleSystem/VModule.hpp>

class VArgList;
class VActionManager;


/// \brief
///   Base class for all actions
/// 
/// The VAction class has four abstract function which all derived classes must implement. The
/// Do(), Redo(), Undo() and IsUndoable() function.
/// 
/// The Do() function gets a argument list object and can query action flags and values from this
/// list. It normally sets up all needed action data (stored in class variables) and calls Redo.
/// 
/// The Redo(), if it is called from Do(), does the action the first time or it is called by the
/// UndoRedo stack to redo the action. It uses the action data to redo the action.
/// 
/// The UndoRedo stack calls the Undo() function to restore the old state before a Do() or a Redo()
/// call.
/// 
/// The Undo() call from the UndoRedo stack is only possible if the  IsUndoable() returns \c TRUE. It
/// signals that the action can be undone. Some actions which are not be undoable must return
/// \c FALSE.
/// 
/// \note
///   The variable m_szActionString is used to display the action data after the execution (for
///   example: as a readable representation of the undo stack).
/// 
/// \note
///   The action class has static functions for creation and execution of an action.
/// 
/// \note
///   You should use the macros for defining, implementing, registering and execution of your action calls.
class VAction : public VTypedObject
{
  V_DECLARE_DYNAMIC_DLLEXP(VAction,VBASE_IMPEXP)
  friend struct VActionType;
  friend class VActionManager;

public:
  
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VAction();
  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VAction();

  ///
  /// @}
  ///

  ///
  /// @name Action overridables
  /// @{
  ///


  /// \brief
  ///   Overridable Do function. This function is called when the action is executed. Return \c TRUE if the operation was successful.
  VBASE_IMPEXP virtual VBool Do(const VArgList &argList) { return FALSE; }

  /// \brief
  ///   Overridable Redo function. This function is called when the action is executed again. \c Return TRUE if the operation was successful
  VBASE_IMPEXP virtual VBool Redo() { return FALSE; }

  /// \brief
  ///   Overridable Undo function. This function is called when the action manager wants to undo the operation. Return \c TRUE if the operation was successful
  VBASE_IMPEXP virtual VBool Undo() { return FALSE; }

  /// \brief
  ///   Override and return \c TRUE to activate Undo/Redo capabilities of the action
  VBASE_IMPEXP virtual VBool IsUndoable() const { return FALSE; }

  /// \brief
  ///   This overridable flag is queried to determine whether log output via GetActionString is used
  VBASE_IMPEXP virtual VBool IsLoggable() const { return TRUE; }

  ///
  /// @}
  ///
  
  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Returns the m_strActionString member which holds the action string
  inline const char* GetActionString() const
  {
    return m_strActionString.AsChar();
  }

  /// \brief
  ///   Return the owner action manager
  inline VActionManager* GetActionManager() const
  {
    return m_pActionManager;
  }

  /// \brief
  ///   Helper member function. Calls respective function on the parent action manager
  VBASE_IMPEXP void Print(const char *szText, ...);

  /// \brief
  ///   Helper member function. Calls respective function on the parent action manager
  VBASE_IMPEXP void PrintWarning(const char *szText, ...);

  ///
  /// @}
  ///
  
private:
  
  VString m_strActionString; ///< the complete action script string (for example: "Create VDagNode3D -n Name;")
  VActionManager* m_pActionManager; ///< Pointer to the action manager which created this action instance. Set in VActionManager::BaseExecuteOneAction and used in the Print functions
};


// VAction pointer list
DECLARE_LIST(VActionList,VAction,VBASE_IMPEXP)

#ifndef _VISION_DOC

/// \brief
///   Dummy structure for the argument string syntax for an action.
///   Used to display a tooltip help for the action.
struct VActionSyntax
{
  const char* m_szSyntax;
  VActionSyntax(const char* szSyntax)
  {
    m_szSyntax = szSyntax;
  }
};




/// \brief
///   Structure for automatically registering a new Action.
struct VActionType
{
  // data
  const char* m_szAction;
  VType *m_Type;
  const char* m_szHelpLocation;
  const char *m_szArgDescription;
  VActionSyntax *m_pSyntax;

  VActionType *m_pNextActionType;

  /// \brief
  ///   Constructor for the new action type.
  inline VActionType(VModule *pModule, const char *scriptaction_name, VType *class_name, const char* szHelpLocation, VActionSyntax* pSyntax, const char *szArgDescription)
  {
    m_szAction = scriptaction_name;
    m_Type = class_name;
    m_szHelpLocation = szHelpLocation;
    m_pSyntax = pSyntax;
    m_szArgDescription = szArgDescription;
    m_pNextActionType = NULL;

    VASSERT(pModule != NULL); ///< action without a plugin source ???
    pModule->RegisterScriptAction(this);
  }
};

#endif // _VISION_DOC



/// \brief
///   Macro use in the declaration of new VAction derived class.
#define V_DECLARE_ACTION(class_name) \
  V_DECLARE_DYNCREATE(class_name) \
  static VActionType actionclass##class_name;

/// \brief
///   Macro use in the source file of new VAction derived class. Pass script name for the action as
///   the frist parameter.
#define V_IMPLEMENT_ACTION(scriptaction_name, class_name, base_class_name, plugin_source, argdescription) \
  V_IMPLEMENT_DYNCREATE(class_name, base_class_name, plugin_source) \
  VActionType class_name::actionclass##class_name(plugin_source, scriptaction_name, class_name::GetClassTypeId(), NULL, NULL, argdescription);


/* IMPORTANT: DO NOT FORGET TO ADD A #DEFINES FOR YOUR ACTION TO THE
   HTMLHELP/VHELPDEFINITIONS.HPP FILE !                               */
#define V_IMPLEMENT_ACTION_AUTOHELP(scriptaction_name, class_name, syntax, base_class_name, plugin_source) \
  V_IMPLEMENT_DYNCREATE(class_name, base_class_name, plugin_source); \
  VActionType class_name::actionclass##class_name(plugin_source, scriptaction_name, class_name::GetClassTypeId(), HELPACTION##class_name, syntax,NULL);



/// \brief
///   Macro for defining a simple action. A simple action can not undone and only implement the
///   Do() function. Since the simple action instance won't be hanging around in memory, you can
///   use static variables instead of member variables in the class. Use this macro by passing you
///   script name for your action and only implement the Do() function.
#define DEFINE_SIMPLE_ACTION(ACTIONNAME,thisplugin,argdescription) \
class ACTIONNAME : public VAction\
{\
  V_DECLARE_ACTION(ACTIONNAME)\
public:\
  ACTIONNAME() {}\
  ~ACTIONNAME() {}\
  virtual VBool Do(const class VArgList &argList);\
  virtual VBool Redo(){ ASSERT(FALSE); return FALSE;}\
  virtual VBool Undo(){ ASSERT(FALSE); return FALSE;}\
  virtual VBool IsUndoable() const { return FALSE; }\
};\
V_IMPLEMENT_ACTION( V_QUOTE(ACTIONNAME), ACTIONNAME ,VAction, thisplugin, argdescription)


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
