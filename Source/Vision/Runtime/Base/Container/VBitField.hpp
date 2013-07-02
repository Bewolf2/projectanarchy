/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBitField.hpp

#ifndef BITFIELD_HPP_INCLUDED
#define BITFIELD_HPP_INCLUDED

class VChunkFile;
class VBitfield;

/// \brief
///   Template class for a bit field
template <int iStaticBitCount> class VTBitfield
{
public:

  /// \brief
  ///   Constructor that takes the initial size of the bit field
  inline VTBitfield(int iBitCount = 0)
  {
    m_iBitCount = iBitCount;
    m_pField = m_OwnBits;
    AllocateBitfield(iBitCount);
  }

  /// \brief
  ///   Copy Constructor
  inline VTBitfield(const VTBitfield& other)
  {
    m_iBitCount = 0;
    m_pField = m_OwnBits;
    CopyFrom(other);
  }

  /// \brief
  ///   Destructor, frees the allocated resources
  inline ~VTBitfield() 
  {
    FreeBitfield();
  }

  /// \brief
  ///   Frees the allocated resources
  inline void FreeBitfield()
  {
    if (m_pField != m_OwnBits)
    {
      V_SAFE_DELETE_ARRAY(m_pField); 
    }
    m_iBitCount = 0;
    m_pField = m_OwnBits;
  }

  /// \brief
  ///   (Re-)allocates the passed number of bits. The bit field is initialized with all bits set to
  ///   0 or copied from the passed array
  inline void AllocateBitfield(int iBitCount, const unsigned int *pInitArray=NULL)
  {
    FreeBitfield();
    if (iBitCount < 1) 
      return;
    
    m_iBitCount = iBitCount;
    int iIntCount = GetIntCount();
    if (iBitCount > iStaticBitCount)
    {     
      m_pField = new unsigned int[iIntCount];      
    }
    if (pInitArray)
      memcpy(m_pField, pInitArray, iIntCount * sizeof(int));
    else
      memset(m_pField, 0, iIntCount * sizeof(int));
  }

  /// \brief
  ///   Reallocates the passed number of bits. The new data is initialized with all bits set to
  ///   the same value as in the current array.
  inline void ReallocateBitfield(int iBitCount)
  {
    if (iBitCount<1) return;

    VTBitfield temp(iBitCount);
    temp.Add(*this);

    AllocateBitfield(iBitCount);
    Add(temp);
  }

  /// \brief
  ///   Copies all data from other
  inline void CopyFrom(const VTBitfield& other)
  {
    AllocateBitfield(other.m_iBitCount);
    const int iIntCount = other.GetIntCount();
    memcpy(m_pField, other.m_pField, iIntCount * sizeof(int));
    m_iBitCount = other.m_iBitCount;
  }

  /// \brief
  ///   Sets all bits to 0
  inline void Clear()
  {
    const int iIntCount = GetIntCount();
    memset(m_pField, 0, iIntCount * sizeof(int));
  }

  /// \brief
  ///   Sets all bits to 1
  inline void SetAll()
  {
    const int iIntCount = GetIntCount();
    for (int i=0;i<iIntCount;i++) m_pField[i]=0xffffffff;
    MaskOutTail();
  }

  /// \brief
  ///   Inverts all bits
  inline void Invert()
  {
    const int iIntCount = GetIntCount();
    for (int i=0;i<iIntCount;i++) m_pField[i]^=0xffffffff;
    MaskOutTail();
  }

  /// \brief
  ///   Queries the status of the bit with the specified 0-based index
  inline bool IsBitSet(int iBitNumber) const
  {
    VASSERT(iBitNumber>=0 && iBitNumber<m_iBitCount);
    return (m_pField[iBitNumber>>5] & (1U<<(iBitNumber&31))) != 0;
  }

  /// \brief
  ///   Sets the status of the bit with the specified 0-based index to 1
  inline void SetBit(int iBitNumber) const
  {
    VASSERT(iBitNumber>=0 && iBitNumber<m_iBitCount);
    m_pField[iBitNumber>>5] |= (1U<<(iBitNumber&31));
  }

  /// \brief
  ///   Sets the status of the bit with the specified 0-based index to 1
  ///   Calls ReallocateBitfield to resize the available data if necessary.
  inline void SetBitSafe(int iBitNumber) 
  {
    if(iBitNumber+1 > m_iBitCount)
      ReallocateBitfield(iBitNumber+1);
    
    SetBit(iBitNumber);
  }

  /// \brief
  ///   Clears the status of the bit with specified 0-based index
  inline void RemoveBit(int iBitNumber) const
  {
    VASSERT(iBitNumber>=0 && iBitNumber<m_iBitCount);
    m_pField[iBitNumber>>5] &= (~(1U<<(iBitNumber&31)));
  }

  /// \brief
  ///   Evaluates whether any bit is set in the field
  inline bool IsAnyBitSet() const
  {
    const int iIntCount = GetIntCount();
    for (int i=0;i<iIntCount;i++)
      if (m_pField[i])
        return true;
    return false;
  }

  /// \brief
  ///   Returns the 0-based index of the first bit that is set, or -1 if no bit is set
  inline int GetIndexOfFirstSetBit() const
  {
    int iIndex = 0;
    const int iIntCount = GetIntCount();
    for (int i=0;i<iIntCount;i++,iIndex+=32) if (m_pField[i])
      for (int j=0;j<32 && (iIndex+j)<m_iBitCount;j++) 
        if (IsBitSet(iIndex+j))
          return iIndex+j;
    return -1;
  }

  /// \brief
  ///   Returns the 0-based index of the first bit that is not set, or -1 if all bits are set
  inline int GetIndexOfFirstClearedBit() const
  {
    int iIndex = 0;
    const int iIntCount = GetIntCount();
    for (int i=0;i<iIntCount;i++,iIndex+=32)
    {
      if (m_pField[i]!=0xffffffff)
      {
        for (int j=0;j<32 && (iIndex+j)<m_iBitCount;j++) 
        {
          if (!IsBitSet(iIndex+j))
          {
            return iIndex+j;
          }
        }
      }
    }
    return -1;
  }

  /// \brief
  ///   Merges another bitfield into this one using logical OR. The fields don't have to have the same size
  void Add(const VTBitfield &other)
  {
    const int iCount = hkvMath::Min(GetIntCount(),other.GetIntCount());
    for (int i=0;i<iCount;i++)
      m_pField[i] |= other.m_pField[i];
    MaskOutTail();
  }

  /// \brief
  ///   Assignment operator to copy all bits from another field. Also allocates the same amount of bits
  VTBitfield &operator = (const VTBitfield &other)
  {
    CopyFrom(other);
    return *this;
  }

  /// \brief
  ///   Evaluates the number of 32 bit integers necessary to represent the bitfield, i.e. (GetBitCount()+31)/32
  inline int GetIntCount() const
  {
    return (m_iBitCount+31)>>5;
  }

  /// \brief
  ///   Returns a 32 bit integer of specified index, where index must be [0..GetIntCount()-1]
  inline unsigned int GetInt(int idx) const
  {
    return m_pField[idx];
  }

  /// \brief
  ///   Returns a pointer to the 32 bit integer array that represents the bitfield
  inline unsigned int *GetIntArray() const 
  {
    return m_pField;
  }

  /// \brief
  ///   Returns the number of bits (see AllocateBitfield)
  inline int GetBitCount() const 
  {
    return m_iBitCount;
  }
  
  /// \brief
  ///   Counts the number of bits that are set between bits 'start' (included) and 'end' (excluded) - start and end get clamped to 0 and number of bits in bitmask.
  inline int CountSetBits(int start, int end) const
  {
    int iCount = 0;
    start = hkvMath::clamp (start, 0, m_iBitCount);
    end = hkvMath::clamp (end,   0, m_iBitCount);
    for(int i=start; i<end; ++i)
    {
      iCount += int((m_pField[i>>5] & (1U<<(i&31))) != 0);
    }
    return iCount;
  }
  
  /// \brief
  ///   Counts the number of bits that are set
  inline int CountSetBits() const
  {
    const int iIntCount = GetIntCount();
    int iCount = 0;
    for(int i=0; i<iIntCount; ++i)
    {
      unsigned int iInt32 = m_pField[i];
      while (iInt32)
      {
        iCount += (iInt32&1);
        iInt32>>=1;
      }
    }
    return iCount;
  }

  inline void SerializeX(VArchive &ar)
  {
    if (ar.IsLoading())
    {
      int iBitCount;
      ar >> iBitCount;
      AllocateBitfield(iBitCount);
      const int iIntCount = GetIntCount();
      if (iIntCount>0)
        ar.Read(GetIntArray(),iIntCount*sizeof(int),"i",iIntCount);
    } else
    {
      const int iIntCount = GetIntCount();
      ar << m_iBitCount;
      if (iIntCount>0)
        ar.Write(GetIntArray(),iIntCount*sizeof(int),"i",iIntCount);
    }
  }


protected:
  void MaskOutTail()
  {
    int iTailBits = (m_iBitCount&31); // number of bits in the last 32bit int
    if (iTailBits==0)
      return;
    unsigned int iTailMask = (1U<<iTailBits)-1; // make a mask out of it
    m_pField[GetIntCount()-1] &= iTailMask; // and mask out the last int. GetIntCount()>0 since iTailBits!=0
  }

  int m_iBitCount;
  unsigned int *m_pField;
  unsigned int m_OwnBits[(iStaticBitCount+31)/32]; ///< in case we only need <=iStaticBitCount bits, we do not need to allocate
};



/// \brief
///   Specialization of the bitfield template class with 64 bits statically allocated
class VBitfield : public VTBitfield<64>
{
public:

  /// \brief
  ///   Writes the bitfield to a chunk file
  VBASE_IMPEXP void WriteChunk(VChunkFile &f) const;

  /// \brief
  ///   Reads the bitfield from a chunk file
  static VBASE_IMPEXP VBitfield* ReadChunk(VChunkFile &f);
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
