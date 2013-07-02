/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file CriSmpMvVideoDecodeTask_Vision.h

/****************************************************************************
 *
 * Copyright (c) 2008 CRI Middleware, Inc.
 *
 ****************************************************************************/
#ifndef	CRISMPMVVIDEODECODETASK_H_INCLUDED		/* Re-definition prevention */
#define	CRISMPMVVIDEODECODETASK_H_INCLUDED

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <cri_movie.h>

#include <list>

/***************************************************************************
 *      Macro
 ***************************************************************************/
#define NUM_DECODE_FRAMES_PER_TASK (1)

/***************************************************************************
 *      CLASS
 ***************************************************************************/
class CriSmpMvVideoDecodeTask : public VThreadedTask
{
public:
  CriSmpMvVideoDecodeTask();
  ~CriSmpMvVideoDecodeTask();
  virtual void Run(VManagedThread *pThread);
  void StopRequest() { m_bLoopFlag = false; }
  void AddPlayer(CriMvEasyPlayer* pkMvEasy);
  void Update();

private:
  void RemovePlayer(CriMvEasyPlayer* pkMvEasy);

private:
  //std::list<CriMvEasyPlayer*> m_kPlayers;
  VMutex m_kMutex;
  CriUint32 m_uiNumPlayers;
  CriMvEasyPlayer** m_kPlayers;
  volatile CriBool m_bLoopFlag;
};


#endif	/* CRISMPMVVIDEODECODETASK_H_INCLUDED */

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
