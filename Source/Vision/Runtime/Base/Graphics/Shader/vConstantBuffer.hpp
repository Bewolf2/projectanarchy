/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vConstantBuffer.hpp

#ifndef VCONSTANTBUFFER_HPP_INCLUDED
#define VCONSTANTBUFFER_HPP_INCLUDED


VBASE_IMPEXP int GetRegisterCountFromValueType(VValueType_e eType);

class VShaderConstantTable;
class VFloatConstantBuffer;
class IVConstantBuffer;

typedef VSmartPtr<VShaderConstantTable> VShaderConstantTablePtr;

#if defined(_VR_DX11)

  #define VCONSTANTBUFFER_REGISTER_GRANULARITY 16
  #define VCONSTANTBUFFER_REGISTER_SIZE 1
  typedef IVConstantBuffer     VShaderConstantBuffer; ///< generic buffer
  typedef void VConstantBufferRegisterType;

#else

  #define VCONSTANTBUFFER_REGISTER_GRANULARITY 1
  #define VCONSTANTBUFFER_REGISTER_SIZE 16
  typedef VFloatConstantBuffer VShaderConstantBuffer; ///< always float
  typedef float VConstantBufferRegisterType;

#endif

#ifndef VIS_LOCKFLAG_DISCARDABLE
  #define VIS_LOCKFLAG_DISCARDABLE        0x00000001
#endif

/// \brief
///   Represents a single entry in a table of type VShaderConstantTable
struct VShaderConstantTableEntry
{
  /// \brief
  ///   Constructor
  inline VShaderConstantTableEntry()
  {
    m_iRegisterIndex = -1;
    m_eValueType = VALUETYPE_UNKNOWN;
    m_iArrayElements = 1;
  }

  /// \brief
  ///   Returns whether the register index is valid
  inline bool IsValid() const 
  {
    return m_iRegisterIndex>=0;
  }

  /// \brief
  ///   Returns whether this entry targets a shader constant buffer (e.g. float type) and not a
  ///   texture sampler etc.
  inline bool IsConstantBufferEntry() const 
  {
    if (!IsValid() || m_eValueType==VALUETYPE_UNKNOWN || m_eValueType==VALUETYPE_STRING || m_eValueType==VALUETYPE_TEXTURE || m_eValueType==VALUETYPE_CUBEMAP || m_eValueType==VALUETYPE_TEXTURE3D || m_eValueType==VALUETYPE_TEXTURE2DARRAY)
      return false;

    // variables that start with _noExport will not be added to the constant table
    if (!_strnicmp(m_sVarname.AsChar(),"_noExport",strlen("_noExport")))
      return false;

    return true;
  }

  /// \brief
  ///   Returns whether this entry targets a texture sampler.
  inline bool IsSamplerType() const
  {
    return (m_eValueType==VALUETYPE_TEXTURE || m_eValueType==VALUETYPE_CUBEMAP || m_eValueType==VALUETYPE_TEXTURE3D || m_eValueType==VALUETYPE_TEXTURE2DARRAY);
  }

  /// \brief
  ///   Copies an Entry
  inline VShaderConstantTableEntry &operator = (const VShaderConstantTableEntry &other)
  {
    m_sVarname = other.m_sVarname;
    m_eValueType = other.m_eValueType;
    m_iArrayElements = other.m_iArrayElements;
    m_iRegisterIndex = other.m_iRegisterIndex;
    return *this;
  }

  /// \brief
  ///   Evaluates the number of constant table registers used by this entry (using type and array
  ///   size)
  inline int GetRegisterCount() const 
  {

#if defined(_VR_DX11)

    return m_iArrayElements; ///< this member actually stores the overall number of bytes in DX11

#else

    return GetRegisterCountFromValueType(m_eValueType)*m_iArrayElements;

#endif

  }

  /// \brief
  ///   Returns the min/max register index used by this entry
  void GetRegisterRange(int &iFirst, int &iLast) const
  {
    iFirst = m_iRegisterIndex;
    iLast = iFirst + GetRegisterCount() -1;
  }

  VString m_sVarname;
  VValueType_e m_eValueType;
  int m_iArrayElements;
  int m_iRegisterIndex;
};



