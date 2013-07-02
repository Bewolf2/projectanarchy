/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Common/Base/Config/hkConfigVersion.h>
#if (HAVOK_SDK_VERSION_MAJOR >= 2010)

// Serialization and patches
#include <Common/Base/KeyCode.h>

#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
//#define HK_EXCLUDE_FEATURE_MemoryTracker
//#define HK_EXCLUDE_FEATURE_RegisterVersionPatches 

// Plugins have to reg thier own classes, always, even if not a dll.
#undef HAVOK_ANIMATION_KEYCODE
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HAVOK_BEHAVIOR_KEYCODE
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#undef HAVOK_CLOTH_KEYCODE
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HAVOK_DESTRUCTION_2012_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#undef HAVOK_AI_KEYCODE 
#undef HK_FEATURE_PRODUCT_AI 
#undef HAVOK_PHYSICS_KEYCODE
#undef HK_FEATURE_PRODUCT_PHYSICS
#undef HAVOK_DESTRUCTION_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HAVOK_SIMULATION_KEYCODE
#undef HK_FEATURE_PRODUCT_MILSIM

#include <Common/Base/Config/hkProductFeatures.cxx>

#else // 7.1 etc

// Register Havok classes that are appropriate for the current keycode strings
#include <Common/Base/KeyCode.h>
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkKeyCodeClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

// Register versioning information.
// If you don't want the ability to load previous Havok versions then use hkCompat_None instead to reduce code size
#define HK_COMPAT_FILE <Common/Compat/hkCompat.h>

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
