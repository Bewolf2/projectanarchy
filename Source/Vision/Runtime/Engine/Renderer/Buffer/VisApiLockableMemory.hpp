/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VIS_API_LOCKABLE_MEMORY_HPP
#define VIS_API_LOCKABLE_MEMORY_HPP

#if defined( SUPPORTS_LOCKABLE_MEMORY )

  #if defined( _VR_GCM )
    typedef VGcmMemory VGpuMemory;
    typedef uint32_t VLabel;
  #elif defined( _VR_GXM )
    struct VGxmMemory
    {
      void*     m_pAddress;
      SceUID    m_sceUID;
      uint32_t  m_uiSize;
      uint32_t  m_uiAlignment;
    };
    typedef VGxmMemory VGpuMemory;
    typedef SceGxmNotification VLabel;
  #elif defined( _VR_GX2 )
    typedef GX2RBuffer VGpuMemory;
    typedef u64 VLabel;
  #else
    #error Undefined platform!
  #endif

/// \brief
///   A utility class for encapsulating and safely freeing graphics memory.
///
/// This class manages pre-allocated graphics memory, which can then
/// be locked for read/write access and/or safely released. In all cases, a label mechanism is used to 
/// grant access only if the memory is not currently in use by the GPU.
///
/// If usage information is not provided via the BeginUse() / EndUse() API, a 
/// conservative labeling strategy is used.
///
/// \note
///   GPU memory is to be allocated outside of this class.
///
/// \platforms
///   PS3, PS Vita, Wii U
class VLockableMemory
{
public:
  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Standard Constructor
  VISION_APIFUNC VLockableMemory();

  /// \brief
  ///   Constructor that initializes the information about the memory to be managed.
  ///
  /// \param pMemory
  ///   A pointer to the (pre-allocated) memory to be managed by this instance.
  ///
  /// \param bSafeCleanUp
  ///   Does the memory clean-up have to be secured by a label as well? Set this to
  ///   false if the memory is only to be released when it is guaranteed that it 
  ///   is not in use anymore, e.g., at the end of the program.
  ///
  /// \sa VLockableMemory::ReleaseMemory
  VISION_APIFUNC VLockableMemory( VGpuMemory* pMemory, bool bSafeCleanUp = true );

  /// \brief
  ///   Destructor - does not release the encapsulated memory.
  ///
  /// \note 
  ///   This destructor does not automatically release the encapsulated memory.
  ///   Use VLockableMemory::ReleaseMemory() for that.
  ///
  /// \ sa VLockableMemory::ReleaseMemory
  ~VLockableMemory();

  ///
  /// @}
  ///


  /// \brief
  ///   Safely releases the encapsulated memory.
  VISION_APIFUNC void ReleaseMemory();

  /// \brief
  ///   Lock the GPU memory for reading and/or writing.
  /// 
  /// This method provides safe access to GPU memory for reading and/or writing. By specifying appropriate
  /// lock flags, performance can be improved considerably as it allows for skipping explicit synchronization and/or additional
  /// data transfer.
  ///
  /// If GPU usage information is provided via the BeginUse() / EndUse() API, it is used to control access to this resource.
  /// If no such information is provided, a more conservative approach is used by assuming that this memory has been in use by
  /// the GPU until the last applicable GPU operation.
  ///
  /// \param uiLockFlags
  ///   In order to optimize performance, the intended use can be indicated through a combination of the following flags:
  ///   \li VIS_LOCKFLAG_DISCARDABLE: Specify this flag if _all_ data in the memory area is to be overwritten.
  ///     This allows for replacing all internal data with freshly allocated memory in order to avoid having to
  ///     wait for the GPU to finish using this resource. The current memory will be released as soon as it is not
  ///     in use anymore.
  ///   \li VIS_LOCKFLAG_READONLY: Specify this flag if there will be no data written to the memory area. This allows
  ///     for skipping GPU data upload.
  ///   \li VIS_LOCKFLAG_NOOVERWRITE: Specify this flag if the locked memory area is guaranteed not to be in use 
  ///     by the GPU anymore. This allows for direct access to this memory area without having to wait for any labels
  ///     to be reached.
  ///   \li VIS_LOCKFLAG_REPLACE_ALL: Specify this flag if the complete _locked_ memory area is to be overwritten.
  ///     Depending on the platform, this allows for skipping GPU data download into CPU-accessible memory.
  ///
  /// \param uiOffset
  ///   Starting offset of the memory area to be locked.
  ///
  /// \param uiSize
  ///   Size of the memory area to be locked.
  ///
  /// \returns
  ///   A pointer to the locked data.
  ///
  /// \sa VGpuMemory::Unlock
  /// \sa VGpuMemory::BeginUse
  /// \sa VGpuMemory::EndUse
  inline void* Lock( unsigned int uiLockFlags, unsigned int uiOffset = 0, unsigned int uiSize = 0 );

