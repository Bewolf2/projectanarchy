/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBackgroundThread.hpp

#ifndef VBACKGROUNDTHREAD_HPP_INCLUDED
#define VBACKGROUNDTHREAD_HPP_INCLUDED



/// \brief
///   A platform independent thread which runs in the background
class VBackgroundThread
{
  public:

    /// \brief  Constructor. 
    ///
    /// Note that the thread is not started until the Start() method is called.
    ///
    /// \param  ePriority The priority of the thread. 
    /// \param [in] pThreadName The name of the thread. This is used to display the thread name for example in the debugger.
    /// \param uiXBoxThreadProcessor (Xbox360 only) The processor on which the thread should work on.
    ///
    VBASE_IMPEXP VBackgroundThread(VThreadPriority ePriority, const char* pThreadName = NULL, unsigned int uiXBoxThreadProcessor = 5);

    /// \brief   
    ///   Destructor.
    VBASE_IMPEXP virtual ~VBackgroundThread();

    /// \brief  
    ///   Starts the thread.
    VBASE_IMPEXP void Start();

    /// \brief  
    ///   Virtual run function. Override this function to implement the actual work of the thread.
    virtual void Run() = 0;


  protected:

#ifdef WIN32
  static DWORD WINAPI Process(void *pParam);


#elif defined(_VISION_XENON)
  static DWORD Process(void *pParam);



#elif defined(_VISION_PS3)
  static void Process(uint64_t arg);



#elif defined(_VISION_PSP2)
    static SceInt32 Process(SceSize ArgSize, void* pArgBlock);
#elif defined(_VISION_WIIU)
    static int Process(int ArgSize, void* pArgBlock);
#elif defined(_VISION_POSIX)
  static void* Process(void* pParam);
#else
  #error "Missing Platform!"
#endif

    //! Stores the thread priority. 
    VThreadPriority m_eThreadPriority;

    VThread *m_pThread;

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
