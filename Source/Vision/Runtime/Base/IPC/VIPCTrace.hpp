/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef _TRACE_H_
#define _TRACE_H_

#define TRACE_BUFFER_LENGTH 1024

#if 0
#define TRACE0(msg) \
  { \
    char __trace_buffer[TRACE_BUFFER_LENGTH]; \
    _snprintf_s(__trace_buffer, TRACE_BUFFER_LENGTH, "%s(%d): TRACE: %s\r\n", __FILE__, __LINE__, msg); \
    OutputDebugStringA(__trace_buffer); \
  }

#define TRACE1(msg, _1) \
  { \
    char __msg_buffer[TRACE_BUFFER_LENGTH]; \
    _snprintf_s(__msg_buffer, TRACE_BUFFER_LENGTH, msg, _1); \
    TRACE0(__msg_buffer) \
  }

#define TRACE2(msg, _1, _2) \
  { \
    char __msg_buffer[TRACE_BUFFER_LENGTH]; \
    _snprintf_s(__msg_buffer, TRACE_BUFFER_LENGTH, msg, _1, _2); \
    TRACE0(__msg_buffer) \
  }

#else
#define TRACE0(msg)
#define TRACE1(msg, _1)
#define TRACE2(msg, _1, _2)
#endif

#endif

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
