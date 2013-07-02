/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ANIMATION_BINDING_SET_H
#define HKB_ANIMATION_BINDING_SET_H

#include <Animation/Animation/hkaAnimation.h>
#include <Animation/Animation/Animation/hkaAnimationBinding.h>

#include <Behavior/Utilities/Utils/hkbAssetLoader.h>

	/// This class stores an animation binding pointer along with the triggers that 
	/// were converted from annotations.
class hkbAnimationBindingWithTriggers : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		virtual ~hkbAnimationBindingWithTriggers();

			/// A trigger (event on the animation timeline) that was converted from an annotation on the animation.
		struct Trigger
		{
				/// The time of the event on the animation timeline.
			hkReal m_time;

				/// The event to send.
			hkInt32 m_eventId;
		};

			/// A pointer to an animation binding.
		hkRefPtr<hkaAnimationBinding> m_binding;

			/// The triggers on the animation.
		hkArray<Trigger> m_triggers;

			/// The name of the animation for debug purposes
		HK_ON_DEBUG(hkStringPtr m_name;)
};

	/// An set of animation bindings with triggers that have been converted from annotations.
class hkbAnimationBindingSet : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		~hkbAnimationBindingSet();

			// Clear all bindings.
		void clear();

			/// An array of bindings (a map from integers to bindings).  A reference will be removed from each
			/// element on destruction, so you are responsible for adding referenced when populating this array.
		hkArray<hkbAnimationBindingWithTriggers*> m_bindings;

			/// An array of bundles from which the bindings were loaded from.
			/// This is strictly for refcounting purposes. The linking utils
			/// sets the bundles it loads to be owned by the animation
			/// sets they are bound to. This is consistent with how it
			/// handles individually loaded bindings.
		hkArray< hkRefPtr<hkbAssetBundle> > m_assetBundles;
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
