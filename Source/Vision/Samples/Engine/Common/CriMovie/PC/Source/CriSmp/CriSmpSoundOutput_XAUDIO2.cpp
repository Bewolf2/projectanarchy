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
 * Copyright (c) 2008-2009 CRI Middleware Co., Ltd.
 *
 * Library  : Sample Library
 * Module   : Sound Output using XAudio2
 * File     : CriSmpSoundOuput_XAUDIO2.cpp
 * Date     : 2009-07-29
 * Version  : 1.07
 *
 ****************************************************************************/

/***************************************************************************
 *      Include files
 ***************************************************************************/
#include "cri_xpt.h"
#include "CriSmpSoundOutput.h"
#include "CriSmpSoundOutput_XAUDIO2.h"

#if defined(_XBOX)
#include <xtl.h>
#else
#define _WIN32_DCOM
#include <windows.h>
#include <process.h>
#include "CriSmpSoundOutput_DUMMY.h"
#endif

#if !defined(_XBOX)
#define INITGUID
#endif

#include <XAudio2.h>

/***************************************************************************
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      Macro Functions
 ***************************************************************************/

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
LONG CriSmpSoundOutputXAudio2::s_init_count = 0;
IXAudio2* CriSmpSoundOutputXAudio2::s_xa2 = NULL;
IXAudio2MasteringVoice* CriSmpSoundOutputXAudio2::s_mastering_voice = NULL;
XAUDIO2_VOICE_DETAILS CriSmpSoundOutputXAudio2::s_mv_details = { 0, };
const float CriSmpSoundOutputXAudio2::DOWNMIX_MATRIX[NUM_CHANNELS * 2] = {
	1.0f,									/* L   -> Lt */
	0.0f,									/* R   -> Lt */
	0.70710678118654752440084436210485f,	/* C   -> Lt */
	0.0f,									/* LFE -> Lt */
	0.70710678118654752440084436210485f,	/* Ls  -> Lt */
	0.0f,									/* Rs  -> Lt */
	0.0f,									/* L   -> Rt */
	1.0f,									/* R   -> Rt */
	0.70710678118654752440084436210485f,	/* C   -> Rt */
	0.0f,									/* LFE -> Rt */
	0.0f,									/* Ls  -> Rt */
	0.70710678118654752440084436210485f,	/* Rs  -> Rt */
};

/***************************************************************************
 *      Function Definition
 ***************************************************************************/
/****
*			Functions of Sound Output
***/
CriSmpSoundOutputXAudio2::CriSmpSoundOutputXAudio2(void)
{
	this->stat = STOP;
	this->func = CRI_NULL;
	this->obj = CRI_NULL;
	this->m_source_voice = NULL;
	ZeroMemory(&this->temp_buffer, sizeof(this->temp_buffer));
	ZeroMemory(&this->sound_buffer, sizeof(this->sound_buffer));
	this->write_position = 0;
	ZeroMemory(&this->cs, sizeof(this->cs));
	this->cs_initialized = CRI_FALSE;
	this->hthread = NULL;
	this->thread_exit_flag = CRI_FALSE;
}

CriSmpSoundOutputXAudio2::~CriSmpSoundOutputXAudio2(void)
{
}

