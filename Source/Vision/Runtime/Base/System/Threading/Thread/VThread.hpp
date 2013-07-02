/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VThread.hpp

#ifndef VTHREAD_HPP_INCLUDED
#define VTHREAD_HPP_INCLUDED

#ifdef _VISION_PS3
#include <sys/ppu_thread.h>
#include <sys/synchronization.h>
#endif

#ifdef _VISION_POSIX
#include <pthread.h>
#include <semaphore.h>
#endif

/// \brief
///   enum used to specify the priority of a VThread object
#if defined(_VISION_WINRT)
enum VThreadPriority
{
  THREADPRIORITY_VERYLOW  =  0,
  THREADPRIORITY_LOW      =  1,
  THREADPRIORITY_NORMAL   =  2,
  THREADPRIORITY_HIGH     =  3,
  THREADPRIORITY_VERYHIGH =  4
};

typedef DWORD (__stdcall *VPlatformThreadFunc)(LPVOID);

#elif defined(WIN32)  || defined(_VISION_XENON) 
enum VThreadPriority
{
  THREADPRIORITY_VERYLOW  =  THREAD_PRIORITY_LOWEST,
  THREADPRIORITY_LOW      =  THREAD_PRIORITY_BELOW_NORMAL,
  THREADPRIORITY_NORMAL   =  THREAD_PRIORITY_NORMAL,
  THREADPRIORITY_HIGH     =  THREAD_PRIORITY_ABOVE_NORMAL,
  THREADPRIORITY_VERYHIGH =  THREAD_PRIORITY_HIGHEST
};

typedef DWORD (__stdcall *VPlatformThreadFunc)(LPVOID);

#elif defined(_VISION_PS3)
enum VThreadPriority
{
  THREADPRIORITY_VERYLOW  =  3000,
  THREADPRIORITY_LOW      =  2000,
  THREADPRIORITY_NORMAL   =  1001,
  THREADPRIORITY_HIGH     =  500,
  THREADPRIORITY_VERYHIGH =  250
};

typedef void (*VPlatformThreadFunc)(uint64_t);

#elif defined(_VISION_POSIX)
enum VThreadPriority  ///< POSIX is user defined - we use 1 for lowest and 5 for highest
{
  THREADPRIORITY_VERYLOW  =  1,
  THREADPRIORITY_LOW      =  2,
  THREADPRIORITY_NORMAL   =  3,
  THREADPRIORITY_HIGH     =  4,
  THREADPRIORITY_VERYHIGH =  5
};

typedef void* (*VPlatformThreadFunc)(void*);

#elif defined(_VISION_PSP2)
enum VThreadPriority
{
  THREADPRIORITY_VERYLOW  =  SCE_KERNEL_LOWEST_PRIORITY_USER /* 191 */,
  THREADPRIORITY_LOW      =  170,
  THREADPRIORITY_NORMAL   =  SCE_KERNEL_DEFAULT_PRIORITY,
  THREADPRIORITY_HIGH     =  90,
  THREADPRIORITY_VERYHIGH =  SCE_KERNEL_HIGHEST_PRIORITY_USER /* 64 */
};

typedef SceInt32 (*VPlatformThreadFunc)(SceSize, void*);


#elif defined(_VISION_WIIU)
enum VThreadPriority
{
  THREADPRIORITY_VERYLOW  =  OS_PRIORITY_MIN, //0
  THREADPRIORITY_LOW      =  OS_PRIORITY_MIN + (OS_PRIORITY_APP_DEFAULT - OS_PRIORITY_MIN) / 2, 
  THREADPRIORITY_NORMAL   =  OS_PRIORITY_APP_DEFAULT, //16
  THREADPRIORITY_HIGH     =  OS_PRIORITY_APP_DEFAULT + (OS_PRIORITY_MAX - OS_PRIORITY_APP_DEFAULT) / 2, 
  THREADPRIORITY_VERYHIGH =  OS_PRIORITY_MAX // 31
};
typedef int (*VPlatformThreadFunc)(int intArg, void *ptrArg);

#else
#error "Missing Platform!"
#endif

/// \brief
///   Simple generic thread class.
///
/// This class is intentionally low-level and should rarely be used in game code.
/// For long-running tasks, it is recommended derive from the VBackgroundThread class.
/// For small work items, derive from the VThreadedTask class.
class VThread
{
public:
  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor; creates the thread. Note that the thread is not started until the Start() method is called.
  inline VThread(VPlatformThreadFunc pStartFunction, void *pArgument, int iStackSize, VThreadPriority ePriority);

  /// \brief
  ///   Destructor; destroys the thread object.
  inline ~VThread();

  /// \brief
  ///   Starts the thread.
  inline void Start();

  /// \brief
  ///   Returns the thread handle
  inline HANDLE GetHandle() const;

  /// \brief
  ///   Sets the thread processor
  inline void SetProcessor(int iProcessor);

  /// \brief
  ///   Returns the thread processor
  inline int GetProcessor() const
  {
    return m_iProcessor;
  }

  /// \brief
  ///   Sets the thread priority
  inline bool SetPriority(VThreadPriority ePriority);

  /// \brief
  ///   Returns the thread priority
  inline VThreadPriority GetPriority() const
  {
    return m_ePriority;
  }

  /// \brief
  ///   Returns the thread's stack size
  inline int GetStackSize() const
  {
    return m_iStackSize;
  }

  /// \brief
  ///   Joins the thread; i.e., returns when the thread has terminated.
  inline void Join();

private:

  VPlatformThreadFunc m_pStartFunc;

  #if defined(_VISION_WINRT)  
    Windows::System::Threading::Core::PreallocatedWorkItem^ m_startItem;
	Windows::Foundation::IAsyncAction^ m_thread;

  #elif defined(WIN32) || defined(_VISION_XENON)  
    volatile HANDLE m_hHandle;
  
  #elif defined(_VISION_PS3)
    sys_ppu_thread_t m_Thread;
    void* m_pArgument;

    inline static void ThreadFunc(uint64_t arg);

  #elif defined(_VISION_POSIX)
    pthread_t m_Thread;
    void* m_pArgument;
    friend class VThreadManager;

  #elif defined(_VISION_PSP2)
    SceUID m_Thread;
    void* m_pArgument;

  #elif defined(_VISION_WIIU)
    OSThread    m_Thread;
    static const int MAX_WIIU_THREAD_STACK_SIZE = 1* 1024 * 1024;
    u8          m_ThreadStack[MAX_WIIU_THREAD_STACK_SIZE];
    friend class VThreadManager;

  #else
    #error "Missing Platform!"
  #endif

  volatile int m_iProcessor;
  volatile int m_iStackSize;
  volatile VThreadPriority m_ePriority;
};


#if defined(_VISION_WINRT)
  #include <Vision/Runtime/Base/System/Threading/Thread/VThreadWinRT.inl>
#elif defined(WIN32)
  #include <Vision/Runtime/Base/System/Threading/Thread/VThreadPC.inl>
#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Base/System/Threading/Thread/VThreadXenon.inl>
#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Base/System/Threading/Thread/VThreadPS3.inl>
#elif defined(_VISION_POSIX)
  #include <Vision/Runtime/Base/System/Threading/Thread/VThreadPosix.inl>
#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Base/System/Threading/Thread/VThreadPSP2.inl>
#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Base/System/Threading/Thread/VThreadWiiU.inl>
#else
  #error Undefined platform!
#endif


#endif  // VTHREAD_HPP_INCLUDED

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
