/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file svfile.hpp


#ifndef SVFILE_INCLUDED
#define SVFILE_INCLUDED

/// \brief
///   Extract directory
/// 
/// ExtractDir will extract the directory part of a string containing a complete file name.
/// 
/// \param s
///   The string with the file name
/// 
/// \return
///   Newly allocated char pointer to the directory part
/// 
/// \note
///   User must free the returned string with SVFree(). No directory part in string, or NULL will
///   return an empty ("") string.
VBASE_IMPEXP char *ExtractDir(const char *s);

/// \brief
///   Extract directory
/// 
/// ExtractDir will extract the directory part of a string containing a complete file name.
/// 
/// \param s
///   The string with the file name
/// 
/// \note
///   Modifies the original string by truncating it. No directory in string will modify the string
///   to empty. NULL has no effect.
VBASE_IMPEXP void ExtractDir(char *s);


/// \brief
///   Extract parent directory
/// 
/// ExtractParentDir will extract the parent directory part of a string containing a complete file
/// name or path only.
/// 
/// \param s
///   The string with the file name or path
/// 
/// \return
///   VBASE_IMPEXP const char*: Newly allocated char pointer to the parent path
/// 
/// \note
///   User must free the returned string with SVFree().
/// 
/// \note
///   If given an empty string or a NULL pointer, a NULL pointer is returned.
/// 
/// \note
///   If there is no parent path (i.e. the path is a root path or not a path at all), an empty
///   string is returned. Note that this string must still be freed using the SVFree() function.
/// 
/// \note
///   To use this function with a path only, the path must end with a backslash ('\\') or slash
///   ('/').
VBASE_IMPEXP const char* ExtractParentDir(const char* s);


/// \brief
///   Extract parent directory
/// 
/// ExtractParentDir will extract the parent directory part of a string containing a complete file
/// name or path only.
/// 
/// \param s
///   The string with the file name or path
/// 
/// \note
///   Modifies the original string by truncating it.
/// 
/// \note
///   If given an empty string or NULL pointer, the function does nothing.
/// 
/// \note
///   If there is no parent path (i.e. the path is a root path or not a path at all), the string is
///   truncated to an empty string.
/// 
/// \note
///   To use this function with a path only, the path must end with a backslash ('\\') or slash
///   ('/').
VBASE_IMPEXP void ExtractParentDir(char* s);


/// \brief
///   Extract last directory
/// 
/// ExtractLastDir will extract the last directory in a path string.
/// 
/// \param s
///   The string with the path
/// 
/// \return
///   VBASE_IMPEXP const char*: Newly allocated char pointer to the last directory
/// 
/// \note
///   User must free the returned string with SVFree().
/// 
/// \note
///   If given an empty string or a NULL pointer, a NULL pointer is returned.
/// 
/// \note
///   If there is no path in the string (i.e. the path is a root path or not a path at all), an
///   empty string is returned. Note that this string must still be freed using the SVFree()
///   function.
/// 
/// \note
///   To use this function with a path only, the path must end with a backslash ('\\') or slash
///   ('/').
VBASE_IMPEXP const char* ExtractLastDir(const char* s);


/// \brief
///   Extract last directory
/// 
/// ExtractLastDir will extract the last directory in a path string.
/// 
/// \param s
///   The string with the path
/// 
/// \note
///   Modifies the original string by "truncating" it from both sides.
/// 
/// \note
///   If given an empty string or NULL pointer, the function does nothing.
/// 
/// \note
///   If there is no path in the string (i.e. the path is a root path or not a path at all), the
///   string is truncated to an empty string.
/// 
/// \note
///   To use this function with a path only, the path must end with a backslash ('\\') or slash
///   ('/').
VBASE_IMPEXP void ExtractLastDir(char* s);


/// \brief
///   Extract file name
/// 
/// ExtractFile will extract the file name part (without extension) of a string containing a
/// complete file name (including path).
/// 
/// \param s
///   The string with the complete file name
/// 
/// \return
///   Newly allocated char pointer to the file name
/// 
/// \note
///   User must free the returned string with SVFree(). No file part in string, or NULL will return
///   an empty ("") string. If there is no slash or backslash in the input string, it will be
///   considered to be a directory, and this function will return an empty string.
VBASE_IMPEXP char *ExtractFile(const char *s);

/// \brief
///   Extract file name
/// 
/// ExtractFile will extract the file name part (without extension) of a string containing a
/// complete file name (including path).
/// 
/// \param s
///   The string with the complete file name
/// 
/// \note
///   Modifies the original string by overwriting it. No file in string will modify the string to
///   empty. NULL has no effect. If there is no slash or backslash in the input string, it will be
///   considered to be a directory, and this function will return an empty string.
VBASE_IMPEXP void ExtractFile(char *s);



/// \brief
///   Extract file extension 
/// 
/// ExtractExt will extract the file extension of a string containing a (complete) file name.
/// 
/// \param s
///   The string with the complete file name
/// 
/// \return
///   Newly allocated char pointer to the file extension
/// 
/// \note
///   User must free the returned string with SVFree(). No extension in string, or NULL will return
///   an empty ("") string.
VBASE_IMPEXP char *ExtractExt(const char *s);

/// \brief
///   Extract file extension 
/// 
/// ExtractExt will extract the file extension of a string containing a (complete) file name.
/// 
/// \param s
///   The string with the complete file name
/// 
/// \note
///   Modifies the original string by overwriting it. No ext in string will modify the string to
///   empty. NULL has no effect.
VBASE_IMPEXP void ExtractExt(char *s);



