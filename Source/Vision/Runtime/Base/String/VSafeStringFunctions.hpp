/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_SAFE_STRING_FUNCTIONS_HPP
#define V_SAFE_STRING_FUNCTIONS_HPP

// There are SDKs defining the _s functions, but not the templated extensions which deduct the size automatically
#if !defined(V_SAFE_STRING_FUNCTION_ONLY_TEMPLATE_EXTENSIONS)

inline void strcpy_s(char* szDest, size_t uiNum, const char* const szSrc)
{
  char *it1 = szDest;
  char *end = szDest + (uiNum-1);
  const char *it2 = szSrc;
  for(; *it2 != '\0' && it1 < end; ++it1,++it2)
  {
    *it1 = *it2;
  }
  *it1 = '\0';
}

#endif

template<int SIZE>
inline void strcpy_s(char(&szDest)[SIZE], const char* const szSrc)
{
  strcpy_s(szDest, SIZE, szSrc);
}

// There are SDKs defining the _s functions, but not the templated extensions which deduct the size automatically
#if !defined(V_SAFE_STRING_FUNCTION_ONLY_TEMPLATE_EXTENSIONS)

inline void strcat_s(char* szDest, size_t uiNum, const char* const szSrc)
{
  char* start = szDest;
  char* end = szDest + (uiNum - 1);
  while(*start != '\0' && start < end) ++start;
  strcpy_s(start, uiNum - (start - szDest), szSrc);
}

#endif

template<int SIZE>
inline void strcat_s(char(&szDest)[SIZE], const char* const szSrc)
{
  strcat_s(szDest, SIZE, szSrc);
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