/// \brief
///   Represents a table of shader register constants, each a pair of register name and index range
/// 
/// This table is associated with a VFloatConstantBuffer instance that allows locking variables by name.
/// 
/// Each variable name entry provides information about the variable's value type (enum
/// VValueType_e) and its start index in the VFloatConstantBuffer.
/// 
/// Since registers in the VFloatConstantBuffer are always represented as 4-component float
/// vectors, a variable of type VALUETYPE_FLOAT4 reserves 1 register. However, also a single float
/// type (VALUETYPE_FLOAT) requires 1 full register, since packing is not supported and full 16 Byte
/// Registers are the granularity for accessing data inside the table.
class VShaderConstantTable : public VRefCounter
{
public:

  /// \brief
  ///   Constructor
  inline VShaderConstantTable() 
  {
    m_iEntryCount=0;m_pEntry=NULL;
  }

  /// \brief
  ///   Destructor
  inline ~VShaderConstantTable() 
  {
    Reset();
  }

  /// \brief
  ///   Resets the table
  inline void Reset() 
  {
    m_iEntryCount=0;V_SAFE_DELETE_ARRAY(m_pEntry);
  }

  /// \brief
  ///   Reserves memory for iCount table entries
  VBASE_IMPEXP void AllocateEntries(int iCount);

  /// \brief
  ///   Helper function to evaluate the min/max register indices over all table entries
  VBASE_IMPEXP void GetRegisterRange(int &iFirst, int &iLast) const;

  /// \brief
  ///   Returns the maximum index of a texture sampler (or -1 if no samplers are used)
  VBASE_IMPEXP int GetMaxSamplerIndex() const;

  /// \brief
  ///   Returns the table entry (or NULL) that is a texture sampler and maps to the specified
  ///   sampler index
  VBASE_IMPEXP VShaderConstantTableEntry* FindSamplerWithIndex(int iSampler) const;

  /// \brief
  ///   Looks up a table entry by variable name
  /// 
  /// The string comparison is case-insensitive.
  /// 
  /// \param szVarName
  ///   Variable name to find in this table.
  /// 
  /// \return
  ///   VShaderConstantTableEntry* pEntry: The table entry, if found. Otherwise NULL.
  VBASE_IMPEXP VShaderConstantTableEntry* FindByName(const char *szVarName) const;

  /// \brief
  ///   Looks up a table entry by variable name hash
  VBASE_IMPEXP VShaderConstantTableEntry* FindByNameHash(unsigned int iVarNameHash) const;

  /// \brief
  ///   Finds a sampler entry type by name and returns the sampler index (or -1).
  VBASE_IMPEXP int GetSamplerIndexByName (const char *szSamplerName) const;

  /// \brief
  ///   Finds a shader constant table entry that uses the passed start register.
  VBASE_IMPEXP VShaderConstantTableEntry* FindByStartRegister(int iStartReg) const;

  /// \brief
  ///   Reads/writes table data from binary file
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

  /// \brief
  ///   Returns the number of entries in the shader constant table
  VBASE_IMPEXP int GetNumEntries (void) const { return (m_iEntryCount); }

  /// \brief
  ///   Returns the entry at a given index in the shader constant table
  VBASE_IMPEXP const VShaderConstantTableEntry* GetEntry (int iEntry) const {return (&m_pEntry[iEntry]); }

  /// \brief
  ///   Returns the entry at a given index in the shader constant table
  VBASE_IMPEXP VShaderConstantTableEntry* GetEntry (int iEntry) {return (&m_pEntry[iEntry]); }

private:
  int m_iEntryCount;
  VShaderConstantTableEntry *m_pEntry;
};



/// \brief
///   Generic constant buffer. Platforms on which Vision does not support native constant buffer objects will
///   always operate on float4 registers.
/// 
/// A VFloatConstantBuffer instance is used by a compiled shader to store shader specific ranges of
/// a vertex/constant table.
/// 
/// At compile time, the min/max register is extracted from the Vertex resp. Pixel Shader. The
/// Shader then reserves the specific range in the buffer.
/// 
/// When binding a Shader, the whole range of the constant buffer is uploaded to the graphics card.
/// 
/// To modify constants in a buffer, use the Lock/Unlock mechanism or use the SetSingleParameter
/// functions that wrap around the table lookups.
class IVConstantBuffer
{
public:


  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor
   VBASE_IMPEXP IVConstantBuffer();

