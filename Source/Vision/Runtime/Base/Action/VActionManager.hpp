/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VActionManager.hpp

#ifndef MU_VISION_VACTIONMANAGER_HPP
#define MU_VISION_VACTIONMANAGER_HPP

#include <Vision/Runtime/Base/Container/VMapStrToPtr.hpp>
#include <Vision/Runtime/Base/System/SenderReceiver/VBroadcaster.hpp>
#include <Vision/Runtime/Base/Types/VTypeManager.hpp>

struct VActionType;
class VUndoRedoStack;
class IVFileOutStream;
class VAction;


/// \brief
///   Class that manages the action type information of the complete application.
/// 
/// As type information is stored separately per plugin and application,  there is a class for managing the action 
/// type information of the complete application. Each module's actions have to be added to this action manager
/// at module initialization time.
/// 
/// The manager has its own list of type information (references) which is the sum of all plugins added
/// to that manager instance. There are functions to add and remove plugins and to get access to
/// the action type information.
class VActionManager
{
public:
  
  // constructor
  VBASE_IMPEXP VActionManager();

  // destructor
  VBASE_IMPEXP ~VActionManager();
  
  /// \brief
  ///   Executes the passed string
  /// 
  /// The passed string can contain multiple script action separated by semicolons. This
  /// function extracts each script action and calls the VAction::BaseExecuteOneAction to execute
  /// the actions. If one action fails, the function returns FALSE.
  /// 
  /// \param szScriptString
  ///   string with script commands
  /// 
  /// \param undoable
  ///   Pass true if the action should be pushed onto the undo/redo stack
  /// 
  /// \param addScriptToLog
  ///   Pass false if the action shouldn't be added to the action script log
  /// 
  /// \return
  ///   VBool: TRUE if the complete action string could be executed
  /// 
  /// \example
  ///   \code
  ///   m_ActionManager.Execute("Create DagNode; Undo; Redo");
  ///   \endcode
  VBASE_IMPEXP VBool Execute(VString szScriptString, VBool undoable, VBool addScriptToLog = TRUE);
  
  /// \brief
  ///   Opens the passed file, builds one big script string and calls the VAction::Execute
  ///   function. If one action fails the function returns FALSE.
  VBASE_IMPEXP VBool ExecuteScriptFile(const char* szScriptFile, VBool undoable, VBool addScriptToLog = TRUE);  
  

  /// \brief
  ///   Creates an action from a given action classname string
  /// 
  ///  It searches for the VActionType with the passed string. Checks if the class is derived from
  /// VAction, creates an instance of the class and returns the pointer.
  /// 
  /// \param szScriptAction
  ///   action classname string
  /// 
  /// \return
  ///   VAction*: created VAction object
  /// 
  /// \example
  ///   \code
  ///   VAction* pAction = m_ActionManager->Create("ExportV3DAction");
  ///   \endcode
  VBASE_IMPEXP VAction* Create(const char* szScriptAction);
    
  
  /// \brief
  ///   Return the number of registered actions in the global action type list.
  /// 
  /// \return
  ///   int: action type count.
  VBASE_IMPEXP int GetActionTypeCount();

  /// \brief
  ///   Return a VActionType object pointer by index.
  /// 
  /// \param iIndex
  ///   index in the applications action-type list.
  /// 
  /// \return
  ///   VActionType *: NULL if the index was out of bounds.
  /// 
  /// \example
  ///   \code
  ///   for ( int i = 0; i < VActionManager::GetActionTypeCount(); i++ )
  ///   {
  ///     VActionType* pType = g_pApp->m_ActionManager.GetActionType(i);
  ///     ... do something with pType ...
  ///   }
  ///   \endcode
  VBASE_IMPEXP VActionType *GetActionType(UINT iIndex);
 
