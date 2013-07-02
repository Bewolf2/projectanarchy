/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRawDataBlock.hpp

#ifndef VRAWDATABLOCK_HPP_INCLUDED
#define VRAWDATABLOCK_HPP_INCLUDED



/// \brief
///   Class that represents a block of memory to store raw data.
///
/// Supports helper functions to search for specific sub blocks to share memory. Accordingly a string pool can be implemented with this class
class VRawDataBlock : public VBaseObject
{
public:

  /// \brief
  ///   Constructor
  inline VRawDataBlock()
  {
    m_iDataSize = 0;
  }

  /// \brief
  ///   De-allocates the data
  inline void Reset()
  {
    m_iDataSize = 0;
    m_RawData.Reset();
  }

  /// \brief
  ///   Helper function to avoid fragmentation while filling it. Reserves iSize bytes in the binary block 
  inline void EnsureCapacity(int iSize)
  {
    m_RawData.EnsureSize(iSize);
  }

  /// \brief
  ///   Copies a binary block of data into this pool. Tries to re-use an existing part of the block with the same data (byte-wise comparison)
  ///
  /// \param pData
  ///   Pointer to the source block of data
  ///
  /// \param iLength
  ///   size (in bytes)of the block
  ///
  /// \returns
  ///    int handle that represents the position inside the binary block. The handle is identical when adding the same data twice. The return value
  ///    is -1 in case the source data is NULL or has length 0.
  VBASE_IMPEXP int AddDataBlock(const void *pData, int iLength);

  /// \brief
  ///   Tries to find a block of data inside the binary block (byte-wise comparison). Returns -1 if the block cannot be found, otherwise, it represents the position inside the block 
  VBASE_IMPEXP int FindDataBlock(const void *pData, int iLength) const;

  /// \brief
  ///   Returns the raw memory pointer for the specified position. This can for instance be a handle obtained via AddDataBlock. If -1 is passed, this function returns NULL
  inline const void *GetData(int iHandle) const
  {
    if (iHandle<0) // defined behavior
      return NULL;
    VASSERT(iHandle<m_iDataSize);
    return &m_RawData.GetDataPtr()[iHandle];
  }

  /// \brief
  ///   Helper function that wraps around AddDataBlock with the string length+1 passed as size. Returns -1 for a NULL string
  inline int AddString(const char *szString)
  {
    if (szString!=NULL)
      return AddDataBlock(szString,(int)strlen(szString)+1);
    return -1;
  }

  /// \brief
  ///   Helper function that wraps around GetData
  inline const char *GetString(int iHandle) const
  {
    return (const char *)GetData(iHandle);
  }

  /// \brief
  ///   Saves the whole binary block to file. Use classes such as VChunkFileOutStream to use inside chunk files
  VBASE_IMPEXP void WriteToStream(IVFileOutStream *pOut);

  /// \brief
  ///   Loads the whole binary block from file. Use classes such as VChunkFileInStream to use inside chunk files
  VBASE_IMPEXP void ReadFromStream(IVFileInStream *pIn);

private:
  int m_iDataSize;
  DynArray_cl<char> m_RawData;
};


#endif // VRAWDATABLOCK_HPP_INCLUDED

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