  /// \brief
  ///   Destructor
  inline ~IVConstantBuffer() 
  {
    FreeBuffer();
  }


  ///
  /// @}
  ///

  ///
  /// @name Allocation
  /// @{
  ///


  /// \brief
  ///   Allocates memory for iEntryCount entries setting the start index to iFirstReg. Used
  ///   internally when creating shader instances.
  /// 
  /// Optionally a custom memory block can be supplied. If this pointer is not NULL it must hold
  /// enough memory for the passed registers. This feature is useful in the DX11 versions to wrap a constant buffer around a custom
  /// data structure.
  ///
  /// \param iFirstReg
  ///   First register index to use.
  ///
  /// \param iEntryCount
  ///   Number of registers to use.
  ///
  /// \param pCustomMem
  ///   Optionally, a custom memory block can be specified. This can be used to allow a constant buffer to wrap
  ///   around a custom structure in DX11 and up.
  VBASE_IMPEXP void AllocateBuffer(int iFirstReg, int iEntryCount, void *pCustomMem=NULL);

  /// \brief
  ///   This version of AllocateBuffer uses the associated constant table to allocate a range. Used
  ///   internally when creating shader instances.
  VBASE_IMPEXP void AllocateBuffer(void *pCustomMem = NULL);

  /// \brief
  ///   Internal function to clamp the allocated range against the passed min/max
  VBASE_IMPEXP void ClampRange(int iMin, int iMax);

  /// \brief
  ///   Free the buffer
  VBASE_IMPEXP void FreeBuffer();


  ///
  /// @}
  ///

  ///
  /// @name Constant Table Access
  /// @{
  ///

  /// \brief
  ///   Return the VShaderConstantTable reference that is associated with this buffer. This table
  ///   can be used for register name lookups.
  inline const VShaderConstantTable* GetConstantTable() const {return m_spTable.GetPtr ();}

  /// \brief
  ///   Helper function that wraps around VShaderConstantTable::FindByName using the associated
  ///   constant table
  inline int GetRegisterByName(const char *szVarName)
  {
    if (!m_spTable)
      return -1;
    VShaderConstantTableEntry *pEntry = m_spTable->FindByName(szVarName);
    if (pEntry)
      return pEntry->m_iRegisterIndex;
    return -1;
  }

  /// \brief
  ///   Helper function that wraps around VShaderConstantTable::FindByNameHash using the associated
  ///   constant table
  inline int GetRegisterByNameHash(unsigned int iVarNameHash)
  {
    if (!m_spTable)
      return -1;
    VShaderConstantTableEntry *pEntry = m_spTable->FindByNameHash(iVarNameHash);
    if (pEntry)
      return pEntry->m_iRegisterIndex;
    return -1;
  }


  ///
  /// @}
  ///

  ///
  /// @name Modifying Values
  /// @{
  ///


