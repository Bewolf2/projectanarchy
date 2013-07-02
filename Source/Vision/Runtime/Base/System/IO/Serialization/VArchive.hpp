/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VArchive.hpp

#ifndef _VISION_VARCHIVE_HPP
#define _VISION_VARCHIVE_HPP

class IVFileOutStream;
class IVFileInStream;

#include <Vision/Runtime/Base/Container/VMaps.hpp>
#include <Vision/Runtime/Base/Container/VPList.hpp>
#include <Vision/Runtime/Base/Container/VStack.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VSerializationProxy.hpp>
#include <Vision/Runtime/Base/Math/Type/VFloat16.hpp>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>

#define dwNullTag        ((DWORD)0)           ///< special tag indicating NULL ptrs
#define dwClassTag       ((DWORD)0x80000000)  ///< 0x8000000 indicates class tag (OR'd)
#define dwNewClassTag    ((DWORD)0xFFFFFFFF)  ///< special tag indicating new runtime class

#define V_VERSIONABLE_SCHEMA  (0x80000000)

#define V_LOAD_CHAR_ARRAY(ar,var,len)         \
  {                                           \
    for(int vlcac = 0; vlcac < len; vlcac++)  \
      ar >> var[vlcac];                       \
  }

#define V_STORE_CHAR_ARRAY(ar,var,len)        \
  {                                           \
    for(int vscac = 0; vscac < len; vscac++)  \
      ar << var[vscac];                       \
  }

struct VType;
class VTypedObject;
class VTypeManager;
class VArchive;
class IVSerializationProxyCreator;
class IVSerializationProxy;
class VisZoneResource_cl;

/// \brief
///   Serialization event types
enum SerializeEvent
{
  PRE_SERIALIZE,  ///< Event triggered before serialization
  POST_SERIALIZE  ///< Event triggered after serialization
};


typedef void (*V_ARCHIVECALLBACK_ONCLOSE)( VArchive &ar );

// This checks that things are OK at compile time
V_COMPILE_ASSERT(dwNullTag == 0);
#ifndef _VISION_WIIU //(GHS does not like using sizeof on 0
V_COMPILE_ASSERT(sizeof(dwNullTag) == 4);
V_COMPILE_ASSERT(sizeof(dwNewClassTag) == 4);
#endif
V_COMPILE_ASSERT((dwNewClassTag & dwClassTag) == dwClassTag);

// internal
#define OBJECT_LENGTH_STACK_DEPTH 128

/// \brief
///   Class that describes a serialization session. 
///
/// An instance of this class is passed to VTypedObject::WantsDeserializationCallback and VTypedObject::OnDeserializationCallback
class VSerializationContext
{
public:
  /// \brief
  ///   Enum that describes the type of serialization session.
  enum VSerializationType
  {
    VSERIALIZATION_UNKNOWN, ///< Unknown serialization type
    VSERIALIZATION_ARCHIVE, ///< Deserialization from an archive, e.g. during vscene loading
    VSERIALIZATION_EDITOR,  ///< Called by the editor after engine instances have been created
    VSERIALIZATION_NETWORKREPLICATION,  ///< Called by the engine's network manager when an object should be fully replicated
  };

  /// \brief
  ///   Constructor; initializes the serialization context with the specified values.
  /// \param eType
  ///   Serialization type
  /// \param pArchive
  ///   pointer to a VArchive instance, if type is \c VSERIALIZATION_ARCHIVE
  inline VSerializationContext(VSerializationType eType, VArchive *pArchive) :
    m_eType(eType), m_pArchive(pArchive)
  {
  }

  /// \brief
  ///   Constructor; initializes the serialization context with default values
  inline VSerializationContext() :
    m_eType(VSERIALIZATION_UNKNOWN), m_pArchive(NULL)
  {
  }

  VSerializationType m_eType; ///< Serialization type
  VArchive *m_pArchive;       ///< If type is \c VSERIALIZATION_ARCHIVE, this points to a valid VArchive instance
};


/// \brief
///   Archive class for serialization
///
/// VArchive is the main class for serialization/deserialization in Vision. It handles buffered IO to and from files.
/// It also provides support for serialization of dynamically typed objects and basic data
/// primitives like float, integers and strings.
///
/// All serializable classes in Vision have their own Serialize/SerializeX functions that operate on a VArchive instance.
/// 
/// \sa IVFileOutStream
/// \sa IVFileInStream
class VArchive
{
public:
  friend class VSceneExporter;
  friend class VSceneLoader;
  friend class VisTypedEngineObject_cl;

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Archive constructor for loading from an archive
  ///
  /// \param szArchiveName
  ///   Archive name. May be NULL
  /// \param pStream
  ///   Input stream to read from.
  /// \param pTypeManager
  ///   Type Manager to use in order to resolve class names to types. Usually Vision::GetTypeManager()
  /// \param nBufSize
  ///   Buffer size to use for read/write operations. Larger buffer sizes often result in faster IO performance.
  VBASE_IMPEXP VArchive(const char* szArchiveName, IVFileInStream*  pStream, VTypeManager *pTypeManager, int nBufSize = 4096);

