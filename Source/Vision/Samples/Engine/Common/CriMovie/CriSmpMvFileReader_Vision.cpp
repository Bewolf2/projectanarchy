/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/****************************************************************************
 *
 * Copyright (c) 2008-2009 CRI Middleware Co, Inc.
 *
 * Created  Date : 2008-03-12
 * Modified Date : 2009-04-24
 *
 ****************************************************************************/

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/CriMovie/CriSmpMvFileReader_Vision.h>

/***************************************************************************
 *      MACRO CONSTANT
 ***************************************************************************/

/***************************************************************************
 *      Process MACRO
 ***************************************************************************/
#define CRISMPMV_NOP(arg)	{ if((arg) == (arg)){}}	


/***************************************************************************
 *      Function Declaration
 ***************************************************************************/

/***************************************************************************
 *      Variable Definition
 ***************************************************************************/
CriUint8 CriSmpMvFileReaderVision::ms_uiRefCounter = 0;
COMMANDQUEUE* CriSmpMvFileReaderVision::ms_pkCmdQueue = NULL;
VMutex* CriSmpMvFileReaderVision::ms_pkMutex = NULL;
CriSmpMvFileReaderVision::FileIoThreadedTask *CriSmpMvFileReaderVision::ms_pkIoThread = NULL;

/***************************************************************************
 *      Static Function Definition
 ***************************************************************************/
void CriSmpMvFileReaderVision::Init()
{
	ms_pkCmdQueue = new COMMANDQUEUE();
	ms_pkMutex = new VMutex();

	/* Create I/O thread */
	ms_pkIoThread = new FileIoThreadedTask();
	Vision::GetThreadManager()->ScheduleTask(ms_pkIoThread, 0);
}

void CriSmpMvFileReaderVision::DeInit()
{
	/* Destroy I/O thread */
	ms_pkIoThread->Stop();
	Vision::GetThreadManager()->WaitForTask(ms_pkIoThread, false);
	V_SAFE_DELETE(ms_pkIoThread);

	V_SAFE_DELETE(ms_pkMutex);
	V_SAFE_DELETE(ms_pkCmdQueue);
}

/***************************************************************************
 *      Function Definition
 ***************************************************************************/

/* 
 * File I/O thread to execute actual Open/Read/Close functions
 */
void CriSmpMvFileReaderVision::FileIoThreadedTask::Run(VManagedThread * /* pThread */)
{
	while( m_bLoopFlag == true ) {
		while ( !CriSmpMvFileReaderVision::ms_pkCmdQueue->empty() ) {

			ms_pkMutex->Lock();
			CRISMP_FILEIO_COMMAND kCmd = CriSmpMvFileReaderVision::ms_pkCmdQueue->front();
			CriSmpMvFileReaderVision::ms_pkCmdQueue->pop();
			ms_pkMutex->Unlock();

			switch(kCmd.eCommandId) {

				case FILEIO_COMMAND_OPEN:
					kCmd.pkFreader->ExecuteOpen(kCmd.acFname);
					break;
				case FILEIO_COMMAND_READ:
					kCmd.pkFreader->ExecuteRead(kCmd.pBuffer,kCmd.siReqSize);
					break;
				case FILEIO_COMMAND_CLOSE:
				default:
					kCmd.pkFreader->ExecuteClose();
					break;
			}
		}

		// Will occupy this thread until called Stop().
		// You need to keep this task alive while playing movie.
		Sleep(1);
	}

	return;
}


/* 
 * Called from Vision Engine on lost device. 
 */
void CriSmpMvFileReaderVision::FileIoThreadedTask::OnCancel()
{
	// Stop the file reading task once since Vision Eingine will wait for all the task is finished.
	Stop();
}


CriSmpMvFileReaderVision *CriSmpMvFileReaderVision::Create(CriHeap pkHeap, IVFileStreamManager *pkFmnger)
{
	CriSmpMvFileReaderVision *freader;

	if (ms_uiRefCounter == 0) {
		/* Create a file I/O task */
		Init();
	}
	ms_uiRefCounter++;

	/* Create myself */
	freader = new (pkHeap, "CriSmpMvFileReaderVision", 4) CriSmpMvFileReaderVision;
	freader->m_pkHeap = pkHeap;

	/* Set default parameters */
	freader->m_siFileSize		= -1;
	freader->m_uiTotalReadSize		= 0;
	freader->m_acFname            = NULL;

	/* Set default status */
	freader->m_eOpenStat  = CriMvFileReaderInterface::ASYNC_STATUS_STOP;
	freader->m_eReadStat  = CriMvFileReaderInterface::ASYNC_STATUS_STOP;
	freader->m_eCloseStat = CriMvFileReaderInterface::ASYNC_STATUS_STOP;

	/* Set up file system from Vision */
	freader->m_pkFmnger  = pkFmnger;
	freader->m_pkFstream = NULL;

	return freader;
}


void CriSmpMvFileReaderVision::Destroy(void)
{
	this->m_pkHeap = NULL;
	/* Destroy myself */
	delete this;

	ms_uiRefCounter--;
	if (ms_uiRefCounter == 0) {
		/* Destroy the file I/O task */
		DeInit();
	}

	return;
}


void CriSmpMvFileReaderVision::Open(CriChar8 *acFname)
{
	/* start processing and reset other status just in case. */
	m_eOpenStat = CriMvFileReaderInterface::ASYNC_STATUS_BUSY;
	m_eReadStat  = CriMvFileReaderInterface::ASYNC_STATUS_STOP;
	m_eCloseStat = CriMvFileReaderInterface::ASYNC_STATUS_STOP;

	this->m_uiLastReadSize = 0;

	CRISMP_FILEIO_COMMAND kCmd;
	kCmd.eCommandId = FILEIO_COMMAND_OPEN;
	kCmd.pkFreader  = this;
	kCmd.acFname    = acFname;

	/* Push new OPEN command into the queue */
	ms_pkMutex->Lock();
	ms_pkCmdQueue->push(kCmd);
	ms_pkMutex->Unlock();

	return;
}


