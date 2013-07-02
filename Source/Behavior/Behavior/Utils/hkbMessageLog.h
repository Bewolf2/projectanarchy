/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MESSAGE_LOG_H
#define HKB_MESSAGE_LOG_H

#include <Common/Base/Container/String/hkString.h>

extern const class hkClass hkbMessageLogClass;

// Keeps a record of log messages for later retrieval
class hkbMessageLog
{
	//+version(0)

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbMessageLog );
		HK_DECLARE_REFLECTION();

		/// Construct with defaults.
		hkbMessageLog();

		/// Destructor
		~hkbMessageLog();

		// Interface for adding and querying messages
		int getNumMessages();
		const char* getMessage( int i );
		void addMessage( const char* msg );
		void clearMessages();

		// Set a limit on the number of messages
		void setMaxMessages( int );

	private:

		hkArray< hkStringPtr >* m_messages; //+nosave
		int m_maxMessages; //+nosave

	public:

		hkbMessageLog( hkFinishLoadedObjectFlag flag ) {}
};

#endif // HKB_MESSAGE_LOG_H

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