  /// \brief
  ///   Archive constructor for saving to an archive
  ///
  /// \param szArchiveName
  ///   Archive name. May be NULL
  /// \param pStream
  ///   Output Stream to write to.
  /// \param pTypeManager
  ///   Type Manager to use in order to resolve class names to types. Usually Vision::GetTypeManager()
  /// \param nBufSize
  ///   Buffer size to use for read/write operations. Larger buffer sizes often result in faster IO performance.
  VBASE_IMPEXP VArchive(const char* szArchiveName, IVFileOutStream* pStream, VTypeManager *pTypeManager, int nBufSize = 4096);

  /// \brief
  ///   Archive constructor for loading or saving.
  ///
  /// Depending on whether pInStream or pOutStream are non-NULL, the archive will either be opened for loading or saving. Either
  /// pInStream or pOutStream has to be non-NULL, but it is not valid to pass in two non-NULL streams.
  ///
  /// \param szArchiveName
  ///   Archive name. May be NULL
  ///
  /// \param pInStream
  ///   Input stream to read from. Note that only one of pInStream and pOutStream may be non-null.
  ///
  /// \param pOutStream
  ///   Output Stream. Note that only one of pInStream and pOutStream may be non-null.
  ///
  /// \param pTypeManager
  ///   Type Manager to use in order to resolve class names to types. Usually Vision::GetTypeManager()
  ///
  /// \param nBufSize
  ///   Buffer size to use for read/write operations. Larger buffer sizes often result in faster IO performance.
  VBASE_IMPEXP VArchive(const char* szArchiveName, IVFileInStream* pInStream, IVFileOutStream* pOutStream, VTypeManager *pTypeManager, int nBufSize = 4096);

private:
  /// \brief
  ///   Disallow copy construction
  VArchive(const VArchive&);

  /// \brief
  ///   Disallow assignment
  VArchive& operator=(const VArchive&);

public:
  /// \brief
  ///   Archive destructor
  VBASE_IMPEXP virtual ~VArchive();

  /// \brief
  ///   Re-initializes the archive so the same instance can be used for reading again. Call this before starting a second deserialization session with the same archive object.
  /// \param pStream
  ///   Input stream to read from.
  VBASE_IMPEXP void ReInit(IVFileInStream*  pStream);

  /// \brief
  ///   Re-initializes the archive so the same instance can be used for writing again. Call this before starting a second serialization session with the same archive object.
  /// \param pStream
  ///   Output Stream to write to.
  VBASE_IMPEXP void ReInit(IVFileOutStream*  pStream);

  /// \brief
  ///   Returns the serialization context that is associated with this archive
  inline VSerializationContext& Context()
  {
    return m_Context;
  }

  /// \brief
  ///   Internal function that is called automatically when an archive is closed: Triggers the VTypedObject::OnDeserializationCallback for all instances in the queue and resets the queue.
  VBASE_IMPEXP void TriggerDeserializationCallbacks();

  ///
  /// @}
  ///

  ///
  /// @name Read / Write
  /// @{
  ///

public:
  /// \brief
  ///   Returns the filename of the archive
  VBASE_IMPEXP const char *GetFilename() const;

  /// \brief
  ///   Indicates whether this archive is an archive used for deserialization
  inline VBool IsLoading() const {  return (m_pInStream!=NULL);  }

  /// \brief
  ///   Indicates whether this archive is an archive used for serialization
  inline VBool IsSaving()  const {  return (m_pOutStream!=NULL);  }

  /// \brief
  ///   Reads a given number of bytes from the archive and returns the number of bytes read. 
  ///
  /// The archive must be open for reading. The returned size may differ from the passed size if the end of the file is reached.
  /// Note that this function does not perform any endianness conversion, so the data read may differ depending on the platform's
  /// endianness. If you want to perform cross-platform serialization, please use one of the operator overloads or the endianness-
  /// aware Read/Write functions instead.
  /// 
  /// \param data
  ///   Pointer to the buffer into which to read
  ///
  /// \param size
  ///   Number of bytes to read
  ///
  /// \returns
  ///   Number of bytes read.
  VBASE_IMPEXP virtual UINT Read(void* data, UINT size);

  /// \brief
  ///   Version of Read function that performs endianness conversion 
  /// \param data
  ///   Pointer to the buffer into which to read
  /// \param size
  ///   Number of bytes to read
  /// \param pFormat
  ///   Format string describing the data to be read. See EndianSwitchWorker()
  ///   for the syntax of this string.
  /// \param iRepetitions
  ///   Number of times the pattern described in \c pFormat is to be repeated
  /// \returns
  ///   Number of bytes read.
  VBASE_IMPEXP UINT Read(void* data, UINT size, const char *pFormat, unsigned int iRepetitions = 1);