CriBool CriSmpSoundOutputXAudio2::Initialize(void)
{
	HRESULT hr;
	UINT32 flags;
	LONG ret;
	
	// Update the initialization counter
	ret = InterlockedIncrement(&s_init_count);
	if (ret != 1) {
		// Already Initialized
		return (CRI_TRUE);
	}
	
#if !defined(_XBOX)
	// Initialization of COM
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	
	// Selection of initialization method
	// If debugging, use XAUDIO2_DEBUG_ENGINE
#if defined(_DEBUG) && defined(_MSC_VER) && ( (_MSC_VER < 1700) || defined(_VISION_DIRECTX_2010) )
	flags = XAUDIO2_DEBUG_ENGINE;
#else
	flags = 0;
#endif
	
	// Creation of XAudio2 object
	hr = XAudio2Create(&s_xa2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		CriSmpSoundOutputXAudio2::Finalize();
		return (CRI_FALSE);
	}
	
	// Creation of mastering voice
	hr = s_xa2->CreateMasteringVoice(&s_mastering_voice,
		XAUDIO2_DEFAULT_CHANNELS, SAMPLING_RATE, 0, 0, NULL);
	if (FAILED(hr)) {
		CriSmpSoundOutputXAudio2::Finalize();
		return (CRI_FALSE);
	}
	
	// Get details of the mastering voice
	s_mastering_voice->GetVoiceDetails(&s_mv_details);
	
	return (CRI_TRUE);
}

void CriSmpSoundOutputXAudio2::Finalize(void)
{
	LONG ret;
	
	// Update the initialization counter
	ret = InterlockedDecrement(&s_init_count);
	if (ret != 0) {
		// Some module still uses the sound output
		return;
	}
	
	// Release of the mastering voice
	if (s_mastering_voice != NULL) {
		s_mastering_voice->DestroyVoice();
		s_mastering_voice = NULL;
	}
	
	// Release of the XAudio2 object
	if (s_xa2 != NULL) {
		s_xa2->Release();
		s_xa2 = NULL;
	}
	
#if !defined(_XBOX)
	// Uninitialization of COM
	CoUninitialize();
#endif
	
	return;
}

CriBool CriSmpSoundOutputXAudio2::IsInitialized(void)
{
	if (s_init_count > 0) {
		return (CRI_TRUE);
	} else {
		return (CRI_FALSE);
	}
}

IXAudio2* CriSmpSoundOutputXAudio2::GetXAudio2(void)
{
	return (s_xa2);
}

IXAudio2MasteringVoice* CriSmpSoundOutputXAudio2::GetXAudio2MasteringVoice(void)
{
	return (s_mastering_voice);
}

CriSmpSoundOutputXAudio2* CriSmpSoundOutputXAudio2::Create(void)
{
	HRESULT hr;
	WAVEFORMATEXTENSIBLE wfext;
	WAVEFORMATEX *pwfex;
	CriSmpSoundOutputXAudio2* sndout;
	
	// Check whether the initialization has been completed
	if (s_init_count == 0) {
		// if not initialized
		return (CRI_NULL);
	}
	
	// Allocate Sound Output Object
	sndout = new CriSmpSoundOutputXAudio2();
	if (sndout == NULL) {
		return (CRI_NULL);
	}
	
	// Create a critical section for synchronization
	InitializeCriticalSection(&sndout->cs);
	sndout->cs_initialized = CRI_TRUE;

	// Clear WAVEFORMATEXTENSIBLE structure
	ZeroMemory(&wfext, sizeof(wfext));
	pwfex = &wfext.Format;

	// Configure WAVEFORMATEX structure
	pwfex->wFormatTag = WAVE_FORMAT_EXTENSIBLE ;
	pwfex->nChannels = NUM_CHANNELS;
	pwfex->wBitsPerSample = SAMPLE_SIZE * 8;
	pwfex->nSamplesPerSec = SAMPLING_RATE;
	pwfex->nBlockAlign = (pwfex->wBitsPerSample / 8) * pwfex->nChannels;
	pwfex->nAvgBytesPerSec = pwfex->nBlockAlign * pwfex->nSamplesPerSec;
	pwfex->cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

	// Set params of WAVEFORMATEXTENSIBLE struct
	wfext.Samples.wValidBitsPerSample =  pwfex->wBitsPerSample;
	wfext.dwChannelMask = SPEAKER_CONFIG;
	wfext.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

	// Creation of source voice
	hr = s_xa2->CreateSourceVoice(
		&sndout->m_source_voice, ( WAVEFORMATEX* )&wfext, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL);
	if (FAILED(hr)) {
		sndout->Destroy();
		return (CRI_NULL);
	}
	
	// If output device is stereo, downmix the output of source voice
	if (s_mv_details.InputChannels == 2) {
		sndout->m_source_voice->SetOutputMatrix(
			s_mastering_voice, NUM_CHANNELS, 2, DOWNMIX_MATRIX, XAUDIO2_COMMIT_NOW);
	}
	
	// Creation of thread
#if defined(_XBOX)
	sndout->hthread = CreateThread(
		NULL, 0, AudioProc, sndout, CREATE_SUSPENDED, NULL);
#else
	sndout->hthread = (HANDLE)_beginthreadex(
		NULL, 0, AudioProc, sndout, CREATE_SUSPENDED, NULL);
#endif
	if (sndout->hthread == NULL) {
		sndout->m_source_voice->DestroyVoice();
		sndout->Destroy();
		return (CRI_NULL);
	}
	
	// Change the priority of the thread to THREAD_PRIORITY_HIGHEST
	SetThreadPriority(sndout->hthread, THREAD_PRIORITY_HIGHEST);
	
#if defined(_XBOX)
	// If needed, change the processor of the thread
	XSetThreadProcessor(sndout->hthread, 5);
#endif
	
	// Start thread processing
	ResumeThread(sndout->hthread);
	
	// Call ExecuteAudioFrame function to invoke callback
	sndout->ExecuteAudioFrame();
	
	// Start playback
	sndout->m_source_voice->Start(0, 0);
	
	return (sndout);
}