  /// \brief
  ///   Returns a pointer to the VActionType object for the passed action name.
  /// 
  /// \param szScriptAction
  ///   name (or part of the name) for the action
  /// 
  /// \param bExact
  ///   - if FALSE, you get the first action that has a name starting with the passed string.
  ///   - if TRUE, only an exact string match will result in the corresponding action being returned.
  /// 
  /// \return
  ///   VActionType *: NULL if no action was found.
  VBASE_IMPEXP VActionType *GetActionType(const char* szScriptAction, VBool bExact = TRUE);
  
  /// \brief
  ///   Append all actions names in the passed list that have names starting with the given filter.
  /// 
  /// \param szActionFilter
  ///   Substring the action name has to start with
  /// 
  /// \param actionNameList
  ///   List to hold the action names that were found
  /// 
  /// \example
  ///   \code
  ///   GetActionNameList("VBrush", list);
  ///   list will contain all action names beginning with "VBrush": 
  ///   - "VBrushAction",
  ///   - "VBrushEditMoveTool",
  ///   - "VBrushEditMoveAction"
  ///   - etc...
  ///   \endcode
  VBASE_IMPEXP void GetActionNameList(const char* szActionFilter, VPList &actionNameList);


  /// \brief
  ///   Returns the currently executed action string
  inline const char* GetCurrentAction() const { return m_strCurrentAction.AsChar(); }
  

  /// \brief
  ///   Returns the currently used undo redo stack
  inline VUndoRedoStack* GetUndoRedoStack() const { return m_pUndoRedoStack; }

  /// \brief
  ///   Sets a new undo redo stack (argument could be NULL)
  inline void SetUndoRedoStack(VUndoRedoStack* pNewUndoRedoStack) { m_pUndoRedoStack = pNewUndoRedoStack; }


  /// \brief
  ///   Printf like function for logging normal tests Sends the passed text with the Action
  ///   broadcaster
  VBASE_IMPEXP void Print(const char *szText, ...);

  /// \brief
  ///   Printf like function for logging warning messages Sends the passed text with the Action
  ///   broadcaster
  VBASE_IMPEXP void PrintWarning(const char *szText, ...);
  

  inline void StopLogging(VBool bStopLogging) { m_bStopLogging = bStopLogging; }
  
  
  /// \brief
  ///   Possible values for the VSenderRecevier::OnReceive iMsg variable value
  enum eACTIONBROADCAST_IDS
  {
    ACTION_MSG_TEXT,       ///< message is a text
    ACTION_MSG_WARNING,    ///< message is a warning
    ACTION_MSG_EXCEPTION   ///< action produced an exception
  };
  
  
  // broadcaster for action messages and warnings
  VBroadcaster m_ActionBroadcaster;
  

  // file logging functions
  VBASE_IMPEXP void InternalLog(const char *szText);
  inline void InternalLogActivate(VBool bLog) { m_bInternalLog=bLog; }
  VBASE_IMPEXP void InternalLogClear();
  VBASE_IMPEXP void InternalLogSetFile(const char* szFileName);
  inline const char* InternalLogGetFile() const { return m_vsInternalLogFile.AsChar(); }

  
  // action history functions
  inline const VPList *GetActionStringHistory() {return m_pActionStringHistory;}
  VBASE_IMPEXP void AddToActionStringHistory(const char *szText);
  VBASE_IMPEXP void FreeActionStringHistory();
  
  
  /// \brief
  ///   Helper function that checks if the passed string is a comment or not
  VBASE_IMPEXP static VBool IsComment(const char *szActionString);
  
  
  
  /// \brief
  ///   Register the entire action type information from this module
  /// 
  /// \param pModule
  ///   Plugin to register.
  /// 
  /// \return
  ///   VBool: TRUE if successful.
  VBASE_IMPEXP VBool RegisterModule(VModule *pModule);
  
