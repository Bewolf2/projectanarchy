/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvProcessRunner.hpp

#ifndef HKV_PROCESS_RUNNER_HPP_INCLUDED
#define HKV_PROCESS_RUNNER_HPP_INCLUDED

#include <Common/Base/Container/String/hkStringBuf.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

/// \class hkvProcessRunner
/// \brief
///   Executes a Win32 application and captures its output on stdout and stderr.
class hkvProcessRunner
{
public:
  ASSETFRAMEWORK_IMPEXP hkvProcessRunner(const char* program, const char* arguments);
private:
  hkvProcessRunner();
  hkvProcessRunner(const hkvProcessRunner&);
  hkvProcessRunner& operator=(const hkvProcessRunner&);
public:
  ASSETFRAMEWORK_IMPEXP ~hkvProcessRunner();

public:
  ASSETFRAMEWORK_IMPEXP void setWorkingDirectory(const char* dir);

  ASSETFRAMEWORK_IMPEXP bool isIdlePriority() const;
  ASSETFRAMEWORK_IMPEXP void setIdlePriority(bool idlePriority);

  ASSETFRAMEWORK_IMPEXP hkResult runProcess();
  ASSETFRAMEWORK_IMPEXP void kill();
  ASSETFRAMEWORK_IMPEXP void waitFinished() const;

  ASSETFRAMEWORK_IMPEXP hkBool wasStarted() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getExitCode() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getLastError() const;
  ASSETFRAMEWORK_IMPEXP void getStderrText(hkStringBuf& out_buf) const;
  ASSETFRAMEWORK_IMPEXP void getStdoutText(hkStringBuf& out_buf) const;

private:
  hkResult createEvents();
  hkResult createPipes();
  hkResult createPipeSet(const char* name, HANDLE& out_server, HANDLE& out_client);
  hkResult createProcess();
  void handleProcessFinished();
  void handleKillRequest();
  void handleStdOutData();
  void handleStdErrData();
  void initiateRead(HANDLE& file, DWORD bytesToRead, unsigned char* buffer, 
    HANDLE completionEvent, LPOVERLAPPED overlapped);
  void monitorProcess();

private:
  hkStringPtr m_program;
  hkStringPtr m_arguments;
  hkStringPtr m_workingDirectory;
  bool m_idlePriority;
  hkCriticalSection m_protect;
  
  HANDLE m_processHandle;
  HANDLE m_outPipeHandleParent;
  HANDLE m_outPipeHandleChild;
  HANDLE m_errPipeHandleParent;
  HANDLE m_errPipeHandleChild;
  HANDLE m_killEvent;
  HANDLE m_outReadEvent;
  HANDLE m_errReadEvent;
  HANDLE m_finishedEvent;

  hkUint8* m_stdOutReadBuffer;
  hkUint8* m_stdErrReadBuffer;

  OVERLAPPED m_stdOutOverlapped;
  OVERLAPPED m_stdErrOverlapped;

  DWORD m_lastError;
  hkUint32 m_exitCode;
  hkBool m_startAttempted;
  hkBool m_startSucceeded;
  hkBool m_killed;
  hkStringBuf m_outText;
  hkStringBuf m_errText;

  static hkUint32 s_uniquePipeCount;
  static const hkUint32 s_readBufferSize;
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