  /// \brief
  ///   Locks a register range for read/write access
  /// 
  /// Each register consists of a 4-component float vector, thus the locked region is always
  /// aligned to 4-component floats.
  /// 
  /// There is no mechanism to lock a sub-range inside a register; the granularity is always a 16-Byte register.
  /// 
  /// After modifying a range inside the buffer, the buffer has to be unlocked again.
  /// 
  /// Updating shader constant data on the GPU is deferred to shader bind or draw time (depending on the platform) and
  /// batched to the entire modified register range, so even if multiple updates to the same register data are performed,
  /// only one update of the data on the GPU is performed. Thus, this operation is not very performance-critical.
  /// 
  /// \param iFirstEntry
  ///   The first entry to lock
  /// 
  /// \param iEntryCount
  ///   Number of registers to lock (note that each register represents 4 floats)
  /// 
  /// \param iLockFlags
  ///   Flags for locking (ignored in DX9). The following flags are supported and should be
  ///   specified accordingly:
  ///   \li VIS_LOCKFLAG_DISCARDABLE : The returned values are not guaranteed to be initialized
  ///     with valid values. Should be used for write-only access.
  /// 
  ///   \li VIS_LOCKFLAG_READONLY : Read-only access to the returned data
  /// 
  /// \return
  ///   float *pFirstReg : system memory address of the first register
  /// 
  /// \sa IVConstantBuffer::Unlock
  /// \sa IVConstantBuffer::LockByName
  /// \sa VShaderConstantTable
  inline VConstantBufferRegisterType* Lock(int iFirstEntry, int iEntryCount, int iLockFlags)
  {
    iFirstEntry -= m_iFirstRegister;
    VASSERT(iFirstEntry>=0);
    VASSERT((iFirstEntry+iEntryCount)<=m_iAllocatedEntries);

    #if defined(_VR_DX11)
      m_bIsDirty = true;
      return (float*)((INT_PTR)m_pBuffer + iFirstEntry);
    #else

      m_iDirtyRange[0] = hkvMath::Min (m_iDirtyRange[0],iFirstEntry);
      m_iDirtyRange[1] = hkvMath::Max (m_iDirtyRange[1],iFirstEntry+iEntryCount);
      return &m_pBuffer[iFirstEntry*4];

#endif

  }

  /// \brief
  ///   Unlocks the buffer after a Lock operation. Does not upload the data to the GPU - this is
  ///   deferred to draw or shader bind time.
  /// 
  /// \sa IVConstantBuffer::Lock
  inline void Unlock()
  {
  }

  /// \brief
  ///   Helper function to lock a register range using the variable's name
  /// 
  /// This function uses the associated constant table to look up the variable's register range.
  /// 
  /// Since this function involves string comparisons, it should not be used frequently at
  /// runtime. Instead, register indices should be evaluated at compile time using
  /// GetRegisterByName, stored as custom shader members and passed to the Lock function or
  /// variants of SetSingleRegister
  /// 
  /// \param szVarname
  ///   The variable's name as used in the shader source code (string match is case-insensitive)
  /// 
  /// \param iLockFlags
  ///   Lock flags; see Lock operation for supported flag constants
  /// 
  /// \param pRegCount
  ///   Optional int reference that will receive the number of 4-component registers used by this
  ///   variable. The value is the same as VShaderConstantTableEntry::GetRegisterCount for the
  ///   resp. variable.
  /// 
  /// \return
  ///   float *pFirstReg : system memory address of the first register (or NULL if variable does
  ///   not exist)
  /// 
  /// \sa IVConstantBuffer::Lock
  inline VConstantBufferRegisterType* LockByName(const char *szVarname, int iLockFlags, int *pRegCount=NULL)
  {
    VASSERT(m_spTable);
    VShaderConstantTableEntry *pEntry = m_spTable->FindByName(szVarname);
    if (!pEntry || !pEntry->IsConstantBufferEntry())
      return NULL;
    VASSERT(pEntry->m_iRegisterIndex>=0);
    int iRegCount = pEntry->GetRegisterCount();
    if (pRegCount) *pRegCount=iRegCount;
    return Lock(pEntry->m_iRegisterIndex,iRegCount,iLockFlags);
  }


  /// \brief
  ///   Helper function to modify a single variable
  /// 
  /// This function wraps around LockByName and fills the buffer with the parsing result of the
  /// value string.
  /// 
  /// See performance hints there.
  /// 
  /// \param szVarName
  ///   The variable to modify
  /// 
  /// \param szValueStr
  ///   The value string to parse and write into the locked buffer. Vector portions have to be
  ///   separated by comma, e.g. "0,100,200". If there are not enough parameters in the string
  ///   (e.g. passing "0,100,200" for a VALUETYPE_FLOAT4 variable type) the remaining portions are
  ///   not modified (rather than setting to 0.0). If there are too many portions in the value
  ///   string the last ones are ignored.
  /// 
  /// \return
  ///   bool bResult: true, if the operation was successful (e.g. the variable has to exist)
  /// 
  /// \sa IVConstantBuffer::LockByName
  /// \sa IVConstantBuffer::SetSingleRegisterF
  VBASE_IMPEXP bool ParseSingleParameter(const char *szVarName, const char *szValueStr);

