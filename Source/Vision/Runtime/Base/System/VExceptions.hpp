/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VExceptions.hpp


#ifndef _VISION_EXCEPTIONS_HPP_
#define _VISION_EXCEPTIONS_HPP_

/// \brief
///   Exception for errors in VArchive operations
class VArchiveException
{
public:

  /// Error causes
  enum
  {
    none, ///< No error
    general, ///< Generic error, no specific cause could be determined
    readOnly, ///< Attempt to write to a read-only archive
    endOfFile, ///< Attempt to read beyond the end of the archive file or stream
    writeOnly, ///< Attempt to read from a write-only archive
    badIndex, ///< Invalid class index encountered
    badClass, ///< Class of the deserialized object does not match expected class
    badSchema, ///< Bad serialization schema encountered
    unknownClass ///< Class to be deserialized is not known
  };
    
  /// \brief
  ///   Constructor
  /// \param cause
  ///   the cause of the exception
  /// \param lpszArchiveName
  ///   the name of the archive in which the exception occurred
  /// \param lpszClassName
  ///   the name of the class being serialized when the exception occurred
  inline VArchiveException( int cause, const char* lpszArchiveName, const char *lpszClassName=NULL)
  {
    m_reason = cause;
    m_strFileName = lpszArchiveName;
    m_strClassName = lpszClassName;
  }

  // Attributes
  int     m_reason; ///< The cause of this exception
  VString m_strFileName; ///< The name of the archive in which the exception occurred
  VString m_strClassName; ///< The name of the class being serialized when the exception occurred
  
  // Implementation
public:
  /// \brief
  ///   Destructor.
  VBASE_IMPEXP virtual ~VArchiveException() { }

  /// \brief
  ///   Creates a string representation of the information in this exception.
  /// \param lpszError
  ///   output buffer for storing the error message
  /// \param nMaxError
  ///   the size of the output buffer
  /// \return
  ///   \c TRUE if the error message could be successfully generated; \c FALSE if not
  VBASE_IMPEXP virtual VBool GetErrorMessage(char* lpszError, UINT nMaxError)
  { 
    char szBuffer[1024];
    const char* szFileName = m_strFileName.GetSafeStr();
    const char* szClassName = m_strClassName.GetSafeStr();
    switch (m_reason)
    {
    case general: sprintf(szBuffer,"General Error in archive '%s'",szFileName);break;
    case readOnly: sprintf(szBuffer,"Archive '%s' is read-only",szFileName);break;
    case endOfFile: sprintf(szBuffer,"End of file reached in Archive '%s'",szFileName);break;
    case writeOnly: sprintf(szBuffer,"Archive '%s' is write-only",szFileName);break;
    case badIndex: sprintf(szBuffer,"Archive '%s': Bad class index. (Class '%s'). Please check Serialization code for consistency.",szFileName,szClassName);break;
    case badClass: sprintf(szBuffer,"Archive '%s': Bad class. (Class '%s'). Please check Serialization code for consistency.",szFileName,szClassName);break;
    case badSchema: sprintf(szBuffer,"Archive '%s': Bad class scheme. (Class '%s')",szFileName,szClassName);break;
    case unknownClass: sprintf(szBuffer,"Archive '%s': Unknown class '%s'. Please check plugin dependencies.",szFileName,szClassName);break;
    default:return FALSE;
    }

    if (nMaxError<sizeof(szBuffer)) szBuffer[nMaxError-1] = 0;
    strcpy(lpszError,szBuffer);
    return TRUE; 
  };
};


/// \brief
///   Exception class for memory-related errors
class VMemoryException
{
public:
  /// \brief
  ///   Constructor.
  inline VMemoryException() { }
  /// \brief
  ///   Destructor.
  inline ~VMemoryException() { }
};


/// \brief
///   Exception class for errors relating to unsupported functionality
class VNotSupportedException
{
public:
  /// \brief
  ///   Constructor.
  inline VNotSupportedException() { }
  /// \brief
  ///   Destructor.
  inline ~VNotSupportedException() { }
};

/// \brief
///   Throws a VArchiveException
/// \param reason
///   the cause of the exception
/// \param filename
///   the name of the archive in which the exception occurred
/// \param szClassName
///   the name of the class being serialized when the exception occurred
void VBASE_IMPEXP_CLASS VThrowArchiveException(int reason,const char *filename, const char *szClassName=NULL);

/// \brief
///   Throws a VMemoryException.
void VBASE_IMPEXP_CLASS VThrowMemoryException();

/// \brief
///   Throws a VNotSupportedException.
void VBASE_IMPEXP_CLASS VThrowNotSupportedException();

#endif // _VISION_EXCEPTIONS_HPP_

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
