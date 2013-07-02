/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_INCLUDES_HPP
#define __VHAVOK_AI_INCLUDES_HPP

#include <Common/Base/KeyCode.h>
#ifndef HAVOK_AI_KEYCODE
#error "A valid Havok AI keycode is required to build vHavokAiPlugin. Please enter your keycode(s) in <Common/Base/KeyCode.h>"
#endif

//
// Import/export
//

#ifdef WIN32
  #ifdef VHAVOKAIMODULE_EXPORTS
	#define VHAVOKAI_IMPEXP __declspec(dllexport)
  #elif defined VHAVOKAIMODULE_IMPORTS
	#define VHAVOKAI_IMPEXP __declspec(dllimport)
  #else
	#define VHAVOKAI_IMPEXP
  #endif
  
#elif defined (_VISION_XENON)
	#define VHAVOKAI_IMPEXP   

#elif defined (_VISION_PS3)
	#define VHAVOKAI_IMPEXP
    
#elif defined (_VISION_IOS) || defined(_VISION_ANDROID)
	#define VHAVOKAI_IMPEXP

#elif defined(_VISION_PSP2)
	#define VHAVOKAI_IMPEXP 

#elif defined(_VISION_WIIU)
	#define VHAVOKAI_IMPEXP 

#else
	#error Undefined platform!
#endif


//
// Globals
//

extern VModule g_vHavokAiModule;

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>


//
// Havok includes
//

// Always include the Havok headers as unmanaged code, since there are constructs
// contained in there which don't compile to managed targets.
#pragma managed(push, off)

// Base
#include <Common/Base/hkBase.h>
#include <Common/Base/Ext/hkBaseExt.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Thread/hkThreadMemory.h>

// Ai Base
#include <Ai/Pathfinding/hkaiBaseTypes.h>

#pragma managed(pop)


#endif //__VHAVOK_AI_INCLUDES_HPP

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
