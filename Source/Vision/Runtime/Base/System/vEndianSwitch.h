/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vEndianSwitch.h

#pragma once
#ifndef ENDIANSWITCH_H
#define ENDIANSWITCH_H

#if !defined(__GCC__) && !defined(__SNC__) && !defined(__GNUC__)
  // Disable warnings about using constant conditionals - they are used
  // intentionally.
  #pragma warning(disable : 4127)
#endif // !defined(__GCC__) && !defined(__SNC__) && !defined(__GNUC__)

#if defined(WIN32)
#define VISION_LITTLE_ENDIAN

#elif defined(_VISION_LINUX)
#define VISION_LITTLE_ENDIAN


#elif defined(_VISION_XENON)
#define VISION_BIG_ENDIAN


#elif defined(_VISION_PS3)
#define VISION_BIG_ENDIAN


#elif defined(_VISION_WIIU)
  #define VISION_BIG_ENDIAN

#elif defined(_VISION_PSP2)
  #define VISION_LITTLE_ENDIAN
#elif defined(_VISION_ANDROID)
  #define VISION_LITTLE_ENDIAN
#elif defined(_VISION_IOS)
  #define VISION_LITTLE_ENDIAN
#else
#error Undefined plattform!
#endif


/// \brief
///   Worker function for byte swapping of complex data structures.
/// \param pData
///   pointer to the start of the data on which to perform endian switching
/// \param pEnd
///   pointer to the end of the data. May be \c NULL to skip sanity checking.
/// \param format
///   the format of the data to work on. See below for a detailed description.
/// \param blockRepeatCount
///   number of times to repeat the current block (usually used in recursive calls
///   of this function)
/// \param updatedFormat
///   used for recursive calls to EndianSwitchWorker, so the callee can advance
///   the position in the format string
/// \return
///   a pointer to the address following the last modified data element
///
/// The format string can contain the following elements:
/// - \b numbers: a number prefixed to a format specifier or a group repeats that
///   data element or group the specified number of times.
/// - \b groups: format specifiers can be grouped with parentheses. If then prefixed
///   with a number, the repetition affects the whole group.
/// - \b q: Quad word (8 bytes)
/// - \b d: Double (8 bytes, switched the same way as a quad word)
/// - \b i: Integer (4 bytes)
/// - \b f: Float (4 bytes, swapped the same way as an integer)
/// - \b s: Single (2 bytes)
/// - \b c: Char (1 byte, no swapping)
///
/// The following example demonstrates a complex format pattern:
/// \verbatim q4cii3(s2cf)d \endverbatim
/// - Switch one quad word
/// - Skip 4 bytes
/// - Switch two integers (could, of course, also be written \c 2i)
/// - Repeat the following group three times:
///   - Switch one short
///   - Skip 2 bytes
///   - Switch one float
/// - Switch one double
///
/// \note
///   It is usually better to call the higher level functions, such as 
///   LittleEndianToNative().
VBASE_IMPEXP void* EndianSwitchWorker( void* pData, void* pEnd, const char* format,
                         int blockRepeatCount = 1, const char** updatedFormat = 0 );

/// \brief
///   Byte swap the specified number of two-byte items.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param count
///   the number of elements to process
/// \return
///   a pointer to the address following the last modified data element
VBASE_IMPEXP void* EndianSwitchWords( void* pData, int count );

/// \brief
///   Byte swap the specified number of four-byte items.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param count
///   the number of elements to process
/// \return
///   a pointer to the address following the last modified data element
VBASE_IMPEXP void* EndianSwitchDWords( void* pData, int count );

/// \brief
///   Byte swap the specified number of eight-byte items.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param count
///   the number of elements to process
/// \return
///   a pointer to the address following the last modified data element
VBASE_IMPEXP void* EndianSwitchQWords( uint64* pData, int count );

/// \brief
///   Byte swap a number of 2-byte elements from little-endian representation to the native 
///   representation of the current platform.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param iRepetitions
///   the number of elements to process
VBASE_IMPEXP void LittleEndianToNativeWords( void* pData, int iRepetitions );

/// \brief
///   Byte swap one 4-byte element from little-endian representation to the native 
///   representation of the current platform.
/// \param pData
///   pointer to the data to perform the byte swap on
VBASE_IMPEXP void LittleEndianToNativeDWord( void* pData );

/// \brief
///   Byte swap a number of 4-byte elements from little-endian representation to the native 
///   representation of the current platform.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param iRepetitions
///   the number of elements to process
VBASE_IMPEXP void LittleEndianToNativeDWords( void* pData, int iRepetitions );

/// \brief
///   Performs a complex byte swap from little-endian representation to the native representation
///   of the current platform.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param format
///   the format of the data. See EndianSwitchWorker() for a detailed format description.
void LittleEndianToNative(void *pData, const char *format);

/// \brief
///   Performs a complex byte swap from little-endian representation to the native representation
///   of the current platform.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param iSize
///   the size of the data. This is used to perform sanity checks and to assert if the specified size 
///   and the size computed from the format specification don't match.
/// \param format
///   the format of the data. See EndianSwitchWorker() for a detailed format description.
void LittleEndianToNative(void *pData, int iSize, const char *format);

/// \brief
///   Performs a complex byte swap from little-endian representation to the native representation
///   of the current platform.
/// \param pData
///   pointer to the data to perform the byte swap on
/// \param iSize
///   the size of the data. This is used to perform sanity checks and to assert if the specified size 
///   and the size computed from the format specification don't match.
/// \param format
///   the format of the data. See EndianSwitchWorker() for a detailed format description.
/// \param iRepetitions
///   the number of times to repeat the format pattern
void LittleEndianToNative(void *pData, int iSize, const char *format, unsigned int iRepetitions);

/// \brief
///   Performs a complex byte swap from little-endian representation to the native representation
///   of the current platform, not assuming aligned data.
/// \param pSourceData
///   pointer to the data to perform the byte swap on
/// \param pTargetData
///   pointer to the target buffer for the byte-swapped data
/// \param iSize
///   the size of the data. This is used to perform sanity checks and to assert if the specified size 
///   and the size computed from the format specification don't match.
/// \param format
///   the format of the data. See EndianSwitchWorker() for a detailed format description.
void LittleEndianToNativeMisaligned(void *pSourceData, void *pTargetData, int iSize, const char *format);

/// \brief
///   Byte swap an integer value from little-endian representation to the native 
///   representation of the current platform.
/// \param iVal
///   the integer value to perform the byte swap on
/// \return
///   the resulting value
int LittleEndianToNativeInt(int iVal);

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
