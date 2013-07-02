/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VClipboardHelper.hpp

#ifndef VCLIPBOARDHELPER_HPP_INCLUDED
#define VCLIPBOARDHELPER_HPP_INCLUDED
 

/// \brief
///   Clipboard helper class. 
/// 
/// This is a wrapper around the operating system's clipboard.
class VBASE_IMPEXP_CLASS VClipBoardHelper
{
public:
  /// \brief
  ///   Places \c pText in the operating system's clipboard.
  /// \param pText
  ///   the text to place in the clipboard. Must not be \c NULL.
  /// \return
  ///   \c true if the operation was successful; \c false if an error occurred.
  /// \note
  ///   It is not possible to use this function to clear the clipboard by passing \c NULL.
  ///   If you pass \c NULL, this function always fails.
  static bool SetText(const char *pText);
};


#endif // VCLIPBOARDHELPER_HPP_INCLUDED

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