  /// \brief
  ///   For loading only: Checks if the end of the archive (i.e. the referenced in-stream) has been
  ///   reached. 
  /// 
  /// This function has a slightly different behavior than file streams: An archive switches to EOF mode
  /// after the last byte has been read, while a stream still returns FALSE at this point (it finally
  /// switches to EOF after trying to read beyond the file size).
  /// 
  /// This difference is important so that the code example (s.b.) works.
  /// 
  /// \example
  ///   \code
  ///   while (!ar.IsEOF()) ar.ReadObject(NULL);
  ///   \endcode
  VBASE_IMPEXP BOOL IsEOF();

  /// \brief
  ///   Returns the current file position while loading and saving
  /// 
  /// The return value of this function is byte-accurate; while the GetPos function of the underlying 
  /// stream is not, as the archive is loading blocks of data rather than individual bytes.
  /// 
  /// To get the file size, use the underlying in/out stream instead.
  inline int GetPos() const {return m_iFilePos;}

  /// \brief
  ///   Reads a string in binary format. See also VFileHelper::ReadStringBinary.
  ///
  /// \param szDestBuffer
  ///   The destination buffer to write the string into.
  /// 
  /// \param iMaxSize
  ///   The size of the destination buffer (i.e., the maximum number of bytes to read).
  ///   If you do not provide this optional parameter there is no size check, your buffer has to be big enough!
  ///
  /// \returns
  ///   The number of bytes written to the destination buffer.
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines an empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  VBASE_IMPEXP int ReadStringBinary(char *szDestBuffer, int iMaxSize=-1);

  /// \brief
  ///   This version of ReadStringBinary reads all bytes and tries to use the passed buffer
  /// 
  /// If the bAllocated flag is set after the function returns, the return value has to be free'd
  /// with VBaseDeAlloc.
  /// 
  /// If the string uses not more than iMaxCount bytes (i.e. it fits into szDestBuffer), then the
  /// return value matches the passed buffer
  ///
  /// \param szDestBuffer
  ///   The destination buffer to write the string into.
  /// 
  /// \param iMaxSize
  ///   The size of the destination buffer (i.e., the maximum number of bytes to read).
  ///
  /// \param bAllocated
  ///   If the bAllocated flag is set after the function returns, the return value has to be free'd
  ///   with VBaseDeAlloc.
  ///
  /// \returns
  ///   A pointer to the buffer containing the read string (needs to be free'd with VBaseDeAlloc if bAllocated==true).
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  ///
  /// \example
  ///   \code
  ///     char buffer[1024];
  ///     bool bAllocated = false;
  ///     char* szString = ar.ReadStringBinary(buffer,sizeof(buffer),bAllocated);
  ///     ...
  ///     DoSomethingWithTheString(szString);
  ///     ...
  ///     if (bAllocated) VBaseDealloc(szString);
  ///   \endcode
  ///
  /// \see ReadEncryptedString
  /// \see WriteStringBinary
  VBASE_IMPEXP char *ReadStringBinary(char *szDestBuffer, int iMaxSize, bool &bAllocated);

  /// \brief
  ///   Reads a string that is compatible with the string written with WriteEncryptedString.
  ///    
  /// If the bAllocated flag is set after the function returns, the return value has to be free'd
  /// with VBaseDeAlloc.
  /// 
  /// If the string uses not more than iMaxCount bytes (i.e. it fits into szDestBuffer), then the
  /// return value matches the passed buffer
  ///
  /// \param szDestBuffer
  ///   The destination buffer to write the decrypted string into.
  /// 
  /// \param iMaxSize
  ///   The size of the destination buffer (i.e., the maximum number of bytes to read).
  ///
  /// \param bAllocated
  ///   If the bAllocated flag is set after the function returns, the return value has to be free'd
  ///   with VBaseDeAlloc.
  ///
  /// \returns
  ///   A pointer to the buffer containing the read string (needs to be free'd with VBaseDeAlloc if bAllocated==true).
  ///
  /// \example
  ///   \code
  ///     char buffer[1024];
  ///     bool bAllocated = false;
  ///     char* szString = ar.ReadEncryptedString(buffer,sizeof(buffer),bAllocated);
  ///     ...
  ///     DoSomethingWithTheString(szString);
  ///     ...
  ///     if (bAllocated) VBaseDealloc(szString);
  ///   \endcode
  ///
  /// \see WriteEncryptedString
  /// \see ReadStringBinary
  VBASE_IMPEXP char *ReadEncryptedString(char *szDestBuffer, int iMaxSize, bool &bAllocated);

  /// \brief
  ///   Writes data to the archive without performing endianness conversion. The archive must be opened for writing.
  /// \param data
  ///   Pointer to the data to write
  /// \param size
  ///   Number of bytes to write
  VBASE_IMPEXP virtual void Write(const void* data, UINT size);

  /// \brief
  ///   Writes data to the archive and performs endianness conversion. The archive must be opened for writing.
  /// \param data
  ///   Pointer to the data to write
  /// \param size
  ///   Number of bytes to write
  /// \param szFormatString
  ///   Format string describing the data to be written. See EndianSwitchWorker()
  ///   for the syntax of this string.
  /// \param iNumRepetitions
  ///   Number of times the pattern described in \c szFormatString is to be repeated
  VBASE_IMPEXP virtual void Write(const void* data, UINT size, const char *szFormatString, unsigned int iNumRepetitions = 1);

