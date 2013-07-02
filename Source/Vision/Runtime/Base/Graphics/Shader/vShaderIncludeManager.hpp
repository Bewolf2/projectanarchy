/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderIncludeManager.hpp

#ifndef VSHADERINCLUDEMANAGER_HPP_INCLUDED
#define VSHADERINCLUDEMANAGER_HPP_INCLUDED

// NB: All shader compile related functions are strictly PC Win32 specific
#ifdef WIN32

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/Base/System/Resource/VResourceManager.hpp>

/// \brief
///   This class is used to access include files used in shader compilation through a custom file stream manager.
///  
/// Only supported on Windows, as this functionality is only used for offline shader compilation.
class VShaderIncludeManager
{
public:
  /// \brief Returns the file-stream manager to use for accessing include files
  VBASE_IMPEXP static IVFileStreamManager* GetFileStreamManager ();

  /// \brief Sets the file-stream manager to use for include-file open operations.
  VBASE_IMPEXP static void SetFileStreamManager (IVFileStreamManager* pMan) { s_pFileStreamManager = pMan; }

private:
  static IVFileStreamManager* s_pFileStreamManager;
};

#endif // WIN32

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
