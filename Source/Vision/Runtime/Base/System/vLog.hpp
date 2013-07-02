/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vLog.hpp

#ifndef VLOG_HPP_INCLUDED
#define VLOG_HPP_INCLUDED

/// \brief
///   Pure virtual interface to output log messages
class IVLog : public VBaseObject
{
public:

  /// \brief
  ///   Puts a fatal error message into the log
  virtual void Error(const char *szMessage) = 0;

  /// \brief
  ///   Puts a warning message into the log
  virtual void Warning(const char *szMessage) = 0;

  /// \brief
  ///   Puts an information text message into the log
  virtual void Info(const char *szMessage) = 0;

  /// \brief
  ///   Forward incoming message to Error, Warning or Info, i.e. according to message prefix
  virtual void ParseMessage(const char *szMessage) = 0;

  /// \brief
  /// If pLog is != NULL, the message is formated and passed to pLog->Error.
  VBASE_IMPEXP static void Error (IVLog* pLog, const char* szMessageTemplate, ...);

  /// \brief
  /// If pLog is != NULL, the message is formated and passed to pLog->Warning.
  VBASE_IMPEXP static void Warning (IVLog* pLog, const char* szMessageTemplate, ...);

  /// \brief
  /// If pLog is != NULL, the message is formated and passed to pLog->Info.
  VBASE_IMPEXP static void Info (IVLog* pLog, const char* szMessageTemplate, ...);

  /// \brief
  /// If pLog is != NULL, the message is formated and passed to pLog->ParseMessage.
  VBASE_IMPEXP static void ParseMessage (IVLog* pLog, const char* szMessageTemplate, ...);
};



/// \brief
///   Implements the IVLog interface and discards log outputs.
class VLogNull : public IVLog
{
public:
  VBASE_IMPEXP virtual void Error       (const char *szMessage)          { }
  VBASE_IMPEXP virtual void Warning     (const char *szMessage)          { }
  VBASE_IMPEXP virtual void Info        (const char *szMessage)          { }
  VBASE_IMPEXP virtual void ParseMessage(const char *szMessage)          { }
};



/// \brief
///   Implements the IVLog interface and outputs the log to the log window using VGLLogPrint.
///   ParseMessage will always be forwarded to Info.
class VLogWindow : public IVLog
{
public:
  VBASE_IMPEXP virtual void Error       (const char *szMessage);
  VBASE_IMPEXP virtual void Warning     (const char *szMessage);
  VBASE_IMPEXP virtual void Info        (const char *szMessage);
  VBASE_IMPEXP virtual void ParseMessage(const char *szMessage) { Info(szMessage); }
};



/// \brief
///   Implements the IVLog interface and stores the log into a linked list.
///   FlushEntries can be used to flush the stored log to another IVLog.
///   This Logger is useful when logging inside a thread.
class VLogThread : public IVLog
{
public:
  VBASE_IMPEXP virtual void Error(const char *szMessage);
  VBASE_IMPEXP virtual void Warning(const char *szMessage);
  VBASE_IMPEXP virtual void Info(const char *szMessage);
  VBASE_IMPEXP virtual void ParseMessage(const char *szMessage);
  
  VBASE_IMPEXP void FlushEntries(IVLog *pLog);

private:

  enum LogType
  {
    LogTypeError=0,
    LogTypeWarning,
    LogTypeInfo,
    LogTypeParseMessage,
    LogTypeCount
  };

  struct LogEntry 
  {
    LogType type;
    VString szMessage;
  };

  VArray<LogEntry> entries;
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
