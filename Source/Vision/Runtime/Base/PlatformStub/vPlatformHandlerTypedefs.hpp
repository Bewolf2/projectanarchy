/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vPlatformHandlerTypedefs.hpp

#ifndef VPLATFORMHANDLERTYPEDEFSINCLUDED_HPP
#define VPLATFORMHANDLERTYPEDEFSINCLUDED_HPP

#ifndef _VISION_DOC

/// \brief 
///  Enumeration defining the supported types of remote notifications
enum VRemoteNotificationType
{
  VRN_CUSTOMMESSAGE,            ///< Custom Message
  VRN_DEBUGSTR,                 ///< Debug String
  VRN_EXCEPTION,                ///< Exception occurred on remote machine
  VRN_ASSERT,                   ///< Assertion occurred on remote machine
  VRN_UNKNOWN,                  ///< Unknown message type
  VRN_VISIONERROR,              ///< A fatal Vision Error occurred on remote machine
  VRN_VISIONWARNING,            ///< Vision Warning
  VRN_VISIONSYSTEMMESSAGE,      ///< Vision System Message
  VRN_ACK                       ///< Command Acknowledgement
};

/// \brief 
///  Structure defining a single notification message from the remote machine.
struct VRemoteNotification
{
  const char *pszString;          ///< The notification string (can be NULL).
  VRemoteNotificationType eType;  ///< Notification type.
  void *pData;                    ///< Notification-specific data.
};


typedef BOOL (VISION_STDCALL * VISNOTIFICATIONFUNCPTR)(const VRemoteNotification *p);


typedef const char* (* VISGETVXSTRINGPTR) ();
typedef void (* VISVXREGISTERNOTIFICATIONPROCESSORPTR) (VISNOTIFICATIONFUNCPTR);
typedef void (* VISVXUNREGISTERNOTIFICATIONPROCESSORPTR) ();
typedef BOOL (* VISVXSETACTIVEVCONSOLEPTR) (const char *);
typedef BOOL (* VISVXINITPTR) ();
typedef BOOL (* VISVXDEINITPTR) ();
typedef BOOL (* VISVXCONNECTPTR) ();
typedef BOOL (* VISVXDISCONNECTPTR) ();
typedef BOOL (* VISVXSENDCOMMANDPTR) (const char *);
typedef const char * (*VISVXGETLASTANSWERPTR) ();
typedef const char * (*VISVXGETLASTERRORPTR) ();
typedef BOOL (*VISVXSENDFILEPTR) (const char *, const char *);
typedef BOOL (*VISVXREBOOTPTR) (DWORD, const char *, const char *);
typedef BOOL (*VISVXMKDIRPTR) (const char *);
typedef BOOL (*VISVXCOMPILESHADERPTR) (struct VShaderSourceInfo &info);
typedef void (*VISVXFREESHADERRESOURCESPTR) (struct VShaderSourceInfo &info);

#define VISION_ERROR_IDENTIFIER "VISION_ERROR#"
#define VISION_SYSMESSAGE_IDENTIFIER "VISION_SYSMESSAGE#"
#define VISION_WARNING_IDENTIFIER "VISION_WARNING#"
#define VISION_CUSTOMMESSAGE_IDENTIFIER "VISION_CUSTOMMESSAGE#"
#define VISION_ACK_IDENTIFIER "VISION_ACK#"

#endif //_VISION_DOC

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
