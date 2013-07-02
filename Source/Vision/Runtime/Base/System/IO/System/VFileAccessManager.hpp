/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFileAccessManager.hpp

#ifndef DC_VFILEACCESSMANAGER_HPP
#define DC_VFILEACCESSMANAGER_HPP

class VFileLock;

/// \brief
///   Monostate for controlling file access through Revision Control or disk based locks.
///
/// Relevant for PC only
class VFileAccessManager
{
public:

  /// \brief
  /// Returns the current Revision Control System
  VBASE_IMPEXP static IVRevisionControlSystem* GetRCS();

  /// \brief
  /// Sets the current Revision Control System
  VBASE_IMPEXP static void SetRCS(IVRevisionControlSystem* pRCS);

  /// \brief
  ///   Returns whether RCS is active. 
  ///
  /// Active means we are in vForge and the current project wants to have files automatically 
  /// checked in/out. This is only a central status indicator for plugins and does not affect the 
  /// RCS's operation. All RCS operations can still be performed even when the RCS isn't active.
  VBASE_IMPEXP static bool IsActive() { return m_bActive; }
  
  /// \brief
  ///   Sets whether the RCS is active
  ///
  /// See IsActive() for more information
  ///
  /// \param bActive
  ///   the new RCS state
  VBASE_IMPEXP static void SetActive(bool bActive) { m_bActive = bActive; }

  /// \brief
  ///   Retrieves a (disk based) lock for a file. 
  VBASE_IMPEXP static VFileLock*  GetFileLock(const char *pszAbsFileName, IVFileStreamManager *pFileManager);

  /// \brief
  ///   Returns the current Revision Control System if it's active.
  VBASE_IMPEXP static IVRevisionControlSystem* GetActiveRCS()
  {
    return IsActive() ? GetRCS() : NULL;
  }

  /// \brief
  ///   Performs one or more actions on a file; returns which actions have 
  ///   been performed successfully.
  /// \param szFilename
  ///   Relative or absolute file name. If not absolute, szProjectDir must be specified.
  /// \param iActions
  ///   The actions to perform. See VRevisionControlActions - you can or together multiple
  ///   values.
  /// \param szProjectDir
  ///   If \c szFilename is a relative path, \c szProjectDir must contain root to which
  ///   the path is relative.
  /// \return
  ///   A bitfield in which all successful actions are flagged. See VRevisionControlActions
  ///   for possible values.
  VBASE_IMPEXP static int RCSPerformAction(const char *szFilename,int iActions, const char *szProjectDir = NULL);
  
  /// \brief
  ///   Gets edit permission if the file exists and RCS is active.
  /// \param szFilename
  ///   The file to get edit permission for
  /// \return
  ///   \c false if and only if the file exists, an RCS is active, and the RCS fails to get edit
  ///   permissions for the file. \c true in any other case.
  VBASE_IMPEXP static bool RCSEditFile(const char *szFilename);

  /// \brief
  ///   Updates the file if RCS is active
  VBASE_IMPEXP static bool RCSUpdateFile(const char *szFilename);

  /// \brief
  ///   Adds the file if RCS is active
  VBASE_IMPEXP static bool RCSAddFile(const char *szFilename, bool bIsBinaryFile);

private:
  static VSmartPtr<IVRevisionControlSystem> m_spRCS;
  static bool m_bActive;
  
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