  /// \brief
  ///   Writes the string to the archive in binary format. The data that is written out starts with an int that specifies the length.
  ///
  /// \param szString
  ///   The 0-terminated buffer to write.
  ///
  /// \see ReadStringBinary
  VBASE_IMPEXP void WriteStringBinary(const char *szString);

  /// \brief
  ///   Writes the string to the archive in binary format using a simple encryption method. Not meant to be safe but strings are not recognizable in a HEX viewer.
  ///
  /// \param szString
  ///   The 0-terminated buffer to encrypt and write.
  ///
  /// \see ReadEncryptedString
  VBASE_IMPEXP void WriteEncryptedString(const char *szString);

  /// \brief
  ///   Flushes the write cache. Will be called automatically when closing the file.
  VBASE_IMPEXP void Flush();

  /// \brief
  ///   Closes the archive, but NOT the file stream
  VBASE_IMPEXP void Close();

private:
  /// \brief
  ///   Internal function
  VBASE_IMPEXP void Abort();

  ///
  /// @}
  ///

  ///
  /// @name Write Operators
  /// @{
  ///

public:
  /// \brief
  ///   Writes a signed char to the archive
  VBASE_IMPEXP VArchive& operator<<(signed char ch);

  /// \brief
  ///   Writes a BYTE to the archive
  VBASE_IMPEXP VArchive& operator<<(BYTE by);

  /// \brief
  ///   Writes a WORD to the archive
  VBASE_IMPEXP VArchive& operator<<(WORD w);

  /// \brief
  ///   Writes a DWORD to the archive
  VBASE_IMPEXP VArchive& operator<<(DWORD dw);

  /// \brief
  ///   Writes an __int64 to the archive
  VBASE_IMPEXP VArchive& operator<<(__int64 i);

  /// \brief
  ///   Writes a VFloat16 to the archive
  VBASE_IMPEXP VArchive& operator<<(const VFloat16& h);

#ifdef WIN32
  /// \brief
  ///   Writes a LONG to the archive
  VBASE_IMPEXP VArchive& operator<<(LONG l);
#endif

  /// \brief
  ///   Writes a float to the archive
  VBASE_IMPEXP VArchive& operator<<(float f);

  /// \brief
  ///   Writes a double to the archive
  VBASE_IMPEXP VArchive& operator<<(double d);

  /// \brief
  ///   Writes an int to the archive
  VBASE_IMPEXP VArchive& operator<<(int i);

  /// \brief
  ///   Writes a short to the archive
  VBASE_IMPEXP VArchive& operator<<(short w);

  /// \brief
  ///   Writes a char to the archive
  VBASE_IMPEXP VArchive& operator<<(char ch);

  /// \brief
  ///   Writes an unsigned int to the archive
  VBASE_IMPEXP VArchive& operator<<(unsigned u);

  /// \brief
  ///   Writes a bool to the archive
  VBASE_IMPEXP VArchive& operator<<(bool b);

  /// \brief
  ///   Writes a string to the archive
  VBASE_IMPEXP VArchive& operator<<(const char* str);

  /// \brief
  ///   Writes a wide-character string to the archive
  VBASE_IMPEXP VArchive& operator<<(const wchar_t* str);

  ///
  /// @}
  ///

  ///
  /// @name Read Operators
  /// @{
  ///

  /// \brief
  ///   Reads a signed char from the archive
  VBASE_IMPEXP VArchive& operator>>(signed char& ch);

  /// \brief
  ///   Reads a BYTE from the archive
  VBASE_IMPEXP VArchive& operator>>(BYTE& by);

  /// \brief
  ///   Reads a WORD from the archive
  VBASE_IMPEXP VArchive& operator>>(WORD& w);

  /// \brief
  ///   Reads a DWORD from the archive
  VBASE_IMPEXP VArchive& operator>>(DWORD& dw);

  /// \brief
  ///   Reads an __int64 from the archive
  VBASE_IMPEXP VArchive& operator>>(__int64& i);

  /// \brief
  ///   Reads a VFloat16 from the archive
  VBASE_IMPEXP VArchive& operator>>(VFloat16& h);

#ifdef WIN32

  /// \brief
  ///   Reads a LONG from the archive
  VBASE_IMPEXP VArchive& operator>>(LONG& l);
#endif

  /// \brief
  ///   Reads a float from the archive
  VBASE_IMPEXP VArchive& operator>>(float& f);

  /// \brief
  ///   Reads a double from the archive
  VBASE_IMPEXP VArchive& operator>>(double& d);

  /// \brief
  ///   Reads an int from the archive
  VBASE_IMPEXP VArchive& operator>>(int& i);

  /// \brief
  ///   Reads a short from the archive
  VBASE_IMPEXP VArchive& operator>>(short& w);

  /// \brief
  ///   Reads a char from the archive
  VBASE_IMPEXP VArchive& operator>>(char& ch);

  /// \brief
  ///   Reads an unsigned int from the archive
  VBASE_IMPEXP VArchive& operator>>(unsigned& u);