  /// \brief
  ///   Helper function to modify a single variable.
  /// 
  /// This function wraps around LockByName and copies the source buffer into the lock target.
  /// 
  /// See performance hints there.
  /// 
  /// \param szVarName
  ///   The variable to modify
  /// 
  /// \param pValues
  ///   The source buffer. This buffer has to point to a float array that is large enough to copy
  ///   at least 4 floats. The actual number of floats copied depends on the iMaxRegister parameter
  ///   resp. the register range used by this variable. However, it is always a multiple of 4
  ///   floats.
  /// 
  /// \param iMaxRegister
  ///   If specified, at most iMaxRegister*4 floats are copied. Otherwise, the number of floats is
  ///   evaluated from the variable type and array size.
  /// 
  /// \return
  ///   bool bResult: true, if the operation was successful (e.g. the variable has to exist)
  /// 
  /// \sa IVConstantBuffer::LockByName
  /// \sa IVConstantBuffer::SetSingleRegisterF
  VBASE_IMPEXP bool SetSingleParameterF(const char *szVarName, const float *pValues, int iMaxRegister=-1);

  /// \brief
  ///   Helper function to modify a single variable
  /// 
  /// This function wraps around LockByName and copies the passed float scalars into the first
  /// register.
  /// 
  /// Thus this version is suitable for single float types or float vectors, but not for matrices
  /// or arrays.
  /// 
  /// See performance hints in LockByName.
  /// 
  /// \param szVarName
  ///   The variable to modify
  /// 
  /// \param x,y,z,w
  ///   4 float scalars that are written into the first register.
  /// 
  /// \return
  ///   bool bResult: true, if the operation was successful (e.g. the variable has to exist)
  /// 
  /// \sa IVConstantBuffer::LockByName
  /// \sa IVConstantBuffer::SetSingleRegisterF
  VBASE_IMPEXP bool SetSingleParameterF(const char *szVarName, float x, float y, float z, float w);

  /// \brief
  ///   Helper function to modify a single register
  /// 
  /// This function should be used if the register index is known since no name lookups have to be
  /// performed.
  /// 
  /// It uses the Lock/unlock mechanism copy 4 values into a single register.
  /// 
  /// \param iRegister
  ///   Register index. Must be in valid min/max range supplied by this buffer.
  /// 
  /// \param pValues
  ///   Source buffer of 4 float values to be copied.
  /// 
  /// \sa IVConstantBuffer::SetSingleParameterF
  inline void SetSingleRegisterF(int iRegister, const float *pValues)
  {
    VASSERT(iRegister>=0 && pValues!=NULL);
    float *pDest = (float *)Lock(iRegister,VCONSTANTBUFFER_REGISTER_GRANULARITY,VIS_LOCKFLAG_DISCARDABLE);
    pDest[0] = pValues[0];
    pDest[1] = pValues[1];
    pDest[2] = pValues[2];
    pDest[3] = pValues[3];
    Unlock();
  }

  /// \brief
  ///   Helper function to modify a single register
  /// 
  /// This function should be used if the register index is known since no name lookups have to be
  /// performed.
  /// 
  /// It uses the Lock/unlock mechanism copy 4 scalar values into a single register.
  /// 
  /// \param iRegister
  ///   Register index. Must be in valid min/max range supplied by this buffer.
  /// 
  /// \param x,y,z,w
  ///   4 scalar values that are copied into the register
  /// 
  /// \sa IVConstantBuffer::SetSingleParameterF
  inline void SetSingleRegisterF(int iRegister, float x, float y, float z, float w)
  {
    VASSERT(iRegister>=0);
    float *pDest = (float *)Lock(iRegister,VCONSTANTBUFFER_REGISTER_GRANULARITY,VIS_LOCKFLAG_DISCARDABLE);
    pDest[0] = x;
    pDest[1] = y;
    pDest[2] = z;
    pDest[3] = w;
    Unlock();
  }


