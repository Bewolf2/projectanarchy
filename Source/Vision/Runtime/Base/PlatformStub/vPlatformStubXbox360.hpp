/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vPlatformStubXbox360.hpp


#ifndef VPLATFORMSTUBXBOX360_HPP
#define VPLATFORMSTUBXBOX360_HPP

// this define should only be set when the WIN32 version
// supports the Xbox360 (i.e. to save shaders from vForge, ...)
#define WIN32_SUPPORTS_XENON

#include <Vision/Runtime/Base/PlatformStub/vPlatformStubBase.hpp>

//VBASE_IMPEXP BOOL TestExpFunc(VISNOTIFICATIONFUNCPTR pNotificationProcessor);

#ifndef _VISION_DOC

class VBASE_IMPEXP_CLASS vPlatformStubXbox360 : public vPlatformStubBase
{
public:
  vPlatformStubXbox360();
  virtual ~vPlatformStubXbox360();

  virtual BOOL IsInitialized();

  virtual BOOL Connect(const char *pszRemoteSystem);
  virtual BOOL Disconnect();
  virtual BOOL SendString(const char *pszMessage);
  virtual BOOL SendFile(const char *pszSourcePath, const char *pszTargetPath);
  virtual BOOL SendFiles(const char *pszSourcePath, const char *pszTargetPath, BOOL bOverwriteExisting, BOOL bRecurseSubdirectories);
  virtual BOOL DeleteFiles(const char *pszTargetPath, BOOL bRecurseSubdirectories);
  virtual const char* GetRemoteSystem() const;
  virtual const char* GetLastError() const;
  virtual const char* GetLastAnswer() const;
  virtual BOOL SetNotificationProcessor(VISNOTIFICATIONFUNCPTR pNotificationProcessor);
  virtual BOOL StartApplication(const char *pszApp, BOOL bForceColdReboot = FALSE);
  virtual BOOL IsConnected();
  virtual BOOL CanIssueCommands();
  virtual BOOL CreateDirectory(const char *pszDir);
  virtual BOOL SendClientHandshake();
  virtual BOOL CompileShader(VShaderSourceInfo &info);
  virtual void FreeShaderResources(VShaderSourceInfo &info);

private:
  BOOL EnsureConsoleSelected();
  void SetLastError(const char *pszLastError);
  char *m_pszConsole;
  char m_szLastError[1024];
  BOOL m_bCanIssueCommands;
  BOOL m_bIsConnected;
  static void GetProcAddresses();
  
  static HINSTANCE m_hHandlerDLL;
  static int m_iRefCount;
  static char g_szSelectedConsole[];
};

#endif // _VISION_DOC

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