  /// \brief
  ///   Reads a bool from the archive
  VBASE_IMPEXP VArchive& operator>>(bool &b);

  /// \brief
  ///   Reads a string from the archive
  VBASE_IMPEXP VArchive& operator>>(char*& str);

  /// \brief
  ///   Reads a const string from the archive
  VBASE_IMPEXP VArchive& operator>>(const char*& str);

  /// \brief
  ///   Reads a wide-character string from the archive
  VBASE_IMPEXP VArchive& operator>>(wchar_t*& str);

  // Object I/O is pointer based to avoid added construction overhead.
  // Use the Serialize member function directly for embedded objects.
  // object streaming binary operators - see their implementation in this file
  friend VBASE_IMPEXP VArchive& operator<<( VArchive& ar, const VTypedObject* pObj );
  friend VBASE_IMPEXP VArchive& operator>>( VArchive& ar, VTypedObject*& pObj );
  friend VBASE_IMPEXP VArchive& operator>>( VArchive& ar, const VTypedObject*& pObj );

private:
  VBASE_IMPEXP void FillBuffer(UINT nBytesNeeded);

  ///
  /// @}
  ///

  ///
  /// @name Get Stream
  /// @{
  ///

public:
  /// \brief
  ///   Returns the input stream. Do not use to read from it since this messes with the read cache.
  inline IVFileInStream*  GetInStream()   {  return m_pInStream;  }

  /// \brief
  ///   Returns the output stream. Do not use to write to it since this messes with the write cache.
  inline IVFileOutStream* GetOutStream()  {  return m_pOutStream;  }
  
  // Exclude from docs
  #ifndef _VISION_DOC

  /// \brief
  ///   GetObjectVersion (NOT USED, USE GETLOADINGVERSION INSTEAD !!!)
  /// 
  /// Called from the Serialize function to determine the version of the  object that is being
  /// deserialized. Calling this function is only  valid when the VArchive object is being loaded
  /// (VArchive::IsLoading  returns nonzero). It should be the first call in the Serialize function
  /// and called only once.
  /// 
  /// \return
  ///   UINT: Version number. A value of (UINT)-1 indicates that the version number is unknown)
  HKV_DEPRECATED_2013_1 VBASE_IMPEXP UINT GetObjectVersion();


  /// \brief
  ///   SetObjectVersion (NOT USED, USE SETLOADINGVERSION INSTEAD !!!)
  /// 
  /// Call this member function to set the object schema stored in the  archive object to nSchema.
  /// The next call to GetObjectSchema will return the value stored in nSchema. Use SetObjectSchema
  /// for advanced versioning; for example, when you want to force a particular version to be read
  /// in a Serialize function of a derived class.
  ///
  /// \param nVersion
  ///   Version number to set.
  HKV_DEPRECATED_2013_1 inline void SetObjectVersion( UINT nVersion ) 
  {
    HKV_DISABLE_DEPRECATION
    m_nObjectVersion = nVersion;
    HKV_ENABLE_DEPRECATION
  }

  #endif

  /// \brief
  ///   Read object of the required type from the archive. This function reads the type of the object as well
  ///   as its data by calling the object's Serialize() method.
  /// \param pClassRefRequested 
  ///   Optional parameter; to be used to validate the type of the object being read.
  ///   If this is non-NULL, the function will check whether the passed type matches the loaded object's type or not.
  /// \param pObjectSize
  ///   pointer to a variable the object size should be written to, can be NULL
  /// \return
  ///   the object that has been read
  VBASE_IMPEXP virtual VTypedObject* ReadObject( const VType* pClassRefRequested, unsigned int* pObjectSize = NULL );

  
  /// \brief
  ///   Write object to the archive. It writes the object type information, following by the object's data, by calling
  ///   the object's Serialize() method.
  /// \param pObj
  ///   the object to be written
  /// \param pForceClass
  ///   the type information that should be written for this object. If \c NULL (default), the object's reflected 
  ///   type information will be written.
  VBASE_IMPEXP virtual void WriteObject( const VTypedObject* pObj, const VType* pForceClass=NULL);

  /// \brief
  ///   Internal function: Store object to the map containing pointers already saved/loaded objects
  ///   when serializing.
  /// 
  /// Store object to the map containing pointers already saved/loaded objects when serializing.
  /// This map is used to avoid the objects serializing twice (or more) when their serialization
  /// methods are called multiple times from different places of the code.
  ///
  /// \param pObj
  ///   Object to store.
  VBASE_IMPEXP void MapObject( const VTypedObject* pObj );

  /// \brief
  ///   Mark an object as 'dead' (saving and loading).
  ///
  /// If an object is destroyed during serialization, this function must be called to avoid conflicts when a new
  /// object is created during the same session that coincidentally has the same pointer
  ///
  /// \param pObj
  ///   Object to remove from all internal lists.
  VBASE_IMPEXP void UnMapObject( const VTypedObject* pObj );

public:
  /// \brief
  ///   Write class (type) info to the archive.
  VBASE_IMPEXP void WriteClass( const VType* pType );
  
