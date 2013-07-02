/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VPLATFORMHANDLERHELPER_HPP
#define VPLATFORMHANDLERHELPER_HPP

enum VExternalProcess_e
{
  VEP_CreateProcessFailed = 0,
  VEP_RunProcessFailure = 1,
  VEP_RunProcessSuccess = 2
};

VBASE_IMPEXP VExternalProcess_e ExecuteExternalProcess(const char* pszCmdLine, VString& out_sErrorBuffer, VString* out_pPipeBuffer = NULL, unsigned int uiExpectedReturnCode = 0);
VBASE_IMPEXP void GenerateTemporaryFileName(const char* tempPath, const char* prefix, char* result, size_t resultLength);

template <size_t N>
HKV_FORCE_INLINE void GenerateTemporaryFileName(const char* tempPath, const char* prefix, char (&result)[N])
{
  GenerateTemporaryFileName(tempPath, prefix, result, N);
}

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
