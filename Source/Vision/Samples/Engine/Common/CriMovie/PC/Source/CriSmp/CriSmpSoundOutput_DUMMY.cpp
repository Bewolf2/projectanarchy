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
 * Copyright (c) 2006-2009 CRI Middleware Co., Ltd.
 *
 * Library  : Sample Library
 * Module   : Sound Output for PC without sound device
 * File     : CriSmpSoundOuput_DUMMY.cpp
 * Date     : 2009-04-09
 * Version  : 1.02
 *
 ****************************************************************************/

/***************************************************************************
 *      Include files
 ***************************************************************************/

#include <windows.h>
#include <process.h>

#include "CriSmpSoundOutput.h"
#include "CriSmpSoundOutput_DUMMY.h"

/***************************************************************************
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      Macro Functions
 ***************************************************************************/
#define UNUSED(arg)					{ if ((arg) == (arg)) {} }

/***************************************************************************
 *      Data Type Declarations
 ***************************************************************************/

/***************************************************************************
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      Function Definition
 ***************************************************************************/
/****
*			Functions of Sound Output
***/
CriSmpSoundOutputDummy::CriSmpSoundOutputDummy(void)
{
	this->stat = STOP;
	this->func = CRI_NULL;
	this->obj = CRI_NULL;
	this->frequency = SAMPLING_RATE;
	this->remain_samples = 0;
	ZeroMemory(&this->temp_buffer, sizeof(this->temp_buffer));
	ZeroMemory(&this->cs, sizeof(this->cs));
	this->cs_initialized = CRI_FALSE;
	this->hevent = NULL;
	this->hthread = NULL;
	this->timer_id = NULL;
	this->thread_exit_flag = CRI_FALSE;
}

CriSmpSoundOutputDummy::~CriSmpSoundOutputDummy(void)
{
}

CriSmpSoundOutputDummy* CriSmpSoundOutputDummy::Create(void)
{
	// Allocate Sound Output Object
	CriSmpSoundOutputDummy* sndout = new CriSmpSoundOutputDummy();
	if (sndout == NULL) {
		return (CRI_NULL);
	}
	
	// Create a critical section for synchronization
	InitializeCriticalSection(&sndout->cs);
	sndout->cs_initialized = CRI_TRUE;
	
	// Create an event object for periodical processing
	sndout->hevent = CreateEvent(NULL, FALSE, FALSE, NULL);	// auto reset event
	if (sndout->hevent == NULL) {
		sndout->Destroy();
		return (CRI_NULL);
	}
	
	// Create a timer for waking up the thread periodically
	sndout->timer_id = timeSetEvent(TIMER_INTERVAL, 0, (LPTIMECALLBACK)sndout->hevent,
		(DWORD)NULL, TIME_PERIODIC | TIME_CALLBACK_EVENT_PULSE);
	if (sndout->timer_id == NULL) {
		sndout->Destroy();
		return (CRI_NULL);
	}
	
	// Create thread for audio processing
	sndout->hthread = (HANDLE)_beginthreadex(NULL, 0, SoundOutputTimerProc,
		sndout, CREATE_SUSPENDED, NULL);
	if (sndout->hthread == NULL) {
		sndout->Destroy();
		return (CRI_NULL);
	}
	
	// Change the priority of the thread to THREAD_PRIORITY_HIGHEST
	SetThreadPriority(sndout->hthread, THREAD_PRIORITY_HIGHEST);
	
	// Get current time
	sndout->prev = timeGetTime();
	
	// Start thread processing
	ResumeThread(sndout->hthread);
	
	return (sndout);
}

void CriSmpSoundOutputDummy::Destroy(void)
{
	if (this->hthread != NULL) {
		// Wait until thread processing ends
		this->thread_exit_flag = CRI_TRUE;
		WaitForSingleObject(this->hthread, INFINITE);
		
		// Release of the thread handle
		CloseHandle(this->hthread);
		this->hthread = NULL;
	}
	
	if (this->timer_id != NULL) {
		// Kill timer event
		timeKillEvent(this->timer_id);
		this->timer_id = NULL;
	}
	
	if (this->hevent != NULL) {
		// Release the event handle
		CloseHandle(this->hevent);
		this->hevent = NULL;
	}
	
	// Delete the critical section
	if (this->cs_initialized != CRI_FALSE) {
		DeleteCriticalSection(&this->cs);
		this->cs_initialized = CRI_FALSE;
	}
	
	// Delete the Sound Output Object
	delete this;
	
	return;
}

void CriSmpSoundOutputDummy::Start(void)
{
	EnterCriticalSection(&this->cs);
	
	// Update status
	this->stat = EXEC;
	
	LeaveCriticalSection(&this->cs);

	return;
}

void CriSmpSoundOutputDummy::Stop(void)
{
	EnterCriticalSection(&this->cs);
	
	// Update status
	this->stat = STOP;
	
	this->remain_samples = 0;

	LeaveCriticalSection(&this->cs);
	
	return;
}

void CriSmpSoundOutputDummy::SetNotifyCallback(CriUint32 (*func)(void *obj, CriUint32 nch, CriFloat32 *sample[], CriUint32 nsmpl), void *obj)
{
	EnterCriticalSection(&this->cs);
	
	// Register callback
	this->func = func;
	this->obj  = obj;
	
	LeaveCriticalSection(&this->cs);
	
	return;
}

void CriSmpSoundOutputDummy::ExecuteMain(void)
{
	return;
}

void CriSmpSoundOutputDummy::SetFrequency(CriUint32 frequency)
{
	EnterCriticalSection(&this->cs);
	
	this->frequency = frequency;
	
	LeaveCriticalSection(&this->cs);
	
	return;
}

void CriSmpSoundOutputDummy::ExecuteAudioFrame(void)
{
	CriSint32 i;
	DWORD cur, diff;
	CriUint32 nsmpl;
	CriFloat32 *sample[8];
	
	EnterCriticalSection(&this->cs);
	
	// Calculate elapsed time since previous audio frame
	cur = timeGetTime();
	if (cur >= this->prev) {
		diff = cur - this->prev;
	} else {
		diff = (0xFFFFFFFF - this->prev) + cur;
	}
	
	// Calculate the number of samples to be removed
	nsmpl = (this->frequency * diff) / 1000;
	nsmpl += this->remain_samples;
	if (nsmpl > BUFFER_SAMPLES) {
		nsmpl = BUFFER_SAMPLES;
	}
	
	// Round down the number of samples
	Uint32 tmp_nsmpl = nsmpl;
	nsmpl = (nsmpl / NSMPL_BLK) * NSMPL_BLK;
	this->remain_samples = tmp_nsmpl - nsmpl;	// number of samples rounded down

	// Remove samples
	for (i = 0; i < NUM_CHANNELS; i++) {
		sample[i] = this->temp_buffer[i];
	}
	if ((this->func != CRI_NULL) && (this->stat == EXEC)) {
		(this->func)(this->obj, NUM_CHANNELS, sample, nsmpl);
	}
	
	// Update timer
	this->prev = cur;
	
	LeaveCriticalSection(&this->cs);
	
	return;
}

unsigned WINAPI CriSmpSoundOutputDummy::SoundOutputTimerProc(LPVOID param)
{
	CriSmpSoundOutputDummy *sndout = reinterpret_cast<CriSmpSoundOutputDummy *>(param);
	
	while (sndout->thread_exit_flag == CRI_FALSE) {
		// Sleep until audio frame
		WaitForSingleObject(sndout->hevent, INFINITE);
		
		// Execute sound output processing
		sndout->ExecuteAudioFrame();
	}
	
	return (0);
}

/* --- end of file --- */

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