  /// \brief
  ///   Read class (type) info from the archive
  VBASE_IMPEXP VType* ReadClass( const VType* pClassRefRequested = 0, UINT* pSchema = 0, DWORD* pObjTag = 0 );

  /// \brief
  ///   Serialize class (type) info using Read/Write methods
  VBASE_IMPEXP void SerializeClass( const VType* pClassRef );

private:
  /// \brief
  ///   Stores a runtime class description
  VBASE_IMPEXP void StoreType( VType *pType ) const;

  /// \brief
  ///   Runtime class serialization code, loads a runtime class description
  VBASE_IMPEXP VType* Load( UINT* pwSchemaNum, char *pDestClassname=NULL );

public:
  /// \brief
  ///   Serializes a proxy creator object.
  /// 
  /// This function calls the IVSerializationProxyCreator::CreateProxy function on the passed
  /// object the first time the creator object is written into that archive.
  /// 
  /// Later, WriteProxyObject looks up the proxy object in a map.
  /// 
  /// The proxy object is then fully serialized.
  /// 
  /// \param pCreator
  ///   The creator object to serialize
  /// 
  /// \note
  ///   The archive stores the returned proxy objects and cleans them up when closing the archive.
  /// 
  /// \sa VArchive::ReadProxyObject
  /// \sa IVSerializationProxyCreator
  /// \sa IVSerializationProxyCreator::CreateProxy
  /// \sa IVSerializationProxy
  /// 
  /// \example
  ///   \code
  ///   ar.WriteProxyObject(pEntity->GetModel()); // a managed resource is a proxy creator
  ///   \endcode
  VBASE_IMPEXP void WriteProxyObject(IVSerializationProxyCreator *pCreator);


  /// \brief
  ///   De-serializes a proxy creator object.
  /// 
  /// This function deserializes a proxy object and calls IVSerializationProxy::GetInstance on that
  /// object, which will then be passed on as a return value.
  /// 
  /// The actual implementation of the class derived from IVSerializationProxy is responsible for
  /// returning a fully initialized object.
  /// 
  /// \param pExpectedType
  ///   The expected class of the proxy object (optional, for verification purposes).
  /// 
  /// \return
  ///   The creator object that has been de-serialized indirectly via the proxy object
  /// 
  /// \note
  ///   The archive stores the returned proxy objects and cleans them up when closing the archive.
  /// 
  /// \sa VArchive::WriteProxyObject
  /// \sa IVSerializationProxyCreator
  /// \sa IVSerializationProxyCreator::CreateProxy
  /// \sa IVSerializationProxy
  /// 
  /// \example
  ///   \code
  ///   VisModel_cl *pModel = (VisModel_cl*)ar.ReadProxyObject();
  ///   \endcode
  VBASE_IMPEXP IVSerializationProxyCreator* ReadProxyObject(VType *pExpectedType = NULL);

  /// \brief
  ///   Write out an integer value in compressed format. Small integers use less than 4 bytes in this format.
  /// 
  /// Unlike the '<<' operator for int values, this function analyzes the passed int value and writes out a dynamic number of bytes.
  /// Accordingly, an integer value written by this function can only be reconstructed by reading it with the ReadCompressedInt counterpart.
  /// Since a leading compression header of one byte has to be written, this function writes out 5 bytes in worst case. So only use this function
  /// if the integer value is likely to be a small positive or negative value (e.g. enum value). For instance, values in the range [-31..31] are written as a single byte.
  /// This function is useful if archive size bandwidth is really critical, e.g. for streaming data over the network.
  ///
  /// \param iValue
  ///   Value to write out
  ///
  /// \return
  ///   Number of bytes written (1,2,3,4 or 5)
  VBASE_IMPEXP int WriteCompressedInt(unsigned int iValue);

  /// \brief
  ///   Reads and returns an integer value that has been written with the WriteCompressedInt counterpart
  VBASE_IMPEXP unsigned int ReadCompressedInt();

  /// \brief
  ///   Registers a callback function to be called when the archive is being closed
  VBASE_IMPEXP void RegisterCloseCallback( V_ARCHIVECALLBACK_ONCLOSE onCloseCB );
  
  /// \brief
  ///   Sets the version of the archive when loading data. 
  /// 
  /// This value is used by the engine for version handling inside objects' serialization functions.
  /// Defaults to 0 if it hasn't been set.
  /// See GetLoadingVersion for more information.
  ///
  /// \param iLoadingVersion
  ///   Version number to set.
  inline void SetLoadingVersion(int iLoadingVersion) { m_iLoadingVersion = iLoadingVersion; }

  /// \brief
  ///   Get the version of the archive when loading. 
  /// 
  /// This value has to be set manually by the loading code and can be used by  the serialize
  /// functions in order to e.g. get the current map version. 
  /// 
  /// This value is used by the engine for versioning its objects in the serialization function.
  /// 
  /// If the loading code does not set the loading version, it will be set to 0.
  /// 
  /// See SetLoadingVersion for more info.
  inline int GetLoadingVersion() { return m_iLoadingVersion; }
  
