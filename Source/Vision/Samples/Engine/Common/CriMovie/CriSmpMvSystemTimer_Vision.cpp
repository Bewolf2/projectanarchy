/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2006-2007 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Movie Easy Player
 * Module   : Sample System Timer using Vision::GetTimer()->
 *
 ****************************************************************************/

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/CriMovie/CriSmpMvSystemTimer_Vision.h>

/***************************************************************************
 *      MACRO CONSTANT
 ***************************************************************************/

/***************************************************************************
 *      Process MACRO
 ***************************************************************************/
#define CRISMPMV_NOP(arg)	{ if((arg) == (arg)){}}	

/***************************************************************************
 *      Enum declaration
 ***************************************************************************/

/***************************************************************************
 *      Data type declaration
 ***************************************************************************/

/***************************************************************************
 *      Function Declaration
 ***************************************************************************/

/***************************************************************************
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      Function Definition
 ***************************************************************************/
CriSmpMvSystemTimerVision *CriSmpMvSystemTimerVision::Create(CriHeap pkHeap)
{
	CriSmpMvSystemTimerVision *stimer;

	stimer = new(pkHeap, "CriSmpMvSystemTimerVision", 4) CriSmpMvSystemTimerVision;
	stimer->m_pkHeap = pkHeap;
	return stimer;
}


void CriSmpMvSystemTimerVision::Destroy(void)
{
	this->m_pkHeap = NULL;
	delete this;
}


void CriSmpMvSystemTimerVision::Update(void)
{
  if(m_bPauseFlag)
  {
    m_fTime += 0.033f; // 30 FPS
  }
  else
  {
    m_fTime += Vision::GetTimer()->GetTimeDifference();
  }
}


void CriSmpMvSystemTimerVision::Start(void)
{
  m_fTime = 0.0f;
  m_bPauseFlag = false;
}


void CriSmpMvSystemTimerVision::Stop(void)
{
  m_fTime = 0.0f;
  m_bPauseFlag = true;
}


void CriSmpMvSystemTimerVision::Pause(CriBool sw)
{
  if (this->m_bPauseFlag == sw)
  {
		return;
	}
	this->m_bPauseFlag = sw;
}


void CriSmpMvSystemTimerVision::GetTime(CriUint64 &count, CriUint64 &unit)
{
  count = (CriUint64)(m_fTime * 1000.0f);
  unit = 1000;
}

/* end of file */

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