  /// \brief
  ///   Helper function to modify 4 constant registers holding a matrix. 
  ///
  /// Consider using the more convenient VConstantBufferRegister::SetRegisterValuesMatrix instead.
  ///
  /// \param szVarName
  ///  The name of the variable to modify.
  ///
  /// \param matrix
  ///  The matrix value to set.
  ///
  /// \return
  ///   bool bResult: true, if the operation was successful (e.g. the variable has to exist)
  /// 
  /// \sa VConstantBufferRegister::SetRegisterValuesMatrix
  inline bool SetSingleParameterM(const char* szVarName, const hkvMat4& matrix)
  {
    int iRegister = GetRegisterByName(szVarName);
    if(iRegister == -1)
      return false;

    SetRegisterRangeM(iRegister, &matrix, 1);
    return true;
  }

  /// \brief
  ///   Helper function to modify 4 constant registers holding a matrix.
  ///
  /// Consider using the more convenient VConstantBufferRegister::SetRegisterValuesMatrix instead.
  ///
  /// \param iRegister
  ///  The register index of the variable to modify.
  ///
  /// \param matrix
  ///  The matrix value to set.
  ///
  /// \sa VConstantBufferRegister::SetRegisterValuesMatrix
  inline void SetSingleRegisterM(int iRegister, const hkvMat4& matrix)
  {
    SetRegisterRangeM(iRegister, &matrix, 1);
  }

  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Returns the number of bytes used by this buffer
  inline int GetByteCount() const 
  {
    return m_iAllocatedEntries*VCONSTANTBUFFER_REGISTER_SIZE;
  }


  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///


  /// \brief
  ///   Assignment operator. Deep copies the buffer.
  inline IVConstantBuffer &operator = (const IVConstantBuffer &other)
  {
    AllocateBuffer(other.m_iFirstRegister,other.m_iAllocatedEntries);
    if (m_iAllocatedEntries>0)
      memcpy(m_pBuffer,other.m_pBuffer,GetByteCount());
    m_spTable = ((IVConstantBuffer &)other).m_spTable; ///< no assignment operator!
    return *this;
  }


  /// \brief
  ///   Comparison operator. true if both buffers contain the exact same data
  inline bool operator == (const IVConstantBuffer &other) const
  {
    if (m_iFirstRegister!=other.m_iFirstRegister) return false;
    if (m_iAllocatedEntries!=other.m_iAllocatedEntries) return false;
    return memcmp(m_pBuffer,other.m_pBuffer,GetByteCount())==0;
  }

  /// \brief
  ///   Comparison operator.
  inline bool operator != (const IVConstantBuffer &other) const 
  {
    return !(*this == other);
  }

  /// \brief
  ///   Flushes the modified constant buffer data to its internal DX10/DX11 buffer object. No effect on
  ///   other platforms.
  VBASE_IMPEXP void Flush(int iNumBytes = 0);

  inline void FlagAsDirty()
  {

#if defined(_VR_DX11)

    m_bIsDirty = true;

#else

    m_iDirtyRange[0] = 0;
    m_iDirtyRange[1] = m_iAllocatedEntries;
    
#endif

  }

  inline void UnFlagDirty()
  {

#if defined(_VR_DX11)

    m_bIsDirty = false;

#else

    // for next lock's min/max:
    m_iDirtyRange[0] = m_iAllocatedEntries;
    m_iDirtyRange[1] = 0;

#endif

  }

  /// \brief
  ///   Convenience function for setting a given range of 4x32Bit FP registers in the constant
  ///   buffer.
  inline void SetRegisterRangeF(int iStart, const float *pData, int iNumRegs)
  {
    iStart -= m_iFirstRegister;
    VASSERT(iStart>=0);
    VASSERT((iStart+iNumRegs)<=m_iAllocatedEntries);
    INT_PTR iBuffer = (INT_PTR)m_pBuffer + iStart * VCONSTANTBUFFER_REGISTER_SIZE;
    memcpy((void *)iBuffer, pData, iNumRegs * VCONSTANTBUFFER_REGISTER_SIZE);

#if defined(_VR_DX11)

    m_bIsDirty = true;

#else

    m_iDirtyRange[0] = hkvMath::Min (m_iDirtyRange[0],iStart);
    m_iDirtyRange[1] = hkvMath::Max (m_iDirtyRange[1],iStart+iNumRegs);

#endif

  }

