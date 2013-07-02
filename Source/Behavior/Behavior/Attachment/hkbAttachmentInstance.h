/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_ATTACHMENT_INSTANCE_H
#define HK_ATTACHMENT_INSTANCE_H

#include <Behavior/Behavior/Attachment/hkbAttachmentTargets.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

class hkbAttachmentSetup;

/// An instance of an attachment.  Note that "attachment" does not refer to the entity being attached,
/// but to the notion of there being an attachment between two character.
struct hkbAttachmentInstance
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAttachmentInstance );

		/// The parameters that describe how to do the attachment.
	hkbAttachmentSetup* m_setup;

		/// The targets being attached.
	hkbAttachmentTargets m_targets;

		/// The amount of time that has passed since the attachment was formed.
	hkReal m_timeAttached;

		/// A constraint, if one is being used.
	hkbConstraintHandleRef m_constraint;
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
