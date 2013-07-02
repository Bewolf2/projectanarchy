/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VChunkFile.hpp

#ifndef VCHUNKFILE_HPP_INCLUDED
#define VCHUNKFILE_HPP_INCLUDED



class VChunkFileOutStream;
class VChunkFileInStream;


/// \brief
///   chunk ID type. 32bit integer value
typedef unsigned int CHUNKIDTYPE;

#define CHUNKID_ANY   -1

/// \brief
///   File class for loading/saving chunk based binary files
/// 
/// A chunk based file is a convenient approach for loading binary files that contain different
/// components such as header information, vertex data, index data etc.
/// 
/// Each component can be put into a separate chunk which is identified by its chunk ID (a
/// 32bit identifier in this case)
/// 
/// When writing a chunk file, the respective chunk data is written in between a pair of
/// StartChunk/EndChunk calls.
/// 
/// For reading, you have two options. You can derive a class and override OnStartChunk. This
/// function receives the chunk information (ID and length) and allows you to load all the root
/// chunks by calling ParseFile. You can also use OpenChunk/EndChunk directly.
/// 
/// Chunks that are unknown, i.e. those who have an unsupported chunk ID, can be skipped or be
/// processed by the base implementation.
/// 
/// The Vision SDK ships with a sample that shows how to implement a custom chunk file.
class VChunkFile : public VBaseObject
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor for chunk based file
  VBASE_IMPEXP VChunkFile();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VChunkFile();

  ///
  /// @}
  ///

  ///
  /// @name Custom File Version
  /// @{
  ///


  /// \brief
  ///   Sets a custom usable version number for this file. Ignored by this class. The default is 0.
  inline void SetCustomFileVersion(int iVersion)
  {
    m_iCustomVersion = iVersion;
  }

  /// \brief
  ///   Gets the custom version number that has been set via SetCustomFileVersion
  inline int GetCustomFileVersion() const
  {
    return m_iCustomVersion;
  }

  ///
  /// @}
  ///

  ///
  /// @name File Opening For Loading
  /// @{
  ///

  /// \brief
  ///   Opens a file using an existing archive instance.
  /// 
  /// All read accesses in the Read function will go through this archive
  /// 
  /// \param ar
  ///   The archive used for reading. Must be an archive created from a in-stream.
  /// 
  /// \param bCloseArchive
  ///   If \c TRUE, the archive will be closed after closing the file.
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL Open(VArchive &ar, BOOL bCloseArchive=FALSE);

  /// \brief
  ///   Opens a file using an existing file in stream.
  /// 
  /// All read accesses in the Read function will go through this stream
  /// 
  /// \param pInStream
  ///   The stream used for reading.
  /// 
  /// \param bCloseStream
  ///   If \c TRUE, the stream will be closed after closing the file. The file stream will not be
  ///   available afterwards in this case
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL Open(IVFileInStream *pInStream, BOOL bCloseStream=FALSE);

  /// \brief
  ///   Opens a file using a file stream manager.
  /// 
  /// \param szFilename
  ///   file name of the file to open
  /// 
  /// \param pManager
  ///   pointer to the stream manager used for opening the file. This pointer can be NULL to use
  ///   vBase's standard manager. To use the data directories set in the engine, the
  ///   engine's file manager should be passed here (Vision::File.GetManager())
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL Open(const char *szFilename, IVFileStreamManager *pManager=NULL);



  ///
  /// @}
  ///

  ///
  /// @name File Opening For Writing
  /// @{
  ///

  /// \brief
  ///   Creates a file using an existing archive
  /// 
  /// \param ar
  ///   The archive used for writing.
  /// 
  /// \param bCloseArchive
  ///   If \c TRUE, the archive will be closed after closing the file.
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL Create(VArchive &ar, BOOL bCloseArchive=FALSE);

  /// \brief
  ///   Creates a file using an existing out stream
  /// 
  /// \param pOutStream
  ///   The archive used for writing.
  /// 
  /// \param bCloseStream
  ///   If \c TRUE, the stream will be closed after closing the file. The file stream will not be
  ///   available afterwards in this case
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL Create(IVFileOutStream *pOutStream, BOOL bCloseStream=FALSE);

  /// \brief
  ///   Creates a new file using a file stream manager.
  /// 
  /// \param szFilename
  ///   file name of the file to create
  /// 
  /// \param pManager
  ///   pointer to the stream manager used for creating the file. This pointer can be NULL to use
  ///   vBase's standard manager.
  /// 
  /// \return
  ///   \c TRUE, if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL Create(const char *szFilename, IVFileStreamManager *pManager=NULL);


  ///
  /// @}
  ///

  ///
  /// @name Close File
  /// @{
  ///

  /// \brief
  ///   Closes this chunk file
  /// 
  /// This function flushes remaining data queue and closes the file streams (if bCloseArchive flag
  /// was specified)
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL Close();

  ///
  /// @}
  ///

  ///
  /// @name File Status Functions
  /// @{
  ///


  /// \brief
  ///   Returns the filename
  VBASE_IMPEXP const char *GetFilename() const;

  /// \brief
  ///   Returns the file version number
  inline unsigned int GetFileVersion() const {return m_iFileVersion;}

  /// \brief
  ///   Indicates whether the file is currently loading
  inline BOOL IsLoading() const {return m_eStatus == STATUS_LOADING;}

  /// \brief
  ///   Indicates whether the file is currently saving
  inline BOOL IsSaving() const {return m_eStatus == STATUS_SAVING;}

  /// \brief
  ///   Indicates whether an error has occurred
  inline BOOL IsInErrorState() const {return m_eStatus == STATUS_ERROR;}

  /// \brief
  ///   Indicates whether the end of file has been reached.
  inline BOOL IsEOF() const {return m_eStatus == STATUS_EOF;}

  /// \brief
  ///   Get the current file offset position
  inline unsigned int GetCurrentFilePos() {return m_iFilePos;}

  /// \brief
  ///   Returns the number of remaining bytes in the chunk. Pass -1 to get the current stack depth
  VBASE_IMPEXP unsigned int GetRemainingChunkByteCount(int iStackDepth=-1) const;

  /// \brief
  ///   Internal function to determine the current stack depth for reading/writing chunks
  inline int GetCurrentChunkStackDepth() {return m_iStackDepth+1;}

  /// \brief
  ///   Internal function
  inline int GetChunkStartTagSize() const {return 4+sizeof(CHUNKIDTYPE)+4;}

  /// \brief
  ///   Internal function
  inline int GetChunkEndTagSize() const {return 4+sizeof(CHUNKIDTYPE);}



  ///
  /// @}
  ///

  ///
  /// @name Write Functions
  /// @{
  ///

  /// \brief
  ///   Overridable function that gets called directly after creating a new file using the Create
  ///   function
  VBASE_IMPEXP virtual void OnStartSaving() {}

  /// \brief
  ///   Overridable function that gets called directly before closing the output file.
  VBASE_IMPEXP virtual void OnFinishSaving() {}

  /// \brief
  ///   Starts a new chunk in the output file for writing
  /// 
  /// The chunk is defined by its ID and the chunk length.
  /// 
  /// If the chunk length is not known ahead of time, \c -1 can be passed instead. The
  /// actual length will then be patched when calling the EndChunk function.
  /// 
  /// It is also possible to recursively interleave chunks.
  /// 
  /// \param chunkID
  ///   The chunk ID (32 bit value). Can for instance be a 4-character ID such as 'HEAD' when this
  ///   chunk is supposed to contain header information
  /// 
  /// \param iChunkLen
  ///   The length of the chunk in bytes. If specified, it must be the exact number of bytes
  ///   to be written.
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL StartChunk(CHUNKIDTYPE chunkID, int iChunkLen = -1);

  /// \brief
  ///   Opens a new chunk in the input file for reading.
  ///
  ///   If you specify a expected chunk type when traversing the root chunks, all other chunks will
  ///   be skipped. 
  ///
  /// \param piChunkID
  ///   if not \c NULL, this receives the ID of the chunk that has been opened
  /// \param piChunkLen
  ///   if not \c NULL, this receives the length of the chunk that has been opened
  /// \param iExpectedID
  ///   ID of the chunk to open
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL OpenChunk(CHUNKIDTYPE *piChunkID = NULL, int *piChunkLen = NULL, CHUNKIDTYPE iExpectedID = (CHUNKIDTYPE)CHUNKID_ANY);

  /// \brief
  ///   Ends a read or write chunk
  /// 
  /// If no chunk length has been specified in the StartChunk function, this function will patch
  /// the chunk length and the chunk start position.
  /// 
  /// If a chunk length has been specified in the StartChunk function, and the actual number of
  /// written bytes does not match the specified size, this function will either create a warning
  /// (in case too few bytes have been written so it can be filled up) or an error in case too many
  /// bytes have been written.
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL EndChunk();

  /// \brief
  ///   Writes a data to the current chunk
  /// 
  /// This function wraps the file stream/archive write function and additionally performs some
  /// checks such as checking the chunk limit.
  /// 
  /// \param pData
  ///   Data to write
  /// 
  /// \param iSize
  ///   Number of bytes to write
  /// 
  /// \return
  ///   The actual number of bytes written. If less than iSize, an error might have
  ///   occurred.
  VBASE_IMPEXP unsigned int Write(const void *pData, unsigned int iSize);

  /// \brief
  ///   Writes a specific number of bytes to the current chunk
  /// 
  /// This function wraps the file stream/archive write function and additionally performs some
  /// checks such as checking the chunk limit.
  /// 
  /// Additionally performs endianness conversion if necessary. For this, the format of the data must
  /// be specified with a format string. See the documentation of EndianSwitchWorker() for the syntax
  /// of this format string.
  /// 
  /// \param pData
  ///   Data to write
  /// 
  /// \param iSize
  ///   Number of bytes to write
  /// 
  /// \param pFormat
  ///   The format string describing the data
  /// 
  /// \param iNumRep
  ///   Number of repetitions of the data pattern described by the format string
  /// 
  /// \return
  ///   Actual number of bytes written. If less than iSize, an error might have
  ///   occurred.
  VBASE_IMPEXP unsigned int Write(const void *pData, unsigned int iSize, const char *pFormat, unsigned int iNumRep = 1);


  /// \brief
  ///   Helper function to write an integer value
  inline BOOL WriteInt(int iVal) {return Write(&iVal,sizeof(int),"i")==sizeof(int);}

  /// \brief
  ///   Helper function to write a 64bit integer value
  inline BOOL WriteInt64(__int64 iVal) {return Write(&iVal,sizeof(__int64),"q")==sizeof(__int64);}

  /// \brief
  ///   Helper function to write a short integer value
  inline BOOL WriteShort(short iVal) {return Write(&iVal,sizeof(short),"s")==sizeof(short);}

  /// \brief
  ///   Helper function to write a float value
  inline BOOL WriteFloat(float fVal) {return Write(&fVal,sizeof(float),"f")==sizeof(float);}

  /// \brief
  ///   Helper function to write a BOOL (32bit) value
  inline BOOL WriteBOOL(BOOL bVal) {return Write(&bVal,sizeof(BOOL),"i")==sizeof(BOOL);}

  /// \brief
  ///   Helper function to write a bool (8bit) value
  inline BOOL Writebool(bool bVal) {return Write(&bVal,sizeof(bool))==sizeof(bool);}

  /// \brief
  ///   Helper function to write a char value
  inline BOOL WriteChar(char cVal) {return Write(&cVal,sizeof(char))==sizeof(char);}

  /// \brief
  ///   Helper function to write a string.
  ///   The internal format is writing the string length as an int and then the characters without
  ///   terminating 0. String length of a NULL string is defined as -1. See also
  ///   VStreamHelper::WriteStringBinary.
  VBASE_IMPEXP BOOL WriteString(const char *szStr);

  /// \brief
  ///   Helper function to write a color ref value
  inline BOOL WriteColorRef(const VColorRef& color)
  {
    BOOL success = 1;
    success &= WriteChar(color.r);
    success &= WriteChar(color.g);
    success &= WriteChar(color.b);
    success &= WriteChar(color.a);
    return success;
  }

  /// \brief
  ///   Helper function to write a hkvVec2
  inline BOOL WriteVertex2f(const hkvVec2& v)      { BOOL b=1; b&=WriteFloat(v.x); b&=WriteFloat(v.y); return b; }

  /// \brief
  ///   Helper function to write a hkvVec3
  inline BOOL WriteVertex3f(const hkvVec3& v)      { BOOL b=1; for(int i=0; i<3; ++i) b&=WriteFloat(v.data[i]); return b; }

  /// \brief
  ///   Helper function to write a hkvVec4
  inline BOOL WriteVertex4f(const hkvVec4& v)      { BOOL b=1; for(int i=0; i<4; ++i) b&=WriteFloat(v.data[i]); return b; }

  /// \brief
  ///   Helper function to write a quaternion
  inline BOOL WriteQuat(const hkvQuat& q)              { BOOL b=WriteFloat(q.getX ()); b&=WriteFloat(q.getY ()); b&=WriteFloat(q.getZ ()); float w2 = -q.getW (); b&=WriteFloat(w2); return b; }

  ///
  /// @}
  ///

  ///
  /// @name Read Functions
  /// @{
  ///

 
  /// \brief
  ///   Overridable function that gets called directly after opening a file using the Open function
  VBASE_IMPEXP virtual void OnStartLoading() {}

  /// \brief
  ///   Overridable function that gets called by the Close function
  VBASE_IMPEXP virtual void OnFinishLoading() {}

  /// \brief
  ///   Reads data from the current chunk
  /// 
  /// This function wraps the file stream/archive Read function and additionally performs some
  /// checks such as checking the chunk limit.
  /// 
  /// \param pData
  ///   Data buffer to read to
  /// 
  /// \param iSize
  ///   Number of bytes to read
  /// 
  /// \return
  ///   Actual number of bytes read. If less than iSize, an error might have occurred.
  VBASE_IMPEXP unsigned int Read(void *pData, unsigned int iSize);

  /// \brief
  ///   Reads data from the current chunk
  ///
  /// Additionally performs endianness conversion if necessary. For this, the format of the data must
  /// be specified with a format string. See the documentation of EndianSwitchWorker() for the syntax
  /// of this format string.
  /// 
  /// \param pData
  ///   Data buffer to read to
  /// \param iSize
  ///   Number of bytes to read
  /// \param pFormat
  ///   The format string describing the data
  /// \param iRepetitions
  ///   Number of repetitions of the data pattern described by the format string
  /// \return
  ///   Actual number of bytes read. If less than iSize, an error might have occurred.
  VBASE_IMPEXP unsigned int Read(void *pData, unsigned int iSize, const char *pFormat, unsigned int iRepetitions = 1);

  /// \brief
  ///   Reads one DWORD and performs endianness conversion if necessary.
  /// \param pData
  ///   Buffer to receive the data
  /// \return
  ///   The number of bytes read. 4 if successful, less in case of an error.
  VBASE_IMPEXP unsigned int ReadDWord( void *pData );
  
  /// \brief
  ///   Reads multiple DWORDs and performs endianness conversion if necessary.
  /// \param pData
  ///   Buffer to receive the data
  /// \param iRepetitions
  ///   The number of DWORDs to read
  /// \return
  ///   The number of bytes read. 4 * \c iRepetitions if successful, less in case of an error.
  VBASE_IMPEXP unsigned int ReadDWords( void *pData, unsigned int iRepetitions );

  /// \brief
  ///   Overridable that gets called when starting to read a new chunk (OpenChunk).
  /// 
  /// The ParseFile function will cause this to happen for all root chunks in the file.
  /// 
  /// The user is responsible for reading sub-chunks (see ProcessSubChunk).
  /// 
  /// In this function, a switch statement can be used to read the different chunks from the file.
  /// 
  /// Each chunk type should read all bytes from it (specified by the iChunkLen).
  /// 
  /// If reading less, the remaining bytes will be discarded. Reading more will cause an error.
  /// 
  /// \param chunkID
  ///   The ID of the chunk.
  /// 
  /// \param iChunkLen
  ///   The Length of the chunk.
  /// 
  /// \return
  ///   \c TRUE if everything was OK.
  VBASE_IMPEXP virtual BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen) { return TRUE;}

  /// \brief
  ///   Overridable that gets called after having read a chunk
  /// \param chunkID
  ///   The ID of the chunk.
  /// \return
  ///   \c TRUE if everything was OK.
  VBASE_IMPEXP virtual BOOL OnEndChunk(CHUNKIDTYPE chunkID) {return TRUE;}
  
  /// \brief
  ///   Initiates to read a sub chunk
  /// 
  /// This function might be called within a OnStartChunk call to read a sub-chunk
  /// 
  /// The function verifies that the next bytes in the file mark a valid chunk start and creates an
  /// error otherwise.
  /// 
  /// If successful, it calls OnStartChunk with the new chunk ID of the sub-chunk.
  /// 
  /// The file position of a sub-chunk must exactly match the relative position of the respective
  /// StartChunk calls when writing the file.
  /// 
  /// \param expectedID
  ///   Allows to specify an expected ID which is useful for validation. If the expected ID does
  ///   not match the met ID, an error will be produced. If expectedID is set to the CHUNKID_ANY
  ///   constant, no comparison will be performed and any ID is allowed.
  /// 
  /// \return
  ///   \c TRUE, if everything worked fine. When failed, GetLastError() returns the last
  ///   error string. \c FALSE might also indicate the end of the file.
  VBASE_IMPEXP BOOL ProcessSubChunk(CHUNKIDTYPE expectedID=(CHUNKIDTYPE)CHUNKID_ANY);

  /// \brief
  ///   Skips the current read chunk or the remaining bytes in the chunk
  /// 
  /// \param bStore
  ///   If TRUE, the whole chunk data will be stored in memory and written out as a chunk when
  ///   saving the binary file again.
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL SkipChunk(BOOL bStore);

  /// \brief
  ///   Discards the next iCount number of bytes when reading
  ///
  /// \param iCount
  ///   Number of bytes to skip.
  /// 
  /// \return
  ///   \c TRUE if everything worked fine. When failed, GetLastError() returns the last
  ///   error string.
  VBASE_IMPEXP BOOL SkipBytes(int iCount);
  
  /// \brief
  ///   Helper function to read an integer value
  inline BOOL ReadInt(int &iVal) {return ReadDWord(&iVal)==sizeof(int);}

  /// \brief
  ///   Helper function to read a 64 bit integer value
  inline BOOL ReadInt64(__int64 &iVal) {return Read(&iVal,sizeof(__int64),"q")==sizeof(__int64);}

  /// \brief
  ///   Helper function to read short integer value
  inline BOOL ReadShort(short &iVal) {return Read(&iVal,sizeof(short),"s")==sizeof(short);}
  
  /// \brief
  ///   Helper function to read a float value
  inline BOOL ReadFloat(float &fVal) {return ReadDWord(&fVal)==sizeof(float);}

  /// \brief
  ///   Helper function to read a BOOL (32bit) value
  inline BOOL ReadBOOL(BOOL &bVal) {return ReadDWord(&bVal)==sizeof(BOOL);}

  /// \brief
  ///   Helper function to read a bool (8bit) value
  inline BOOL Readbool(bool &bVal) {return Read(&bVal,sizeof(bool))==sizeof(bool);}

  /// \brief
  ///   Helper function to read a character value
  inline BOOL ReadChar(char &cVal) {return Read(&cVal,sizeof(char))==sizeof(char);}

  /// \brief
  ///   Helper function to read a character string.
  ///   The string pointer will be stored in **szStr. The pointer is NULL for a string of length -1
  ///   The pointer has to be free'd again using VBaseDealloc().
  ///   See also VStreamHelper::ReadStringBinary function
  VBASE_IMPEXP BOOL ReadString(char **szStr);

  /// \brief
  ///   Helper function to read a character string.
  ///   This version of ReadString puts the read string into the VString reference
  VBASE_IMPEXP BOOL ReadString(VString &str);

  /// \brief
  ///   Helper function to read a character string.
  ///   This version of ReadString puts the read string into the VString reference
  VBASE_IMPEXP BOOL ReadString(VHashString &str);

  /// \brief
  ///   Helper function to read a character string into a buffer.
  ///   The buffer will be filled with at most iBufferSize-1 characters.
  ///   If the string in the file is actually longer, a warning will occur and the string is
  ///   truncated.
  ///   The remaining bytes in the file are skipped so passing a too small buffer will not mess the
  ///   parsing.
  VBASE_IMPEXP BOOL ReadString(char *szBuffer, int iBufferSize);

  /// \brief
  ///   Helper function to read a color ref value
  inline BOOL ReadColorRef(VColorRef &color) 
  {
    // Read the color in the RGBA order (check if this works correct)
    char red,green,blue,alpha;
    BOOL success = 1;
    success &= ReadChar(red);
    success &= ReadChar(green);
    success &= ReadChar(blue);
    success &= ReadChar(alpha);
    color.SetRGBA(red,green,blue,alpha);
    return success;
  }

  /// \brief
  ///   Helper function to read a hkvVec2
  inline BOOL ReadVertex2f(hkvVec2& v)     { BOOL b = ReadFloat (v.x); b &= ReadFloat (v.y); return b; }

  /// \brief
  ///   Helper function to read a hkvVec3
  inline BOOL ReadVertex3f(hkvVec3& v)     { BOOL b = ReadFloat (v.x); b &= ReadFloat (v.y); b &= ReadFloat (v.z); return b; }

  /// \brief
  ///   Helper function to read a hkvVec4
  inline BOOL ReadVertex4f(hkvVec4& v)     { BOOL b = ReadFloat (v.x); b &= ReadFloat (v.y); b &= ReadFloat (v.z); b &= ReadFloat (v.w); return b; }

  /// \brief
  ///   Helper function to read a quaternion
  inline BOOL ReadQuat(hkvQuat& q)             { float f[4]; BOOL b = ReadFloat (f[0]); b &= ReadFloat (f[1]); b &= ReadFloat (f[2]); b &= ReadFloat (f[3]); q.setValuesDirect_Old (f[0], f[1], f[2], f[3]); return b; }

  ///
  /// @}
  ///

  ///
  /// @name Misc Read and Write
  /// @{
  ///

  /// \brief
  ///   Reads or writes data according to the current access type of the file
  VBASE_IMPEXP BOOL Serialize(void *pData, int iSize);

  /// \brief
  ///   Reads or writes a string according to the current access type of the file
  VBASE_IMPEXP BOOL SerializeString(VString &str);


  ///
  /// @}
  ///

  ///
  /// @name Error and Warning
  /// @{
  ///


  /// \brief
  ///   Overridable function that gets called when an error occurs.
  /// 
  /// An error occurs in case there is a critical bug in the file structure that makes proceeding
  /// impossible.
  /// 
  /// After an error has occurred, all file reading/writing operations will return FALSE and the
  /// IsInErrorState function returns TRUE
  /// 
  /// Override this function to find problems (both when reading and writing) immediately.
  /// 
  /// \param szError
  ///   String that describes the error
  /// 
  /// \param chunkID
  ///   the ID of the chunk where the error occurred
  /// 
  /// \param iChunkOfs
  ///   The relative file offset in the chunk
  VBASE_IMPEXP virtual void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs) {}

  /// \brief
  ///   Overridable function that gets called when an a non-critical warning occurs.
  /// 
  /// \param szWarning
  ///   String that describes the warning.
  /// 
  /// \param chunkID
  ///   The ID of the chunk where the warning occurred.
  /// 
  /// \param iChunkOfs
  ///   The relative file offset in the chunk.
  VBASE_IMPEXP virtual void OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs) {}

  /// \brief
  ///   Gets error information of the last error. See also OnError
  VBASE_IMPEXP static const char *GetLastError(CHUNKIDTYPE *pLastChunk=NULL, unsigned int *pChunkOfs=NULL);

  /// \brief
  ///   Set a custom error when implementing a custom file format.
  /// 
  /// After calling this function, the file will switch to error state
  /// 
  /// \param szError
  ///   String that describes the error
  VBASE_IMPEXP void SetError(const char *szError);

  /// \brief
  ///   Outputs a custom warning string
  /// 
  /// \param szWarning
  ///   String that describes the warning
  VBASE_IMPEXP void Warning(const char *szWarning);



  ///
  /// @}
  ///

  ///
  /// @name Embedded Streams
  /// @{
  ///


  /// \brief
  ///   Returns an out stream to write into the current chunk
  /// 
  /// This stream wraps the Write function of the chunk file.
  /// 
  /// Getting the stream is useful to embed archives into chunk files, i.e. write serialisation
  /// data  into a file chunk.
  /// 
  /// \return
  ///   The output stream
  /// 
  /// \sa IVFileOutStream
  VBASE_IMPEXP IVFileOutStream *GetChunkOutStream();

  /// \brief
  ///   Returns an in stream to read from the current chunk
  /// 
  /// This stream wraps the Read function of the chunk file.
  /// 
  /// The file length is set to the (remaining) size of the chunk.
  /// 
  /// Getting the stream is useful to embed archives into chunk files, i.e. read serialisation data
  /// from a file chunk.
  /// 
  /// \return
  ///   The input stream
  /// 
  /// \sa IVFileInStream
  VBASE_IMPEXP IVFileInStream *GetChunkInStream();

  /// \brief
  ///   Parses an input file, i.e. starts calling the OnStartChunk function for all root chunks.
  /// 
  /// This function is usually called directly after successfully opening a chunk file
  /// 
  /// \return
  ///   \c TRUE if successful. When failed, GetLastError() returns the last error
  ///   string.
  /// 
  /// \sa VChunkFile::OnStartChunk
  VBASE_IMPEXP BOOL ParseFile();

  /// \brief
  ///   When called, all Write operations are ignored and just sum up the sizes. See EndDryRun().
  VBASE_IMPEXP void BeginDryRun();

  /// \brief
  ///   Returns the number of bytes written since BeginDryRun(). Can be used to determine the exact
  ///   chunk size, so no temp allocation takes place in writing.
  VBASE_IMPEXP int EndDryRun();