  /// \brief
  ///   Unregister the entire action type information previously registered from the specified Module.
  /// 
  /// \param pModule
  ///   Plugin to unregister.
  /// 
  /// \return
  ///   VBool: TRUE if successful.
  VBASE_IMPEXP VBool UnregisterModule(VModule *pModule);
  
  /// \brief
  ///   Returns a pointer to the list of registered modules
  inline VModuleList* GetModuleList()  { return &m_moduleList; }

  /// \brief
  ///   Sets a global instance of VActionManager. Used to trigger actions from the resource viewer
  ///   tool
  VBASE_IMPEXP static void SetGlobalManager(VActionManager *pManager);

  /// \brief
  ///   Gets the global instance of VActionManager.
  VBASE_IMPEXP static VActionManager *GetGlobalManager();

protected:
    
  /// \brief
  ///   Get the VActionType static object pointer for the action class specified by action name.
  /// 
  /// Every action class has a static object of type VActionType which holds runtime information about the action.
  /// 
  /// \param szActionName
  ///   action name
  /// 
  /// \return
  ///   VActionType *: NULL if the action wasn't registered.
  VBASE_IMPEXP VActionType *FromName(const char *szActionName);
  
  /// \brief
  ///   Executes the passed script string.
  /// 
  /// This script string must contain the action name and an ending semicolon.
  /// 
  /// The function creates an instance for the action (with VAction::Create), sets up the
  /// argument list and calls the VAction::Do function. If the action is undoable and the undoable
  /// flag is set, the action will be added to the current Undo/Redo stack.
  /// 
  /// If the action fails the function returns FALSE. The function returns TRUE after executing an
  /// action without errors, for a comment string (//), and for an empty string.
  ///
  /// \param sScriptString
  ///   script string to parse
  ///
  /// \param undoable
  ///   indicates whether the action should be undoable or not (requires the action itself to support it)
  ///
  /// \param addScriptToLog
  ///   Indicates whether the script should write into the log
  ///
  VBASE_IMPEXP VBool BaseExecuteOneAction(VString sScriptString, VBool undoable, VBool addScriptToLog = TRUE);
  
  /// \brief
  ///   Private Print function that does not add // in front of the string
  VBASE_IMPEXP void PrintWithoutComments(const char *szText, ...);
  
  
  VMapStrToPtr m_actionNameMap; ///< applications global action type list
  
  // we hold the data in a separate VPList also, for iterating for actions using
  // a part of their name
  // Using a hash table in this case isn't a very good idea. The hash key is generated 
  // from the whole string (name), only a part of the name will generate a different 
  // hash key. In this case, we must use the GetFirst()-GetNext() iterate method, 
  // but this is by far slower as iterating a VPList.
  VPList m_actionTypeList;
  
  // list of registered plugins
  VModuleList m_moduleList; 
  
  // this string contains the current action string which is executed in the BaseExecuteOneAction function
  VString m_strCurrentAction;
  
  // pointer to the used undo redo stack
  VUndoRedoStack *m_pUndoRedoStack;
  
  // if the variable equals TRUE no messages is broadcasted if Print* is called
  VBool m_bStopLogging;
  
  // variables for logging
  IVFileOutStream* m_pInternalLogFile; ///< log file handle
  VBool m_bInternalLog; ///< TRUE if the text from the Prrintf* functions are also wriiten to the log file
  VString m_vsInternalLogFile; ///< log file name
  
  // data members for the action strings history list
  VPList *m_pActionStringHistory;

  
  // compare function for sorting the action names
  VBASE_IMPEXP static int VISION_CDECL ActionNameCompare( const void *arg1, const void *arg2 );

  // helper which checks if the string stars with //
  // if not then the two slashs were added
  VBASE_IMPEXP static void AddCommentsIfNecessary(VString &str);
  
  
  VString m_sFullPathLogFile; ///< full path to log file

  VBASE_IMPEXP static VActionManager *g_pGlobalManager;
};

#endif // MU_VISION_VACTIONMANAGER_HPP

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
