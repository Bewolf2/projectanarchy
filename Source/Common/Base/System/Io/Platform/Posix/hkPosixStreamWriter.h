/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_BASE_POSIXSTREAMWRITER_H
#define HK_BASE_POSIXSTREAMWRITER_H

#include <Common/Base/System/Io/Writer/hkStreamWriter.h>

class hkPosixStreamWriter : public hkStreamWriter
{
	public:

		static hkPosixStreamWriter* open(const char* name, int flags, int mode=0644);

		hkPosixStreamWriter(int handle, hkBool shouldClose);

		void close();
		virtual ~hkPosixStreamWriter();
		virtual int write( const void* buf, int nbytes);
		virtual void flush();
		virtual hkBool isOk() const;
	
		virtual hkBool seekTellSupported() const;
		virtual hkResult seek( int offset, SeekWhence whence);
		virtual int tell() const;

	protected:

		int m_handle;
		hkBool m_shouldClose;
};

typedef hkPosixStreamWriter DefaultFileWriter;

#endif //HK_BASE_POSIXSTREAMWRITER_H

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