protected:

  ///
  /// @}
  ///

private:
  enum ChunkFileStatus_e
  {
    STATUS_UNDEFINED = 0,
    STATUS_LOADING,
    STATUS_SAVING,
    STATUS_EOF,
    STATUS_ERROR
  };
  unsigned int _Write(const void *pData, unsigned int iSize);
  unsigned int _Read(void *pData, unsigned int iSize);
  BOOL PushChunk(CHUNKIDTYPE expectedID=(CHUNKIDTYPE)CHUNKID_ANY, CHUNKIDTYPE *piID = NULL, int *piChunkSize = NULL);
  BOOL PopChunk();
  BOOL _OnStartLoading();
  BOOL _OnStartSaving();
  void ResetParsingVars();
  void WriteFileEndTag();
  void ResetStoredChunkData();
  BOOL StoreChunkData(CHUNKIDTYPE chunkID, int iSize);
  BOOL SaveStoredChunkData();
  BOOL WriteDummyBytes(int iCount, char iByte);
  // properties
  unsigned int m_iFileVersion;

  // reading + writing
  ChunkFileStatus_e m_eStatus;
  IVFileInStream* m_pInStream;
  IVFileOutStream* m_pOutStream;
  VArchive *m_pArchive;
  bool m_bOwnsStreams;

  // memory writing
  DynArray_cl<char> m_TempMem;
  int m_iTempMemPos;

  // chunk hierarchy
  CHUNKIDTYPE m_iCurrentChunk;
  DynArray_cl<CHUNKIDTYPE> m_ChunkStack;
  DynArray_cl<int> m_ChunkSizeOfs;
  DynArray_cl<int> m_ChunkSizeTempMemOfs;
  int m_iStackDepth;
  int m_iFirstUndefinedSize;
  unsigned int m_iFilePos;

  // storing unknown chunks
  int m_iNumStoredChunks;
  DynArray_cl<int *> m_StoredChunkData;
  
  // error related
  static VString g_szLastError;
  static CHUNKIDTYPE g_iLastChunk; 
  static unsigned int g_iLastChunkPos;

  int m_iDryRunByteCount;
  int m_iCustomVersion;
};


