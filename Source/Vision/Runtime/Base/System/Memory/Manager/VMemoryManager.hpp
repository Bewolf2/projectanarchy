/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMemoryManager.hpp

#ifndef DC_VMEMORYMANAGER_HPP
#define DC_VMEMORYMANAGER_HPP

class IVMemoryManager;

VBASE_IMPEXP extern IVMemoryManager* g_pCurrentMemoryManager;

/// \brief
///   Gets the default memory manager provided by the engine.
///
VBASE_IMPEXP IVMemoryManager* GetDefaultVMemoryManager();

/// \brief
///   Gets a memory manager that tracks allocation callstacks on supported platforms.
///
VBASE_IMPEXP IVMemoryManager* GetTrackingVMemoryManager();

/// \brief
///   Gets a memory manager that prevents read and write accesses to out-of-bounds array indices and freed pointers.
VBASE_IMPEXP IVMemoryManager* GetGuardingMemoryManager();

/// \brief
///   Gets the currently set memory manager.
///
///
/// You must provide the implementation of this function using the DEFINE_MEMORYMANAGER helper.
VBASE_IMPEXP IVMemoryManager* GetVMemoryManager();

/// \brief
///   Sets the current memory manager at runtime.
///
/// \param pManager
///   The memory manager that will be used for all subsequent allocations.
///
/// \returns
///  The previous memory manager
VBASE_IMPEXP IVMemoryManager* SetVMemoryManager(IVMemoryManager* pManager);

#ifdef WIN32

// Dynamically linked - we need to fetch the default manager through the .exe
#define DEFINE_MEMORYMANAGER(manager)         \
  extern "C" __declspec(dllexport) void* __cdecl GetVMemoryManagerImpl()    \
{                                             \
  return manager;                             \
}                                          

#else

// Statically linked - declare function directly
#define DEFINE_MEMORYMANAGER(manager)         \
  IVMemoryManager* GetVMemoryManager()        \
{                                             \
  if(!g_pCurrentMemoryManager)                \
  g_pCurrentMemoryManager = manager;          \
  return g_pCurrentMemoryManager;             \
}                                          

#endif

#define DEFINE_DEFAULT_MEMORYMANAGER DEFINE_MEMORYMANAGER(GetDefaultVMemoryManager())
#define DEFINE_TRACKING_MEMORYMANAGER DEFINE_MEMORYMANAGER(GetTrackingVMemoryManager())
#define DEFINE_GUARDING_MEMORYMANAGER DEFINE_MEMORYMANAGER(GetGuardingMemoryManager())

/// \brief
///   Low level memory manager interface
///
/// All memory managers for allocations in main memory have to be derived from this interface. Vision's default memory manager
/// (VMemoryManager_CRT) as well as the optimized SmallBlockMemoryManager (VSmallBlockMemoryManager) are naturally also derived
/// from IVMemoryManager.
///
/// In order to use a custom memory manager, simply create a manager by implementing IVMemoryManager. To install a custom memory
/// manager, pass a pointer to it to the global SetVMemoryManager() function. Once this is done, all system memory management functions
/// will be forwarded to the custom implementation.
///
/// In order for this to work correctly, it is essential that the custom allocator has a way to determine whether an allocation
/// was performed by itself or a previously registered memory manager. For instance, the default memory manager may have been used
/// for allocating memory inside static initializers (i.e., before the custom memory manager was registered). These allocations need
/// to be freed by the default memory manager, so your custom implementation should forward all deallocations of memory blocks it does
/// not know about to the default implementation (VMemoryManager_CRT).
class IVMemoryManager
{
public:

  /// \brief
  ///   Default constructor of low level memory manager interface
  IVMemoryManager()
  {
    //Nothing to do here
  }

  /// \brief
  ///   Destructor of low level memory manager interface
  virtual ~IVMemoryManager()
  {
    //Nothing to do here
  }

  /// \brief
  ///   Allocates memory from the heap
  ///
  /// \param iSize
  ///   Number of bytes to allocate
  ///
  /// \returns
  ///   Pointer to the allocated memory, or \c NULL if requested memory could not be allocated.
  virtual void*   Alloc(size_t iSize) = 0;

  /// \brief
  ///   Allocates memory from the heap at an aligned address
  ///
  /// \param iSize
  ///   Number of bytes to allocate
  /// 
  /// \param iAlignment
  ///   Alignment (in bytes) of the allocation.
  ///
  /// \returns
  ///   Pointer to the allocated memory, or \c NULL if requested memory could not be allocated. 
  ///   If successful, the pointer is aligned according to the iAlignment parameter.
  virtual void*   AlignedAlloc(size_t iSize, int iAlignment) = 0;

  /// \brief
  ///   Frees memory on the heap. 
  ///
  /// \param ptr
  ///   Pointer to the address of the memory to be freed.
  ///
  virtual void    Free       (void* ptr) = 0;

  /// \brief
  ///   Frees aligned memory on the heap. 
  ///
  /// Data previously allocated with AlignedAlloc() will be freed using this method.
  ///
  /// \param ptr
  ///   Aligned pointer to the address of the memory to be freed. 
  ///
  virtual void    AlignedFree(void* ptr) = 0;

  /// \brief
  ///   Returns the size of an allocation
  ///
  /// Returns the size of the allocation pointed to by the passed address.
  ///
  /// \param ptr
  ///   Pointer to the address of the memory to be examined. 
  ///
  /// \returns
  ///   Size of the allocation
  virtual size_t  MemSize       (void* ptr) = 0;

  /// \brief
  ///   Returns the size of an aligned allocation
  ///
  /// Returns the size of the aligned allocation (previously allocated with AlignedAlloc()) pointed to by the passed address.
  ///
  /// \param ptr
  ///   Aligned pointer to the address of the memory to be examined. 
  ///
  /// \param iAlignment
  ///   Alignment (in bytes) of the allocation.
  ///
  /// \returns
  ///   Size of the allocation
  virtual size_t  AlignedMemSize(void* ptr, int iAlignment) = 0;

  /// \brief
  ///   Dumps memory leaks to the debug output channel of the current platform.
  ///
  /// Optional method - does not have to be implemented.
  virtual void DumpLeaks() {}

  /// \brief
  ///   Loads debug symbols to be used for retrieving additional information to code addresses.
  ///
  /// Optional method - does not have to be implemented.
  virtual void LoadSymbols() {}
};

#endif // DC_VMEMORYMANAGER_HPP

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
