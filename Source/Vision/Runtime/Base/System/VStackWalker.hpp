/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
#ifndef V_STACK_WALKER_HPP
#define V_STACK_WALKER_HPP

#ifndef V_HAS_STACKWALKER
#error "VStackWalker included but V_HAS_STACKWALKER not defined on this platform"
#endif

/// \brief
///  a class to get a stack trace from any point in the code
class VStackWalker
{
public:
  typedef DWORD64 address_t; ///< type for a instruction address

  /// \brief
  ///  Get a given number of addresses from the callstack
  ///  
  /// \param pFunctionAddresses
  ///   buffer to write the found addresses to
  /// 
  /// \param uiMaxNumAddresses
  ///   maximum number of addresses to write
  ///
  /// \return the number of addresses written
  VBASE_IMPEXP static size_t GetCallstack(address_t* pFunctionAddresses, size_t uiMaxNumAddresses);

#if defined(WIN32) && !defined(_VISION_WINRT)
  /// \brief
  ///  Get a given number of addresses from the callstack
  ///  
  /// \param pContext
  ///  The context that should be used to genereate the trace
  ///
  /// \param pFunctionAddresses
  ///   buffer to write the found addresses to
  /// 
  /// \param uiMaxNumAddresses
  ///   maximum number of addresses to write
  ///
  /// \return the number of addresses written
  VBASE_IMPEXP static size_t GetCallstack(PCONTEXT pContext, address_t* pFunctionAddresses, size_t uiMaxNumAddresses);
#endif

  /// \brief
  ///  resolves a number of given addresses to function names + file & line number
  ///
  /// \param pFunctionAddresses
  ///  list of function addresses previously collected with GetCallstack
  ///
  /// \param uiNumAddresses
  ///  number of addresses to resolve
  ///
  /// \param pFunctionNames
  ///   array of function names which is uiMaxFuncNameLength * uiNumAddresses * sizeof(char) in length
  ///
  /// \param uiMaxFuncNameLength
  ///   maximum length of a function name string
  VBASE_IMPEXP static void ResolveCallstack(address_t* pFunctionAddresses, size_t uiNumAddresses, char* pFunctionNames, size_t uiMaxFuncNameLength);

private:
#if defined(WIN32) && !defined(_VISION_WINRT)
  static bool m_bModulesLoaded;

  static VString GenerateSearchPath();
  static bool LoadModules();
  static void LoadModule( HANDLE hProcess, PCWSTR img, PCWSTR mod, DWORD64 baseAadr, DWORD size);
#endif
};

#endif //V_STACK_WALKER_HPP

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