/// \brief
///   Helper chunk based file class that implements functionality to output the chunk structure to
///   a text file
/// 
/// It reads the whole file and outputs the chunk information. However, it ignores sub-chunks,
/// because it does not know anything about relative positions within the chunks
class VChunkFileDebugOut_cl : public VChunkFile
{
public:

  /// \brief
  ///   Opens both the source file and the output text file
  /// 
  /// \param szFilename
  ///   file name of the chunk file to open
  /// 
  /// \param szOutName
  ///   output file name of the text file
  /// 
  /// \param pManager
  ///   Stream manager used to create file streams
  VBASE_IMPEXP BOOL Open(const char *szFilename, const char *szOutName, IVFileStreamManager *pManager=NULL);

  /// \brief
  ///   Closes the file again
  VBASE_IMPEXP BOOL Close();


  VBASE_IMPEXP virtual BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL OnEndChunk(CHUNKIDTYPE chunkID) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs) HKV_OVERRIDE;

private:
  IVFileOutStream* m_pTextOut;
};


typedef VChunkFile VChunkFile_cl; ///<backwards compatibility


/// \brief
///   Wrapper class to access a VChunkFile instance as a file stream
class VChunkFileInStream : public IVFileInStream
{
public:

  /// \brief
  ///   Constructor that takes the source chunk file as input. The remaining bytes in the current chunk are used as file size
  VChunkFileInStream(VChunkFile &_ar) : IVFileInStream(NULL), ar(_ar)
  {
    m_iRemaining = m_iFileSize = ar.GetRemainingChunkByteCount();
    m_bEOF = m_iFileSize==0;
  }

