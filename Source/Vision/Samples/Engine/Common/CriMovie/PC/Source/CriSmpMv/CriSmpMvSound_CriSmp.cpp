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
 * Library  : CRI Movie Easy Player
 * Module   : Sample Sound using CriSmpSoundOutput
 * File     : CriSmpMvSound_CriSmp.cpp
 * Date     : 2009-04-09
 * Version  : 
 *
 ****************************************************************************/

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <string.h>

#include <CriSmpMvSound_CriSmp.h>

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
 *      Class Definition
 ***************************************************************************/
class CriSmpMvSoundLoc : public CriSmpMvSound, public CriAllocator
{
public:
	static CriSmpMvSoundLoc* Create(CriHeap heap);
	virtual void Destroy(void);
	virtual Bool CreateOutput(CriHeap heap, Uint32 channel, Uint32 samplerate);
	virtual void DestroyOutput(void);
	virtual CriMvSoundInterface::PcmFormat GetPcmFormat(void);	// Float32
	virtual void SetCallbackGetFloat32PcmData(Uint32 (*func)(void *obj, Uint32 nch, Float32 *pcmbuf[], Uint32 req_nsmpl), void *obj);
	virtual void SetCallbackGetSint16PcmData(Uint32 (*func)(void *obj, Uint32 nch, Sint16 *pcmbuf[], Uint32 req_nsmpl), void *obj);
	virtual void Start(void);
	virtual void Stop(void);
	virtual CriMvSoundInterface::Status GetStatus(void);
	virtual void Pause(Bool sw);
	virtual void GetTime(Uint64 &count, Uint64 &unit);

private:
	static Uint32 CriSmpMvSoundCallback(void *obj, Uint32 nch, Float32 *sample[], Uint32 nsmpl);

private:
	CriHeap				heap;
	CriSmpSoundOutput*	smpsout;
	Uint32				out_channels;
	Uint32 (*usrfunc)(void *obj, Uint32 nch, Float32 *sample[], Uint32 nsmpl);
	void				*usrobj;
	CriMvSoundInterface::Status	stat;
	Bool				pause_flag;
	Uint64				total_got_nsmpls;
	Uint64				samplerate;
};
/***************************************************************************
 *      Function Definition
 ***************************************************************************/
CriSmpMvSoundLoc *CriSmpMvSoundLoc::Create(CriHeap heap)
{
	CriSmpMvSoundLoc *sndout;

	sndout = new(heap, "CriSmpMvSoundLoc", 4) CriSmpMvSoundLoc;
	if (sndout == NULL) {
		return NULL;
	}

	sndout->heap = heap;

	sndout->smpsout = NULL;
	sndout->out_channels = 0;

	return sndout;
}


void CriSmpMvSoundLoc::Destroy(void)
{
	this->heap = NULL;

	delete this;

	return;
}


Bool CriSmpMvSoundLoc::CreateOutput(CriHeap heap, Uint32 channel, Uint32 samplerate)
{
	CRISMPMV_NOP(heap);

	if (channel == 0) {
		return FALSE;
	}

	// NOTE: For PC, if no sound device is available, then CriSmpSoundOutput will return dummy sound module to cosume samples.
	this->smpsout = CriSmpSoundOutput::Create();
	if (this->smpsout == NULL) {
		return FALSE;
	}
	this->out_channels = channel;
	this->smpsout->SetFrequency(samplerate);
	this->samplerate = samplerate;

	return TRUE;
}


void CriSmpMvSoundLoc::DestroyOutput(void)
{
	if (this->smpsout == NULL) {
		return;
	}

	this->smpsout->Destroy();
	this->smpsout = NULL;
	this->out_channels = 0;

	return;
}


CriMvSoundInterface::PcmFormat CriSmpMvSoundLoc::GetPcmFormat(void)
{
	return (CriMvSoundInterface::MVEASY_PCM_FLOAT32);
}


void CriSmpMvSoundLoc::SetCallbackGetFloat32PcmData(Uint32 (*func)(void *obj, Uint32 nch, Float32 *sample[], Uint32 nsmpl), void *obj)
{
	this->usrobj = obj;
	this->usrfunc = func;
	if (this->smpsout != NULL) {
		this->smpsout->SetNotifyCallback(CriSmpMvSoundCallback, this);
	}
	return;
}

void CriSmpMvSoundLoc::SetCallbackGetSint16PcmData(Uint32 (*func)(void *obj, Uint32 nch, Sint16 *sample[], Uint32 nsmpl), void *obj)
{
	CRISMPMV_NOP(func);
	CRISMPMV_NOP(obj);

	/* NO SUPPORT */

	return;
}


void CriSmpMvSoundLoc::Start(void)
{
	if (this->smpsout == NULL) {
		return;
	}

	this->stat = CriMvSoundInterface::MVEASY_SOUND_STATUS_EXEC;
	this->smpsout->Start();
	this->total_got_nsmpls = 0;

	return;
}


void CriSmpMvSoundLoc::Stop(void)
{
	if (this->smpsout == NULL) {
		return;
	}

	this->smpsout->Stop();
	this->smpsout->SetNotifyCallback(NULL, NULL);	// no more CRI Movie Sound Callback
													// Be sure to call this after Stop function.
	this->stat = CriMvSoundInterface::MVEASY_SOUND_STATUS_STOP;

	this->total_got_nsmpls = 0;

	return;
}


CriMvSoundInterface::Status CriSmpMvSoundLoc::GetStatus(void)
{
	return this->stat;
}

void CriSmpMvSoundLoc::Pause(Bool sw)
{
	if (this->smpsout == NULL) {
		return;
	}

	this->pause_flag = sw;

	if (this->pause_flag == ON) {
		this->smpsout->Stop();
	} else {
		this->smpsout->Start();
	}

	return;
}


void CriSmpMvSoundLoc::GetTime(Uint64 &count, Uint64 &unit)
{	
	if ( this->total_got_nsmpls == 0 ) {
		count = 0;
		unit  = 1000;
	} else {
		count = this->total_got_nsmpls * 1000 / this->samplerate;
		unit = 1000;
	}

	return;
}


Uint32 CriSmpMvSoundLoc::CriSmpMvSoundCallback(void *obj, Uint32 nch, Float32 *sample[], Uint32 nsmpl)
{
	CriSmpMvSoundLoc *sndout = (CriSmpMvSoundLoc*)obj;
	Uint32 got_nsmpl = 0;

	if (sndout->out_channels == 1) {
		/* MONO */
		if (sndout->usrfunc != NULL) {
			/* Get PCM data to center channle buffer */
			Uint32 i, center_ch;
			center_ch = 4;
			/* zero clear the other than cencter channel */
			for (i = 0; i < nch; i++) {
				if (i != center_ch) {
					memset(sample[i], 0, nsmpl * sizeof(Float32));
				}
			}
			got_nsmpl = (sndout->usrfunc)(sndout->usrobj, sndout->out_channels, &sample[4], nsmpl);
		}
	} else {
		/* STEREO or 5.1ch */
		if (sndout->usrfunc != NULL) {
			got_nsmpl = (sndout->usrfunc)(sndout->usrobj, nch, sample, nsmpl);
		}
	}

	sndout->total_got_nsmpls += got_nsmpl;

	return got_nsmpl;
}

//	Create a Sound Object
CriSmpMvSound* CriSmpMvSound::Create(CriHeap heap)
{
	return (CriSmpMvSound*)CriSmpMvSoundLoc::Create(heap);
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
