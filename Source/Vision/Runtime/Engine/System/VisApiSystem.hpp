/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSystem.hpp

#ifndef DEFINE_VISAPISYSTEM
#define DEFINE_VISAPISYSTEM

/// \brief
///   Provides basic memory-related functionality. Can be accessed via Vision::System.
class VisSystem_cl
{
public:
  
  ///
  /// @name Memory Related Functions
  /// @{
  ///

  /// \brief
  ///   Allocates memory (malloc wrapper)
  /// 
  /// Allocates iNumBytes Bytes of memory. This function wraps malloc.
  /// 
  /// This function is useful to guarantee that the memory is allocated via the same memory module
  /// that the engine uses.
  /// 
  /// \param iNumBytes
  ///   Number of bytes to allocate.
  /// 
  /// \return
  ///   pointer to allocated memory, never \c NULL
  /// 
  /// \note
  ///   If not successful (e.g. no memory left), the engine quits with a fatal error
  /// 
  /// \note
  ///   if iNumBytes is zero, this function returns a NULL pointer
  VISION_APIFUNC void *Malloc(unsigned int iNumBytes);

  /// \brief
  ///   Frees a memory block that has been allocated via VisSystem_cl::Malloc
  /// 
  /// This function wraps free()
  /// 
  /// \param pMem
  ///   The block to free.
  /// 
  /// \sa VisSystem_cl::Malloc()
  VISION_APIFUNC void Free(void *pMem);


#ifdef _VISION_XENON

  /// \brief
  ///   Get the amount of free memory (in Bytes) on the Xbox360 console
  /// 
  /// This function returns the amount of available physical memory (in Bytes) on the Xbox360
  /// console.
  /// 
  /// This function calls the GlobalMemoryStatus() function, so the granularity depends on this
  /// function.
  /// 
  /// This function can be called at any time after Vision has been initialised.
  /// 
  /// This function might have a significant speed impact.
  /// 
  /// \return
  ///   unsigned int freemem: Number of free bytes
  VISION_APIFUNC unsigned int GetFreeMemory();

#endif

  ///
  /// @}
  ///
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