  /// \brief
  ///   Stores the version of the archive when loading data. 
  /// 
  /// This value should be set by code after the file has been opened.
  /// The version does not get serialized.
  /// See GetUserVersion for more info.
  ///
  /// \param iUserVersion
  ///   User version to set.
  inline void SetUserVersion(int iUserVersion) { m_iUserVersion = iUserVersion; }

  /// \brief
  ///   Retrieves the stored user version number when loading data. 
  /// 
  /// This value should be set by code after the file has been opened.
  /// 
  /// This value or the Loading Version can be used by the serialization functions for versioning
  /// user objects.
  /// 
  /// See SetLoadingVersion for more info.
  inline int GetUserVersion() { return m_iUserVersion; }

  /// \brief
  ///   Can be used to offset shape positions inside this archive
  /// 
  /// Positionable shapes in this archive (e.g. VisObject3D_cl) can be transformed during serialization already. This feature
  /// can be used for prefab implementations that de-serialize objects with a specific prefab instance transformation.
  /// Custom shape types must implement the offset properly, but they inherit this feature automatically from class VisObject3D_cl.
  /// 
  /// \param offset
  ///   receives the position translation
  /// 
  /// \param rotation
  ///   receives the rotation matrix
  /// 
  /// \param rotationAsEuler
  ///   receives the rotation as Euler angles (must represent the same rotation though)
  ///
  /// \return
  ///   TRUE if a custom transformation should be used, FALSE if not. If the return value is FALSE, the passed references can remain uninitialized.
  VBASE_IMPEXP virtual BOOL GetCustomShapeTransformation(hkvVec3& offset, hkvMat3& rotation, hkvVec3& rotationAsEuler)
  {
    return FALSE;
  }

  /// \brief
  ///  Checks if the archive has length information for the current object being deserialized
  ///
  /// \return
  ///  true if yes, false otherwise
  inline bool HasObjectLength() const 
  {
    return ( m_bUseObjectLengths && (m_remainingObjectLengths.Size() > 0) );
  }

  /// \brief
  ///  Gets the length of the current object being deserialized
  ///
  /// \return
  ///  the size of the object in byte
  inline unsigned int GetObjectLength() const 
  {
    VASSERT_MSG(HasObjectLength(), "Archive does not have object length information");
    return m_remainingObjectLengths.Top();
  }

  ///
  /// @}
  ///

protected:
  VString m_szArchiveName; ///< Archive name used in error messages and exceptions
  HKV_DEPRECATED_2013_1 UINT m_nObjectVersion;

  VTypeManager* m_pTypeManager; ///< Pointer to the type manager which is used to create objects
  VisZoneResource_cl *m_pCurrentZone; ///< Pointer to the zone currently being serialized (if any)

  VMapPtrToUInt m_storeMap; ///< Map for storing Pointer->ID assignments during saving

private:
  IVFileInStream*   m_pInStream;
  IVFileOutStream*  m_pOutStream;

  // Buffer management
  int m_nBufSize;
  char *m_lpBufStart,*m_lpBufMax,*m_lpBufCur;

  // Object reference management
  VPList m_loadArray;       // array for loading
  VPList m_DeserializationObjects; // separate array for de-serialized typed objects that need to call the callback in the end
  UINT m_nMapCount;

  VMapPtrToUInt* m_pSchemaMap; // map to keep track of mismatched schemas

  // map for proxy creator<->obj translation
  VMapPtrToPtr m_ProxyMap;
  VSerializationProxyCollection m_ProxyList;

  // list of the all registered OnClose callback functions. 
  // See RegisterCloseCallback method for more info.
  VPList* m_pListOnCloseCallBacks;

  // flag to identify if the nodes are saved recursively to the archive
  // Use SetRecurseNodes method to set this flag.
  int m_iLoadingVersion;
  int m_iUserVersion;
  int m_iFilePos; ///< helper variable for GetPos function
  char m_StaticBuffer[4096];
  VSerializationContext m_Context;

  //Used by constructors
  void InitArchive(const char* szArchiveName, VTypeManager *pTypeManager, int nBufSize);
  void BaseReInit(IVFileInStream* pInStream, IVFileOutStream* pOutStream);
  void DecrementObjectLength(unsigned int size);

protected:
  bool m_bUseObjectLengths; // if every object has a associated object length or not

  typedef VFixedSizeStack<unsigned int,OBJECT_LENGTH_STACK_DEPTH> remainingObjectLengths_t;
  remainingObjectLengths_t m_remainingObjectLengths; // stores the remaining amount of bytes that can be read for the current object

};


/// \brief
///   Wrapper class to access an archive as a file input stream
class VArchiveInStream : public IVFileInStream
{
public:
  VArchiveInStream(VArchive &_ar) : IVFileInStream(NULL), ar(_ar) {VASSERT(_ar.IsLoading());}
  virtual size_t Read(void* pBuffer,int iLen) {return ar.Read(pBuffer,iLen);}
  virtual void Close() {delete this;}

