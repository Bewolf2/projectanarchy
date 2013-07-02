/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvRcs.hpp

#ifndef HKVRCS_HPP_INCLUDED
#define HKVRCS_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Common/Base/Types/hkBaseTypes.h>

class hkvRcs
{
public:
  enum RcsCommands
  {
    RCSC_ADD,
    RCSC_REMOVE,
    RCSC_EDIT,
    RCSC_LOCK,
    RCSC_UNLOCK,
    RCSC_IS_LOCKED,
    RCSC_IS_LOCKED_BY_USER,
    RCSC_GET_LOCK_OWNER,
  };

  enum RcsFileMode
  {
    RCSFM_BINARY = 0,
    RCSFM_TEXT
  };

  /// \brief
  ///   Data object for the OnRunRcsCommand callback in hkvRcs.
  class RcsCommandData : public hkvCallbackData
  {
  public:
    ASSETFRAMEWORK_IMPEXP RcsCommandData(const hkvCallback* sender, const char* absoluteFileName, RcsCommands command, RcsFileMode fileMode = RCSFM_BINARY) :
      hkvCallbackData(sender), m_success(false), m_fileName(absoluteFileName), m_command(command), m_fileMode(fileMode)
    {
    }

    hkBool m_success;
    hkStringPtr m_returnValue;
    const char* m_fileName;
    hkEnum<RcsCommands, hkUint32> m_command;
    RcsFileMode m_fileMode;
  };

  class ScopedEditOrAdd
  {
  public:
    ASSETFRAMEWORK_IMPEXP ScopedEditOrAdd(const char* absolutePath, RcsFileMode fileMode);
    ASSETFRAMEWORK_IMPEXP ~ScopedEditOrAdd();

  private:
    hkBool m_exists;
    hkStringPtr m_path;
    RcsFileMode m_fileMode;
  };

  /// Adds a file to revision control
  ASSETFRAMEWORK_IMPEXP static bool addFile(const char* absoluteFileName, RcsFileMode fileMode);

  /// Removes a file from revision control
  ASSETFRAMEWORK_IMPEXP static bool removeFile(const char* absoluteFileName);

  /// Makes the file editable (writeable)
  /// Returns true if the file doesn't exist, because we're allowed to edit it
  ASSETFRAMEWORK_IMPEXP static bool editFile(const char* absoluteFileName);

  ///  Checks out the file for exclusive editing 
  /// ( SVN Lock,  Perforce: Edit/Checkout Lock)
  ASSETFRAMEWORK_IMPEXP static bool lockFile(const char* absoluteFileName, bool succeedIfLockedByMe = false);

  /// Unlocks a file if it hasn't been modified
  ASSETFRAMEWORK_IMPEXP static bool unlockFile(const char* absoluteFileName);

  /// Returns if a file has been locked
  ASSETFRAMEWORK_IMPEXP static bool isLocked(const char* absoluteFileName);

  ASSETFRAMEWORK_IMPEXP static bool isLockedByCurrentUser(const char* absoluteFileName);

  ASSETFRAMEWORK_IMPEXP static void getLockOwner(const char* absoluteFileName, hkStringBuf& out_userName);

public: // Data
  ASSETFRAMEWORK_IMPEXP static void TriggerOnRunRcsCommandCallback(hkvCallbackData* pData);

public:
  ASSETFRAMEWORK_IMPEXP static hkvCallbackWrapper OnRunRcsCommand;

private:
  static bool callOnRunRcsCommand(const char* absoluteFileName, RcsCommands command, RcsFileMode fileMode = RCSFM_BINARY);
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
