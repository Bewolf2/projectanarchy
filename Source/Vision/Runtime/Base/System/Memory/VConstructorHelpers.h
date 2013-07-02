/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
template<class TYPE>
inline void VDestructElements(TYPE* pElements, int nCount)
{
  // call the destructor(s)
  for (; nCount--; pElements++)
    pElements->~TYPE();
}

template<class TYPE>
inline void VConstructElementsZeroInit(TYPE* pElements, int nCount)
{
  // first do bit-wise zero initialization
  memset((void*)pElements, 0, nCount * sizeof(TYPE));

  // then call the constructor(s)
  for (; nCount--; pElements++)
    ::new((void*)pElements) TYPE;
}

template<class TYPE>
inline void VConstructElements(TYPE* pElements, int nCount)
{
  // Call the constructor(s)
  for (; nCount--; pElements++)
    ::new((void*)pElements) TYPE;
}

template<class TYPE>
inline void VCopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
  // default is element-copy using assignment
  while (nCount--)
    *pDest++ = *pSrc++;
}

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
