/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ATTACHMENT_MODIFIER_H
#define HKB_ATTACHMENT_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Attachment/hkbAttachmentSetup.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

extern const class hkClass hkbAttachmentModifierClass;

class hkbCharacter;
class hkbSymbolIdMap;
class hkbHandle;
struct hkbAttachmentInstance;

/// A modifier that turns on and off attachments in activate and deactivate.
class hkbAttachmentModifier : public hkbModifier
{
	public:
		// +version(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		/// Empty constructor.
		hkbAttachmentModifier();

	protected:

		hkbAttachmentModifier( const hkbAttachmentModifier& mod );

	public:

		~hkbAttachmentModifier();

		//////////////////////
		// hkbNode interface
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timeStep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

		////////////////////
		// local methods
		////////////////////

		void setAttachmentSetup( hkbAttachmentSetup* setup );

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// This event is sent to the attacher when an attachment is formed.
		class hkbEventProperty m_sendToAttacherOnAttach;	//+hk.Description("This event is sent to the attacher when an attachment is formed.")

			/// This event is sent to the attachee when an attachment is formed.
		class hkbEventProperty m_sendToAttacheeOnAttach;	//+hk.Description("This event is sent to the attachee when an attachment is formed.")

			/// This event is sent to the attacher when an attachment is released.
		class hkbEventProperty m_sendToAttacherOnDetach;	//+hk.Description("This event is sent to the attacher when an attachment is released.")

			/// This event is sent to the attachee when an attachment is released.
		class hkbEventProperty m_sendToAttacheeOnDetach;	//+hk.Description("This event is sent to the attachee when an attachment is released.")

			/// The attachment setup required to perform the attachment.  Don't set this directly.  Use setAttachmentSetup().
		hkRefPtr<hkbAttachmentSetup> m_attachmentSetup;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE") 
														//+hk.Description("The setup data required to perform the attachment.")

			/// The handle on the attacher where we want to attach.
		hkbHandle* m_attacherHandle;	//+default(HK_NULL)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description("The handle on the attacher where we want to attach.")

			/// The handle on the attachee where we want to attach.
		hkbHandle* m_attacheeHandle;	//+default(HK_NULL)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description("The handle on the attachee where we want to attach.")

			/// The layer to put the attachee in.
		hkInt32 m_attacheeLayer;	//+default(-1)
									//+hk.RangeInt32(absmin=-1,absmax=31)
									//+hk.Description("The layer to put the attachee in.")

	private:

		// form an attachment
		void attach( const hkbContext& context );

		// release the attachment
		void detach( const hkbContext& context );

		void sendEventToCharacter( const hkbContext& contex, const hkbEventProperty& ep, hkbSymbolIdMap* eventIdMap, hkbCharacter* character );

		// the rigid body I'm attaching to
		hkbRigidBodyHandleRef m_attacheeRB; //+nosave+default(HK_NULL)

		// the old motion type of the rigid body when keyframing the attachee
		hkEnum<hkbRigidBodySetup::Type, hkUint8> m_oldMotionType; //+nosave

		// save the collision filter info of the attachment
		hkInt32 m_oldFilterInfo; //+nosave+default(0)

		// the attachment instance returned by the attachment manager
		hkbAttachmentInstance* m_attachment; //+nosave+default(0)

	public:

		hkbAttachmentModifier( hkFinishLoadedObjectFlag flag );

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
