/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiFence.hpp

#ifndef _VISAPIFENCE_HPP_INCLUDED_
#define _VISAPIFENCE_HPP_INCLUDED_

#if defined(_VR_DX9)
  typedef IDirect3DQuery9* VPlatformInternalFence;
#elif defined(_VR_DX11)
  typedef D3DQuery* VPlatformInternalFence;
#elif defined(_VR_GCM)
  
  typedef unsigned int VPlatformInternalFence;

#ifndef _VISION_DOC

  //
  // *** VisGcmFence ***
  //
  // Synopsis:
  //    - This class encapsulates a fence object for gcm
  //  
  class VisGcmFence
  {

    public:

      //
      // *** GetNextFreeFence ***
      //
      // Synopsis:
      //    - Returns a free fence object
      //
      // Returns:
      //    - Index of free fence
      // 
      static unsigned int GetNextFreeFence();

      //
      // *** CreateNewFence ***
      //
      // Synopsis:
      //    - Creates a new fence object ready for usage
      //
      // Returns:
      //    - A new fence index
      // 
      static unsigned int CreateNewFence();

      //
      // *** DestroyFence ***
      //
      // Synopsis:
      //    - Frees a fence index
      // 
      static void DestroyFence( unsigned int iIndex );

      //
      // *** InitFenceSystem ***
      //
      // Synopsis:
      //    - Initializes the fence system (Internal use only!)
      //
      // Returns:
      //    - true if the fence system was initialized correctly, false otherwise
      // 
      static bool InitFenceSystem();

      //
      // *** DeInitFenceSystem ***
      //
      // Synopsis:
      //    - Deinitializes the fence system (Internal use only!)
      //
      // Returns:
      //    - true if the fence system was correctly deinitialized, false otherwise
      // 
      static bool DeInitFenceSystem();

      //
      // *** GetNewFinishID ***
      //
      // Synopsis:
      //    - Returns a new finish ID. Finish IDs are required to ensure that the same fence can be issued multiple times.
      //
      static unsigned short GetNewFinishID();

      //
      // *** Insert ***
      //
      // Synopsis:
      //    - Inserts a fence into the command buffer
      //
      static void Insert( unsigned int iIndex );

      //
      // *** IsReached ***
      //
      // Synopsis:
      //    - Checks if the fence was already reached (non-blocking)
      //
      // Returns:
      //    - true if the fence was reached, false otherwise
      // 
      static bool IsReached(unsigned int iIndex);

      //
      // *** IsInserted ***
      //
      // Synopsis:
      //    - Checks if the fence has been inserted
      //
      // Returns:
      //    - true if the fence was inserted, false otherwise
      // 
      static bool IsInserted(unsigned int iIndex);

      //
      // *** WaitFor ***
      //
      // Synopsis:
      //    - Waits for the fence to be reached
      //
      static void WaitFor(unsigned int iIndex);

    private:

      static bool m_bIsInitialized;
      static VGcmMemory m_FenceMemory;
      static bool *m_pFenceAllocated;
      static unsigned short *m_pFinishedID;
      static unsigned short m_iCurrentFinishID;
      static unsigned int m_iCurrentFenceIndex;
  };

#endif // _VISION_DOC


#elif defined(_VR_GXM)
  typedef unsigned int VPlatformInternalFence; // Not really supported
#elif defined(_VR_GX2)
  typedef u64 VPlatformInternalFence;
#elif defined(_VR_GLES2)
  typedef unsigned int VPlatformInternalFence; // Not really supported
#else
  #error "Missing Platform!"
#endif

/// \brief
///   This class encapsulates a fence object for CPU-GPU synchronisation purposes.
///
class VisFenceObject_cl
{
  public:

    ///
    /// \brief
    ///   Creates the fence object state.
    ///
    VisFenceObject_cl();

    ///
    /// \brief
    ///   Releases the fence object, cleans up the internal resources.
    ///
    ~VisFenceObject_cl();

    ///
    /// \brief
    ///   Inserts a fence into the command buffer.
    ///
    void Insert();

    ///
    /// \brief
    ///   Checks if the fence was already reached (non-blocking).
    ///
    /// \returns
    ///   True if the fence was reached, false otherwise.
    bool IsReached();

    ///
    /// \brief
    ///   Waits for the fence to be reached (blocking).
    ///
    void WaitFor();

    ///
    /// \brief
    ///   Returns true if the platform supports fences, false otherwise.
    ///
    inline static bool SupportsFences()
    {
      #if defined(WIN32) || defined(_VISION_XENON) || defined(_VISION_PS3) || defined(_VISION_WIIU)
      return true;
      #elif defined(_VISION_PSP2) || defined(_VISION_IOS) || defined(_VISION_ANDROID)
      return false;
      #else
        #error "Missing platform!"
      #endif
    }
    
    
  private:

    // Internal functions (implemented in platform specific files)

    void Create_Internal();

    void Destroy_Internal();

    void Insert_Internal();

    bool IsReached_Internal();

    void WaitFor_Internal();

    VPlatformInternalFence m_Fence;
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
