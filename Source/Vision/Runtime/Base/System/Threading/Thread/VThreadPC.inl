/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



VThread::VThread(VPlatformThreadFunc pStartFunction, void *pArgument, int iStackSize, VThreadPriority ePriority) :
  m_pStartFunc(pStartFunction),
  m_iProcessor(-1),
  m_ePriority(ePriority),
  m_iStackSize(iStackSize < 0 ? 0 : iStackSize)
{
  m_hHandle = CreateThread(NULL, m_iStackSize, m_pStartFunc, pArgument, CREATE_SUSPENDED, NULL);
  VASSERT(m_hHandle);
  SetPriority(ePriority);
}

VThread::~VThread()
{
  Join();
  CloseHandle(m_hHandle);
}

void VThread::Start()
{
  ResumeThread(m_hHandle);
}

HANDLE VThread::GetHandle() const
{
  return m_hHandle;
}

void VThread::SetProcessor(int iProcessor)
{
  if (iProcessor == -1)
    iProcessor = MAXIMUM_PROCESSORS;
  SuspendThread(m_hHandle);

  #ifdef _VISION_WINRT
    PROCESSOR_NUMBER n;
    n.Group = 0;
    n.Number = iProcessor;
	n.Reserved = 0;
    BOOL bRes = SetThreadIdealProcessorEx(m_hHandle, &n, NULL);
    //XX Currently fails on July XDK :   VASSERT(bRes);
  #else
    int iRes = SetThreadIdealProcessor(m_hHandle, iProcessor);
    VASSERT(iRes>=0);
  #endif
  ResumeThread(m_hHandle);

  m_iProcessor = iProcessor;
}


/// \brief
///   Sets the thread priority
bool VThread::SetPriority(VThreadPriority ePriority)
{
  BOOL bRes = SetThreadPriority(m_hHandle, (int)ePriority);
  if (bRes)
  {
    m_ePriority = ePriority;
  }
  return bRes == TRUE;
}


void VThread::Join()
{
  WaitForSingleObject(m_hHandle, INFINITE);
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
