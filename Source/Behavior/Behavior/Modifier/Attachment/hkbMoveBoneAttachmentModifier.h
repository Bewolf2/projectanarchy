/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MOVE_BONE_ATTACHMENT_MODIFIER_H
#define HKB_MOVE_BONE_ATTACHMENT_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbMoveBoneAttachmentModifierClass;

	/// A modifier that moves a bone attachment from one bone to another
class hkbMoveBoneAttachmentModifier : public hkbModifier
{
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbMoveBoneAttachmentModifier();
		hkbMoveBoneAttachmentModifier( const hkbMoveBoneAttachmentModifier& other );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// When this modifiers m_event is thrown, the attachment is moved
		virtual void handleEvent(const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			/// Returns true if the modifier is valid, false otherwise
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

	public:

			/// The ID of the event that triggers the modifier to move the bone attachment.
		hkInt32 m_activateEventId;	//+default(hkbEvent::EVENT_ID_NULL)
									//+hkb.RoleAttribute("ROLE_EVENT_ID")
									//+hk.Description("Event that triggers the modifier to move the bone attachment.")

			/// The name of the attachment on the character to move
		hkStringPtr m_attachment;	//+hk.Description("The name of the attachment on the character to be modified.")
									//+hkb.RoleAttribute("ROLE_BONE_ATTACHMENT")
									
			/// The local frame that will become the new parent bone of the attachment
		hkStringPtr m_localFrame;	//+hk.Description("The local frame that will become the new parent bone of the attachment.")
									//+hkb.RoleAttribute("ROLE_LOCAL_FRAME")
			

	public:

		hkbMoveBoneAttachmentModifier( hkFinishLoadedObjectFlag flag );
};


#endif // HKB_MOVE_BONE_ATTACHMENT_MODIFIER_H

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
