/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#ifndef VBASE_LIB
// Havok keycode
#include <Common/Base/keycode.cxx>

#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700

#undef HAVOK_ANIMATION_KEYCODE
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HAVOK_BEHAVIOR_KEYCODE
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#undef HAVOK_CLOTH_KEYCODE
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HAVOK_DESTRUCTION_2012_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#undef HK_FEATURE_PRODUCT_MILSIM
#undef HAVOK_PHYSICS_KEYCODE
#undef HK_FEATURE_PRODUCT_PHYSICS

#include <Common/Base/Config/hkProductFeatures.cxx>
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