  /// \brief
  ///   Closes this stream and deletes this instance.
  virtual void Close() HKV_OVERRIDE {delete this;}

  virtual size_t Read(void* pBuffer,int iLen) HKV_OVERRIDE
  {
    if (iLen>m_iRemaining)
    {
      iLen=m_iRemaining;
      m_bEOF = true;
      if (!iLen) return 0;
    }
    int iRead = ar.Read(pBuffer,iLen);
    m_iRemaining-=iRead;
    return iRead;
  }

  /// \brief
  ///  Returns the filename of the underlying chunk file
  virtual const char* GetFileName() HKV_OVERRIDE {return ar.GetFilename();}

  /// \brief
  ///   SetPos is not supported for this strean, so calling this function will assert!
  virtual BOOL SetPos(LONG iPos, int iMode) {VASSERT(FALSE);return FALSE;}

  virtual LONG GetPos() HKV_OVERRIDE {return m_iFileSize-m_iRemaining;}

  /// \brief
  ///   Returns the size of the data chunk in the underlying chunk file
  virtual LONG GetSize() HKV_OVERRIDE {return m_iFileSize;}

private:
  int m_iFileSize;  ///< Size of the open chunk
  int m_iRemaining; ///< Remaining bytes that can be read
  VChunkFile &ar;   ///< Chunk file reference
};


/// \brief
///   Wrapper class to access a VChunkFile instance as a file stream
class VChunkFileOutStream : public IVFileOutStream
{
public:
  /// \brief
  ///   Constructor that takes the target chunk file as input.
  VChunkFileOutStream(VChunkFile &_ar) : IVFileOutStream(NULL), ar(_ar) 
  {
    VASSERT(_ar.IsSaving());
  }

  virtual size_t Write(const void* pBuffer,size_t iLen) HKV_OVERRIDE
  { 
    ar.Write(pBuffer, (unsigned int)iLen);return iLen;
  }

  /// \brief
  ///   Closes the stream and deletes this instance
  virtual void Close() HKV_OVERRIDE {delete this;}
  
  virtual void Flush() HKV_OVERRIDE {}

  /// \brief
  ///   Returns the filename of the underlying chunk file
  virtual const char* GetFileName() HKV_OVERRIDE {return ar.GetFilename();}

private:
  VChunkFile &ar;
};


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
