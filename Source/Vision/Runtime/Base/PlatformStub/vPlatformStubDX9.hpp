/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vPlatformStubDX9.hpp

#ifndef VPLATFORMSTUBDX9_HPP
#define VPLATFORMSTUBDX9_HPP

#include <Vision/Runtime/Base/PlatformStub/vPlatformStubBase.hpp>

#ifndef _VISION_DOC

class VBASE_IMPEXP_CLASS vPlatformStubDX9 : public vPlatformStubBase
{
public:
  vPlatformStubDX9();
  virtual ~vPlatformStubDX9();

  virtual BOOL IsInitialized();
  virtual BOOL CompileShader(VShaderSourceInfo &info);
  virtual void FreeShaderResources(VShaderSourceInfo &info);

  // not implemented
  virtual BOOL Connect(const char *pszRemoteSystem){return FALSE;}
  virtual BOOL Disconnect(){return FALSE;}
  virtual BOOL SendString(const char *pszMessage){return FALSE;}
  virtual BOOL SendFile(const char *pszSourcePath, const char *pszTargetPath){return FALSE;}
  virtual BOOL SendFiles(const char *pszSourcePath, const char *pszTargetPath, BOOL bOverwriteExisting, BOOL bRecurseSubdirectories){return FALSE;}
  virtual BOOL DeleteFiles(const char *pszTargetPath, BOOL bRecurseSubdirectories){return FALSE;}
  virtual const char* GetRemoteSystem() const {return FALSE;}
  virtual const char* GetLastAnswer() const {return FALSE;}
  virtual const char* GetLastError() const {return "";}
  virtual BOOL SetNotificationProcessor(VISNOTIFICATIONFUNCPTR pNotificationProcessor){return FALSE;}
  virtual BOOL StartApplication(const char *pszApp, BOOL bForceColdReboot = FALSE){return FALSE;}
  virtual BOOL IsConnected(){return FALSE;}
  virtual BOOL CanIssueCommands(){return FALSE;}
  virtual BOOL CreateDirectory(const char *pszDir){return FALSE;}
  virtual BOOL SendClientHandshake(){return FALSE;}

private:
  void SetLastError(const char *pszLastError);
  char m_szLastError[1024];
  static void GetProcAddresses();
  
  static HINSTANCE m_hHandlerDLL;
  static int m_iRefCount;
};

#endif   // _VISION_DOC

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