  /// \brief
  ///   Unlock the GPU memory.
  inline void Unlock();

  /// \brief
  ///   Check, whether this memory is currently locked.
  inline bool IsLocked() const { return m_bLocked; }

  /// \brief
  ///   Indicate that this memory is bound for use by the GPU.
  inline void BeginUse() {
    ++m_uiInUse;
  }

  /// \brief
  ///   Indicate that this memory is unbound from GPU usage.
  inline void EndUse() { 
    VASSERT( m_uiInUse > 0 );
    --m_uiInUse; 
    if ( m_uiInUse == 0 ) 
      m_label = GetNextLabel(); 
  }

  /// \brief
  ///   Check, whether this memory is currently bound for use by the GPU.
  inline bool IsInUse() { return m_uiInUse > 0; }

  /// \brief
  ///   For internal use only - safely free the given memory when it is not in use by the GPU anymore 
  ///   by queuing it for later release.
  ///
  /// \note This method is for internal use only. Use VLockableMemory::ReleaseMemory() instead.
  ///
  /// \sa VLockableMemory::ReleaseMemory
  VISION_APIFUNC static void FreeFenced( VLockableMemory* pMemory );

  /// \brief
  ///   For internal use only - sweep through queued resources, freeing them if they are not in use anymore.
  VISION_APIFUNC static void SweepFreeFenced();

#if defined( HK_DEBUG )
  /// \brief
  ///   Retrieve the maximum amount of resources queued for freeing at any one time.
  static inline unsigned int GetPendingFreesHighWatermark() { return hkvMath::Max( s_uiNumPendingFrees, s_uiNumPendingFrees_HighWatermark ); }
  /// \brief
  ///   Reset the maximum amount of resources queued for freeing at any one time.
  static inline void ResetPendingFreesHighWatermark() { s_uiNumPendingFrees_HighWatermark = 0; }
#endif

  /// \brief
  ///   Retrieve the internal GPU memory / resource.
  inline VGpuMemory* GetGpuMemory() const { return &m_gpuMemory; }

  /// \brief
  ///   Check, whether the internal GPU memory / resource is valid (i.e., not released or queued for freeing).
  inline bool IsValid() const { return IsMemoryValid( m_gpuMemory ); }

private:

  inline void InvalidateLabel();
  static inline VLabel GetLastLabel();                // retrieve label for the last submitted draw command(s) / scene
  static inline VLabel GetNextLabel();                // retrieve label for draw command(s) / scene to be submitted next (i.e., the current one)
  static inline void WaitForLabel( VLabel label );    // wait for the given label; if it's invalid, create a new one and wait for that
  static inline bool IsLabelValid( VLabel label );    // check the given label for validity
  static inline bool IsLabelReached( VLabel label );  // NOTE: the given label MUST be valid

  static inline void InvalidateMemory( VGpuMemory& gpuMemory );   // mark the memory info as invalid
  static inline bool IsMemoryValid( VGpuMemory& gpuMemory );      // check whether the given memory info has been marked as invalid
  static inline void FreeMemory( VGpuMemory& gpuMemory );         // release the given GPU memory

  static inline void FlushGraphics();                 // well, flush the graphics pipeline, if applicable

  mutable VGpuMemory  m_gpuMemory;
  VLabel              m_label;
  bool                m_bLocked;
  unsigned int        m_uiLockFlags;
  unsigned int        m_uiInUse;
  bool                m_bSafeCleanUp;

  enum { MAX_PENDING_FREES = 1024 };
  static unsigned int s_uiNumPendingFrees;
  static unsigned int s_uiWritePos;
  static unsigned int s_uiReadPos;
  static VGpuMemory s_pendingFrees[ MAX_PENDING_FREES ];
  static VLabel s_labels[ MAX_PENDING_FREES ];
#if defined( HK_DEBUG )
  static unsigned int s_uiNumPendingFrees_HighWatermark;
#endif
};


#if defined( _VR_GCM )
  #include <Vision/Runtime/Engine/Renderer/Buffer/VisApiLockableMemoryPS3.inl>
#elif defined( _VR_GXM )
  #include <Vision/Runtime/Engine/Renderer/Buffer/VisApiLockableMemoryPSP2.inl>
#elif defined( _VR_GX2 )
  #include <Vision/Runtime/Engine/Renderer/Buffer/VisApiLockableMemoryGX2.inl>
#else
  #error Unknown platform!
#endif

#endif  // defined( SUPPORTS_LOCKABLE_MEMORY )

#endif // VIS_API_LOCKABLE_MEMORY_HPP

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
