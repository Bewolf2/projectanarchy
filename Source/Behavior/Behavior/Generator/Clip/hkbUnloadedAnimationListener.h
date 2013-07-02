/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_UNLOADED_ANIMATION_LISTENER_H
#define HKB_UNLOADED_ANIMATION_LISTENER_H

#include <Common/Base/Object/hkReferencedObject.h>
#include <Animation/Animation/hkaAnimation.h>

class hkbAnimationBindingWithTriggers;
class hkbContext;
class hkbClipGenerator;

	/// Any class that is interested in generator output of a node needs to inherit from this class
class hkbUnloadedAnimationListener : public hkReferencedObject
{
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		hkbUnloadedAnimationListener();

		virtual hkbAnimationBindingWithTriggers* getAnimationBinding( const hkbContext* context, const hkbClipGenerator* clipGen );

	public:

		hkRefPtr<hkaAnimation> m_animation;
};

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
