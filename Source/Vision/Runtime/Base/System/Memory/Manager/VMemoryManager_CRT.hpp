/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VMEMORYMANAGER_CRT_HPP_INCLUDED
#define VMEMORYMANAGER_CRT_HPP_INCLUDED


#include <Vision/Runtime/Base/System/Memory/Manager/VMemoryManager.hpp>


/// \brief
///   The default memory manager
/// 
/// Default raw memory allocator/manager using CRT functions  (and some platform specific
/// extensions).
class VMemoryManager_CRT : public IVMemoryManager
{
public:
  /// \brief
  ///   Constructor.
  VBASE_IMPEXP VMemoryManager_CRT()
  {
    //Nothing to do here
  }

  VBASE_IMPEXP virtual ~VMemoryManager_CRT()
  {
    //Nothing to do here
  }

  VBASE_IMPEXP virtual void*   Alloc       (size_t iSize) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void*   AlignedAlloc(size_t iSize, int iAlignment) HKV_OVERRIDE;

  VBASE_IMPEXP virtual void    Free       (void* ptr) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void    AlignedFree(void* ptr) HKV_OVERRIDE;

  VBASE_IMPEXP virtual size_t  MemSize       (void* ptr) HKV_OVERRIDE;
  VBASE_IMPEXP virtual size_t  AlignedMemSize(void* ptr, int iAlignment) HKV_OVERRIDE;
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
