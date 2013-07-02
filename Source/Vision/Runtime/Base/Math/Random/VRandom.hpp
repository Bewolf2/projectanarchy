/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRandom.hpp

#ifndef RANDOM_HPP_INCLUDED
#define RANDOM_HPP_INCLUDED

#ifdef WIN32
#define RANDOM_TABLE_SIZE   0x10000
#else
#define RANDOM_TABLE_SIZE   0x01000
#endif


///\brief
///  Pseudo random generator for float and int values.
///
///Random numbers can be generated in a reproducible way which is important for vegetation distribution.
///The random values are based on a precomputed table of random values.
class VBASE_IMPEXP VRandom
{
public:
  ///\brief
  ///  Constructor that takes an input seed value.
  inline VRandom(int iStart=0) {Reset(iStart);}
  ///\brief
  ///  Sets the new starting point (i.e. seed value).
  inline void Reset(unsigned int iSeed=0) 
  {
    // Pack all the bits down into the size of the table index
    int iPos = 0;
    while(iSeed)
    {
      iPos += iSeed&(RANDOM_TABLE_SIZE-1);
      iSeed /= RANDOM_TABLE_SIZE;
    }

    m_iIntPos=m_iFloatPos=iPos&(RANDOM_TABLE_SIZE-1);
  }

  ///\brief
  ///  Returns the next integer value.
  inline unsigned int GetInt() const
  {
    unsigned int iVal = g_fIntRand[m_iIntPos];
    m_iIntPos = (m_iIntPos+1)&(RANDOM_TABLE_SIZE-1);
    return iVal;
  }
  ///\brief
  ///  Returns the next float value (which is in 0..1 range).
  inline float GetFloat() const
  {
    float fVal = g_fFloatRand[m_iFloatPos];
    m_iFloatPos = (m_iFloatPos+1)&(RANDOM_TABLE_SIZE-1);
    return fVal;
  }
  ///\brief
  ///  Returns the next float value in -1..1 range.
  inline float GetFloatNeg() const
  {
    return GetFloat()*2.f-1.f;
  }

  ///\brief
  ///  Skips passed number of integer values to maintain reproducibility.
  inline void SkipInts(int iCount) {m_iIntPos = (m_iIntPos+iCount)&(RANDOM_TABLE_SIZE-1);}
  ///\brief
  ///  Skips passed number of float values to maintain reproducibility.
  inline void SkipFloats(int iCount) {m_iFloatPos = (m_iFloatPos+iCount)&(RANDOM_TABLE_SIZE-1);}

  mutable int m_iIntPos,m_iFloatPos;
private:
  static void GenerateRandomTable();
  friend class _RandTableCreator;
  static float g_fFloatRand[RANDOM_TABLE_SIZE];
  static unsigned int g_fIntRand[RANDOM_TABLE_SIZE];
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
