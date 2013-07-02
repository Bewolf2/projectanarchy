/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASEOBJECT_HPP_INCLUDED
#define VBASEOBJECT_HPP_INCLUDED

/// \brief
///  General Vision base class.
///
/// Defines custom new and delete operators to solve different memory module problems.
/// Derive all Vision objects with virtual destructors from this class
/// to prevent memory problems (See MSDN:Q122675)!
class VBaseObject
{
public:
  virtual ~VBaseObject() {}

#ifndef NO_NEW_OVERRIDE
  inline void* operator new  (size_t iSize)                               { return VBaseAlloc(iSize); }
  inline void* operator new[](size_t iSize)                               { return VBaseAlloc(iSize); }
  inline void* operator new  (size_t iSize, size_t iAlign)                { return VBaseAlignedAlloc(iSize,(int)iAlign); }
  inline void* operator new[](size_t iSize, size_t iAlign)                { return VBaseAlignedAlloc(iSize,(int)iAlign); }
  inline void  operator delete  (void *ptr)                               { VBaseDealloc(ptr); }
  inline void  operator delete[](void *ptr)                               { VBaseDealloc(ptr); }
  inline void  operator delete  (void *ptr, size_t iAlign)                { VBaseAlignedDealloc(ptr); }
  inline void  operator delete[](void *ptr, size_t iAlign)                { VBaseAlignedDealloc(ptr); }
#endif
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
