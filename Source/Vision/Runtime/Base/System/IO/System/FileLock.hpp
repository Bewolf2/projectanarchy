/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file FileLock.hpp

#ifndef FILELOCK_HPP_INCLUDED
#define FILELOCK_HPP_INCLUDED

/// \brief
///   Helper class to keep a lock on a file. Also works over the network, or with the currently active revision control system.
class VFileLock
{
public:
  /// \brief
  ///   Constructor.
  VBASE_IMPEXP VFileLock() : m_bMemoryLock(false), m_bRCSLock(false), m_pStream(NULL), m_pManager(NULL) {}
  
  /// \brief
  ///   Destructor; unlocks any file locked by this VFileLock instance.
  VBASE_IMPEXP ~VFileLock() {UnlockFile();}

  /// \brief
  ///   Sets the file to be locked. If the given file cannot be found, the functions returns false and no file is set.
  ///
  /// \param szFilename
  ///   The file to be locked. If a relative path is given, pFileManager will be used to resolve the file.
  ///
  /// \param pFileManager
  ///   The file manager that is used to access the lock and resolve the given filename if it is relative.
  ///
  /// \return
  ///   true if the file exists on disk.
  VBASE_IMPEXP bool SetFile(const char* szFilename, IVFileStreamManager* pFileManager = NULL);

  /// \brief
  ///   Convenience function to set the file to lock and locks it at the same time.
  ///
  /// \param szFilename
  ///   The file to be locked. If a relative path is given pFileManager will be used to resolve the file.
  ///
  /// \param loadedTimeStamp
  ///   The timestamp of the file that is currently loaded in memory.
  ///
  /// \param pFileManager
  ///   The file manager that is used to access the lock and resolve the given filename if it is relative.
  ///
  /// \return
  ///   true if the file could be locked or if the file does not exist.
  VBASE_IMPEXP bool LockFile(const char* szFilename, const VFileTime& loadedTimeStamp, IVFileStreamManager* pFileManager = NULL);

  /// \brief
  ///   Tries to lock the file. The locking fails if the file on disk has a different timestamp than loadedTimeStamp.
  ///   Locking a file that does not exist will always succeed.
  ///
  /// \param loadedTimeStamp
  ///   The timestamp of the file that is currently loaded in memory.
  ///
  /// \return
  ///   true if the file could be locked or if the file does not exist.
  VBASE_IMPEXP bool LockFile(const VFileTime& loadedTimeStamp);

  /// \brief
  ///   Removes the file lock. In case of RCS systems the function may fail if changes have already been made
  ///   to the file and the lock can no longer be removed.
  VBASE_IMPEXP bool UnlockFile();

  /// \brief
  ///   Returns whether this lock instance currently holds a lock.
  VBASE_IMPEXP bool OwnsLock() const;

  /// \brief
  ///   Returns true if an RCS lock exists on the file or if a file lock exists.
  VBASE_IMPEXP bool LockFileExists() const;

  /// \brief
  ///   Tries to get a VRCSFileStatus for the set file. The given VFileTime will affect the
  ///   VRCSFileStatus::RCS_MODE_IS_HEAD_REV flag in case of disk based locking.
  ///
  /// \param out_status
  ///   The VRCSFileStatus instance the status should be written to.
  ///
  /// \param loadedTimeStamp
  ///   The timestamp of the file that is currently loaded in memory.
  ///
  /// \return
  ///   true if the status could be retrieved.
  VBASE_IMPEXP bool GetLockStat(VRCSFileStatus& out_status, const VFileTime& loadedTimeStamp) const;
  
private:
  bool ResolveGivenPath() const;
  static const char* GetWin32MachineName(VString& buffer);
  static const char* GetWin32UserName(VString& buffer);
  static const char* GetFileLockExtension() {return "lock";}

private:
  bool m_bMemoryLock;              ///< file does not exist, locked in memory
  bool m_bRCSLock;                 ///< locked by RCS
  IVFileOutStream* m_pStream;      ///< locked by disk file

  VString m_sGivenPath;            ///< Path given by the SetFile function.
  mutable VString m_sAbsPath;      ///< Absolute path to the file that should be locked.
  IVFileStreamManager* m_pManager; ///< The file manager used to access the to be locked file.
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
