/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file CriSmpMvSystemTimer_Vision.h

/****************************************************************************
 *
 * Copyright (c) 2008 CRI Middleware, Inc.
 *
 * File     : CriSmpMvSystemTimer_Vision.h
 * Date     : 2008-11-11
 *
 ****************************************************************************/
#ifndef	CRISMPMVSYSTEMTIMER_VISION_H_INCLUDED		/* Re-definition prevention */
#define	CRISMPMVSYSTEMTIMER_VISION_H_INCLUDED

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <cri_xpt.h>
#include <cri_error.h>
#include <cri_movie.h>

#include <Vision/Runtime/Engine/System/Timer/VisionTimer.hpp> 

/***************************************************************************
 *      CLASS
 ***************************************************************************/
/*------------------------------------------------------------------*/
/* Sample System Timer for CRI Movie Easy Player                    */
/*------------------------------------------------------------------*/
class CriSmpMvSystemTimerVision : public CriMvSystemTimerInterface, public CriAllocator
{
public:
	static CriSmpMvSystemTimerVision* Create(CriHeap pkHeap);
	virtual void Destroy(void);

	virtual void Update(void);
	virtual void Start(void);
	virtual void Stop(void);
	virtual void Pause(CriBool sw);
	virtual void GetTime(CriUint64 &count, CriUint64 &unit);

private:
	CriHeap			m_pkHeap;
	CriBool			m_bPauseFlag;
  float       m_fTime;
};

#endif	/* CRISMPMVSYSTEMTIMER_VISION_H_INCLUDED */

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
