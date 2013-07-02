/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vPlatformStubBase.hpp

#ifndef VPLATFORMSTUBBASE_HPP
#define VPLATFORMSTUBBASE_HPP

#include <Vision/Runtime/Base/PlatformStub/vPlatformHandlerTypedefs.hpp>
#include <Vision/Runtime/Base/PlatformStub/vPlatformHandlerShaderInfo.hpp>

#ifndef _VISION_DOC


//  Base class for platform handler
//
class VBASE_IMPEXP_CLASS vPlatformStubBase 
{
public:
  vPlatformStubBase() {}
  virtual ~vPlatformStubBase() {}

  virtual BOOL IsInitialized()=0;

  virtual BOOL Connect(const char *pszRemoteSystem)=0;
  virtual BOOL Disconnect()=0;
  virtual BOOL SendString(const char *pszMessage)=0;
  virtual BOOL SendFile(const char *pszSourcePath, const char *pszTargetPath)=0;
  virtual BOOL SendFiles(const char *pszSourcePath, const char *pszTargetPath, BOOL bOverwriteExisting, BOOL bRecurseSubdirectories)=0;
  virtual BOOL DeleteFiles(const char *pszTargetPath, BOOL bRecurseSubdirectories)=0;
  virtual const char* GetRemoteSystem() const=0;
  virtual const char* GetLastError() const=0;
  virtual const char* GetLastAnswer() const=0;
  virtual BOOL SetNotificationProcessor(VISNOTIFICATIONFUNCPTR pNotificationProcessor)=0;
  virtual BOOL StartApplication(const char *pszApp, BOOL bForceColdReboot)=0;
  virtual BOOL IsConnected()=0;
  virtual BOOL CanIssueCommands()=0;
  virtual BOOL SendClientHandshake()=0;
  virtual BOOL CreateDirectory(const char *pszDir)=0;

  virtual BOOL CompileShader(VShaderSourceInfo &info) = 0;
  virtual void FreeShaderResources(VShaderSourceInfo &info) = 0;

protected:
};


#ifdef WIN32

class vPlatformManager
{
public:
  VBASE_IMPEXP static void OneTimeInit();
  VBASE_IMPEXP static void OneTimeDeInit();

  VBASE_IMPEXP static vPlatformStubBase *GetPlatformHandler(VTargetPlatform_e platform);
  VBASE_IMPEXP static void RemoveAllPlatformHandler();

private:
 static vPlatformStubBase* g_pPlatformHandler[TARGETPLATFORM_COUNT];
};

#endif


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
