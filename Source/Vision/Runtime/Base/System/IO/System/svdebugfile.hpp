/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file svdebugfile.hpp
///
/// SVDebugFile provides a wrapper for debug file output. When HK_DEBUG_SLOW is
/// defined, it will provide functionality just like a normal printf
/// and in final version, no extra code will be produced


#ifndef SVDEBUGFILE_DEFINED
#define SVDEBUGFILE_DEFINED

#define FLUSH_EACH 0x0001

  /// \brief
  ///   Opens the debug file for writing
  /// 
  /// \param szFilename
  ///   the name (and path) of the debug file
  /// 
  /// \param params
  ///   Operation parameters, ORed together List of parameters (only one so far):
  ///   \li FLUSH_EACH: Flushes the file after every DebugPrint.  This is very useful for crashing
  ///     applications
  /// 
  /// \note
  ///   This function only works if HK_DEBUG_SLOW is defined.
  /// 
  /// \note
  ///   Only one debug file at a time is supported. This may be subject to change upon request, or
  ///   update this library yourself :)
  HKV_DEPRECATED_2013_1 VBASE_IMPEXP void SVDebugOpen(const char *szFilename, ULONG params);



  /// \brief
  ///   Closes the open debug file
  /// 
  /// \note
  ///   This function only works if HK_DEBUG_SLOW is defined.
  HKV_DEPRECATED_2013_1 VBASE_IMPEXP void SVDebugClose(void);



  /// \brief
  ///   Writes formatted data to the debug file
  /// 
  ///  Behaves exactly like printf. Use it as such.
  /// 
  /// \param s
  ///   String constant with text and formatting
  /// 
  /// \param ...
  ///   Followed by parameters, if any
  /// 
  /// \note
  ///   This function only works if HK_DEBUG_SLOW is defined.
  HKV_DEPRECATED_2013_1 VBASE_IMPEXP void SVDebugPrint(const char *s, ...);



  /// \brief
  ///   Flushes the debug file buffer
  /// 
  /// \note
  ///   This function only works if HK_DEBUG_SLOW is defined.
  HKV_DEPRECATED_2013_1 VBASE_IMPEXP void SVDebugFlush(void);

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
