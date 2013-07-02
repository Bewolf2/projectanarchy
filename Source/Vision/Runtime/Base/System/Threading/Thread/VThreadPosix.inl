/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



VThread::VThread(VPlatformThreadFunc pStartFunction, void *pArgument, int iStackSize, VThreadPriority ePriority) :
  m_pStartFunc(pStartFunction),
  m_pArgument(pArgument),
  m_iProcessor(-1),
  m_iStackSize(iStackSize),
  m_ePriority(ePriority)
{
}

VThread::~VThread()
{
  Join();
  pthread_detach(m_Thread);
}

void VThread::Start()
{
  int result = pthread_create(&m_Thread, NULL, m_pStartFunc, m_pArgument);
  VASSERT(result == 0);
  SetPriority(m_ePriority);
}

HANDLE VThread::GetHandle() const
{
  return (HANDLE)&m_Thread;
}

void VThread::SetProcessor(int iProcessor)
{
  // Not supported
}


bool VThread::SetPriority(VThreadPriority ePriority)
{
  int posixPriority;

#ifndef _VISION_ANDROID
  int prioMin    = sched_get_priority_min(SCHED_OTHER);
  int prioMax    = sched_get_priority_max(SCHED_OTHER);
#else
  // Android doesn't support SCHED_OTHER
  int prioMin    = sched_get_priority_min(SCHED_RR);
  int prioMax    = sched_get_priority_max(SCHED_RR);
#endif

  int prioNormal = prioMin + ((prioMax - prioMin) >> 1);
  int prioUnit   = prioMax / 3;
  VASSERT(prioUnit != 0);

  switch(ePriority)
  {
  case THREADPRIORITY_VERYLOW:  posixPriority = prioNormal - prioUnit * 2; break;
  case THREADPRIORITY_LOW:      posixPriority = prioNormal - prioUnit * 1; break;
  case THREADPRIORITY_NORMAL:   posixPriority = prioNormal;                break;
  case THREADPRIORITY_HIGH:     posixPriority = prioNormal + prioUnit * 1; break;
  case THREADPRIORITY_VERYHIGH: posixPriority = prioNormal + prioUnit * 2; break;
  default:
    VASSERT(false);
    break;
  }

  // Set the thread priority if thread is running.
  sched_param schedParam;
  schedParam.sched_priority = posixPriority;
  if(pthread_setschedparam(m_Thread, SCHED_OTHER, &schedParam) == 0)
  {
    m_ePriority = ePriority;
    return true;
  }
  else
  {
    return false;
  }
}


void VThread::Join()
{
  pthread_join(m_Thread, NULL);
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
