/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// VArrayHelper_cl Class
//***********************************************************************

// you must first include the vision.hpp file to use this header
#ifndef _VArrayHelper_cl_HEADER_
#define _VArrayHelper_cl_HEADER_

/// \brief
///   Splits any zero terminated array (e.g. string as char or wchar_t array) into tokens (zero terminated sub arrays) according to the specified delimiters.
template<class ELEM_TYPE>
class VArrayHelper_cl
{
public:

  /// \brief
  ///   Creates a tokenizer for the specified zero terminated buffer (e.g. C-style string as char *).
  ///
  /// \param pBuffer
  ///   The zero (NULL) terminated buffer to split (e.g. L"This \n is \n a \n test \t !").
  ///
  /// \param pDelimiters
  ///   A zero (NULL) terminated array containing all valid delimiters (e.g. L"\n\t ").
  VArrayHelper_cl(const ELEM_TYPE * pBuffer, const ELEM_TYPE * pDelimiters) 
  {
    //ensure buffers and calc Length
    VASSERT(pBuffer);
    VASSERT(pDelimiters);
    m_iLength = Length(pBuffer);
    m_iDelimiters = Length(pDelimiters);

    //copy the original buffer
    m_pBuffer = Copy(pBuffer, m_iLength);

    //copy the buffer of delimiters
    m_pDelimiters = Copy(pDelimiters, m_iDelimiters);

    //setup start conditions
    m_pCurrentToken = NULL;
    m_iStartIndex = 0;
    m_iNextIndex = -1;

    //search for the first token
    seek();
  }

  /// \brief
  ///   Release internal resources.
  ~VArrayHelper_cl() 
  {
    V_SAFE_FREE(m_pBuffer);
    V_SAFE_FREE(m_pDelimiters);
    V_SAFE_FREE(m_pCurrentToken);
  }

  /// \brief
  ///   Get the next token (if present) or NULL.
  const ELEM_TYPE * NextToken() 
  {
    if(!HasNextToken()) return NULL;

    V_SAFE_FREE(m_pCurrentToken);

    int iBufferLength = m_iNextIndex-m_iStartIndex;
    VASSERT(iBufferLength>0);

    //create the buffer with terminator at the end
    m_pCurrentToken = Copy(&m_pBuffer[m_iStartIndex], iBufferLength);

    //seek for next token
    seek();

    //return the generated buffer;
    return m_pCurrentToken;
  }

  /// \brief
  ///   Returns true if there is still a token present, otherwise false.
  bool HasNextToken() const 
  {
    return m_iStartIndex < m_iLength;
  } 

  /// \brief
  ///   Calculates and returns the overall number of tokens.
  int CalcNumberOfTokens() const 
  {
    int iStart=0;
    int iNext = -1;
    int iTokens = -1;

    while(iStart < m_iLength) 
    {
      //continue from the last delimiter
      iStart = iNext+1;
      iTokens++;

      //skip sequence of delimiters
      while (iStart<m_iLength && Find(m_pDelimiters, m_pBuffer[iStart])!=NULL)
        iStart++;

      //search for next delimiter
      iNext = iStart+1;
      while (iNext<m_iLength && Find(m_pDelimiters, m_pBuffer[iNext])==NULL)
        iNext++;
    } 

    return iTokens;
  }


  ///
  /// @name Static Methods
  /// @{
  ///


  /// \brief
  ///   Check whether a NULL-terminated buffer contains a specified element (returns the pointer to the element or NULL).
  static const ELEM_TYPE * Find(const ELEM_TYPE *pBuffer, ELEM_TYPE symbol) 
  {
    VASSERT(pBuffer);
    const ELEM_TYPE *ptr = pBuffer;

    while(*ptr) 
    {
      if(*ptr==symbol) return ptr;
      ptr++;
    }

    return NULL;
  }


  /// \brief
  ///   Calculate the Length of a specified, NULL-terminated buffer.
  static int Length(const ELEM_TYPE * pBuffer) 
  {
    VASSERT(pBuffer);

    int i=0;
    while (pBuffer[i] != 0) i++;

    return i;
  }

  /// \brief
  ///   Creates a NULL-terminated copy of the specified buffer with the predefined length.
  static ELEM_TYPE * Copy(const ELEM_TYPE * pBuffer, int length) 
  {
    VASSERT(pBuffer);

    //create an empty buffer
    ELEM_TYPE *pCopy = (ELEM_TYPE *) vMemAlloc(sizeof(ELEM_TYPE)*(length+1));

    //copy and terminate
    memcpy((void *)pCopy, (void*) pBuffer, sizeof(ELEM_TYPE)*length);
    pCopy[length] = NULL;

    return pCopy;
  }

  /// \brief
  ///   Creates a copy of a NULL-terminated buffer.
  static ELEM_TYPE * Copy(const ELEM_TYPE * pBuffer) 
  {
    VASSERT(pBuffer);

    //create an empty buffer
    int length = Length(pBuffer);
    ELEM_TYPE *pCopy = (ELEM_TYPE *) vMemAlloc(sizeof(ELEM_TYPE)*(length+1));

    //copy and terminate
    memcpy((void *)pCopy, (void*) pBuffer, sizeof(ELEM_TYPE)*length);
    pCopy[length] = NULL;

    return pCopy;
  }

  ///
  /// @}
  ///

private:
  //members

  //buffer to split and delimiters
  ELEM_TYPE *m_pBuffer;
  ELEM_TYPE *m_pDelimiters;

  //pointer to the current token
  ELEM_TYPE *m_pCurrentToken;

  //overall Length of the buffer and delimiters
  int m_iLength;
  int m_iDelimiters;

  //start and stop index of the current token
  int m_iStartIndex;
  int m_iNextIndex;


  //methods

  //seek for the next token
  void seek() 
  {
    //continue from the last delimiter
    m_iStartIndex = m_iNextIndex+1;

    //skip sequence of delimiters
    while (m_iStartIndex<m_iLength && Find(m_pDelimiters, m_pBuffer[m_iStartIndex])!=NULL)
      m_iStartIndex++;

    //search for next delimiter
    m_iNextIndex = m_iStartIndex+1;
    while (m_iNextIndex<m_iLength && Find(m_pDelimiters, m_pBuffer[m_iNextIndex])==NULL)
      m_iNextIndex++;

  }
};

#endif //_VArrayHelper_cl_HEADER_

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
