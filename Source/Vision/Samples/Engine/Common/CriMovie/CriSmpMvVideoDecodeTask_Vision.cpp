/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/****************************************************************************
 *
 * Copyright (c) 2008-2009 CRI Middleware Co., Ltd.
 *
 * Created  Date : 2008-05-23
 * Modified Date : 2009-04-21
 *
 ****************************************************************************/

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/CriMovie/CriSmpMvVideoDecodeTask_Vision.h>

/***************************************************************************
 *      Function Definition
 ***************************************************************************/
CriSmpMvVideoDecodeTask::CriSmpMvVideoDecodeTask()
{
	m_uiNumPlayers = 0;
	m_kPlayers = (CriMvEasyPlayer**)Vision::System.Malloc(sizeof(CriMvEasyPlayer*) * CriMv::GetMaxNumberOfHandles());
}

CriSmpMvVideoDecodeTask::~CriSmpMvVideoDecodeTask()
{
	V_SAFE_DELETE(m_kPlayers);
}

// Background video decoding
void CriSmpMvVideoDecodeTask::Run(VManagedThread *pThread)
{
	CriMvEasyPlayer *pkPlayer;
	CriBool bWorkFlag = CRI_FALSE;

	for (CriSint16 i=0;i<NUM_DECODE_FRAMES_PER_TASK;i++) {

		for(CriUint32 i=0;i<m_uiNumPlayers;i++) {

			pkPlayer = m_kPlayers[i];

			if (pkPlayer != NULL) {
				// Decode one video frame and audio.
				bWorkFlag = pkPlayer->ExecuteDecode();
			}

			// Decoding is finished
			if (bWorkFlag == CRI_FALSE) {
				RemovePlayer(pkPlayer);
			}
		}

		// Exit the loop when called StopRequest() and CriMvEasyPlayer already finish the tasks t
		if (m_uiNumPlayers == 0 && this->m_bLoopFlag == CRI_FALSE)
			break;

		Sleep(1);
	}
}

void CriSmpMvVideoDecodeTask::AddPlayer(CriMvEasyPlayer* pkMvEasy)
{
	m_kMutex.Lock();
	m_kPlayers[m_uiNumPlayers] = pkMvEasy;
	m_uiNumPlayers++;
	m_kMutex.Unlock();
}

 void CriSmpMvVideoDecodeTask::RemovePlayer(CriMvEasyPlayer* pkMvEasy)
 {
	Uint32 i;

	m_kMutex.Lock();
	for ( i=0; i<m_uiNumPlayers; i++) {
		if (m_kPlayers[i] == pkMvEasy) {
			m_kPlayers[i] = NULL;
			while (i<m_uiNumPlayers) {
				m_kPlayers[i] = m_kPlayers[i+1];
				i++;
			}
			m_kPlayers[i] = NULL;
			m_uiNumPlayers--;
			break;
		}
	}
	m_kMutex.Unlock();
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