  /// \brief
  ///   Uploads an array of matrices with the correct platform layout.
  ///
  /// Consider using the more convenient VConstantBufferRegister::SetRegisterValuesMatrix instead.
  ///
  /// \param iStart
  ///  The index of the first constant register to modify.
  ///
  /// \param pMatrices
  ///  A pointer to the array of matrices to upload.
  ///
  /// \param iNumMatrices
  ///  The number of matrices to set.
  ///
  /// \sa VConstantBufferRegister::SetRegisterValuesMatrix
  inline void SetRegisterRangeM(int iStart, const hkvMat4* pMatrices, int iNumMatrices)
  {
    iStart -= m_iFirstRegister;
    VASSERT(iStart>=0);
    VASSERT((iStart+4*iNumMatrices)<=m_iAllocatedEntries);
    void* pBuffer = hkvAddByteOffset(m_pBuffer, iStart * VCONSTANTBUFFER_REGISTER_SIZE);

#ifdef TRANSPOSE_MATRICES_ON_UPLOAD_TO_SHADER
    for (int i = 0; i < iNumMatrices; ++i)
    {
      memcpy(hkvAddByteOffset (pBuffer, sizeof (hkvMat4) * i), pMatrices[i].getAsArray(hkvMat4::RowMajor).data, sizeof(hkvMat4));
    }
#else
    memcpy(pBuffer, pMatrices, iNumMatrices * sizeof(hkvMat4));
#endif

#if defined(_VR_DX11)
    m_bIsDirty = true;
#else
    m_iDirtyRange[0] = hkvMath::Min (m_iDirtyRange[0],iStart);
    m_iDirtyRange[1] = hkvMath::Max (m_iDirtyRange[1],iStart+4*iNumMatrices);
#endif

  }

  int m_iFirstRegister;
  int m_iAllocatedEntries;
  float *m_pBuffer;

  VShaderConstantTablePtr m_spTable; ///< this points to the table of the owner shader resource (VShaderProgramResource member)

#if defined(_VR_DX11)

  D3DBuffer *m_pD3DBuffer;
  bool m_bIsDirty;

#else

  short m_iDirtyRange[2]; ///< start and end register that needs to be re-uploaded

#endif

  bool m_bOwnsBuffer;

  ///
  /// @}
  ///

};


/// \brief
///   Constant buffer for constant buffers using exclusively float registers. 
class VFloatConstantBuffer : public IVConstantBuffer
{
public:

  /// \brief
  ///   Helper function to modify a single variable.
  /// 
  /// This function wraps around LockByName and copies the source buffer into the lock target.
  /// 
  /// See performance hints there.
  /// 
  /// \param szVarName
  ///   The variable to modify
  /// 
  /// \param pValues
  ///   The source buffer. This buffer has to point to a float array that is large enough to copy
  ///   at least 4 floats. The actual number of floats copied depends on the iMaxRegister parameter
  ///   resp. the register range used by this variable. However, it is always a multiple of 4
  ///   floats.
  /// 
  /// \param iMaxRegister
  ///   If specified, at most iMaxRegister*4 floats are copied. Otherwise, the number of floats is
  ///   evaluated from the variable type and array size.
  /// 
  /// \return
  ///   bool bResult: true, if the operation was successful (e.g. the variable has to exist)
  /// 
  /// \sa IVConstantBuffer::LockByName
  /// \sa IVConstantBuffer::SetSingleRegisterF
  inline bool SetSingleParameter(const char *szVarName, const float *pValues, int iMaxRegister=-1)
  {
    return SetSingleParameterF(szVarName,pValues,iMaxRegister);
  }