void CriSmpMvFileReaderVision::Read(CriUint8 *pBuffer, CriSint64 siReqSize)
{
	/* start processing and reset other status just in case. */
	m_eReadStat = CriMvFileReaderInterface::ASYNC_STATUS_BUSY;
	m_eOpenStat  = CriMvFileReaderInterface::ASYNC_STATUS_STOP;
	m_eCloseStat  = CriMvFileReaderInterface::ASYNC_STATUS_STOP;

	CRISMP_FILEIO_COMMAND kCmd;
	kCmd.eCommandId = FILEIO_COMMAND_READ;
	kCmd.pkFreader  = this;
	kCmd.pBuffer   = pBuffer;
	kCmd.siReqSize = siReqSize;

	/* Push new OPEN command into the queue */
	ms_pkMutex->Lock();
	ms_pkCmdQueue->push(kCmd);
	ms_pkMutex->Unlock();

	return;
}


void CriSmpMvFileReaderVision::Close(void)
{
	/* start processing and reset other status just in case. */
	m_eCloseStat = CriMvFileReaderInterface::ASYNC_STATUS_BUSY;
	m_eOpenStat  = CriMvFileReaderInterface::ASYNC_STATUS_STOP;
	m_eReadStat  = CriMvFileReaderInterface::ASYNC_STATUS_STOP;

	CRISMP_FILEIO_COMMAND kCmd;
	kCmd.eCommandId = FILEIO_COMMAND_CLOSE;
	kCmd.pkFreader  = this;

	/* Push new OPEN command into the queue */
	ms_pkMutex->Lock();
	ms_pkCmdQueue->push(kCmd);
	ms_pkMutex->Unlock();

	return;
}


CriMvFileReaderInterface::AsyncStatus CriSmpMvFileReaderVision::GetOpenStatus(void)
{
	return m_eOpenStat;
}


CriMvFileReaderInterface::AsyncStatus CriSmpMvFileReaderVision::GetReadStatus(void)
{
	return m_eReadStat;
}


CriMvFileReaderInterface::AsyncStatus CriSmpMvFileReaderVision::GetCloseStatus(void)
{
	return m_eCloseStat;
}


CriSint64 CriSmpMvFileReaderVision::GetReadSize(void)
{
	return this->m_uiLastReadSize;
}

CriSint64 CriSmpMvFileReaderVision::Seek(CriSint64 size, CriMvFileReaderInterface::SeekOrigin offset)
{
	LONG   iSeekMode;
	LONG   curPos, newPos;
	LONG   actualSeekVal;
	BOOL   result;

	/* convert argument from CriMvFileReader to CriFs. */
	switch(offset) {
	case CriMvFileReaderInterface::SEEK_FROM_BEGIN:
		iSeekMode = SEEK_SET ;
		break;
	case CriMvFileReaderInterface::SEEK_FROM_CURRENT:
		iSeekMode = SEEK_CUR;;
		break;
	case CriMvFileReaderInterface::SEEK_FROM_END:
		iSeekMode = SEEK_END;
		break;
	default:
		iSeekMode = SEEK_SET ;
		break;
	}
	
	curPos = this->m_pkFstream->GetPos();
	result = this->m_pkFstream->SetPos((LONG)size, iSeekMode);
	newPos = this->m_pkFstream->GetPos();

	/* seek error */
	if( result == 0 )
		return 0;

	actualSeekVal = newPos - curPos;

	return actualSeekVal;
}

CriSint64 CriSmpMvFileReaderVision::GetFileSize(void)
{
	return (CriSint64)this->m_pkFstream->GetSize();
}

void CriSmpMvFileReaderVision::ExecuteOpen(Char8* acFname)
{
	this->m_pkFstream = Vision::File.Open(acFname);

	if (this->m_pkFstream == NULL)
		m_eOpenStat = CriMvFileReaderInterface::ASYNC_STATUS_ERROR;
	else
		m_eOpenStat = CriMvFileReaderInterface::ASYNC_STATUS_COMPLETE;

	return;
}

void CriSmpMvFileReaderVision::ExecuteRead(CriUint8 *pBuffer, CriSint64 siReqSize)
{
	if (this->m_pkFstream == NULL)
		return;

	this->m_uiLastReadSize = this->m_pkFstream->Read(pBuffer, (int)siReqSize);
	this->m_uiTotalReadSize += (DWORD)this->m_uiLastReadSize;

	this->m_eReadStat = CriMvFileReaderInterface::ASYNC_STATUS_COMPLETE;

	return;
}

void CriSmpMvFileReaderVision::ExecuteClose()
{
	if (this->m_pkFstream == NULL) {
		this->m_eCloseStat = CriMvFileReaderInterface::ASYNC_STATUS_ERROR;
	} else {
		this->m_pkFstream->Close();
		this->m_eCloseStat = CriMvFileReaderInterface::ASYNC_STATUS_COMPLETE;
	}

	return;
}

void CriSmpMvFileReaderVision::RestartIoThread(void)
{
	if (ms_pkIoThread->GetState()==TASKSTATE_FINISHED) {
   	  ms_pkIoThread->m_bLoopFlag = true;
	  Vision::GetThreadManager()->ScheduleTask(ms_pkIoThread, 0);
	}
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
