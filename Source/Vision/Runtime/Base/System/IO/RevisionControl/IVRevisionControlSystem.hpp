/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVRevisionControlSystem.hpp

#ifndef VISION_IVREVISIONCONTROLSYSTEM_HPP
#define VISION_IVREVISIONCONTROLSYSTEM_HPP

/// \brief This class can be used to get the Revision Control Status of a file.
class VRCSFileStatus
{
public:
  VRCSFileStatus()
  {
     m_iMode = RCS_MODE_ERROR;
  }
  
  /// \brief
  ///  Flags defining the state of a file in the RCS system.
  enum eMode
  { 
    RCS_MODE_ERROR        = V_BIT(0), ///< the state could not be retrieved (file does not exist etc.)
    RCS_MODE_LOCKED_OTHER = V_BIT(1), ///< locked by another user
    RCS_MODE_EDITABLE     = V_BIT(2), ///< the file can be edited
    RCS_MODE_LOCKED       = V_BIT(3), ///< locked by current user (file can be edited by this user)
    RCS_MODE_IS_HEAD_REV  = V_BIT(4)  ///< the current file on disk is updated to the newest revision
  };
  
  /// \brief Whether the file is locked by another user (and thus cannot be edited by this user).
  inline bool IsLockedOther() const {return (m_iMode & VRCSFileStatus::RCS_MODE_LOCKED_OTHER) > 0;}

  /// \brief Whether the file is read-only (ie. cannot be edited).
  inline bool IsReadOnly() const {return (m_iMode & VRCSFileStatus::RCS_MODE_EDITABLE) == 0;}

  /// \brief Whether the file can be edited by this user.
  inline bool IsEditable() const {return (m_iMode & VRCSFileStatus::RCS_MODE_EDITABLE) > 0;}

  /// \brief Returns true, if the file is locked by THIS user and thus can be edited exclusively. Other users are not able to edit the file.
  inline bool IsLocked() const {return (m_iMode & VRCSFileStatus::RCS_MODE_LOCKED) > 0;}

  /// \brief Returns true, if the file on disk is equal to the HEAD revision in the RCS, meaning it is the latest version.
  inline bool IsHeadRevision() const {return (m_iMode & VRCSFileStatus::RCS_MODE_IS_HEAD_REV) > 0;}

  /// \brief Returns an unsigned int that contains the flags (from VRCSFileStatus::eMode) which describe the state of the file.
  inline unsigned int GetMode() const {return m_iMode;}

  /// \brief Returns the name of the user that holds the lock to this file.
  inline const VString& GetLockOwner() const {return m_sOwner;}

  /// \brief Returns the name of the workspace in which the file has been locked.
  inline const VString& GetLockWorkspace() const {return m_sWorkspace;}

  /// \brief Sets the mode flags.
  inline void SetMode(unsigned int iModeFlags) {m_iMode = iModeFlags;}

  /// \brief Sets the lock owner name.
  inline void SetLockOwner(const char* szOwner) {m_sOwner = szOwner;}

  /// \brief Sets the workspace name in which the file is locked.
  inline void SetLockWorkspace(const char* szWorkspace) {m_sWorkspace = szWorkspace;}

  VString m_sOwner;
  VString m_sWorkspace;
  unsigned int m_iMode; ///< VRCSFileStatus::eMode flags
};

/// \brief
///   Revision Control Actions
enum VRevisionControlActions
{
  RCS_UPDATE          = V_BIT(0), ///< Update the file (ie. get the latest revision)
  RCS_EDIT            = V_BIT(1), ///< Check out the file to make it editable.
  RCS_ADD             = V_BIT(2), ///< Add the file to the RCS.
  RCS_REMOVE          = V_BIT(3), ///< Remove the file from the RCS.
  RCS_LOCK            = V_BIT(4), ///< Lock the file for exclusive access by this user.
  RCS_UNLOCK          = V_BIT(5), ///< Unlock the file to give other users access.
  RCS_FILE_MODE_TEXT  = V_BIT(6)  ///< The file is supposed to be used in text mode.
};

/// \brief
///  Revision Control System Interface.
class IVRevisionControlSystem : public VRefCounter
{
public:

  IVRevisionControlSystem() {}
  virtual ~IVRevisionControlSystem() {}

  /// \brief Called to connect to the RCS system. Must be implemented by a derived class. Returns true, if the connection succeeded.
  virtual bool Connect() = 0;