  /// \brief
  ///   Helper function to modify a single variable.
  /// 
  /// This function wraps around LockByName and copies the passed float scalars into the first
  /// register.
  /// 
  /// Thus this version is suitable for single float types or float vectors, but not for matrices
  /// or arrays.
  /// 
  /// See performance hints in LockByName.
  /// 
  /// \param szVarName
  ///   The variable to modify
  /// 
  /// \param x,y,z,w
  ///   4 float scalars that are written into the first register.
  /// 
  /// \return
  ///   bool bResult: true, if the operation was successful (e.g. the variable has to exist)
  /// 
  /// \sa IVConstantBuffer::LockByName
  /// \sa IVConstantBuffer::SetSingleRegisterF
  inline bool SetSingleParameter(const char *szVarName, float x, float y, float z, float w)
  {
    return SetSingleParameterF(szVarName, x,y,z,w);
  }

  /// \brief
  ///   Helper function to modify a single register
  /// 
  /// This function should be used if the register index is known since no name lookups have to be
  /// performed.
  /// 
  /// It uses the Lock/unlock mechanism copy 4 values into a single register.
  /// 
  /// \param iRegister
  ///   Register index. Must be in valid min/max range supplied by this buffer.
  /// 
  /// \param pValues
  ///   Source buffer of 4 float values to be copied.
  /// 
  /// \sa IVConstantBuffer::SetSingleParameterF
  inline void SetSingleRegister(int iRegister, const float *pValues)
  {
    SetSingleRegisterF(iRegister,pValues);
  }

  /// \brief
  ///   Helper function to modify a single register
  /// 
  /// This function should be used if the register index is known since no name lookups have to be
  /// performed.
  /// 
  /// It uses the Lock/unlock mechanism copy 4 scalar values into a single register.
  /// 
  /// \param iRegister
  ///   Register index. Must be in valid min/max range supplied by this buffer.
  /// 
  /// \param x,y,z,w
  ///   4 scalar values that are copied into the register
  /// 
  /// \sa IVConstantBuffer::SetSingleParameterF
  inline void SetSingleRegister(int iRegister, float x, float y, float z, float w)
  {
    SetSingleRegisterF(iRegister, x,y,z,w);
  }

};


/// \brief
///   Helper template class for convenient wrapping of custom structures into a GPU constant buffer
/// 
/// This template takes a structure as input and adds an instance of this struct as a class member.
/// 
/// Then it allocates the constant buffer using the struct member as memory. This way custom
/// structures can be easily used as constant buffers provided to the GPU.
/// 
/// It is crucial that corresponding the structures and on the CPU and the GPU sides use exactly the same member layout.
/// You also need to take structure member alignment into account.
/// 
/// On platforms where Vision exclusively supports float4 constant registers (DX9, Xbox360, PS3, OpenGL ES 2), the provided structure must
/// have a size that is a multiple of 16 Bytes as this is the register granularity
/// (4*sizeof(float)).
/// 
/// This class is particularly useful when working with DX10/DX11 constant buffers that hold instance-specific data. 
template<class STRUCTMEMBER> class VTypedConstantBuffer : public VShaderConstantBuffer
{
public:

  /// \brief
  ///   Constructor, allocates a constant buffer that points to the structure member
  VTypedConstantBuffer()
  {
    V_COMPILE_ASSERT(sizeof(STRUCTMEMBER)%(4*sizeof(float))==0); ///< size of the structure must be a multiple of register size, which is 4*sizeof(float) (also for DX10/DX11)
    AllocateBuffer(0,sizeof(STRUCTMEMBER) / VCONSTANTBUFFER_REGISTER_SIZE,&m_Data);
  }

  /// \brief
  ///   Locks the full buffer for updating its content and casts the return value to the structure
  ///   reference
  inline STRUCTMEMBER &BeginUpdate()
  {
    return *((STRUCTMEMBER *)Lock(m_iFirstRegister,m_iAllocatedEntries,VIS_LOCKFLAG_DISCARDABLE));
  }

  /// \brief
  ///   Must be called after updating the data via BeginUpdate. This function just wraps around
  ///   Unlock()
  inline void EndUpdate() {Unlock();}

  /// \brief
  ///   Public data member. Its content can be modified at any time, but this should be rather done
  ///   inside a BeginUpdate/EndUpdate block so the dirty flags are set properly
  STRUCTMEMBER m_Data;

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