  virtual BOOL SetPos(LONG iPos, int iMode) {return FALSE;}
  virtual LONG GetPos() {return -1;}
  virtual LONG GetSize() {return -1;}
  virtual const char* GetFileName() {return ar.GetFilename();}

private:
  VArchive &ar;
};

/// \brief
///   Wrapper class to access an archive as a file output stream
class VArchiveOutStream : public IVFileOutStream
{
public:
  VArchiveOutStream(VArchive &_ar) : IVFileOutStream(NULL), ar(_ar) {VASSERT(_ar.IsSaving());}
  virtual size_t Write(const void* pBuffer,size_t iLen) {ar.Write(pBuffer, (UINT) iLen);return iLen;}
  virtual void Close() {delete this;}
  virtual void Flush() {}
  virtual const char* GetFileName() {return ar.GetFilename();}

private:
  VArchive &ar;
};


#if defined(WIN32) && !defined(_VISION_WINRT)

inline VArchive& VArchive::operator<<(int i)
  { return VArchive::operator<<((LONG)i); }
inline VArchive& VArchive::operator<<(unsigned u)
  { return VArchive::operator<<((LONG)u); }
inline VArchive& VArchive::operator>>(int& i)
  { return VArchive::operator>>((LONG&)i); }
inline VArchive& VArchive::operator>>(unsigned& u)
  { return VArchive::operator>>((LONG&)u); }

#elif defined(_VISION_XENON) || defined(_VISION_WINRT) || defined(_VISION_PS3) || defined(_VISION_POSIX) || defined(_VISION_PSP2) || defined(_VISION_WIIU)

inline VArchive& VArchive::operator<<(int i)
  { return VArchive::operator<<((DWORD)i); }
inline VArchive& VArchive::operator<<(unsigned u)
  { return VArchive::operator<<((DWORD)u); }
inline VArchive& VArchive::operator>>(int& i)
  { return VArchive::operator>>((DWORD&)i); }
inline VArchive& VArchive::operator>>(unsigned& u)
  { return VArchive::operator>>((DWORD&)u); }

#endif

inline VArchive& VArchive::operator<<(BYTE by)
  { if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax) Flush();
    *(BYTE*)m_lpBufCur = by; m_lpBufCur += sizeof(BYTE); m_iFilePos += sizeof(BYTE); return *this; }
inline VArchive& VArchive::operator<<(short w)
  { return VArchive::operator<<((WORD)w); }
inline VArchive& VArchive::operator<<(char ch)
  { return VArchive::operator<<((BYTE)ch); }
inline VArchive& VArchive::operator<<(signed char ch)
  { return VArchive::operator<<((BYTE)ch); }
inline VArchive& VArchive::operator<<(bool b)
  { return VArchive::operator<<((BYTE)b); }

inline VArchive& VArchive::operator>>(BYTE& by)
{ 
  if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax)
    FillBuffer(sizeof(BYTE) - (UINT)(m_lpBufMax - m_lpBufCur));
  by = *(BYTE*)m_lpBufCur; 
  m_lpBufCur += sizeof(BYTE);
  m_iFilePos += sizeof(BYTE);
  if( HasObjectLength() )
    DecrementObjectLength(sizeof(BYTE));
  return *this; 
}
inline VArchive& VArchive::operator>>(short& w)
  { return VArchive::operator>>((WORD&)w); }
inline VArchive& VArchive::operator>>(char& ch)
  { return VArchive::operator>>((BYTE&)ch); }
inline VArchive& VArchive::operator>>(signed char& ch)
  { return VArchive::operator>>((BYTE&)ch); }
inline VArchive& VArchive::operator>>(bool &b)
  { return VArchive::operator>>((BYTE&)b); }

// ---------------------------------------------------------------------------------
// inline binary operators for the object serialization
// ---------------------------------------------------------------------------------

inline VBASE_IMPEXP VArchive& operator<<( VArchive& ar, const VTypedObject* pObj )
{
  ar.WriteObject( pObj, NULL );
  return ar;
}

inline VBASE_IMPEXP VArchive& operator>>( VArchive& ar, VTypedObject*& pObj )
{
  pObj = ar.ReadObject( 0 ); 
  return ar;
}

inline VBASE_IMPEXP VArchive& operator>>( VArchive& ar, const VTypedObject*& pObj )
{
  pObj = ar.ReadObject( 0 );
  return ar;
}



// ---------------------------------------------------------------------------------
// Since VArchive uses VRefCountedCollection, we need to put it's Serialize
// definition, which uses VArchive, here.
// ---------------------------------------------------------------------------------
template<class RCCLASS>
void VRefCountedCollection<RCCLASS>::SerializeX(VArchive &ar)
{
  if (ar.IsLoading())
  {
    int iCount;
    ar >> iCount;
    Clear();
    EnsureCapacity(iCount);
    for (int i=0;i<iCount;i++)
    {
      RCCLASS *pObj;
      ar >> pObj;
      Add(pObj);
    }
  } else
  {
    int iCount = Count();
    ar << iCount;
    for (int i=0;i<iCount;i++)
      ar << GetAt(i);
  }
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