  /// \brief Called to check whether there is a connection to the RCS system. Must be implemented by a derived class.
  virtual bool IsConnected() = 0;
  
  /// \brief Called to disconnect from the RCS system. Must be implemented by a derived class. Returns true, if disconnecting succeeded.
  virtual bool Disconnect() = 0;

  /// \brief Performs one or more actions on a file.
  /// Returns which actions were be performed successfully
  /// \param pszAbsFileName
  ///   The file on which to perform the actions.
  /// \param iActions
  ///   Flags that describe which actions to execute on the file.
  ///   See VRevisionControlActions for the possible flags and their meaning.
  /// \param bIsBinaryFile
  ///   Whether the passed in file is a text file or a binary file (encoding might be handled differently by the RCS system).
  /// \return
  ///   Returns a set of VRevisionControlActions flags, that describes which actions succeeded. In case all actions succeeded,
  ///   the return value will be identical to the value of \a iActions.
  VBASE_IMPEXP virtual int PerformAction(const char *pszAbsFileName, int iActions, bool bIsBinaryFile);
  
  /// \brief Adds a file to revision control.
  virtual bool AddFile(const char *pszAbsFileName, bool bIsBinaryFile) = 0;

  /// \brief Removes a file from revision control.
  virtual bool RemoveFile(const char *pszAbsFileName) = 0;

  /// \brief Updates the file to the latest revision (SVN update,  Perforce sync)
  virtual bool UpdateFile(const char *pszAbsFileName) = 0;

  /// \brief Makes the file editable (writable).
  ///  Returns true if the file doesn't exist, because we're allowed to edit it.
  virtual bool EditFile(const char *pszAbsFileName) = 0;

  /// \brief Checks out the file for exclusive editing by this user.
  ///  (SVN Lock,  Perforce: Edit/Checkout Lock)
  virtual bool LockFile(const char *pszAbsFileName) = 0;
  
  /// \brief Unlocks a file if it hasn't been modified.
  virtual bool UnlockFile(const char *pszAbsFileName) = 0;

  /// \brief Returns whether a file has been modified.
  virtual bool IsModified(const char *pszAbsFileName) = 0;
  
  /// \brief Returns true if the status of the given file could be retrieved. The status is written to the \a stat parameter.
  virtual bool GetFileStatus(const char *pszAbsFileName, VRCSFileStatus &stat) = 0;

  /// \brief Writes the last RCS error message to \a res.
  virtual void GetLastError(VString &res) = 0;

  /// \brief Modifies a setting.
  /// \param pszKey
  ///   One of "user", "password", "server", "workspace"
  /// \param pszValue
  ///   The new value for the setting.
  virtual void Set(const char* pszKey, const char* pszValue) = 0;
  
  /// \brief Retrieves a setting by name.
  /// \param pszKey
  ///   One of "user", "password", "server", "workspace"
  /// \param res
  ///   The variable into which the result is written.
  virtual void Get(const char *pszKey, VString& res) = 0;

  /// \brief Returns the workspace name for the revision controlled file.
  /// \param projectFile
  ///   The file for which to retrieve the workspace name.
  /// \param workspace
  ///   The resulting workspace name.
  /// \return false if \a projectFile is not under revision control.
  virtual bool GetWorkspace(const char* projectFile, VString& workspace) { return false; }
  
  /// \brief Sets the project path.
  virtual void SetProjectFile(const char * projectPath) {}
  
  /// \brief Updates and returns the RCS status.
  /// \return true if RCS is active.
  virtual bool UpdateStatus() { return false; }

  /// \brief
  ///   Returns the name of the changelist the revision control system uses to track the changes made
  ///   through the methods of this class.
  ///
  /// \return
  ///   the name of the changelist in the format suitable for the RCS implemented by
  ///   the actual descendent of this class; \c NULL if either the default changelist
  ///   is used, or the underlying RCS does not support changelists.
  virtual const char* GetChangelist() const { return NULL; }

  /// \brief
  ///   Sets the changelist to be used to track the changes made
  ///   through the methods of this class.
  ///
  /// If the underlying RCS does not support changelists, this method has no effect.
  ///
  /// \param pszChangelist
  ///   The name of the changelist to use, in a format suitable for the underlying RCS.
  ///   Can be \c NULL to specify that the default changelist should be used.
  virtual void SetChangelist(const char* pszChangelist) {}

};

#endif  //VISION_IVREVISIONCONTROLSYSTEM_HPP

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