/// \brief
///   Extract file name with extension 
/// 
/// ExtractFile will extract the file name part (including extension) of a string containing a
/// complete file name (including path).
/// 
/// \param s
///   The string with the complete file name
/// 
/// \return
///   Newly allocated char pointer to the file name
/// 
/// \note
///   User must free the returned string with SVFree(). No file part in string, or NULL will return
///   an empty ("") string. If there is no slash or backslash in the input string, it will be
///   considered to be a directory, and this function will return an empty string.
VBASE_IMPEXP char *ExtractFileExt(const char *s);

/// \brief
///   Extract file name with extension 
/// 
/// ExtractFile will extract the file name part (including extension) of a string containing a
/// complete file name (including path).
/// 
/// \param s
///   The string with the complete file name
/// 
/// \note
///   Modifies the original string by overwriting it. No file in string will modify the string to
///   empty. NULL has no effect. If there is no slash or backslash in the input string, it will be
///   considered to be a directory, and this function will return an empty string.
VBASE_IMPEXP void ExtractFileExt(char *s);



/// \brief
///   Compares a filename to a file specification and returns TRUE, if filename is part of the
///   specification
/// 
/// \param name
///   filename; e.g., "test.txt"
/// 
/// \param spec
///   specification with asterisks; e.g., "t*.*"
/// 
/// \return
///   \li TRUE => if (name==spec) or (name (part of) spec)
///   \li FALSE => else
/// 
/// \note
///   spec can only have trailing asterisks like "*.*", "ab*.a*", etc..
///   not possible: "*abc.*", "*.*a"
HKV_DEPRECATED_2013_1 VBASE_IMPEXP BOOL CompareFileToSpec(const char *name, const char *spec);



/// \brief
///   Split up command line string
/// 
/// This function will split up one command line string into its different parts, and save them
/// into *array[].
/// 
/// \param arguments
///   Original command line string
/// 
/// \param array[]
///   Array of strings with the different arguments
/// 
/// \return
///   RETVAL: Number of arguments in the output string array
VBASE_IMPEXP RETVAL SplitString(const char *arguments, char *array[]);


/// \brief
///   Check whether directory exists
/// 
/// on drive or not
/// 
/// \param dir
///   Directory, including or not including drive name
/// 
/// \return
///   RETVAL: TRUE if directory exists, FALSE if not
///
VBASE_IMPEXP RETVAL ExistsDir(const char *dir);


/// \brief
///   Creates a directory recursively
/// 
/// Creates a directory, and all directories above it that are necessary
/// 
/// \param dir
///   Directory to create
/// 
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VERR_NOERROR:  Directory successfully created
///   \li \c VERR_ERROR:  Directory could not be created
VBASE_IMPEXP RETVAL RecursiveMakeDir(const char *dir);



/// \brief
///   Get the relative path from a file with absolute path and a starting path
/// 
/// \param absPath
///   File name and absolute path
/// 
/// \param compareTo
///   Path from which to start (MUST end with \)
/// 
/// \return
///   String with the relative path. Notice that you have to free this string with
///   SVFree().
/// 
/// \example
///   \code
///   GetRelativePath("C:\thisdir\localdir\file.ext", "C:\thisDir\otherDir\")
///   should return the string "..\localdir\file.ext".
///   \endcode
VBASE_IMPEXP char *GetRelativePath(const char *absPath, const char *compareTo);



/// \brief
///   Copy File
/// 
/// \param origFile
///   Location of original file
/// 
/// \param destFile
///   Location of copied file
/// 
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VERR_NOERROR:  File copied fine
///   \li \c VERR_NOMEM:  Could not allocate enough memory for operation
///   \li \c VERR_CANTOPEN:  Source file could not be opened for reading
/// 
/// \note
///   This function is not available normally on all platforms, so it might prove useful when
///   porting.
VBASE_IMPEXP RETVAL FileCopy(char *origFile, char *destFile);



/// \brief
///   Creates a shortcut link
/// 
/// Creates a soft link, or shortcut to a file or application.
/// 
/// \param origin
///   Location of file to link to
/// 
/// \param link
///   Location of link
/// 
/// \param desc
///   Description of link (Windows only)
/// 
/// \return
///   RETVAL
VBASE_IMPEXP RETVAL CreateLink(const char *origin, const char *link, const char *desc);



/// \brief
///   Copy all files in dir
/// 
/// This function will copy all the files in a directory
/// 
/// \param srcDir
///   Source directory
/// 
/// \param destDir
///   Destination (target) directory
/// 
/// \param flags
///   Flag for preserving existing file
///   \li TRUE:                    Do not overwrite existing files
///   \li FALSE:                   Overwrite existing files
/// 
/// \param )
///   Callback function which is called for every copied file
/// 
/// \return
///   RETVAL
/// 
/// \note
///   The callback function is useful for keeping track of progress of file copying, logging copied
///   files, et.c. It takes as an argument a string which contains the name and destination
///   directory of the copied file.
VBASE_IMPEXP RETVAL CopyAll(const char *srcDir, const char *destDir, SLONG flags, void (callback)(char *));

/// \brief
///   Free StdVulp allocated memory
/// 
/// Free memory allocated by StdVulp
/// 
/// \param mem
///   Pointer to allocated memory
/// 
/// \note
///   Use this instead of the normal free function. While that one should work in most cases, there
///   are cases when mixed up library versions can cause conflicts, so it's better to use SVFree.
VBASE_IMPEXP void SVFree(void *mem);

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
