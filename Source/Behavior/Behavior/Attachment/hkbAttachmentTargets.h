/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ATTACHMENT_TARGETS_H
#define HKB_ATTACHMENT_TARGETS_H

#include <Behavior/Behavior/Utils/hkbClosestLocalFrameCollector.h>

class hkbCharacter;

/// The targets to an attachment.  The attachee can be either a character or a 
/// rigid body.  If you specify both, the rigid body should belong to the character,
/// which will override hkbAttachmentSetup::m_attacheeBone.
struct hkbAttachmentTargets
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAttachmentTargets );

	hkbAttachmentTargets()
	{
		m_attacherHandle.clear();
		m_attacheeHandle.clear();
	}

		/// A handle on the attacher.  The attachment will be formed at the handle.
		/// If the attachment involves an attacher rigid body but not a handle, you
		/// need to set the rigid body herein.
	hkbHandle m_attacherHandle;

		/// A handle on the attachee.  The attachment will be formed at the handle.
		/// If the attachment involves an attachee rigid body but not a handle, you
		/// need to set the rigid body herein.
	hkbHandle m_attacheeHandle;
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
