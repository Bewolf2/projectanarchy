/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_DEFAULT_MESSAGE_LOG_H
#define HKB_DEFAULT_MESSAGE_LOG_H

#include <Behavior/Behavior/Utils/hkbMessageLog.h>

extern const class hkClass hkbDefaultMessageLogClass;

/// Class used for communicating error messages with HAT
class hkbDefaultMessageLog
{
	//+version(0)

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbDefaultMessageLog );
		HK_DECLARE_REFLECTION();

		/// Singleton-like interface.  init() should be called after the
		/// memory manager has been initialized, but before any calls to
		/// getOutputLog() or getErrorLog().  destroy() should be called
		/// before the memory manager has been deinitialized, and after
		/// any calls to getOutputLog() or getErrorLog() will be made.
		static void HK_CALL init();
		static void HK_CALL destroy();

		/// Static functions for accessing the logs
		static void HK_CALL addOutputMessage( const char* );
		static void HK_CALL addErrorMessage( const char* );

		/// Access to the logs
		static hkbMessageLog* HK_CALL getOutputLog();
		static hkbMessageLog* HK_CALL getErrorLog();

	private:

		/// Logs for general output, and errors
		static class hkbMessageLog* m_outputLog;
		static class hkbMessageLog* m_errorLog;

	public:	

		hkbDefaultMessageLog( hkFinishLoadedObjectFlag flag ) {}
};

#endif // HKB_DEFAULT_MESSAGE_LOG_H

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
