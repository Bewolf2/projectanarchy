/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokBehaviorIncludes.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok Behavior
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VISION_HAVOKBEHAVIOR_INCLUDED_HPP
#define VISION_HAVOKBEHAVIOR_INCLUDED_HPP

#include <Common/Base/KeyCode.h>
#if !defined(HAVOK_BEHAVIOR_KEYCODE) || !defined(HAVOK_ANIMATION_KEYCODE)
#error "Valid Havok Behavior and Havok Animation keycodes are required to build vHavokBehaviorPlugin. Please enter your keycodes in <Common/Base/KeyCode.h>"
#endif

extern VModule g_vHavokBehaviorModule;

#ifdef WIN32
#	ifdef VHAVOKBEHAVIORMODULE_EXPORTS
#		define VHAVOKBEHAVIOR_IMPEXP __declspec(dllexport)
#	elif defined VHAVOKBEHAVIORMODULE_IMPORTS
#		define VHAVOKBEHAVIOR_IMPEXP __declspec(dllimport)
#	else
#		define VHAVOKBEHAVIOR_IMPEXP
#	endif
#elif defined (_VISION_XENON)
#	define VHAVOKBEHAVIOR_IMPEXP 
#elif defined (_VISION_PS3)
#	define VHAVOKBEHAVIOR_IMPEXP
#elif defined (_VISION_WII)
#	define VHAVOKBEHAVIOR_IMPEXP 
#elif defined (_VISION_IOS) || defined(_VISION_ANDROID)
#	define VHAVOKBEHAVIOR_IMPEXP
#elif defined(_VISION_PSP2)
#	define VHAVOKBEHAVIOR_IMPEXP 
#elif defined(_VISION_WIIU)
#	define VHAVOKBEHAVIOR_IMPEXP 
#else
#	error Undefined platform!
#endif

#endif //VISION_HAVOKBEHAVIOR_INCLUDED_HPP

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
