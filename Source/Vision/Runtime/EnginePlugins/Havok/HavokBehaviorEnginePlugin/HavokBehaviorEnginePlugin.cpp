/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// stdafx.cpp : source file that includes just the standard includes
//	RainSample.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/HavokBehaviorEnginePlugin.hpp>

#ifndef VBASE_LIB // then dll so need some static stuff

// Havok - Keycode
#include <Common/Base/keycode.cxx>

#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_MemoryTracker
//#define HK_EXCLUDE_FEATURE_RegisterVersionPatches

#undef HAVOK_CLOTH_KEYCODE
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HAVOK_DESTRUCTION_2012_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#undef HAVOK_AI_KEYCODE 
#undef HK_FEATURE_PRODUCT_AI
#undef HAVOK_SIMULATION_KEYCODE
#undef HK_FEATURE_PRODUCT_MILSIM
#undef HAVOK_PHYSICS_KEYCODE
#undef HK_FEATURE_PRODUCT_PHYSICS
#undef HAVOK_DESTRUCTION_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION

#include <Common/Base/Config/hkProductFeatures.cxx>

#else

// Static lib, just need to add our classes, which we will do on init
#define HK_FEATURE_REFLECTION_PHYSICS_2012
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorClassReg.cxx>

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