void CriSmpSoundOutputXAudio2::Destroy(void)
{
	// Release of the thread
	if (hthread) {
		// Wait until thread processing ends
		this->thread_exit_flag = CRI_TRUE;
		WaitForSingleObject(hthread, INFINITE);
		
		// Release of the thread handle
		CloseHandle(hthread);
		hthread = NULL;
	}
	
	// Release of the source voice
	if (this->m_source_voice != NULL) {
		this->m_source_voice->Stop(0);
		this->m_source_voice->FlushSourceBuffers();
		this->m_source_voice->DestroyVoice();
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

void CriSmpSoundOutputXAudio2::Start(void)
{
	EnterCriticalSection(&this->cs);
	
	// Update status
	this->stat = EXEC;
	
	LeaveCriticalSection(&this->cs);
	
	return;
}

void CriSmpSoundOutputXAudio2::Stop(void)
{
	EnterCriticalSection(&this->cs);
	
	// Update status
	this->stat = STOP;
	
	LeaveCriticalSection(&this->cs);
	
	return;
}

void CriSmpSoundOutputXAudio2::SetNotifyCallback(CriUint32 (*func)(void *obj, CriUint32 nch, CriFloat32 *sample[], CriUint32 nsmpl), void *obj)
{
	EnterCriticalSection(&this->cs);
	
	// Register callback
	this->func = func;
	this->obj  = obj;
	
	LeaveCriticalSection(&this->cs);
	
	return;
}

void CriSmpSoundOutputXAudio2::ExecuteMain(void)
{
	return;
}

void CriSmpSoundOutputXAudio2::SetFrequency(CriUint32 frequency)
{
	m_source_voice->SetFrequencyRatio((float)frequency / SAMPLING_RATE);
	
	return;
}

void CriSmpSoundOutputXAudio2::ExecuteAudioFrame(void)
{
	CriUint32 i, j, n;
	CriFloat32 *sample[NUM_CHANNELS];
	CriUint32 wsmpl;
	
	// Transfer PCM data to source voice
	
	EnterCriticalSection(&this->cs);
	
	// Get PCM data
	for (i = 0; i < NUM_CHANNELS; i++) {
		sample[i] = this->temp_buffer[i];
	}
	if ((this->func != CRI_NULL) && (this->stat == EXEC)) {
		wsmpl = (this->func)(this->obj, NUM_CHANNELS, sample, SAMPLES_PER_PACKET);
	} else {
		ZeroMemory(this->temp_buffer, sizeof(this->temp_buffer));
		wsmpl = SAMPLES_PER_PACKET;
	}
	
	// Interleaving
	CriFloat32 *pcmbuf = this->sound_buffer[this->write_position];
	for (j = 0; j < wsmpl; j++) {
		static CriUint32 ch_map[8]={ 0, 1, 4, 5, 2, 3, 6, 7 };
		for (n = 0; n < NUM_CHANNELS; n++) {
			pcmbuf[j * NUM_CHANNELS + n] = sample[ch_map[n]][j];
		}
	}
	
	// Submit PCM buffer
	if (wsmpl > 0) {
		XAUDIO2_BUFFER buf = {0};
		buf.AudioBytes = wsmpl * NUM_CHANNELS * SAMPLE_SIZE;
		buf.pAudioData = (BYTE *)pcmbuf;
		m_source_voice->SubmitSourceBuffer(&buf);
	}

	// Update write position
	this->write_position++;
	if(this->write_position >= MAX_PACKETS) {
		this->write_position = 0;
	}
	
	LeaveCriticalSection(&this->cs);
	
	return;
}

// Audio processing thread
// This thread is waked up from XAudio2 callback
#if defined(_XBOX)
DWORD WINAPI CriSmpSoundOutputXAudio2::AudioProc(void *arg)
#else
unsigned WINAPI CriSmpSoundOutputXAudio2::AudioProc(void *arg)
#endif
{
	CriSmpSoundOutputXAudio2 *sndout = (CriSmpSoundOutputXAudio2*)arg;
	
	while (sndout->thread_exit_flag == CRI_FALSE) {
		// Transfer audio data
		// Send all available packets
		XAUDIO2_VOICE_STATE state;
		sndout->m_source_voice->GetState(&state);
		for (int i = state.BuffersQueued; i < MAX_PACKETS; i++) {
			// Execute sound output processing
			sndout->ExecuteAudioFrame();
		}
		
		// Sleep until next audio frame
		Sleep(AUDIO_FRAME_INTERVAL);
	}
	
	return (0);
}

/****
*			Interface of CriSmpSoundOutput
*		   (Internally call CriSmpSoundOutputXAudio2 functions)
***/
CriBool CriSmpSoundOutput::Initialize(void)
{
	CriBool bl_ret;
	
	/* Initialize XAudio2 */
#if defined(_XBOX)
	bl_ret = CriSmpSoundOutputXAudio2::Initialize();
#else
	/* Initialize function always returns CRI_TRUE on PC.	*/
	/* Even if XAudio2 is not available,					*/
	/* it is possible to create CriSmpSoundOutputDummy.		*/
	CriSmpSoundOutputXAudio2::Initialize();
	bl_ret = CRI_TRUE;
#endif
	
	return (bl_ret);
}

void CriSmpSoundOutput::Finalize(void)
{
	CriBool bl_ret;
	
	/* Check whether the XAudio2 is initialized */
	bl_ret = CriSmpSoundOutputXAudio2::IsInitialized();
	if (bl_ret != CRI_FALSE) {
	CriSmpSoundOutputXAudio2::Finalize();
	}
	
	return;
}

CriSmpSoundOutput* CriSmpSoundOutput::Create(void)
{
	CriSmpSoundOutput *sndout;
	
	sndout = CriSmpSoundOutputXAudio2::Create();
	
#if !defined(_XBOX)
	/* On PC, if there are no sound devices,							*/
	/* CriSmpSoundOutputXAudio2::Create function returns CRI_NULL.		*/
	/* If you need to support PC without sound device,					*/
	/* create CriSmpSoundOuptputDummy when the function failed.			*/
	if (sndout == CRI_NULL) {
		sndout = CriSmpSoundOutputDummy::Create();
	}
#endif
	
	return (sndout);
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
