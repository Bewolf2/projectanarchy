/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma managed(push, off)

#include <Common/Base/hkBase.h>

// Vision has a lot of warnings at the moment. We need to disable them..
	#pragma warning(disable: 4100) // unused params
	#pragma warning(disable: 4244) // int to BYTE etc
	#pragma warning(disable: 4512) // assignment op could not be generated
	#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
	#pragma warning(disable: 4127) // conditional expression is constant (VASSERT(FALSE))
	#pragma warning(disable: 4389) // signed/unsigned
	#pragma warning(disable: 4245) // conversion from 'int' to 'unsigned int', signed/unsigned mismatch
	#pragma warning(disable: 4701) // potentially uninitialized local variable 'blah' used
	#pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
	#pragma warning(disable: 4239) // nonstandard extension used : 'argument' : conversion from 'int' to 'void *volatile const &'
	#pragma warning(disable: 4189) // local variable is initialized but not referenced
	#pragma warning(disable: 4130) // VASSERT( "str" == NULL) : '==' : logical operation on address of string constant
	#pragma warning(disable: 4706) // assignment within conditional expression
	#pragma warning(disable: 4505) // 'VGLDontQuit' : unreferenced local function has been removed
	#pragma warning(disable: 4211) // nonstandard extension used : redefined extern to static
	#pragma warning(disable: 4310) // cast truncates constant value   s_KeyCharMap[VGLK_3].m_chUpper  = (char)0xA7;
	#pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'

	#pragma warning(disable: 4131) // old style c syntax
	#pragma warning(disable: 4702) // unreachable code

#ifndef __HAVOK_PARSER__
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Tools/Libraries/Geom2/vGeom2.hpp>
#endif

#undef True
#undef False

#pragma managed(pop)

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
