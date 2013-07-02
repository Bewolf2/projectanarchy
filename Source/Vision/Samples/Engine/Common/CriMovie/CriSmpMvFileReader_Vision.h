/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file CriSmpMvFileReader_Vision.h

/****************************************************************************
 * Copyright (c) 2008-2009 CRI Middleware, Inc.
 *
 * File     : CriSmpMvFileReader_Vision.h
 * Date     : 2009-04-21
 *
 ****************************************************************************/
#ifndef	CRISMPMVFILEREADER_VISION_H_INCLUDED		/* Re-definition prevention */
#define	CRISMPMVFILEREADER_VISION_H_INCLUDED

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <cri_movie.h>

#include <queue>		// Using STL for command queue

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/IVFileStream.hpp> 

class CriSmpMvFileReaderVision;

/***************************************************************************
 *      Data type declaration
 ***************************************************************************/
enum CRISMP_FILEIO_COMMAND_ID {
	FILEIO_COMMAND_OPEN,
	FILEIO_COMMAND_READ,
	FILEIO_COMMAND_CLOSE,
};

struct CRISMP_FILEIO_COMMAND {
	CRISMP_FILEIO_COMMAND_ID eCommandId;
	CriSmpMvFileReaderVision *pkFreader;
	Char8* acFname;
	Uint8* pBuffer;
	Sint64 siReqSize;
};	

typedef std::queue< CRISMP_FILEIO_COMMAND > COMMANDQUEUE;

/***************************************************************************
 *      CLASS
 ***************************************************************************/
/*------------------------------------------------------------------*/
/* Sample File Reader for CRI Movie Easy Player                     */
/*------------------------------------------------------------------*/
class CriSmpMvFileReaderVision : public CriMvFileReaderInterface, public CriAllocator
{
private:
 // File IO thread
	class FileIoThreadedTask : public VThreadedTask
	{
	public:
	  inline FileIoThreadedTask() {
		  this->m_bLoopFlag = true;
	  }
	  virtual void Run(VManagedThread *pThread);
	  virtual void OnCancel();
	  void Stop() { m_bLoopFlag = false; };

	  volatile bool m_bLoopFlag;
	};

public:
	static CriSmpMvFileReaderVision* Create(CriHeap pkHeap, IVFileStreamManager *pkFmnger);
	virtual void Destroy(void);

 // CRI Movie Interfaces for file reading
	virtual void Open(CriChar8 *acFname);
	virtual void Read(CriUint8 *pBuffer, CriSint64 siReqSize);
	virtual void Close(void);
	virtual CriMvFileReaderInterface::AsyncStatus GetOpenStatus(void);
	virtual CriMvFileReaderInterface::AsyncStatus GetCloseStatus(void);
	virtual CriMvFileReaderInterface::AsyncStatus GetReadStatus(void);
	virtual CriSint64 GetReadSize(void);
	virtual CriSint64 Seek(CriSint64 size, CriMvFileReaderInterface::SeekOrigin offset);
	virtual CriSint64 GetFileSize(void);

 // Actual File I/O
	void ExecuteOpen(Char8* acFname);
	void ExecuteClose();
	void ExecuteRead(CriUint8 *pBuffer, CriSint64 siReqSize);

	static void RestartIoThread(void);

private:
	CriHeap   m_pkHeap;
	CriSint64 m_siFileSize;
	size_t    m_uiTotalReadSize;
	size_t    m_uiLastReadSize;

	CriMvFileReaderInterface::AsyncStatus m_eOpenStat;
	CriMvFileReaderInterface::AsyncStatus m_eCloseStat;
	CriMvFileReaderInterface::AsyncStatus m_eReadStat;

private:
	CriChar8 *m_acFname;
	CriUint8 *m_pBuffer;
	CriSint64 m_siReqSize;

	IVFileStreamManager *m_pkFmnger;
	IVFileInStream      *m_pkFstream;

	static CriUint8 ms_uiRefCounter;
	static COMMANDQUEUE* ms_pkCmdQueue;
	static VMutex* ms_pkMutex;
	static FileIoThreadedTask *ms_pkIoThread;

private:
	static void Init();
	static void DeInit();
};



#endif	/* CRISMPMVFILEREADER_VISION_H_INCLUDED */

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
